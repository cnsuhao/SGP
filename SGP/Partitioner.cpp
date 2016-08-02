#include "stdafx.h"
#include "Partitioner.h"
#include "Log.h"
#include <sstream>
#include <queue>
#include "AssignContext.h"

using namespace std;
using namespace stdext;

Partitioner::~Partitioner(void)
{
	delete _stat;
}

void Partitioner::InitStatistic()
{
	_stat=new Statistic(_k);
}

void Partitioner::ClearPartition()
{
	for(int i=0; i<_aPartition.size();i++)
	{
		delete _aPartition.at(i);
	}
	_aPartition.clear();
	if(_stat!=NULL)
		_stat->ResetPartitionInfos();
}

void Partitioner::AppendClusterNode(Cluster* cluster, ClusterNode& node)
{
	cluster->_cluster.push_back(node);
	cluster->_cluster_node_idx.insert(Int_Pair(node._pos, cluster->_cluster.size()-1));
}

void Partitioner::doKL()
{
	this->ClearPartition();
	queue<Cluster*> cluster_to_split;
	AdjTable* g = _graph->GetAdjTableRef();

	//create the first cluster
	Cluster* pCluster = new Cluster();
	for(int i=0; i<_graph->GetVertexNumber(); i++)
	{
		ClusterNode node = {i, 0, 0};//pos, gain, visited
		AppendClusterNode(pCluster, node);
	}
	cluster_to_split.push(pCluster);

	int split_number =_k-1;//2^n-1
	int exchange_count = 0;
	for(int k=1; k<=split_number; k++)
	{
		Log::log("spliting phase : ");
		Log::log(k);
		Log::log("\n");

		//split a cluster
		Cluster* pSplitCluster =cluster_to_split.front();
		cluster_to_split.pop();
		Cluster* pNewClusterA = pSplitCluster;
		Cluster* pNewClusterB = new Cluster();
		int vertex_number_before_split = pSplitCluster->_cluster.size();
		int vertex_number_of_a_cluster = vertex_number_before_split/2;
		
		Log::log("create splited cluster... \n");
		for(int i=vertex_number_of_a_cluster; i<vertex_number_before_split; i++)
		{
			AppendClusterNode(pNewClusterB,pSplitCluster->_cluster.at(i));
			map<int, int>::iterator iter_cluster_node_idx = 
				pNewClusterA->_cluster_node_idx.find(pSplitCluster->_cluster.at(i)._pos);
			pNewClusterA->_cluster_node_idx.erase(iter_cluster_node_idx);
		}
		vector<ClusterNode>::iterator iter = pNewClusterA->_cluster.begin();
		pNewClusterA->_cluster.erase(iter+vertex_number_of_a_cluster, iter+vertex_number_before_split);
		
		Log::log("do partitioning...\n");
		//partitioning the splitted cluster
		int c = doKLPartition(pNewClusterA, pNewClusterB);
		exchange_count += c;

		//push the clusters
		cluster_to_split.push(pNewClusterA);
		cluster_to_split.push(pNewClusterB);
	}
	GetStatistic()->AppendPartitionExchangeCount(exchange_count);

	while(!cluster_to_split.empty())
	{
		_aPartition.push_back(cluster_to_split.front());
		cluster_to_split.pop();
	}

}

int Partitioner::doKLPartition(Cluster* aCluster, Cluster* bCluster)
{
	Log::log("\t doKLPartition...\n");
	bool exchanged = false;
	int total_exchanged_count = 0;
	do {

		exchanged = false;
		
		//init the cluster - compute a vertex gain and insert sorting
		ComputeGainAndSort(aCluster, bCluster);
		int min_cluster_size = aCluster->_cluster.size() > bCluster->_cluster.size() 
			? bCluster->_cluster.size():aCluster->_cluster.size();
		
		int exchange_count = 0;
		for(int i=0; i<min_cluster_size; i++)
		{
			//select two nodes to swap
			int exchanged_a, exchanged_b;
			if(!FindExchangeClusterNode(aCluster, i, exchanged_a, bCluster, i, exchanged_b))
				break;
			/*
			Log::log("\t exchange ");
			Log::log(exchanged_a);
			Log::log(" and ");
			Log::log(exchanged_b);
			Log::log("\n");
			*/

			aCluster->_cluster.at(exchanged_a)._visited = 1;
			bCluster->_cluster.at(exchanged_b)._visited = 1;
			//swap
			SwapClusterNodebyPos(aCluster, exchanged_a, bCluster, exchanged_b);
			SwapClusterNodebyPos(aCluster, exchanged_a, aCluster, i);
			SwapClusterNodebyPos(bCluster, exchanged_b, bCluster, i);
			//update D value
			unordered_set<int> d_changed_list_a, d_changed_list_b;
			UpdateKLDValue(aCluster, i, d_changed_list_a,bCluster, i, d_changed_list_b);
			//sort the reminder of nodes on D value
			SortCluster(aCluster,d_changed_list_a,i);
			SortCluster(bCluster,d_changed_list_b,i);

			exchanged = true;
			exchange_count++;
		}
		total_exchanged_count += exchange_count;
	}while(exchanged);

	return total_exchanged_count;
}

