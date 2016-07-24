#include "stdafx.h"
#include "Graph.h"
#include <stack>
#include "Log.h"
#include <sstream>

EdgeID MakeEdgeID(VERTEX u, VERTEX v)
{
	VERTEX _min_vex = u<v?u:v;
	VERTEX _max_vex = u>v?u:v;
	return (EdgeID)((EdgeID(_min_vex) << 32) | (EdgeID(_max_vex) & 0x00000000FFFFFFFF));
}

EDGE GetEdgeofID(EdgeID eid)
{
	VERTEX _min_vex = (VERTEX)((eid&0xFFFFFFFF00000000)>> 32);
	VERTEX _max_vex = (VERTEX)(eid & 0x00000000FFFFFFFF);
	EDGE e = {_min_vex,  _max_vex};
	return e;
}

void Graph::ClearGraph()
{
	_graph_data._vex_table.clear();
	_graph_data._vertex_to_pos_idx.clear();
	_removed_vex_list.clear();
}

int Graph::GetEdgesNumber()
{
	int e_num = 0;
	for(int i=0; i<_graph_data._vex_table.size(); i++)
	{
		VertexInfo v_info = _graph_data._vex_table.at(i);
		if(v_info._indicator != NORM)
			continue;

		EdgeInfoArray& edges = v_info._edge_list;
		e_num+=edges.size();
	}
	return e_num/2;
}

EdgeInfoArray* Graph::GetAdjEdgeListofVertex(VERTEX& u)
{
	int pos = GetVertexPos(u);
	if(pos == -1)
		return NULL;
	else
		return GetAdjEdgeListofPos(pos);
}

EdgeInfoArray* Graph::GetAdjEdgeListofPos(int pos)
{
	if(pos < 0 || _graph_data._vex_table.at(pos)._indicator != NORM)
		return NULL;
	else
		return &(GetAdjTableRef()->_vex_table.at(pos)._edge_list);
}

VertexInfo* Graph::GetVertexInfoofVertex(VERTEX& u)
{
	int u_pos = GetVertexPos(u);
	if(u_pos == -1) 
		return NULL;
	else
		return GetVertexInfoofPos(u_pos);
}

VertexInfo* Graph::GetVertexInfoofPos(int pos)
{
	if(pos!=-1)
	{
		return &(_graph_data._vex_table.at(pos));
	}
	else
	{
		return NULL;
	}

}

int Graph::GetVertexPos(VERTEX& u)
{
	map<int,int>::iterator iter = _graph_data._vertex_to_pos_idx.find(u);
	if(iter!=_graph_data._vertex_to_pos_idx.end())
		return iter->second;
	else
		return -1;
}

bool Graph::isConnectbyPos(int vex1_pos, int vex2_pos)
{
	if((GetVertexInfoofPos(vex1_pos)->_indicator == REMOVED) || (GetVertexInfoofPos(vex2_pos)->_indicator == REMOVED))
		return false;

	EdgeInfoArray* vex1_edges = GetAdjEdgeListofPos(vex1_pos);
	for(EdgeInfoArray::iterator iter = vex1_edges->begin(); iter!=vex1_edges->end(); iter++)
	{
		if(iter->_adj_vex_pos == vex2_pos) return true;
	}
	return false;
}

bool Graph::isConnect(VERTEX v1, VERTEX v2)
{
	int v1_pos = GetVertexPos(v1);
	int v2_pos = GetVertexPos(v2);
	if(v1_pos == -1 || v2_pos == -1)
		return false;
	else
		return isConnectbyPos(v1_pos, v2_pos);
}

int Graph::InsertVertex(VERTEX u)
{	
	VertexInfo v_info;
	v_info._indicator = NORM;
	v_info._u = u;
	v_info._degree = 0;
	v_info._visisted = false;

	int pos = -1;
	RemoveVertexList::iterator iter = _removed_vex_list.begin();
	if(iter!=_removed_vex_list.end())
	{
		pos = *iter; 
		_graph_data._vex_table.at(pos) = v_info;
		_removed_vex_list.erase(iter);
	}
	else
	{
		_graph_data._vex_table.push_back(v_info);
		pos = _graph_data._vex_table.size()-1;
	}
	_graph_data._vertex_to_pos_idx.insert(Int_Pair(u, pos));
	return pos;
}

