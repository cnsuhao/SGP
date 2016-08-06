#include "StdAfx.h"
#include <time.h>
#include "Log.h"
#include <sstream>
#include "TimeTicket.h"
#include <hash_set>
#include "StreamPartiton.h"
#include "GraphDisk.h"

using namespace std;
using namespace stdext; //for hash set

StreamPartiton::StreamPartiton(void)
{
}

StreamPartiton::~StreamPartiton(void)
{
}

void StreamPartiton::doStreamPartition(StreamPartitionMeasure measure)
{
	switch(measure)
	{
	case HASH:
		doHashStreamPartition();
		break;
	case BALANCE:
		doBalanceStreamPartition();
		break;
	case DG:
		doDeterministicGreadyStreamPartition();
		break;
	case LDG:
		doLinearWeightedDeterministicGreadyStreamPartition();
		break;
	case EDG:
		doExponentialWeightedDeterministicGreadyStreamPartition();
		break;
	case Tri:
		doTriangleStreamPartition();
		break;
	case LTri:
		doLinearTriangleStreamPartition();
		break;
	case EDTri:
		doExponentDeterministicTriangleStreamPartition();
		break;
	case NN:
		doNonNeighborStreamPartition();
		break;
	case FENNEL:
		doFennelStreamPartition();
		break;
	}
}

bool StreamPartiton::ReadEdge(EDGE& e)
{
	while(!_ifs.eof())//empty line maybe exists
	{	
		VERTEX u,v;
		_ifs.read((char*)&u, sizeof(VERTEX));
		_ifs.read((char*)&v, sizeof(VERTEX));
		e._u = u;
		e._v = v;
		return true;
	}
	return false;
}

int hash_stream_partition_find_partition(hash_set<VERTEX>* partitions, int k, VERTEX u)
{
	for(int i=0; i<k; i++)
	{
		if(partitions[i].find(u) != partitions[i].end())
		{
			return i;
		}
	}
	int c = rand(0, k);
	if(c>=k) c=k-1;
	return c;
}

void StreamPartiton::doHashStreamPartition()
{
	Log::logln("doHashStreamPartition.........");

	hash_set<VERTEX>* partitions = new hash_set<VERTEX>[_k];
	ofstream* ofs = new ofstream[_k];
	stringstream str;
	for(int i=0; i<_k; i++)
	{
		str.str("");
		str<<_outfile<<"_edge."<<i;
		ofs[i].open(str.str(), ios_base::trunc|ios::out|ios::binary);
	}
	_ifs.open(_graph_file, ios::in|ios::binary);

	int iread = 0, interlinks =0, exterlinks=0, vex_num=0;
	EDGE e;
	while(ReadEdge(e))
	{
		int u_partition, v_partition;
		u_partition = hash_stream_partition_find_partition(partitions, _k, e._u);
		partitions[u_partition].insert(e._u);
		v_partition = hash_stream_partition_find_partition(partitions, _k, e._v);
		partitions[v_partition].insert(e._v);

		if(u_partition == v_partition)
		{
			//ofs[u_partition]<<e._u<<" "<<e._v<<endl;
			ofs[u_partition].write((char*)&e._u, sizeof(VERTEX));
			ofs[u_partition].write((char*)&e._v, sizeof(VERTEX));
			interlinks++;
		}
		else
		{
			//ofs[u_partition]<<e._u<<" "<<e._v<<endl;
			//ofs[v_partition]<<e._u<<" "<<e._v<<endl;
			ofs[u_partition].write((char*)&e._u, sizeof(VERTEX));
			ofs[u_partition].write((char*)&e._v, sizeof(VERTEX));
			ofs[v_partition].write((char*)&e._u, sizeof(VERTEX));
			ofs[v_partition].write((char*)&e._v, sizeof(VERTEX));
			exterlinks++;
		}

		iread++;
	}
	//write the vertices
	ofstream* ofs_v = new ofstream;
	for(int i=0; i<_k; i++)
	{
		str.str("");
		str<<_outfile<<"_vertices."<<i;
		ofs_v->open(str.str(), ios_base::trunc|ios_base::out|ios_base::binary);
		for(hash_set<VERTEX>::iterator iter = partitions[i].begin(); iter!=partitions[i].end(); iter++)
		{
			//(*ofs_v)<<*iter<<endl;
			VERTEX u = *iter;
			ofs_v->write((char*)&u, sizeof(VERTEX));
			vex_num++;
		}
		ofs_v->close();
	}
	for(int i=0; i<_k; i++)
	{
		ofs[i].close();
	}

	str.str("");
	str<<"k : \t"<<_k<<"\n"
		<<"Total Cut Value: \t"<<exterlinks<<"\n"
		<<"Total Edges: \t"<<exterlinks+interlinks<<"\n"
		<<"Total Vex: \t"<<vex_num<<"\n"
		<<"Total Elapse : \t"<<TimeTicket::total_elapse()<<"\n"
		<<"Partition size: \t";
	int max_size = 0;
	for(int i=0; i<_k; i++)
	{
		if(partitions[i].size()>max_size) 
			max_size = partitions[i].size();

		str<<partitions[i].size()<<"\t";
	}
	str<<"\nCut/Edges: \t"<<exterlinks*1.0f/(exterlinks+interlinks)<<"\n"
		<<"MaxParitionSize/AvgSize: \t"<<max_size*_k*1.0f/vex_num<<"\n";
	Log::logln(str.str());

	delete[] ofs;
	delete ofs_v;
	_ifs.close();
	delete[] partitions;

}