bool Partitioner::FindExchangeClusterNode(Cluster* aCluster,int find_begin_a,int& exchanged_a,
	                                      Cluster* bCluster,int find_begin_b,int& exchanged_b)
{
	vector<ClusterNode>::iterator iter_a = aCluster->_cluster.begin();
	iter_a += find_begin_a;
	int begin_a_gain = iter_a->_gain;
	int cur_a_pos = find_begin_a;

	vector<ClusterNode>::iterator iter_b = bCluster->_cluster.begin();
	iter_b += find_begin_b;
	int begin_b_gain = iter_b->_gain;
	int cur_b_pos = find_begin_b;

	int max_a_pos = cur_a_pos, max_b_pos = cur_b_pos, max_gain = INT_MIN;
	while(iter_a!=aCluster->_cluster.end())
	{
		int cur_a_gain = iter_a->_gain;
		if((begin_a_gain - cur_a_gain) >= 2 )
			break;

		iter_b = bCluster->_cluster.begin();
		iter_b += find_begin_b;
		while(iter_b!=bCluster->_cluster.end())
		{
			int cur_b_gain = iter_b->_gain;
			if((begin_b_gain - cur_b_gain) >= 2 )//the following pairs will be less than it
				break;

			int c=0;
			if(_graph->isConnectbyPos(iter_a->_pos, iter_b->_pos ))
				c = 1;
			int gain = iter_a->_gain+iter_b->_gain-2*c; 
			if(gain>=0 && gain > max_gain)
			{
				max_gain = gain;
				max_a_pos = cur_a_pos;
				max_b_pos = cur_b_pos;
			}

			if(c == 0)//the following pairs will be less than it
				break;

			iter_b++;
			cur_b_pos++;
		}
		iter_a++;
		cur_a_pos++;
	}

	if(max_gain <=0)
	{
		return false;
	}
	else
	{
		exchanged_a = max_a_pos;
		exchanged_b = max_b_pos;
		return true;
	}
}
void Partitioner::UpdateKLDValue(	Cluster* aCluster, int exchanged_node_a_pos, unordered_set<int>& d_changed_list_a, 
									Cluster* bCluster, int exchanged_node_b_pos, unordered_set<int>& d_changed_list_b)
{
	//here, we don't re-calculate the internal cost since it will be freshed by ComputeGainAndSort
	//the exchanged node a into A
	EdgeInfoArray* edges_a = _graph->GetAdjEdgeListofPos(aCluster->_cluster.at(exchanged_node_a_pos)._pos);
	for(EdgeInfoArray::iterator iter_a = edges_a->begin(); iter_a!=edges_a->end(); iter_a++)
	{
		//check if the adj-vex of a is in the A set
		map <int, int> :: const_iterator iter_cluster_a = aCluster->_cluster_node_idx.find(iter_a->_adj_vex_pos);
		if(iter_cluster_a!=aCluster->_cluster_node_idx.end() && aCluster->_cluster.at(iter_cluster_a->second)._visited == 0)
		{
			aCluster->_cluster.at(iter_cluster_a->second)._gain -=1;
			d_changed_list_a.insert(iter_cluster_a->second);
		}
		//check if the adj-vex of a is in the B set
		map <int, int> :: const_iterator iter_cluster_b = bCluster->_cluster_node_idx.find(iter_a->_adj_vex_pos);
		if(iter_cluster_b!=bCluster->_cluster_node_idx.end() && bCluster->_cluster.at(iter_cluster_b->second)._visited == 0)
		{
			bCluster->_cluster.at(iter_cluster_b->second)._gain +=1;
			d_changed_list_b.insert(iter_cluster_b->second);
		}
	}
	//the exchanged node b into B
	EdgeInfoArray* edges_b = _graph->GetAdjEdgeListofPos(bCluster->_cluster.at(exchanged_node_b_pos)._pos);
	for(EdgeInfoArray::iterator iter_b = edges_b->begin(); iter_b!=edges_b->end(); iter_b++)
	{
		//check if the adj-vex of b is in the A set
		map <int, int> :: const_iterator iter_cluster_a = aCluster->_cluster_node_idx.find(iter_b->_adj_vex_pos);
		if(iter_cluster_a!=aCluster->_cluster_node_idx.end()&& aCluster->_cluster.at(iter_cluster_a->second)._visited == 0)
		{
			aCluster->_cluster.at(iter_cluster_a->second)._gain +=1;
			d_changed_list_a.insert(iter_cluster_a->second);
		}
		//check if the adj-vex of b is in the B set
		map <int, int> :: const_iterator iter_cluster_b = bCluster->_cluster_node_idx.find(iter_b->_adj_vex_pos);
		if(iter_cluster_b!=bCluster->_cluster_node_idx.end()&& bCluster->_cluster.at(iter_cluster_b->second)._visited == 0)
		{
			bCluster->_cluster.at(iter_cluster_b->second)._gain -=1;
			d_changed_list_b.insert(iter_cluster_b->second);
		}
	}

}