void Graph::DeleteVertex(VERTEX& u)
{
	int u_pos = GetVertexPos(u);
	if(u_pos == -1) return;

	VertexInfo* v_info = GetVertexInfoofVertex(u);
	
	EdgeInfoArray::iterator iter_e = v_info->_edge_list.begin();
	while(iter_e != v_info->_edge_list.end())
	{
		//对邻接点的边列表进行相应处理
		int adj_pos = (*iter_e)._adj_vex_pos;
		VertexInfo* adj_info = GetVertexInfoofPos(adj_pos);
		if(adj_info->_indicator != NORM)
			continue;

		EdgeInfoArray::iterator iter_adj = adj_info->_edge_list.begin();
		while(iter_adj != adj_info->_edge_list.end() &&
			(*iter_adj)._adj_vex_pos != u_pos)
			iter_adj++;
		if(iter_adj != adj_info->_edge_list.end())//找到，删除
		{
			adj_info->_edge_list.erase(iter_adj);
			adj_info->_degree--;
		}

		iter_e++;
	}

	v_info->_indicator = REMOVED;
	v_info->_degree = -1;
	v_info->_edge_list.clear();
	v_info->_u = -1;
	v_info->_visisted = false;
	
	map<int, int>::iterator iter = _graph_data._vertex_to_pos_idx.find(u);
	_graph_data._vertex_to_pos_idx.erase(iter);

	_removed_vex_list.insert(u_pos);
}

void Graph::InsertEdge(EDGE e)
{
	int u_pos = -1, v_pos = -1;
	u_pos = GetVertexPos(e._u);
	v_pos = GetVertexPos(e._v);
	if(u_pos<0){
		u_pos = InsertVertex(e._u);
	}
	if(v_pos<0){
		v_pos = InsertVertex(e._v);
	}

	if(isConnectbyPos(u_pos, v_pos))//the edge exists
		return;

	EdgeInfo u_adj, v_adj;
	u_adj._adj_vex_pos = v_pos;
	v_adj._adj_vex_pos = u_pos;

	GetVertexInfoofPos(u_pos)->_edge_list.push_back(u_adj);
	GetVertexInfoofPos(u_pos)->_degree++;
	GetVertexInfoofPos(v_pos)->_edge_list.push_back(v_adj);
	GetVertexInfoofPos(v_pos)->_degree++;

}

void Graph::DeleteEdge(EDGE& e)
{
	int u_pos = GetVertexPos(e._u);
	int v_pos = GetVertexPos(e._v);
	EdgeInfoArray* u_edges = GetAdjEdgeListofVertex(e._u);
	EdgeInfoArray* v_edges = GetAdjEdgeListofVertex(e._v);

	for(EdgeInfoArray::iterator iter=u_edges->begin(); iter!=u_edges->end(); iter++)
	{
		if(iter->_adj_vex_pos = v_pos)
		{
			u_edges->erase(iter);
			break;
		}
	}
	for(EdgeInfoArray::iterator iter=v_edges->begin(); iter!=v_edges->end(); iter++)
	{
		if(iter->_adj_vex_pos = u_pos)
		{
			v_edges->erase(iter);
			break;
		}
	}
	
	GetVertexInfoofPos(u_pos)->_degree--;
	GetVertexInfoofPos(v_pos)->_degree--;
}