int balance_stream_partition_find_partition(hash_set<VERTEX>* partitions, int k, VERTEX u)
{
	for(int i=0; i<k; i++)
	{
		if(partitions[i].find(u) != partitions[i].end())
		{
			return i;
		}
	}

	int min_c =-1, min_size = INT_MAX;
	for(int i=0; i<k; i++)
	{
		if(partitions[i].size()<min_size)
		{
			min_c = i;
			min_size = partitions[i].size();
		}
	}
	return min_c;
}

void StreamPartiton::doBalanceStreamPartition()
{
	Log::logln("doBalanceStreamPartition.........");

	hash_set<VERTEX>* partitions = new hash_set<VERTEX>[_k];
	ofstream* ofs = new ofstream[_k];
	stringstream str;
	for(int i=0; i<_k; i++)
	{
		str.str("");
		str<<_outfile<<"_edge."<<i;
		ofs[i].open(str.str(), ios_base::trunc|ios::out|ios::binary);
	}
	_ifs.open(_graph_file, ios::in|ios::binary);

	int iread = 0, interlinks =0, exterlinks=0, vex_num=0;
	EDGE e;
	while(ReadEdge(e))
	{
		int u_partition, v_partition;
		u_partition = balance_stream_partition_find_partition(partitions, _k, e._u);
		partitions[u_partition].insert(e._u);
		v_partition = balance_stream_partition_find_partition(partitions, _k, e._v);
		partitions[v_partition].insert(e._v);

		if(u_partition == v_partition)
		{
			//ofs[u_partition]<<e._u<<" "<<e._v<<endl;
			ofs[u_partition].write((char*)&e._u, sizeof(VERTEX));
			ofs[u_partition].write((char*)&e._v, sizeof(VERTEX));
			interlinks++;
		}
		else
		{
//			ofs[u_partition]<<e._u<<" "<<e._v<<endl;
//			ofs[v_partition]<<e._u<<" "<<e._v<<endl;
			ofs[u_partition].write((char*)&e._u, sizeof(VERTEX));
			ofs[u_partition].write((char*)&e._v, sizeof(VERTEX));
			ofs[v_partition].write((char*)&e._u, sizeof(VERTEX));
			ofs[v_partition].write((char*)&e._v, sizeof(VERTEX));
			exterlinks++;
		}

		iread++;
	}
	//write the vertices
	ofstream* ofs_v = new ofstream;
	for(int i=0; i<_k; i++)
	{
		str.str("");
		str<<_outfile<<"_vertices."<<i;
		ofs_v->open(str.str(), ios_base::trunc|ios::out|ios::binary);
		for(hash_set<VERTEX>::iterator iter = partitions[i].begin(); iter!=partitions[i].end(); iter++)
		{
			//(*ofs_v)<<*iter<<endl;
			VERTEX u = *iter;
			ofs_v->write((char*)&u, sizeof(VERTEX));
			vex_num++;
		}
		ofs_v->close();
	}
	for(int i=0; i<_k; i++)
	{
		ofs[i].close();
	}

	str.str("");
	str<<"k : \t"<<_k<<"\n"
		<<"Total Cut Value: \t"<<exterlinks<<"\n"
		<<"Total Edges: \t"<<exterlinks+interlinks<<"\n"
		<<"Total Vex: \t"<<vex_num<<"\n"
		<<"Total Elapse : \t"<<TimeTicket::total_elapse()<<"\n"
		<<"Partition size: \t";
	int max_size = 0;
	for(int i=0; i<_k; i++)
	{
		if(partitions[i].size()>max_size) 
			max_size = partitions[i].size();

		str<<partitions[i].size()<<"\t";
	}
	str<<"\nCut/Edges: \t"<<exterlinks*1.0f/(exterlinks+interlinks)<<"\n"
		<<"MaxParitionSize/AvgSize: \t"<<max_size*_k*1.0f/vex_num<<"\n";
	Log::logln(str.str());

	delete[] ofs;
	delete ofs_v;
	delete[] partitions;
	_ifs.close();

}

int DeterministicGready_stream_partition_find_partition(hash_set<VERTEX>* partitions, int k, VERTEX u, GraphDisk* graph)
{
	int max_links = 0;
	int max_c = -1;
	gdEdgeInfoList* adj_edges = graph->GetAdjEdgeListofVex(u);
	for(int i=0; i<k; i++)
	{
		int links = 0;
		for(gdEdgeInfoList::iterator iter = adj_edges->begin(); iter!= adj_edges->end(); iter++)
		{
			int adj_pos = iter->_adj_vex_pos;
			VERTEX adj_u = graph->GetVertexAtPos(adj_pos);
			if(partitions[i].find(adj_u)!=partitions[i].end())
			{
				links++;
			}
		}
		if(links>max_links)
		{
			max_links = links;
			max_c = i;
		}
	}
	if(max_c == -1)
	{
		max_c = rand(0, k-1);
	}

	graph->UnLockVertex(u);
	return max_c;
}