void Partitioner::ComputeGainAndSort(Cluster* aCluster, Cluster* bCluster)
{
	for(int i=0; i<aCluster->_cluster.size(); i++)
	{
		aCluster->_cluster.at(i)._visited = 0;
		ComputeGain(aCluster->_cluster.at(i), aCluster, bCluster);
		InsertSort(aCluster, i);
	}
	for(int i=0; i<bCluster->_cluster.size(); i++)
	{
		bCluster->_cluster.at(i)._visited = 0;
		ComputeGain(bCluster->_cluster.at(i), bCluster, aCluster);
		InsertSort(bCluster, i);
	}
}

void Partitioner::ComputeGain(ClusterNode& node_in_aCluster, Cluster* aCluster, Cluster* bCluster)
{
	EdgeInfoArray* edges = _graph->GetAdjEdgeListofPos(node_in_aCluster._pos);
	int internal_cost = 0, external_cost=0;
	internal_cost = SetIntersectionBetweenEdgeListAndCluster(edges, aCluster);
	external_cost = SetIntersectionBetweenEdgeListAndCluster(edges, bCluster);
	node_in_aCluster._gain = external_cost-internal_cost;
	node_in_aCluster._internal_cost = internal_cost;
}

int Partitioner::SetIntersectionBetweenEdgeListAndCluster(EdgeInfoArray* vexlist, Cluster* cluster)
{
	int num=0;
	for(EdgeInfoArray::iterator iter = vexlist->begin(); iter!=vexlist->end(); iter++)
	{
		ClusterNode node;
		node._pos = iter->_adj_vex_pos;
		int found = GetClusterNode(cluster, node);
		if(found >=0)
			num++;
	}
	return num;
	/*for(int i=0; i<vexlist->size(); i++)
	{
		for(int j=0; j<cluster->_cluster.size(); j++)
		{
			if(vexlist->at(i)._adj_vex_pos == cluster->_cluster.at(j)._pos)
			{
				num++;
			}
		}
	}
	return num;*/
}

void Partitioner::InsertSort(Cluster* aCluster, int end_pos)
{
	InsertSort(aCluster, end_pos, 0);
}

void Partitioner::InsertSort(Cluster* aCluster, int end_pos, int begin_pos)
{
	if(end_pos == begin_pos) return;
	ClusterNode node_in_aCluster = aCluster->_cluster.at(end_pos);
	int i = end_pos-1;
	for(; i>=begin_pos && (node_in_aCluster._gain>aCluster->_cluster.at(i)._gain); i--)
	{
		MoveClusterNode(aCluster, i, i+1);
	}

	if(i == end_pos-1) return;

	if(i<begin_pos)
	{
		i=begin_pos-1;
	}
	InsertClusterNode(aCluster, node_in_aCluster, i+1);
}

void  Partitioner::SwapClusterNodebyPos(Cluster* aCluster, int pos_a, Cluster* bCluster, int pos_b)
{
	if(aCluster == bCluster && pos_a == pos_b) return;

	map <int, int> :: iterator iter_a = aCluster->_cluster_node_idx.find(aCluster->_cluster.at(pos_a)._pos);
	aCluster->_cluster_node_idx.erase(iter_a);
	map <int, int> :: iterator iter_b = bCluster->_cluster_node_idx.find(bCluster->_cluster.at(pos_b)._pos);
	bCluster->_cluster_node_idx.erase(iter_b);

	aCluster->_cluster_node_idx.insert(Int_Pair(bCluster->_cluster.at(pos_b)._pos, pos_a));
	bCluster->_cluster_node_idx.insert(Int_Pair(aCluster->_cluster.at(pos_a)._pos, pos_b));

	ClusterNode node = aCluster->_cluster.at(pos_a);
	aCluster->_cluster.at(pos_a) = bCluster->_cluster.at(pos_b);
	bCluster->_cluster.at(pos_b) = node;
}

void Partitioner::MoveClusterNode(Cluster* cluster, int from, int to)
{
	map <int, int> :: iterator iter;
/*	
	iter = cluster->_cluster_node_idx.find(cluster->_cluster.at(to)._pos);
	if(iter!= cluster->_cluster_node_idx.end()) 
		cluster->_cluster_node_idx.erase(iter);
*/
	iter = cluster->_cluster_node_idx.find(cluster->_cluster.at(from)._pos);
	if(iter!= cluster->_cluster_node_idx.end()) 
	{
		iter->second = to;
	}
	else
	{
		cluster->_cluster_node_idx.insert(Int_Pair(cluster->_cluster.at(from)._pos, to));
	}

	cluster->_cluster.at(to) = cluster->_cluster.at(from);
}