/*建议不用
this function has some errors I think, the edgelist of vertex taged as REMOVED has been deleted at the function of DeleteVertex
*/
void Graph::RemoveDeletedVertex()
{
	//modify the adj vertex
	for(RemoveVertexList::iterator iter = _removed_vex_list.begin(); iter!=_removed_vex_list.end(); iter++)
	{
		int u_pos = *iter;
		VertexInfo* u_info = GetVertexInfoofPos(u_pos);
		for(EdgeInfoArray::iterator iter_u = u_info->_edge_list.begin(); iter_u!= u_info->_edge_list.end(); iter_u++)
		{
			int v_pos = iter_u->_adj_vex_pos;
			EdgeInfoArray* v_edges = GetAdjEdgeListofPos(v_pos);
			for(EdgeInfoArray::iterator iter_v = v_edges->begin();iter_v!=v_edges->end(); iter_v++)
			{
				if(iter_v->_adj_vex_pos == u_pos)
				{
					v_edges->erase(iter_v);
					break;
				}
			}
			GetVertexInfoofPos(v_pos)->_degree--;
		}
	}
	//hash idx
	VertexInfoArray::iterator iter_vex_array = _graph_data._vex_table.begin();
	RemoveVertexList::iterator iter = _removed_vex_list.begin();
	for(int del_num = 1; iter!=_removed_vex_list.end(); iter++, del_num++)
	{
		int begin = *iter;
		int end;
		if((iter++) == _removed_vex_list.end())
			end = _graph_data._vex_table.size();//?
		else
			end = *iter;

		for(int i= begin; i<end; i++)
		{
			map<int, int>::iterator iter_idx = _graph_data._vertex_to_pos_idx.find((iter_vex_array+i)->_u);
			iter_idx->second -= del_num;
		}
		iter--;
	}
	//remove
	iter = _removed_vex_list.begin();
	for(int i=0; iter!=_removed_vex_list.end(); iter++, i++)
	{
		iter_vex_array = _graph_data._vex_table.begin();
		int del_pos = *iter;
		map<int, int>::iterator iter_idx = _graph_data._vertex_to_pos_idx.find((iter_vex_array+del_pos)->_u);
		_graph_data._vertex_to_pos_idx.erase(iter_idx);
		_graph_data._vex_table.erase(iter_vex_array+del_pos-i);
	}
}

void Graph::BuildGraphFromDir(string& dir)
{
	this->ClearGraph();

	string path = dir.substr(0, dir.find_last_of('\\'));
	_finddata_t file;
	long lf;
	if((lf = _findfirst(dir.c_str(), &file))==-1l)
	{
		return;
	}
	else
	{
		while( _findnext( lf, &file ) == 0 )
		{
			if(file.attrib == _A_NORMAL || file.attrib == _A_ARCH)
			{
				this->BuildGraphFromFile(path+"\\"+string(file.name));
			}
		}
	}
	_findclose(lf);
}

void Graph::BuildGraphFromFile(string& file)
{
	std::ifstream is(file);
	std::string buf;
	while(std::getline(is, buf))
	{
		if(buf.empty()) continue;

		int idx = buf.find_first_of(" ");
		string temp = buf.substr(0, idx);
		int u = stoi(temp);
		temp = buf.substr(idx+1, buf.length()-idx-1);
		int v= stoi(temp);
		EDGE e={u, v};
		this->InsertEdge(e);
	}
}

void Graph::WriteGraphToFileByBFS(string& file)
{
	ofstream os(file);
	queue<int> bfs_queue;
	unordered_set<EdgeID> edges_visited;

	VertexInfoArray* vexs = &(GetAdjTableRef()->_vex_table);
	for(VertexInfoArray::iterator iter_vexs = vexs->begin(); iter_vexs!=vexs->end(); iter_vexs++)
	{
		if(iter_vexs->_visisted)
			continue;

		VERTEX u = iter_vexs->_u;
		int u_pos = GetVertexPos(u);
		bfs_queue.push(u_pos);
		while(bfs_queue.size()>0)
		{
			int u_pos = bfs_queue.front();
			bfs_queue.pop();

			VertexInfo* u_info = GetVertexInfoofPos(u_pos);
			u_info->_visisted = true;
			VERTEX u = u_info->_u;

			EdgeInfoArray* u_edges = &(u_info->_edge_list);
			for(EdgeInfoArray::iterator iter_edges = u_edges->begin(); iter_edges!=u_edges->end(); iter_edges++)
			{
				int v_pos = iter_edges->_adj_vex_pos;
				VertexInfo* v_info = GetVertexInfoofPos(v_pos);
				VERTEX v = v_info->_u;

				bool edge_visited = false;
				EdgeID e_id = MakeEdgeID(u,v);
				unordered_set<EdgeID>::iterator iter_edges_visited = edges_visited.find(e_id);
				if(iter_edges_visited == edges_visited.end())
				{
					edge_visited = false;
					edges_visited.insert(e_id);
				}
				else
				{
					edge_visited = true;
				}

				if(v_info->_visisted)
				{
					
					if(!edge_visited)//to confirm a edge written once
					{
						os<<u<<" "<<v<<endl;
					}
				}
				else
				{
					bfs_queue.push(v_pos);
					os<<u<<" "<<v<<endl;
				}
			}
		}
	}
	os.close();
}