int find_vex_partition(hash_set<VERTEX>* partitions, int k, VERTEX u)
{
	for(int i=0; i<k; i++)
	{
		if(partitions[i].find(u)!=partitions[i].end())
		{
			return i;
		}
	}
	return -1;
}

void write_partitions(hash_set<VERTEX>* partitions, int k, GraphDisk* graph, string& outfile, int& interlinks, int& exterlinks)
{
	ofstream* ofs = new ofstream[k];
	stringstream str;
	for(int i=0; i<k; i++)
	{
		str.str("");
		str<<outfile<<"_edge."<<i;
		ofs[i].open(str.str(), ios_base::trunc|ios::out|ios::binary);
	}

	//write edges
	VertexInfoList* vex_list = graph->GetVertexList();
	for(int i=0;i<graph->GetVertexNum();i++)
	{
		VERTEX u = vex_list->at(i)._u;
		gdEdgeInfoList* adj_edges = graph->GetAdjEdgeListofVex(u);
		for(gdEdgeInfoList::iterator iter = adj_edges->begin(); iter!= adj_edges->end(); iter++)
		{
			int adj_pos = iter->_adj_vex_pos;
			VERTEX adj_u = graph->GetVertexAtPos(adj_pos);
			if(u < adj_u)//to sure the edge processed once
			{
				int u_c = find_vex_partition(partitions, k, u);
				int adj_u_c = find_vex_partition(partitions, k, adj_u);
				if(u_c ==-1 || adj_u_c == -1)
				{
					Log::logln("Error::StreamPartiton::compute_stat::a vex not found in the partitions");
				}

				if(u_c == adj_u_c)
				{
					//ofs[u_c]<<u<<" "<<adj_u<<endl;
					ofs[u_c].write((char*)&u, sizeof(VERTEX));
					ofs[u_c].write((char*)&adj_u, sizeof(VERTEX));
					interlinks++;
				}
				else
				{
					//ofs[u_c]<<u<<" "<<adj_u<<endl;
					ofs[u_c].write((char*)&u, sizeof(VERTEX));
					ofs[u_c].write((char*)&adj_u, sizeof(VERTEX));
					//ofs[adj_u_c]<<u<<" "<<adj_u<<endl;
					ofs[adj_u_c].write((char*)&u, sizeof(VERTEX));
					ofs[adj_u_c].write((char*)&adj_u, sizeof(VERTEX));
					exterlinks++;
				}
			}
		}
		graph->UnLockVertex(u);
	}
	for(int i=0; i<k; i++)
	{
		ofs[i].close();
	}
	delete[] ofs;
	//write vertex
	ofstream* ofs_v = new ofstream;
	for(int i=0; i<k; i++)
	{
		str.str("");
		str<<outfile<<"_vertices."<<i;
		ofs_v->open(str.str(), ios_base::trunc|ios::out|ios::binary);
		for(hash_set<VERTEX>::iterator iter = partitions[i].begin(); iter!=partitions[i].end(); iter++)
		{
			//(*ofs_v)<<*iter<<endl;
			VERTEX u = *iter;
			ofs_v->write((char*)&u, sizeof(VERTEX));
		}
		ofs_v->close();
	}
	delete ofs_v;
}

void StreamPartiton::doDeterministicGreadyStreamPartition()
{
	Log::logln("doDeterministicGreadyStreamPartition.........");
	stringstream str;

	GraphDisk graph;
	graph.SetGraphFile(_graph_file);
	graph.SetMaxDegree(_max_d);
	graph.SetMaxEdges(_max_edges);
	graph.InitAdjTable();
	str.str("");
	str<<_outfile<<"_cachetmp.bin";
	graph.InitTempFile(str.str(), TEMPFILE_MAX_SIZE);
	graph.BuildAdjTable();

	VertexInfoList* vex_list = graph.GetVertexList();
	//init partitions
	hash_set<VERTEX>* partitions = new hash_set<VERTEX>[_k];
	int i=0;
	for(i=0; i<_k; i++)
	{
		VERTEX u = vex_list->at(i)._u;
		partitions[i].insert(u);
	}
	//do parititioning
	for(;i<graph.GetVertexNum();i++)
	{
		VERTEX u = vex_list->at(i)._u;
		int c = DeterministicGready_stream_partition_find_partition(partitions, _k, u, &graph);
		partitions[c].insert(u);
	}
	//compute cut
	int interlinks =0, exterlinks=0;
	write_partitions(partitions, _k, &graph, _outfile, interlinks, exterlinks);
	
	str.str("");
	str<<"k : \t"<<_k<<"\n"
		<<"Total Cut Value: \t"<<exterlinks<<"\n"
		<<"Total Edges: \t"<<exterlinks+interlinks<<"\n"
		<<"Total Vex: \t"<<graph.GetVertexNum()<<"\n"
		<<"Total Elapse : \t"<<TimeTicket::total_elapse()<<"\n"
		<<"Partition size: \t";
	int max_size = 0;
	for(int i=0; i<_k; i++)
	{
		if(partitions[i].size()>max_size) 
			max_size = partitions[i].size();

		str<<partitions[i].size()<<"\t";
	}
	str<<"\nCut/Edges: \t"<<exterlinks*1.0f/graph.GetEdgeNum()<<"\n"
		<<"MaxParitionSize/AvgSize: \t"<<max_size*_k*1.0f/graph.GetVertexNum()<<"\n";
	Log::logln(str.str());
	delete[] partitions;
}