int Partitioner::GetClusterNode(Cluster* cluster, ClusterNode& node)
{
	map <int, int> :: iterator iter = cluster->_cluster_node_idx.find(node._pos);
	if(iter!=cluster->_cluster_node_idx.end()) 
		return iter->second;
	else
		return -1;
}

int Partitioner::GetClusterLabelOfVex(VERTEX u)
{
	int u_pos = _graph->GetVertexPos(u);
	if(u_pos == -1) return -1;

	ClusterNode node;
	node._pos = u_pos;

	bool found = false; 
	int cluster_pos = 0;
	Partition::const_iterator iter = _aPartition.begin();
	while(iter!=_aPartition.end())
	{
		Cluster* cluster = (*iter);
		if(GetClusterNode(cluster, node) != -1)
		{
			found = true;
			break;
		}
		iter++;
		cluster_pos++;
	}
	if(found)
		return cluster_pos;
	else
		return -1;
}

void Partitioner::InsertClusterNode(Cluster* cluster, ClusterNode node, int at)
{
	map <int, int> :: iterator iter = cluster->_cluster_node_idx.find(node._pos);
	if(iter!= cluster->_cluster_node_idx.end()) 
		iter->second = at;
	else
		cluster->_cluster_node_idx.insert(Int_Pair(node._pos, at));
	cluster->_cluster.at(at) = node;
}

void Partitioner::DeleteClusterNodeAtPos(Cluster* cluster, int at)
{
	//modify the idx after the nodes of the pos of 'at'
	vector<ClusterNode>::iterator iter = cluster->_cluster.begin();
	map <int, int> :: iterator iter1;
	for(iter+=at; iter!=cluster->_cluster.end(); iter++)
	{
		iter1 = cluster->_cluster_node_idx.find(iter->_pos);
		if(iter1!=cluster->_cluster_node_idx.end())
			iter1->second -= 1;
	}
	//delete the node
	iter = cluster->_cluster.begin();
	iter += at;
	int node = iter->_pos;
	cluster->_cluster.erase(iter);
	iter1 = cluster->_cluster_node_idx.find(node);
	if(iter1!= cluster->_cluster_node_idx.end()) 
	{
		cluster->_cluster_node_idx.erase(iter1);
	}

}

void Partitioner::SortCluster(Cluster* cluster, unordered_set<int>& node_pos_of_d_changed, int from)
{
	//duplicate the deleted nodes
	vector<ClusterNode> temp;
	unordered_set<int>::iterator iter = node_pos_of_d_changed.begin();
	set<int, less<int>> order_node_pos_of_d_changed;
	for(; iter!=node_pos_of_d_changed.end(); iter++) 
	{
		temp.push_back(cluster->_cluster.at(*iter));
		order_node_pos_of_d_changed.insert(*iter);
	}
	//delete node acesendingly
	set<int, less<int>>::iterator order_iter = order_node_pos_of_d_changed.begin();
	for(int i=0;order_iter!=order_node_pos_of_d_changed.end();order_iter++, i++)
	{
		DeleteClusterNodeAtPos(cluster, (*order_iter)-i);
	}
	//insert the cache nodes
	for(vector<ClusterNode>::iterator it = temp.begin(); it!=temp.end(); it++)
	{
		AppendClusterNode(cluster, *it);
		InsertSort(cluster, cluster->_cluster.size()-1, from);
	}

}

void Partitioner::doMaxMin()
{

}

void Partitioner::WriteVerticesOfPartitions()
{
	Partition::iterator iter = _aPartition.begin();
	for(int i=0; iter!=_aPartition.end(); iter++, i++)
	{
		stringstream out;
		out<<_outfile<<"_cluster_vertices."<<i;
		ofstream ofs(out.str());
		
		stringstream log_str;
		log_str<<" writing the vertices of the partition " <<i<<" into "<<out.str()<<"\n";
		Log::log(log_str.str());
		vector<ClusterNode>::iterator iter_node = (*iter)->_cluster.begin();
		for(; iter_node!=(*iter)->_cluster.end(); iter_node++)
		{
			VERTEX u = _graph->GetVertexInfoofPos(iter_node->_pos)->_u;
			ofs<<u<<endl;
		}
		log_str.str("");
		log_str<<" the number of vertices of the partition "<<i<<" : "<<(*iter)->_cluster.size()<<"\n";
		Log::log(log_str.str());

		//statistic
		GetStatistic()->SetPartitionVexNumber(i, (*iter)->_cluster.size());
	}
}