void Graph::WriteGraphToFileByDFS(string& file)
{
	ofstream os(file);
	stack<int> dfs_stack;
	unordered_set<EdgeID> edges_visited;

	VertexInfoArray* vexs = &(GetAdjTableRef()->_vex_table);
	for(VertexInfoArray::iterator iter_vexs = vexs->begin(); iter_vexs!=vexs->end(); iter_vexs++)
	{
		if(iter_vexs->_visisted)
			continue;

		VERTEX u = iter_vexs->_u;
		int u_pos = GetVertexPos(u);
		dfs_stack.push(u_pos);
		while(dfs_stack.size()>0)
		{
			int u_pos = dfs_stack.top();
			VertexInfo* u_info = GetVertexInfoofPos(u_pos);
			u_info->_visisted = true;
			VERTEX u = u_info->_u;
			bool all_adj_vexs_visisted = true;

			EdgeInfoArray* u_edges = &(u_info->_edge_list);
			for(EdgeInfoArray::iterator iter_edges = u_edges->begin(); iter_edges!=u_edges->end(); iter_edges++)
			{
				int v_pos = iter_edges->_adj_vex_pos;
				VertexInfo* v_info = GetVertexInfoofPos(v_pos);
				VERTEX v = v_info->_u;

				bool edge_visited = false;
				EdgeID e_id = MakeEdgeID(u,v);
				unordered_set<EdgeID>::iterator iter_edges_visited = edges_visited.find(e_id);
				if(iter_edges_visited == edges_visited.end())
				{
					edge_visited = false;
					edges_visited.insert(e_id);
				}
				else
				{
					edge_visited = true;
				}
				
				if(v_info->_visisted)
				{
					if(!edge_visited)//to confirm a edge written once
					{
						os<<u<<" "<<v<<endl;
					}
				}
				else
				{
					dfs_stack.push(v_pos);
					os<<u<<" "<<v<<endl;
					all_adj_vexs_visisted = false;
					break;
				}
			}
			if(all_adj_vexs_visisted)
				dfs_stack.pop();
		}
	}
	os.close();
}

void Graph::BuildGraphFromEdgesCache(vector<EDGE>& edges_cache)
{
	vector<EDGE>::const_iterator iter = edges_cache.begin();
	while(iter!=edges_cache.end())
	{
		InsertEdge(*iter);
		iter++;
	}
}

void Graph::BuildGraphFromEdgesCache(map<EdgeID, Edge_Item>& edges_cache)
{
	map<EdgeID, Edge_Item>::iterator iter = edges_cache.begin();
	while(iter!=edges_cache.end())
	{
		EDGE e = GetEdgeofID(iter->first);
		InsertEdge(e);
		iter++;
	}
}