int LinearWeightedDeterministicGready_stream_partition_find_partition(hash_set<VERTEX>* partitions, int k, VERTEX u, GraphDisk* graph)
{
	int* links = new int[k];
	gdEdgeInfoList* adj_edges = graph->GetAdjEdgeListofVex(u);
	for(int i=0; i<k; i++)
	{
		links[i] = 0;
		for(gdEdgeInfoList::iterator iter = adj_edges->begin(); iter!= adj_edges->end(); iter++)
		{
			int adj_pos = iter->_adj_vex_pos;
			VERTEX adj_u = graph->GetVertexAtPos(adj_pos);
			if(partitions[i].find(adj_u)!=partitions[i].end())
			{
				links[i]++;
			}
		}
	}
	float max_w = 0; 
	int max_c = -1;
	int n = graph->GetVertexNum();
	for(int i=0; i<k; i++)
	{
		float link = links[i];
		float size = partitions[i].size();
		float mid = 1-size*k/n;
		float w = link*mid;
		if(w > max_w)
		{
			max_w = w;
			max_c = i;
		}
	}
	if(max_c == -1)
	{
		max_c = rand(0, k-1);
	}

	graph->UnLockVertex(u);
	delete[] links;
	return max_c;
}

void StreamPartiton::doLinearWeightedDeterministicGreadyStreamPartition()
{
	Log::logln("doLinearWeightedDeterministicGreadyStreamPartition.........");
	stringstream str;

	GraphDisk graph;
	graph.SetGraphFile(_graph_file);
	graph.SetMaxDegree(_max_d);
	graph.SetMaxEdges(_max_edges);
	graph.InitAdjTable();
	str.str("");
	str<<_outfile<<"_cachetmp.bin";
	graph.InitTempFile(str.str(), TEMPFILE_MAX_SIZE);
	graph.BuildAdjTable();

	VertexInfoList* vex_list = graph.GetVertexList();
	//init partitions
	hash_set<VERTEX>* partitions = new hash_set<VERTEX> [_k];
	int i=0;
	for(i=0; i<_k; i++)
	{
		VERTEX u = vex_list->at(i)._u;
		partitions[i].insert(u);
	}
	//do parititioning
	for(;i<graph.GetVertexNum();i++)
	{
		VERTEX u = vex_list->at(i)._u;
		int c = LinearWeightedDeterministicGready_stream_partition_find_partition(partitions, _k, u, &graph);
		partitions[c].insert(u);
	}
	//compute cut
	int interlinks =0, exterlinks=0;
	write_partitions(partitions, _k, &graph, _outfile, interlinks, exterlinks);

	str.str("");
	str<<"k : \t"<<_k<<"\n"
		<<"Total Cut Value: \t"<<exterlinks<<"\n"
		<<"Total Edges: \t"<<exterlinks+interlinks<<"\n"
		<<"Total Vex: \t"<<graph.GetVertexNum()<<"\n"
		<<"Total Elapse : \t"<<TimeTicket::total_elapse()<<"\n"
		<<"Partition size: \t";
	int max_size = 0;
	for(int i=0; i<_k; i++)
	{
		if(partitions[i].size()>max_size) 
			max_size = partitions[i].size();

		str<<partitions[i].size()<<"\t";
	}
	str<<"\nCut/Edges: \t"<<exterlinks*1.0f/graph.GetEdgeNum()<<"\n"
		<<"MaxParitionSize/AvgSize: \t"<<max_size*_k*1.0f/graph.GetVertexNum()<<"\n";
	Log::logln(str.str());
	delete[] partitions;
}

int ExponentialWeightedDeterministicGready_stream_partition_find_partition(hash_set<VERTEX>* partitions, int k, VERTEX u, GraphDisk* graph)
{
	int* links = new int[k];
	gdEdgeInfoList* adj_edges = graph->GetAdjEdgeListofVex(u);
	for(int i=0; i<k; i++)
	{
		links[i] = 0;
		for(gdEdgeInfoList::iterator iter = adj_edges->begin(); iter!= adj_edges->end(); iter++)
		{
			int adj_pos = iter->_adj_vex_pos;
			VERTEX adj_u = graph->GetVertexAtPos(adj_pos);
			if(partitions[i].find(adj_u)!=partitions[i].end())
			{
				links[i]++;
			}
		}
	}
	float max_w = 0; 
	int max_c = -1;
	int n = graph->GetVertexNum();
	for(int i=0; i<k; i++)
	{
		float w = links[i]*(1-exp(partitions[i].size()-n*1.0f/k));
		if(w > max_w)
		{
			max_w = w;
			max_c = i;
		}
	}
	if(max_c == -1)
	{
		max_c = rand(0, k-1);
	}

	graph->UnLockVertex(u);
	delete[] links;
	return max_c;
}