void Partitioner::WriteClusterEdgesOfPartitions()
{
	Partition::iterator iter = _aPartition.begin();
	for(int cluster_id=0; iter!=_aPartition.end(); iter++, cluster_id++)
	{
		stringstream log_str;
		log_str<<" writing the edges of the partition " <<cluster_id<<" into "<<_outfile<<"_cluster_edges."<<cluster_id<<"\n";
		Log::log(log_str.str());
		int edges_num = 0;
		vector<ClusterNode>::iterator iter_node = (*iter)->_cluster.begin();
		for(; iter_node!=(*iter)->_cluster.end(); iter_node++)
		{
			VERTEX u = _graph->GetVertexInfoofPos(iter_node->_pos)->_u;
			EdgeInfoArray* edges = _graph->GetAdjEdgeListofPos(iter_node->_pos);
			EdgeInfoArray::const_iterator iter_edges = edges->begin();
			while(iter_edges != edges->end())
			{
				VERTEX v = _graph->GetVertexInfoofPos(iter_edges->_adj_vex_pos)->_u;
				int cluster_id_v = GetClusterLabelOfVex(v);
				if(v<u)
				{
					WriteClusterEdge(u, cluster_id, v, cluster_id_v);
					edges_num++;
				}
				//if(cluster_id_v == cluster_id)
				//{
				//	if( v<u )//write the internal edge once
				//	{
				//		WriteClusterEdge(u, cluster_id, v, cluster_id_v);
				//		edges_num++;
				//	}
				//}
				//else
				//{
				//	WriteClusterEdge(u, cluster_id, v, cluster_id_v);
				//	edges_num++;
				//}
				iter_edges++;
			}
		}
		log_str.str("");
		log_str<<" the number of edges of the partition "<<cluster_id<<" : "<<edges_num<<"\n";
		Log::log(log_str.str());
	}
}

void Partitioner::WriteAssignEdge(VERTEX u, int u_cluster, VERTEX v, int v_cluster)
{
	if(u_cluster == v_cluster)
	{
		stringstream str;
		str<<_outfile<<"_assign_edge."<<u_cluster;
		ofstream ofs(str.str(), ofstream::app);
		ofs<<u<<" "<<u_cluster<<" "<<v<<" "<<v_cluster<<endl;
		ofs.close();

		//statistic
		GetStatistic()->IncreaseAssignInternalLinks(u_cluster);
	}
	else
	{
		stringstream str;
		str<<_outfile<<"_assign_edge."<<u_cluster;
		ofstream ofs(str.str(), ofstream::app);
		ofs<<u<<" "<<u_cluster<<" "<<v<<" "<<v_cluster<<endl;
		ofs.close();

		str.str("");
		str<<_outfile<<"_assign_edge."<<v_cluster;
		ofs.open(str.str(), ofstream::app);
		ofs<<v<<" "<<v_cluster<<" "<<u<<" "<<u_cluster<<endl;
		ofs.close();

		//statistic
		GetStatistic()->IncreaseAssignExternalLinks(u_cluster);
		GetStatistic()->IncreaseAssignExternalLinks(v_cluster);
	}
}

void Partitioner::WriteClusterEdge(VERTEX u, int u_cluster, VERTEX v, int v_cluster)
{
	if(u_cluster == v_cluster)
	{
		stringstream str;
		str<<_outfile<<"_cluster_edges."<<u_cluster;
		ofstream ofs(str.str(), ofstream::app);
		ofs<<u<<" "<<u_cluster<<" "<<v<<" "<<v_cluster<<endl;
		ofs.close();

		//statistic
		GetStatistic()->IncreasePartitionInternalLinks(u_cluster);
	}
	else
	{
		stringstream str;
		str<<_outfile<<"_cluster_edges."<<u_cluster;
		ofstream ofs(str.str(), ofstream::app);
		ofs<<u<<" "<<u_cluster<<" "<<v<<" "<<v_cluster<<endl;
		ofs.close();

		str.str("");
		str<<_outfile<<"_cluster_edges."<<v_cluster;
		ofs.open(str.str(), ofstream::app);
		ofs<<v<<" "<<v_cluster<<" "<<u<<" "<<u_cluster<<endl;
		ofs.close();

		//statistic
		GetStatistic()->IncreasePartitionExternalLinks(u_cluster);
		GetStatistic()->IncreasePartitionExternalLinks(v_cluster);
	}
}

void Partitioner::WriteAssignVerticesOfPartitions()
{
	AssignContextManager* ac = AssignContextManager::GetAssignManager();
	map<VERTEX, int>* ac_vexs = ac->GetAssignVexInfo();
	ofstream* ofs_arry = new ofstream[_k];
	for(int i=0; i<_k; i++)
	{
		stringstream out;
		out<<_outfile<<"_assign_vertices."<<i;
		ofs_arry[i].open(out.str());
	}

	for(map<VERTEX, int>::iterator iter = ac_vexs->begin(); iter != ac_vexs->end(); iter++)
	{
		VERTEX v = iter->first;
		int partition = iter->second;

		if(partition >= 0)
		{
			//AC中存在已被采样并划分的节点
			int cluster = GetClusterLabelOfVex(v);
			if(cluster!=-1)
				continue;

			//stringstream log_str;
			//log_str<<" writing the assigned vertice "<< v << " into the partition " << partition <<"\n";
			//Log::log(log_str.str());

			ofs_arry[partition]<<v<<endl;

			GetStatistic()->IncreaseAssignVexNumber(partition);
		}
	}

	for(int i=0; i<_k; i++)
	{
		ofs_arry[i].flush();
		ofs_arry[i].close();
	}
	delete[] ofs_arry;
}