void Graph::ComputeShortestPathsFromVertex(VERTEX u, vector<int>& shortest_path_lens)
{
	shortest_path_lens.clear();
	vector<bool> shortest_path_indicator;

	VertexInfoArray& vex_table = GetAdjTableRef()->_vex_table;
	VertexInfoArray::const_iterator iter_graph  = vex_table.begin();
	while(iter_graph != vex_table.end())
	{
		VERTEX vex = iter_graph->_u;
		if(u == vex)
		{
			shortest_path_lens.push_back(0);
			shortest_path_indicator.push_back(true);
		}
		else if( isConnect(u, vex) ) 
		{
			shortest_path_lens.push_back(1);
			shortest_path_indicator.push_back(false);
		}
		else//节点存在但没有边，或者，节点已被删除
		{
			shortest_path_lens.push_back(INT_MAX);
			shortest_path_indicator.push_back(false);
		}
		iter_graph++;
	}
	
	int vex_num = vex_table.size();
	for(int i=1; i<vex_num; i++)
	{
		int min = INT_MAX;
		int min_pos = 0;
		for(int j=0; j<vex_num; j++)
		{
			if(!shortest_path_indicator.at(j))
			{
				if(shortest_path_lens.at(j)<min)
				{
					min_pos = j; min=shortest_path_lens.at(j);
				}
			}
		}
		
		shortest_path_indicator.at(min_pos) = true;

		for(int k=0; k<vex_num; k++)
		{
			if(!shortest_path_indicator.at(k))
			{
				if(isConnectbyPos(min_pos, k))
				{
					if(min+1<shortest_path_lens.at(k))
					{
						shortest_path_lens.at(k) = min+1;
					}
				}
			}
		}
	}

	return;
}

void Graph::doGraphStatistic()
{
	Log::logln("====================Graph Statistic======================");
	stringstream log_str;
	log_str<<" the number of vertex:\t"<<GetVertexNumber()
		<<"\n the number of edge: \t"<<GetEdgesNumber();
	Log::logln(log_str.str());
	
	log_str.str("");
	map<int, int> degree_distribution;//degree, count
	for(VertexInfoArray::iterator iter_vex = GetAdjTableRef()->_vex_table.begin();
		iter_vex!=GetAdjTableRef()->_vex_table.end();
		iter_vex++)
	{
		int d = iter_vex->_degree;
		map<int, int>::iterator iter_dist = degree_distribution.find(d);
		if(iter_dist == degree_distribution.end())
		{
			degree_distribution.insert(pair<int,int>(d,1));
		}
		else
		{
			iter_dist->second++;
		}
	}

	log_str.str("");
	log_str<<"degree:\t count:";
	Log::logln(log_str.str());
	float total_d=0;
	for(map<int, int>::iterator iter_dist = degree_distribution.begin();
		iter_dist!=degree_distribution.end();
		iter_dist++)
	{
		log_str.str("");
		log_str<<iter_dist->first<<"\t"<<iter_dist->second;
		Log::logln(log_str.str());

		total_d+=iter_dist->first*iter_dist->second;
	}
	log_str.str("");
	log_str<<"avg degree:\t"<<total_d/GetVertexNumber();
	Log::logln(log_str.str());
	Log::logln("====================Graph Statistic======================");
}

void Graph::UpdateSampleGraph(hash_set<EdgeID> add_set, hash_set<EdgeID> delete_set)
{
	hash_set<EdgeID>::iterator iter_add = add_set.begin();
	while(iter_add!=add_set.end())
	{
		EDGE e = GetEdgeofID(*iter_add);
		InsertEdge(e);
		iter_add++;
	}

	hash_set<EdgeID>::iterator iter_del = delete_set.begin();
	while(iter_del!=delete_set.end())
	{
		EDGE e = GetEdgeofID(*iter_del);
		DeleteEdgeAndRemoveVertex(e);
		iter_del++;
	}
	//不用remove。已标记为REMOVE。提高些效率
	//RemoveDeletedVertex();//可能存在bug，见RemoveDeletedVertex注释
}

void Graph::DeleteEdgeAndRemoveVertex(EDGE& e)
{
	DeleteEdge(e);

	int u_pos = GetVertexPos(e._u);
	if(u_pos != -1 && GetVertexInfoofPos(u_pos)->_degree == 0)
		DeleteVertex(e._u);

	int v_pos = GetVertexPos(e._v);
	if(v_pos!=-1 && GetVertexInfoofPos(v_pos)->_degree == 0)
		DeleteVertex(e._v);
}

int Graph::GetExistVertexNumber()
{
	int n = 0;
	for (VertexInfoArray::iterator iter = _graph_data._vex_table.begin(); iter!=_graph_data._vex_table.end(); iter++)
	{
		if(iter->_indicator == NORM)
			n++;
	}
	return n;
}