void StreamPartiton::doExponentialWeightedDeterministicGreadyStreamPartition()
{
	Log::logln("doExponentialWeightedDeterministicGreadyStreamPartition.........");
	stringstream str;

	GraphDisk graph;
	graph.SetGraphFile(_graph_file);
	graph.SetMaxDegree(_max_d);
	graph.SetMaxEdges(_max_edges);
	graph.InitAdjTable();
	str.str("");
	str<<_outfile<<"_cachetmp.bin";
	graph.InitTempFile(str.str(), TEMPFILE_MAX_SIZE);
	graph.BuildAdjTable();

	VertexInfoList* vex_list = graph.GetVertexList();
	//init partitions
	hash_set<VERTEX>* partitions = new hash_set<VERTEX>[_k];
	int i=0;
	for(i=0; i<_k; i++)
	{
		VERTEX u = vex_list->at(i)._u;
		partitions[i].insert(u);
	}
	//do parititioning
	for(;i<graph.GetVertexNum();i++)
	{
		VERTEX u = vex_list->at(i)._u;
		int c = ExponentialWeightedDeterministicGready_stream_partition_find_partition(partitions, _k, u, &graph);
		partitions[c].insert(u);
	}
	//compute cut
	int interlinks =0, exterlinks=0;
	write_partitions(partitions, _k, &graph, _outfile, interlinks, exterlinks);

	str.str("");
	str<<"k : \t"<<_k<<"\n"
		<<"Total Cut Value: \t"<<exterlinks<<"\n"
		<<"Total Edges: \t"<<exterlinks+interlinks<<"\n"
		<<"Total Vex: \t"<<graph.GetVertexNum()<<"\n"
		<<"Total Elapse : \t"<<TimeTicket::total_elapse()<<"\n"
		<<"Partition size: \t";
	int max_size = 0;
	for(int i=0; i<_k; i++)
	{
		if(partitions[i].size()>max_size) 
			max_size = partitions[i].size();

		str<<partitions[i].size()<<"\t";
	}
	str<<"\nCut/Edges: \t"<<exterlinks*1.0f/graph.GetEdgeNum()<<"\n"
		<<"MaxParitionSize/AvgSize: \t"<<max_size*_k*1.0f/graph.GetVertexNum()<<"\n";
	Log::logln(str.str());
	delete[] partitions;
}

int Triangle_stream_partition_find_partition(hash_set<VERTEX>* partitions, int k, VERTEX u, GraphDisk* graph)
{
	int* links = new int[k];
	hash_set<VERTEX>* intersect_vex = new hash_set<VERTEX>[k];

	gdEdgeInfoList* adj_edges = graph->GetAdjEdgeListofVex(u);
	for(int i=0; i<k; i++)
	{
		links[i] = 0;
		for(gdEdgeInfoList::iterator iter = adj_edges->begin(); iter!= adj_edges->end(); iter++)
		{
			int adj_pos = iter->_adj_vex_pos;
			VERTEX adj_u = graph->GetVertexAtPos(adj_pos);
			if(partitions[i].find(adj_u)!=partitions[i].end())
			{
				links[i]++;
				intersect_vex[i].insert(adj_u);
			}
		}
	}
	graph->UnLockVertex(u);

	float max_w = 0; 
	int max_c = -1;
	int n = graph->GetVertexNum();

	for(int i=0; i<k; i++)
	{
		//compute weight
		float w;
		int triangle = 0;
		for(hash_set<VERTEX>::iterator iter_u = intersect_vex[i].begin(); iter_u!=intersect_vex[i].end(); iter_u++)
		{
			VERTEX u = *iter_u;
			for(hash_set<VERTEX>::iterator iter_v = intersect_vex[i].begin(); iter_v!=intersect_vex[i].end(); iter_v++)
			{
				VERTEX v = *iter_v;
				if(u<v)
				{
					if(graph->isConnectbyVex(u, v))
					{
						triangle++;
					}

				}
			}
		}
		w = 1.0f*triangle/(n*(n-1));
		//find max weight
		if(w > max_w)
		{
			max_w = w;
			max_c = i;
		}
	}
	if(max_c == -1)
	{
		max_c = rand(0, k-1);
	}

	delete[] links;
	delete[] intersect_vex;
	return max_c;
}

void StreamPartiton::doTriangleStreamPartition()
{
	Log::logln("doTriangleStreamPartition.........");
	stringstream str;

	GraphDisk graph;
	graph.SetGraphFile(_graph_file);
	graph.SetMaxDegree(_max_d);
	graph.SetMaxEdges(_max_edges);
	graph.InitAdjTable();
	str.str("");
	str<<_outfile<<"_cachetmp.bin";
	graph.InitTempFile(str.str(), TEMPFILE_MAX_SIZE);
	graph.BuildAdjTable();

	VertexInfoList* vex_list = graph.GetVertexList();
	//init partitions
	hash_set<VERTEX>* partitions = new hash_set<VERTEX>[_k];
	int i=0;
	for(i=0; i<_k; i++)
	{
		VERTEX u = vex_list->at(i)._u;
		partitions[i].insert(u);
	}
	//do parititioning
	for(;i<graph.GetVertexNum();i++)
	{
		VERTEX u = vex_list->at(i)._u;
		int c = Triangle_stream_partition_find_partition(partitions, _k, u, &graph);
		partitions[c].insert(u);
	}
	//compute cut
	int interlinks =0, exterlinks=0;
	write_partitions(partitions, _k, &graph, _outfile, interlinks, exterlinks);

	

	str.str("");
	str<<"k : \t"<<_k<<"\n"
		<<"Total Cut Value: \t"<<exterlinks<<"\n"
		<<"Total Edges: \t"<<exterlinks+interlinks<<"\n"
		<<"Total Vex: \t"<<graph.GetVertexNum()<<"\n"
		<<"Total Elapse : \t"<<TimeTicket::total_elapse()<<"\n"
		<<"Partition size: \t";
	int max_size = 0;
	for(int i=0; i<_k; i++)
	{
		if(partitions[i].size()>max_size) 
			max_size = partitions[i].size();

		str<<partitions[i].size()<<"\t";
	}
	str<<"\nCut/Edges: \t"<<exterlinks*1.0f/graph.GetEdgeNum()<<"\n"
		<<"MaxParitionSize/AvgSize: \t"<<max_size*_k*1.0f/graph.GetVertexNum()<<"\n";
	Log::logln(str.str());
	delete[] partitions;
}