int Partitioner::ComputeCutValue()
{
	int cut=0;
	Partition::iterator iter = _aPartition.begin();
	for(int i=0; iter!=_aPartition.end(); iter++, i++)
	{
		vector<ClusterNode>::iterator iter_node = (*iter)->_cluster.begin();
		for(; iter_node!=(*iter)->_cluster.end(); iter_node++)
		{
			VertexInfo* u_info = _graph->GetVertexInfoofPos(iter_node->_pos);
			int degree = u_info->_degree;
			int internal_cost = iter_node->_internal_cost;
			cut += (degree-internal_cost);
		}
	}
	return cut/2;
}

int Partitioner::ComputeLinkstoClusterFromVex(VERTEX vex, int cluster_pos)
{
	EdgeInfoArray* edges = _graph->GetAdjEdgeListofVertex(vex);
	Cluster* cluster = _aPartition.at(cluster_pos);

	return SetIntersectionBetweenEdgeListAndCluster(edges, cluster);
}

void Partitioner::SetPartitionNumber(int k) { 
	
	this->_k = k;
	InitStatistic();
}

bool Partitioner::CheckIfAdjust(map<VERTEX, int>& partitions_change_vex, vector<ReAdjustPartitionPair>& adjust_partitions)
{
	typedef struct {
		bool isRepartition;
	} Cluster_Repartition_info;

	int BT_nodes = 2*_k-1; //complete binary tree
	int BT_height = int(log(float(_k))/log(2.0f))+1;

	vector<Cluster_Repartition_info> cluster_info;
	for(int i=0; i<BT_nodes; i++)
	{
		Cluster_Repartition_info info;
		info.isRepartition = false;
		cluster_info.push_back(info);
	}


	map<VERTEX, int>::iterator iter = partitions_change_vex.begin();
	int  j=0;
	while(iter!=partitions_change_vex.end())
	{
		int level = 2;//from level 2 not root level (1)
		int cluster_of_changed_vex=-1; //the vex'partition in the level, begin with 1
		int cluster_sibling = -1;//the sibling of cluster_changed_vex, begin with 1
		int leaf_of_vex = -1;//the cluster id of changed vex on the leafs, begin with 1

		VERTEX u = iter->first;
		vector<int> partition_u;//the leaf set of the node containing u in BT at the level
		vector<int> partition_not_u; //the leaf set of the node's sibling

		leaf_of_vex = iter->second;
		leaf_of_vex += _k;// //level coding and begin with 1 

		//find a cluster to adjust from the top level, if yes, the following sub-nodes should not be
		//considered.
		for(level=2; level<=BT_height; level++)
		{
			cluster_of_changed_vex = leaf_of_vex;
			//find the cluster of vex in the level
			for(int i = BT_height ; i>level; i--)
			{
				cluster_of_changed_vex = floor(cluster_of_changed_vex/2.0f);
			}
			
			cluster_sibling = (cluster_of_changed_vex%2==0)?cluster_of_changed_vex+1:cluster_of_changed_vex-1;

			if(cluster_info[cluster_of_changed_vex-1].isRepartition)
			{
				break; //the sub-tree needn't to be checked
			}

			//check if adjust
			partition_u.clear();
			partition_not_u.clear();
			int level_delta = BT_height - level;
			int size = (int)pow(2.0f, level_delta);
			int start_leaf = cluster_of_changed_vex*size-_k; //the leftest node (leaf) in the sub-tree, begin with 0
			/*if(start_leaf < 0)
			{
				continue;
			}*/

			for(int i = start_leaf; i<start_leaf+size; i++)
				partition_u.push_back(i);
			start_leaf = cluster_sibling*size-_k; //begin with 0
			for(int i = start_leaf; i<start_leaf+size; i++)
				partition_not_u.push_back(i);

			if(CheckClusterAdjust(u, partition_u, partition_not_u))
			{
				cluster_info[cluster_of_changed_vex-1].isRepartition = true;
				cluster_info[cluster_sibling-1].isRepartition = true;
			}
		}

		iter++; j++;
	}

	//record all the top partition pair to adjust by level traverse on BT
	queue<int> temp_queue;
	temp_queue.push(1);//root node
	while(!temp_queue.empty())
	{
		int next_cluster = temp_queue.front();
		temp_queue.pop();
		if(!cluster_info[next_cluster-1].isRepartition)
		{
			temp_queue.push(next_cluster*2); //push the left child
			temp_queue.push(next_cluster*2+1); //push the right child
		}
		else
		{
			ReAdjustPartitionPair pair;
			pair._part1 = next_cluster;//must be the left
			pair._part2 = next_cluster+1;

			adjust_partitions.push_back(pair);
			temp_queue.pop();//pop the right
		}
	}

	return true;
}

bool Partitioner::CheckClusterAdjust(VERTEX u, vector<int>& partition_u, vector<int>& partition_not_u)
{
	int inter_connections = GetConnectionsToClusterSet(u, partition_u);
	int external_connections = GetConnectionsToClusterSet(u, partition_not_u);
	if(inter_connections - external_connections >=0)
		return false;
	else
		return true;
}

int Partitioner::GetConnectionsToClusterSet(VERTEX u, vector<int>& partitions)
{
	EdgeInfoArray* edge_list =_graph->GetAdjEdgeListofVertex(u);
	int connections =0;
	for(int i=0; i<partitions.size(); i++)
	{
		Cluster* cluster = _aPartition[partitions[i]];
		connections += SetIntersectionBetweenEdgeListAndCluster(edge_list, cluster);
	}
	return connections;
}

void Partitioner::RepartitionKL(vector<ReAdjustPartitionPair>& adjust_partitions)
{
	int BT_nodes = 2*_k-1; //complete binary tree
	int BT_height = int(log(float(_k))/log(2.0f))+1;
	int exchange_count = 0;
	Log::log("Repartition ....... \n");
	for(int i=0; i<adjust_partitions.size(); i++)
	{
		ReAdjustPartitionPair pair = adjust_partitions[i];

		//从i开始调整，将调整结果更新到apartition。先将叶子中的顶点合并成两个分区，然后重新划分。
		//如果知道哪些节点发生变化，则可优化（改动太大，暂时未处理）。
		Log::log("do partitioning...\n");
		Cluster* cluster_left = MergeLeafofNode(pair._part1);
		Cluster* cluster_right = MergeLeafofNode(pair._part2);
		int c = doKLPartition(cluster_left, cluster_right);
		exchange_count += c;

		queue<Cluster*> cluster_to_split;
		cluster_to_split.push(cluster_left);
		cluster_to_split.push(cluster_right);

		int bt_node = pair._part1;
		int node_level = int(log(float(bt_node))/log(2.0f))+1;
		int subtree_height = BT_height - node_level+1;
		int split_number = (int)pow(2.0f, subtree_height)-2;//2^(h-l+1)-2
		
		for(int k=1; k<=split_number; k++)
		{
			Log::log("spliting phase .... ");
			Log::log(k);
			Log::log("\n");

			//split a cluster
			Cluster* pSplitCluster =cluster_to_split.front();
			cluster_to_split.pop();
			Cluster* pNewClusterA = pSplitCluster;
			Cluster* pNewClusterB = new Cluster();
			int vertex_number_before_split = pSplitCluster->_cluster.size();
			int vertex_number_of_a_cluster = vertex_number_before_split/2;

			Log::log("create splited cluster... \n");
			for(int i=vertex_number_of_a_cluster; i<vertex_number_before_split; i++)
			{
				AppendClusterNode(pNewClusterB,pSplitCluster->_cluster.at(i));
				map<int, int>::iterator iter_cluster_node_idx = 
					pNewClusterA->_cluster_node_idx.find(pSplitCluster->_cluster.at(i)._pos);
				pNewClusterA->_cluster_node_idx.erase(iter_cluster_node_idx);
			}
			vector<ClusterNode>::iterator iter = pNewClusterA->_cluster.begin();
			pNewClusterA->_cluster.erase(iter+vertex_number_of_a_cluster, iter+vertex_number_before_split);

			Log::log("do partitioning...\n");
			//partitioning the splitted cluster
			int c = doKLPartition(pNewClusterA, pNewClusterB);
			exchange_count += c;
			//push the clusters
			cluster_to_split.push(pNewClusterA);
			cluster_to_split.push(pNewClusterB);
		}

		//update the corresponding leafs
		int level_delta = BT_height - node_level;
		int size = (int)pow(2.0f, level_delta);
		int start_leaf = bt_node*size-_k; //the leftest node (leaf) in the sub-tree, begin with 0
		for(int i= start_leaf; i<2*size; i++)//注意：2*size，每个子树的叶子数为size
		{
			_aPartition[i] = cluster_to_split.front();
			cluster_to_split.pop();
		}
	}
	GetStatistic()->AppendPartitionExchangeCount(exchange_count);
}

void Partitioner::RepartitionMaxMin(vector<ReAdjustPartitionPair>& adjust_partitions)
{
}

Cluster* Partitioner::MergeLeafofNode(int bt_node)
{
	Cluster* merge_cluster = new Cluster();
	int BT_nodes = 2*_k-1; //complete binary tree
	int BT_height = int(log(float(_k))/log(2.0f))+1;
	int node_level = int(log(float(bt_node))/log(2.0f))+1;
	int level_delta = BT_height - node_level;
	int size = (int)pow(2.0f, level_delta);
	int start_leaf = bt_node*size-_k; //the leftest node (leaf) in the sub-tree, begin with 0
	for(int i = start_leaf; i<start_leaf+size; i++)
	{
		for(int j=0; j<_aPartition[i]->_cluster.size(); j++)
		{
			AppendClusterNode(merge_cluster, _aPartition[i]->_cluster[j]);
		}
		delete _aPartition[i];
		_aPartition[i] = NULL;
	}
	return merge_cluster;
}