int LinearTriangle_stream_partition_find_partition(hash_set<VERTEX>* partitions, int k, VERTEX u, GraphDisk* graph)
{
	int* links = new int[k];
	hash_set<VERTEX>* intersect_vex = new hash_set<VERTEX>[k];

	gdEdgeInfoList* adj_edges = graph->GetAdjEdgeListofVex(u);
	for(int i=0; i<k; i++)
	{
		links[i] = 0;
		for(gdEdgeInfoList::iterator iter = adj_edges->begin(); iter!= adj_edges->end(); iter++)
		{
			int adj_pos = iter->_adj_vex_pos;
			VERTEX adj_u = graph->GetVertexAtPos(adj_pos);
			if(partitions[i].find(adj_u)!=partitions[i].end())
			{
				links[i]++;
				intersect_vex[i].insert(adj_u);
			}
		}
	}
	
	float max_w = 0; 
	int max_c = -1;
	int n = graph->GetVertexNum();

	for(int i=0; i<k; i++)
	{
		//compute weight
		float w;
		int triangle = 0;
		for(hash_set<VERTEX>::iterator iter_u = intersect_vex[i].begin(); iter_u!=intersect_vex[i].end(); iter_u++)
		{
			VERTEX u = *iter_u;
			for(hash_set<VERTEX>::iterator iter_v = intersect_vex[i].begin(); iter_v!=intersect_vex[i].end(); iter_v++)
			{
				VERTEX v = *iter_v;
				if(u<v)
				{
					if(graph->isConnectbyVex(u, v))
					{
						triangle++;
					}

				}
			}
		}
		w = (1-partitions[i].size()*k*1.0f/n)*triangle/(n*(n-1));
		//find max weight
		if(w > max_w)
		{
			max_w = w;
			max_c = i;
		}
	}
	if(max_c == -1)
	{
		max_c = rand(0, k-1);
	}

	graph->UnLockVertex(u);
	delete[] links;
	delete[] intersect_vex;
	return max_c;
}

void StreamPartiton::doLinearTriangleStreamPartition()
{
	Log::logln("doLinearTriangleStreamPartition.........");
	stringstream str;

	GraphDisk graph;
	graph.SetGraphFile(_graph_file);
	graph.SetMaxDegree(_max_d);
	graph.SetMaxEdges(_max_edges);
	graph.InitAdjTable();
	str.str("");
	str<<_outfile<<"_cachetmp.bin";
	graph.InitTempFile(str.str(), TEMPFILE_MAX_SIZE);
	graph.BuildAdjTable();

	VertexInfoList* vex_list = graph.GetVertexList();
	//init partitions
	hash_set<VERTEX>* partitions = new hash_set<VERTEX>[_k];
	int i=0;
	for(i=0; i<_k; i++)
	{
		VERTEX u = vex_list->at(i)._u;
		partitions[i].insert(u);
	}
	//do parititioning
	for(;i<graph.GetVertexNum();i++)
	{
		VERTEX u = vex_list->at(i)._u;
		int c = LinearTriangle_stream_partition_find_partition(partitions, _k, u, &graph);
		partitions[c].insert(u);
	}
	//compute cut
	int interlinks =0, exterlinks=0;
	write_partitions(partitions, _k, &graph, _outfile, interlinks, exterlinks);

	str.str("");
	str<<"k : \t"<<_k<<"\n"
		<<"Total Cut Value: \t"<<exterlinks<<"\n"
		<<"Total Edges: \t"<<exterlinks+interlinks<<"\n"
		<<"Total Vex: \t"<<graph.GetVertexNum()<<"\n"
		<<"Total Elapse : \t"<<TimeTicket::total_elapse()<<"\n"
		<<"Partition size: \t";
	int max_size = 0;
	for(int i=0; i<_k; i++)
	{
		if(partitions[i].size()>max_size) 
			max_size = partitions[i].size();

		str<<partitions[i].size()<<"\t";
	}
	str<<"\nCut/Edges: \t"<<exterlinks*1.0f/graph.GetEdgeNum()<<"\n"
		<<"MaxParitionSize/AvgSize: \t"<<max_size*_k*1.0f/graph.GetVertexNum()<<"\n";
	Log::logln(str.str());
	delete[] partitions;
}

int ExponentDeterministic_stream_partition_find_partition(hash_set<VERTEX>* partitions, int k, VERTEX u, GraphDisk* graph)
{
	int* links = new int[k];
	hash_set<VERTEX>* intersect_vex = new hash_set<VERTEX>[k];

	gdEdgeInfoList* adj_edges = graph->GetAdjEdgeListofVex(u);
	for(int i=0; i<k; i++)
	{
		links[i] = 0;
		for(gdEdgeInfoList::iterator iter = adj_edges->begin(); iter!= adj_edges->end(); iter++)
		{
			int adj_pos = iter->_adj_vex_pos;
			VERTEX adj_u = graph->GetVertexAtPos(adj_pos);
			if(partitions[i].find(adj_u)!=partitions[i].end())
			{
				links[i]++;
				intersect_vex[i].insert(adj_u);
			}
		}
	}
	
	float max_w = 0; 
	int max_c = -1;
	int n = graph->GetVertexNum();

	for(int i=0; i<k; i++)
	{
		//compute weight
		float w;
		int triangle = 0;
		for(hash_set<VERTEX>::iterator iter_u = intersect_vex[i].begin(); iter_u!=intersect_vex[i].end(); iter_u++)
		{
			VERTEX u = *iter_u;
			for(hash_set<VERTEX>::iterator iter_v = intersect_vex[i].begin(); iter_v!=intersect_vex[i].end(); iter_v++)
			{
				VERTEX v = *iter_v;
				if(u<v)
				{
					if(graph->isConnectbyVex(u, v))
					{
						triangle++;
					}

				}
			}
		}
		w = (1-exp(partitions[i].size()-n*1.0f/k))*triangle/(n*(n-1));
		//find max weight
		if(w > max_w)
		{
			max_w = w;
			max_c = i;
		}
	}
	if(max_c == -1)
	{
		max_c = rand(0, k-1);
	}

	graph->UnLockVertex(u);
	delete[] links;
	delete[] intersect_vex;
	return max_c;
}

void StreamPartiton::doExponentDeterministicTriangleStreamPartition()
{
	Log::logln("doExponentDeterministicTriangleStreamPartition.........");
	stringstream str;

	GraphDisk graph;
	graph.SetGraphFile(_graph_file);
	graph.SetMaxDegree(_max_d);
	graph.SetMaxEdges(_max_edges);
	graph.InitAdjTable();
	str.str("");
	str<<_outfile<<"_cachetmp.bin";
	graph.InitTempFile(str.str(), TEMPFILE_MAX_SIZE);
	graph.BuildAdjTable();

	VertexInfoList* vex_list = graph.GetVertexList();
	//init partitions
	hash_set<VERTEX>* partitions = new hash_set<VERTEX>[_k];
	int i=0;
	for(i=0; i<_k; i++)
	{
		VERTEX u = vex_list->at(i)._u;
		partitions[i].insert(u);
	}
	//do parititioning
	for(;i<graph.GetVertexNum();i++)
	{
		VERTEX u = vex_list->at(i)._u;
		int c = ExponentDeterministic_stream_partition_find_partition(partitions, _k, u, &graph);
		partitions[c].insert(u);
	}
	//compute cut
	int interlinks =0, exterlinks=0;
	write_partitions(partitions, _k, &graph, _outfile, interlinks, exterlinks);

	str.str("");
	str<<"k : \t"<<_k<<"\n"
		<<"Total Cut Value: \t"<<exterlinks<<"\n"
		<<"Total Edges: \t"<<exterlinks+interlinks<<"\n"
		<<"Total Vex: \t"<<graph.GetVertexNum()<<"\n"
		<<"Total Elapse : \t"<<TimeTicket::total_elapse()<<"\n"
		<<"Partition size: \t";
	int max_size = 0;
	for(int i=0; i<_k; i++)
	{
		if(partitions[i].size()>max_size) 
			max_size = partitions[i].size();

		str<<partitions[i].size()<<"\t";
	}
	str<<"\nCut/Edges: \t"<<exterlinks*1.0f/graph.GetEdgeNum()<<"\n"
		<<"MaxParitionSize/AvgSize: \t"<<max_size*_k*1.0f/graph.GetVertexNum()<<"\n";
	Log::logln(str.str());
	delete[] partitions;
}

int NonNeighbor_stream_partition_find_partition(hash_set<VERTEX>* partitions, int k, VERTEX u, GraphDisk* graph)
{
	int* links = new int[k];
	gdEdgeInfoList* adj_edges = graph->GetAdjEdgeListofVex(u);
	for(int i=0; i<k; i++)
	{
		links[i] = 0;
		for(gdEdgeInfoList::iterator iter = adj_edges->begin(); iter!= adj_edges->end(); iter++)
		{
			int adj_pos = iter->_adj_vex_pos;
			VERTEX adj_u = graph->GetVertexAtPos(adj_pos);
			if(partitions[i].find(adj_u)!=partitions[i].end())
			{
				links[i]++;
			}
		}
	}
	float min_w = INT_MAX; 
	int min_c = -1;
	for(int i=0; i<k; i++)
	{
		float w = partitions[i].size()-links[i];
		if(w < min_w)
		{
			min_w = w;
			min_c = i;
		}
	}
	if(min_c == -1)
	{
		min_c = rand(0, k-1);
	}

	graph->UnLockVertex(u);
	delete[] links;
	return min_c;
}