void Partitioner::RemoveClusterNode(hash_set<VERTEX>& vexs)
{
	for(hash_set<VERTEX>::iterator iter_v = vexs.begin(); iter_v!=vexs.end(); iter_v++)
	{
		VERTEX v = *iter_v;
		int k = GetClusterLabelOfVex(v);
		if( k == -1)
		{
			Log::logln("Partitioner:RemoveClusterNode: the cluster of vex not found : NOTE: the process will not be terminated, you should check it");
		}
		Cluster* cluster = _aPartition.at(k);
		int pos_in_sample = _graph->GetVertexPos(v); //TODO:检查一下是否修改sample graph邻接表，如真正删除顶点
		if( pos_in_sample == -1)
		{
			Log::logln("Partitioner:RemoveClusterNode: the pos of vex in the sample graph not found : NOTE: the process will not be terminated, you should check it");
		}
		ClusterNode node;
		node._pos = pos_in_sample;
		int pos_in_cluster = GetClusterNode(cluster, node);
		if( pos_in_cluster == -1)
		{
			Log::logln("Partitioner:RemoveClusterNode: the pos of vex in the cluster not found : NOTE: the process will not be terminated, you should check it");
		}
		DeleteClusterNodeAtPos(cluster, pos_in_cluster);
	}
}

void Partitioner::RandomInsertNewVertices(hash_set<VERTEX>& vexs, map<VERTEX, int>& partitions_changed_vertex)
{
	for(hash_set<VERTEX>::iterator iter_v = vexs.begin(); iter_v!=vexs.end(); iter_v++)
	{
		VERTEX vex = *iter_v;
		int min = INT_MAX;
		Cluster* min_size_cluster;
		int cluster_id = -1; int i=0;
		for(Partition::iterator iter_cluster = _aPartition.begin(); iter_cluster != _aPartition.end(); iter_cluster++)
		{
			Cluster* cluster = *iter_cluster;
			if(cluster->_cluster.size() <= min )
			{
				min_size_cluster = cluster;
				min = cluster->_cluster.size();
				cluster_id = i;
			}
			i++;
		}
		//some new vertices' partition is -1, so...
		map<VERTEX, int>::iterator iter_changed = partitions_changed_vertex.find(vex);
		if(iter_changed == partitions_changed_vertex.end())
		{
			Log::logln("Partitioner:RandomInsertNewVertices: partitions_changed_vertex didn't contain the vex : NOTE: the process will not be terminated, you should check it");
		}
		else
		{
			iter_changed->second = cluster_id;
		}

		InsertNewVertexInCluster(min_size_cluster, vex);
	}
}

void Partitioner::InsertNewVertexInCluster(Cluster* cluster, VERTEX& vex)
{
	int pos = _graph->GetVertexPos(vex); 
	if( pos == -1)
	{
		Log::logln("Partitioner:InsertNewVertexInCluster: the pos of vex in the sample graph not found : NOTE: the process will not be terminated, you should check it");
	}

	ClusterNode node; 
	node._pos = pos;
	node._visited = 0;

	AppendClusterNode(cluster, node);
}

int Partitioner::GetClusterNodeNumber_Debug()
{
	int total = 0;
	Partition::const_iterator iter = _aPartition.begin();
	while(iter!=_aPartition.end())
	{
		total += (*iter)->_cluster.size();
		if((*iter)->_cluster.size() != (*iter)->_cluster_node_idx.size())
		{
			Log::logln("Partitioner:GetClusterNodeNumber: _cluster.size() != _cluster_node_idx.size()");
		}
		iter++;
	}
	return total;
}

void Partitioner::InitPartitionerOutFile()
{
	stringstream str;
	for(int i=0; i<_k; i++)
	{
		str.str("");
		str<<_outfile<<"_cluster_edges."<<i;
		ofstream ofs1(str.str(), ofstream::trunc);
		ofs1.close();
		str.str("");
		str<<_outfile<<"_cluster_vertices."<<i;
		ofstream ofs2(str.str(), ofstream::trunc);
		ofs2.close();

		str.str("");
		str<<_outfile<<"_assign_edge."<<i;
		ofstream ofs3(str.str(), ofstream::trunc);
		ofs3.close();
		str.str("");
		str<<_outfile<<"_assign_edge_tmp."<<i;
		ofstream ofs4(str.str(), ofstream::trunc);
		ofs4.close();
		str.str("");
		str<<_outfile<<"_assign_vertices."<<i;
		ofstream ofs5(str.str(), ofstream::trunc);
		ofs5.close();
	}
}