void StreamPartiton::doNonNeighborStreamPartition()
{
	Log::logln("doNonNeighborStreamPartition.........");
	stringstream str;

	GraphDisk graph;
	graph.SetGraphFile(_graph_file);
	graph.SetMaxDegree(_max_d);
	graph.SetMaxEdges(_max_edges);
	graph.InitAdjTable();
	str.str("");
	str<<_outfile<<"_cachetmp.bin";
	graph.InitTempFile(str.str(), TEMPFILE_MAX_SIZE);
	graph.BuildAdjTable();

	VertexInfoList* vex_list = graph.GetVertexList();
	//init partitions
	hash_set<VERTEX>* partitions = new hash_set<VERTEX>[_k];
	int i=0;
	for(i=0; i<_k; i++)
	{
		VERTEX u = vex_list->at(i)._u;
		partitions[i].insert(u);
	}
	//do parititioning
	for(;i<graph.GetVertexNum();i++)
	{
		VERTEX u = vex_list->at(i)._u;
		int c = NonNeighbor_stream_partition_find_partition(partitions, _k, u, &graph);
		partitions[c].insert(u);
	}
	//compute cut
	int interlinks =0, exterlinks=0;
	write_partitions(partitions, _k, &graph, _outfile, interlinks, exterlinks);

	str.str("");
	str<<"k : \t"<<_k<<"\n"
		<<"Total Cut Value: \t"<<exterlinks<<"\n"
		<<"Total Edges: \t"<<exterlinks+interlinks<<"\n"
		<<"Total Vex: \t"<<graph.GetVertexNum()<<"\n"
		<<"Total Elapse : \t"<<TimeTicket::total_elapse()<<"\n"
		<<"Partition size: \t";
	for(int i=0; i<_k; i++)
	{
		str<<partitions[i].size()<<"\t";
	}
	Log::logln(str.str());
	delete[] partitions;
}

int Fennel_stream_partition_find_partition(hash_set<VERTEX>* partitions, int k, VERTEX u, GraphDisk* graph)
{
	int* links = new int[k];
	gdEdgeInfoList* adj_edges = graph->GetAdjEdgeListofVex(u);
	for(int i=0; i<k; i++)
	{
		links[i] = 0;
		for(gdEdgeInfoList::iterator iter = adj_edges->begin(); iter!= adj_edges->end(); iter++)
		{
			int adj_pos = iter->_adj_vex_pos;
			VERTEX adj_u = graph->GetVertexAtPos(adj_pos);
			if(partitions[i].find(adj_u)!=partitions[i].end())
			{
				links[i]++;
			}
		}
	}
	
	float max_w = INT_MIN; 
	int max_c = -1;
	int n = graph->GetVertexNum();
	int m = graph->GetEdgeNum();
	//the follwing parameters are referred to the paper
	float alpha = sqrt((float)k)*m/pow(n,1.5);
	float gamma = 1.5f;
	float nu = 1.1f;

	for(int i=0; i<k; i++)
	{
		//compute weight
		float w =links[i]-alpha*gamma*pow(partitions[i].size(),gamma-1);
		if(w>max_w && partitions[i].size()<nu*n/k)
		{
			max_w = w;
			max_c = i;
		}
	}
	//if(max_c == -1)
	//{
	//	max_c = rand(0, k-1);
	//}

	graph->UnLockVertex(u);
	delete[] links;
	return max_c;
}

void StreamPartiton::doFennelStreamPartition()
{
	Log::logln("doFennelStreamPartition.........");
	stringstream str;

	GraphDisk graph;
	graph.SetGraphFile(_graph_file);
	graph.SetMaxDegree(_max_d);
	graph.SetMaxEdges(_max_edges);
	graph.InitAdjTable();
	str.str("");
	str<<_outfile<<"_cachetmp.bin";
	graph.InitTempFile(str.str(), TEMPFILE_MAX_SIZE);
	graph.BuildAdjTable();

	VertexInfoList* vex_list = graph.GetVertexList();
	//init partitions
	hash_set<VERTEX>* partitions = new hash_set<VERTEX>[_k];
	int i=0;
	for(i=0; i<_k; i++)
	{
		VERTEX u = vex_list->at(i)._u;
		partitions[i].insert(u);
	}
	//do parititioning
	for(;i<graph.GetVertexNum();i++)
	{
		VERTEX u = vex_list->at(i)._u;
		int c = Fennel_stream_partition_find_partition(partitions, _k, u, &graph);
		partitions[c].insert(u);
	}
	//compute cut
	int interlinks =0, exterlinks=0;
	write_partitions(partitions, _k, &graph, _outfile, interlinks, exterlinks);

	str.str("");
	str<<"k : \t"<<_k<<"\n"
		<<"Total Cut Value: \t"<<exterlinks<<"\n"
		<<"Total Edges: \t"<<exterlinks+interlinks<<"\n"
		<<"Total Vex: \t"<<graph.GetVertexNum()<<"\n"
		<<"Total Elapse : \t"<<TimeTicket::total_elapse()<<"\n"
		<<"Partition size: \t";
	int max_size = 0;
	for(int i=0; i<_k; i++)
	{
		if(partitions[i].size()>max_size) 
			max_size = partitions[i].size();

		str<<partitions[i].size()<<"\t";
	}
	str<<"\nCut/Edges: \t"<<exterlinks*1.0f/graph.GetEdgeNum()<<"\n"
		<<"MaxParitionSize/AvgSize: \t"<<max_size*_k*1.0f/graph.GetVertexNum()<<"\n";

	Log::logln(str.str());
	delete[] partitions;

}