#include "StdAfx.h"
#include "SGPLoader.h"
#include <time.h>
#include "Log.h"
#include <sstream>

using namespace std;
using namespace stdext; //for hash set

int rand(int range_min, int range_max)
{
	return (double)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min;
}

float randf(float range_min, float range_max)
{
	return (float)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min;
}

SGPLoader::SGPLoader(void)
{
}

SGPLoader::~SGPLoader(void)
{
}

bool SGPLoader::doGraphSampling()
{
	_ifs.open(_graph_file.c_str());
	_sample_vertex_items.clear();
	_sample_edge_items.clear();

	switch(_sample_mode)
	{
	case FIX_RATIO:
		return doGraphSamplingByFixRatioMode();
	case FIX_MEM_EQ:
		return doGraphSamplingByFixMemEq();
	case FIX_MEM_UNEQ:
		return doGraphSamplingByFixMemUneq();
	case RESERVOIR_DBS:
		return doGraphSamplingByDBS();
	}
	return false;
}

bool SGPLoader::doGraphSamplingByFixMemEq()
{
	_sample_reserior.clear();
	int iread = 0;
	EDGE e;
	while(iread<_edges_limition && ReadEdge(e))
	{
		_sample_reserior.push_back(MakeEdgeID(e._u, e._v));
		iread++;
	}
	if(iread < _edges_limition)
	{
		Log::log("the graph is too small!\n");
		return false;
	}

	srand( (unsigned)time( NULL ) );
	while(ReadEdge(e))
	{
		//if(isEdgeExist(e)) continue;
		iread++;
		int i = rand(1, iread);
		if(i<=_edges_limition)
		{
			int replace = rand(1, _edges_limition);
			_sample_reserior.at(replace) = MakeEdgeID(e._u, e._v);
		}
 	}

	vector<EdgeID>::iterator iter = _sample_reserior.begin();
	Edge_Item e_item = {0,0,0};
	while(iter != _sample_reserior.end())
	{
		_sample_edge_items.insert(pair<EdgeID, Edge_Item>(*iter, e_item));
		iter++;
	}

	_sample_reserior.clear();
	return true;
}

bool SGPLoader::doGraphSamplingByFixRatioMode()
{
	return false;
}

bool SGPLoader::doGraphSamplingByFixMemUneq()
{
	int iread = ReadInitSamples_Uneq();
	if(iread < _edges_limition)
	{
		Log::log("Unequal Sampling: the graph is too small!\n");
		return false;
	}

	if(!InitEdgeWeightInEs())
	{
		Log::log("Unequal Sampling: InitEdgeWeightInEs error!\n");
		return false;
	}

	srand( (unsigned)time( NULL ) );
	EDGE e;
	while(ReadEdge(e))
	{
		if(isEdgeExist(e)) 	continue;

		UpdateVertexWeight_Uneq(e);
		
		int i = rand(1, iread);
		if(i>_edges_limition) continue;

		SelectNewEdge_Uneq(e);
 	}
	return true;
}

int SGPLoader::ReadInitSamples_Uneq()
{
	EDGE e;
	int iread = 0;
	while(iread<_edges_limition && ReadEdge(e))
	{
		if(AppendEdgeSample_Uneq(e))
		{
			iread++;
		}
	}
	return iread;
}

bool SGPLoader::AppendEdgeSample_Uneq(EDGE e)
{
	EdgeID e_id = MakeEdgeID(e._u,e._v);
	if(_sample_edge_items.find(e_id) != _sample_edge_items.end())
		return false; //edge exits

	Edge_Item e_item = {0,0,0};
	_sample_edge_items.insert(pair<EdgeID, Edge_Item>(e_id, e_item));
	UpdateVertexWeight_Uneq(e);
	return true;
}

bool SGPLoader::InitEdgeWeightInEs()
{
	map<EdgeID, Edge_Item>::iterator iter_edges = _sample_edge_items.begin();
	while(iter_edges != _sample_edge_items.end())
	{
		EDGE e = GetEdgeofID(iter_edges->first);

		int min_degree = INT_MAX;
		map<VERTEX, Vertex_Item>::iterator iter_vexs;
		for(int j=0;j<2; j++)
		{
			VERTEX vex = (j==0)?e._u:e._v;
			iter_vexs = _sample_vertex_items.find(vex);
			if(iter_vexs == _sample_vertex_items.end())
			{
				Log::log("Unequal Sampling: get the degree of vertex of edge : error occur!!! ");
				return false;
			}
			else
			{
				if(iter_vexs->second.degree < min_degree)
				{
					min_degree = iter_vexs->second.degree;
				}
			}
		}
		
		iter_edges->second._weight = _max_d - min_degree;
		_sum_weight += iter_edges->second._weight;
		iter_edges++;
	}

	return true;
}

bool SGPLoader::UpdateVertexWeight_Uneq(EDGE e)
{
	map<VERTEX, Vertex_Item>::iterator iter;
	for(int j=0;j<2; j++)
	{
		VERTEX vex = (j==0)?e._u:e._v;
		iter = _sample_vertex_items.find(vex);
		if(iter == _sample_vertex_items.end())
		{
			Vertex_Item v_item = {1};
			_sample_vertex_items.insert(pair<VERTEX, Vertex_Item>(vex, v_item));
		}
		else
		{
			(iter->second).degree++;
		}
	}
	return true;
}

bool SGPLoader::SelectNewEdge_Uneq(EDGE e)
{
	EdgeID e_id = MakeEdgeID(e._u,e._v);
	if(_sample_edge_items.find(e_id) != _sample_edge_items.end())
		return false; //edge exits

	//select a edge to replace
	int r = rand(1, _sum_weight);
	int sum = 0;
	map<EdgeID, Edge_Item>::iterator iter_edges = _sample_edge_items.begin();
	while(iter_edges != _sample_edge_items.end())
	{
		sum += (int)(iter_edges->second._weight);
		if(sum < r)
			iter_edges++;
		else
			break;
	}
	_sum_weight -= (int)(iter_edges->second._weight);
	_sample_edge_items.erase(iter_edges);

	//insert new edge
	int min_d = INT_MAX;
	map<VERTEX, Vertex_Item>::iterator iter;
	for(int j=0;j<2; j++)
	{
		VERTEX vex = (j==0)?e._u:e._v;
		iter = _sample_vertex_items.find(vex);
		if(iter == _sample_vertex_items.end())
		{
			Log::log("Unequal Sampling: SelectNewEdge_Uneq : find the degree of vertex : error!\n");
			return false;
		}
		else
		{
			if(iter->second.degree < min_d)
			{
				min_d = iter->second.degree;
			}
		}
	}
	Edge_Item e_item = {_max_d-min_d,0,0};
	_sample_edge_items.insert(pair<EdgeID, Edge_Item>(e_id, e_item));
	_sum_weight += _max_d-min_d;

	return true;
}

bool SGPLoader::doGraphSamplingByDBS()
{
	_sample_edge_items.clear();
	_sample_vertex_items.clear();
	_dbs_edges_cache_to_process.clear();
	
	//step1 - step 5. read the first \rho edges
	if(ReadInitSamples_DBS() < _edges_limition)
	{
		Log::log("the graph is too small!\n");
		return false;
	}

	//step 6 - step 10 : initalize the item of edge and vertex
	InitSamples_DBS();

	//step11 to step 27 : sampling on \eta edges read until the end of stream.
	while(ReadNextEdgesCache_DBS())
	{
		UpdateWeightofEdgesInEs();
		SamplingEdgeCache();
	}
	return true;
}

int SGPLoader::ReadInitSamples_DBS()
{
	EDGE e;
	int iread = 0;
	while(iread<_edges_limition && ReadEdge(e))
	{
		if(AppendEdgeSample_DBS(e))
		{
			iread++;
		}
	}
	return iread;
}

bool SGPLoader::InitSamples_DBS()
{
	srand((unsigned)time(NULL));
	map<EdgeID, Edge_Item>::iterator iter_edges = _sample_edge_items.begin();
	while(iter_edges != _sample_edge_items.end())
	{
		EDGE e = GetEdgeofID(iter_edges->first);

		int min_degree = INT_MAX;
		map<VERTEX, Vertex_Item>::iterator iter_vexs;
		for(int j=0;j<2; j++)
		{
			VERTEX vex = (j==0)?e._u:e._v;
			iter_vexs = _sample_vertex_items.find(vex);
			if(iter_vexs == _sample_vertex_items.end())
			{
				Log::log("DBS : get the degree of vertex of edge : read first \\rho edges : error occur!!! ");
				return false;
			}
			else
			{
				if(iter_vexs->second.degree < min_degree)
				{
					min_degree = iter_vexs->second.degree;
				}
			}
		}
		
		iter_edges->second._weight = min_degree;
		iter_edges->second._random = randf(0.0f, 1.0f);
		iter_edges->second._key = pow(iter_edges->second._random, 1.0/iter_edges->second._weight);

		iter_edges++;
	}

	return true;
}

bool SGPLoader::ReadNextEdgesCache_DBS()
{
	_dbs_edges_cache_to_process.clear();

	EDGE e;
	int iread = 0;
	while(iread<_edges_cache_limitation && ReadEdge(e))
	{
		//check if e has exists in E_s
		EdgeID e_id = MakeEdgeID(e._u,e._v);
		if(_sample_edge_items.find(e_id) == _sample_edge_items.end())
			continue;

		//save e in the edge cache
		_dbs_edges_cache_to_process.push_back(e);

		//update the degree
		Vertex_Item v_item = {0};
		map<VERTEX, Vertex_Item>::iterator iter;
		for(int j=0;j<2; j++)
		{
			VERTEX vex = (j==0)?e._u:e._v;
			iter = _sample_vertex_items.find(vex);
			if(iter == _sample_vertex_items.end())
			{
				v_item.degree = 1;
				_sample_vertex_items.insert(pair<VERTEX, Vertex_Item>(vex, v_item));
			}
			else
			{
				(iter->second).degree++;
			}
		}
		iread++;
	}

	if(iread > 0 )
		return true;
	else
		return false;
}

void SGPLoader::UpdateWeightofEdgesInEs()
{
	_min_weight = DBL_MAX;
	srand((unsigned)time(NULL));
	map<EdgeID, Edge_Item>::iterator iter_edges = _sample_edge_items.begin();
	while(iter_edges != _sample_edge_items.end())
	{
		EDGE e = GetEdgeofID(iter_edges->first);

		//find the minimum degree of vertices of e
		int min_degree = INT_MAX;
		map<VERTEX, Vertex_Item>::iterator iter_vexs;
		for(int j=0;j<2; j++)
		{
			VERTEX vex = (j==0)?e._u:e._v;
			iter_vexs = _sample_vertex_items.find(vex);
			if(iter_vexs == _sample_vertex_items.end())
			{
				Log::log("DBS : get the degree of vertex of edge : read first \\rho edges : error occur!!! ");
				return;
			}
			else
			{
				if(iter_vexs->second.degree < min_degree)
				{
					min_degree = iter_vexs->second.degree;
				}
			}
		}
		
		if(iter_edges->second._weight != min_degree)
		{
			iter_edges->second._weight = min_degree;
			//don't generate the random number again
			iter_edges->second._key = pow(iter_edges->second._random, 1.0/iter_edges->second._weight);
		}

		//update the minimum key
		if(iter_edges->second._key < _min_weight)
		{
			_min_weight = iter_edges->second._key;
			_min_weight_edge_id = iter_edges->first;
		}

		iter_edges++;
	}
}

bool SGPLoader::SamplingEdgeCache()
{
	vector<EDGE>::iterator iter = _dbs_edges_cache_to_process.begin();
	while(iter != _dbs_edges_cache_to_process.end())
	{
		int min_degree = INT_MAX;
		EDGE e = *iter;
		
		for(int j=0;j<2; j++)
		{
			VERTEX vex = (j==0)?e._u:e._v;
			map<VERTEX, Vertex_Item>::iterator iter_vexs = _sample_vertex_items.find(vex);
			if(iter_vexs == _sample_vertex_items.end())
			{
				Log::log("DBS : get the degree of vertex of edge : read first \\rho edges : error occur!!! ");
				return false;
			}
			else
			{
				if(iter_vexs->second.degree < min_degree)
				{
					min_degree = iter_vexs->second.degree;
				}
			}
		}
		double r = randf(0.0f, 1.0f);
		double key = pow(r, 1.0/min_degree);
		if(key > _min_weight)
		{
			map<EdgeID, Edge_Item>::iterator iter_edges = _sample_edge_items.find(_min_weight_edge_id);
			if(iter_edges == _sample_edge_items.end())
			{
				Log::log("DBS : SamplingEdgeCache : error occur!!! ");
				return false;
			}
			_sample_edge_items.erase(iter_edges);
			Edge_Item e_item = {min_degree, r, key};
			EdgeID e_id = MakeEdgeID(e._u, e._v);
			_sample_edge_items.insert(pair<EdgeID, Edge_Item>(e_id, e_item));

			SearchMinimumKey();//it suffers from low efficiency. to improve it in the future by insert sorting with the index
		}
		iter++;
	}
	return true;
}

bool SGPLoader::AppendEdgeSample_DBS(EDGE e)
{
	EdgeID e_id = MakeEdgeID(e._u,e._v);
	if(_sample_edge_items.find(e_id) != _sample_edge_items.end())
		return false; //edge exits

	Edge_Item e_item = {0,0,0};
	Vertex_Item v_item = {0};

	_sample_edge_items.insert(pair<EdgeID, Edge_Item>(e_id, e_item));

	map<VERTEX, Vertex_Item>::iterator iter;
	for(int j=0;j<2; j++)
	{
		VERTEX vex = (j==0)?e._u:e._v;
		iter = _sample_vertex_items.find(vex);
		if(iter == _sample_vertex_items.end())
		{
			v_item.degree = 1;
			_sample_vertex_items.insert(pair<VERTEX, Vertex_Item>(vex, v_item));
		}
		else
		{
			(iter->second).degree++;
		}
	}
	return true;
}

void SGPLoader::SearchMinimumKey()
{
	_min_weight = DBL_MAX;
	map<EdgeID, Edge_Item>::iterator iter_edges = _sample_edge_items.begin();
	while(iter_edges != _sample_edge_items.end())
	{
		if(iter_edges->second._key < _min_weight)
		{
			_min_weight = iter_edges->second._key;
			_min_weight_edge_id =  iter_edges->first;
		}
		iter_edges++;
	}

}

void SGPLoader::BuildSampleGraph()
{
	_graph_sample.ClearGraph();
	switch(_sample_mode)
	{
	case FIX_RATIO:
	case FIX_MEM_EQ:
	case FIX_MEM_UNEQ:
	case RESERVOIR_DBS:
		_graph_sample.BuildGraphFromEdgesCache(_sample_edge_items);
		break;
	}
}

void SGPLoader::doGraphSamplePartition(PartitionAlgorithm partition_algorithm)
{
	_partitions_in_memory.ClearPartition();
	_partitions_in_memory.SetPartitionNumber(_k);

	BuildSampleGraph();
	_partitions_in_memory.SetGraph(&_graph_sample);

	_graph_sample.doGraphStatistic();

	switch(partition_algorithm)
	{
	case KL:
		_partitions_in_memory.doKL();
		break;
	case MaxMin:
		_partitions_in_memory.doMaxMin();
		break;
	}

}

void SGPLoader::doAssignReminderEdges()
{
	ResetGraphInputStream();
	EDGE e;
	while(ReadEdge(e))
	{
		if(isSampled(e)) continue;

		VERTEX u = e._u, v = e._v;
		int cluster_u, cluster_v;
		cluster_u = _partitions_in_memory.GetClusterLabelOfVex(u);
		if(cluster_u == -1)
			cluster_u = _partitions_in_memory.GetAssignedLabelOfVex(u);
		cluster_v = _partitions_in_memory.GetClusterLabelOfVex(v);
		if(cluster_v == -1)
			cluster_v = _partitions_in_memory.GetAssignedLabelOfVex(v);

		if(cluster_u != -1 && cluster_v != -1)
		{
			_partitions_in_memory.WriteAssignEdge(u, cluster_u, v, cluster_v);
		}
		else
		{
			_assign_manager.CreateAndAppendContext(e, &_partitions_in_memory, &_graph_sample, _assign_win_size);
			_assign_manager.AppendEdge(e);
			_assign_manager.doManager();
		}
	}

	_assign_manager.Flush();
}

void SGPLoader::SetEdgeOrderMode(EdgeOrderMode mode)
{
	this->_edge_order_mode = mode;
}

void SGPLoader::SetGraphFile(string graphfile)
{
	this->_graph_file = graphfile;
}

void SGPLoader::SetK(int k)
{
	this->_k = k;
}

void SGPLoader::SetSampleRation(float ratio)
{
	this->_sample_ratio = ratio;
}

void SGPLoader::SetEdgesLimition(int limit)
{
	this->_edges_limition = limit;
}

void SGPLoader::SetSampleMode(SampleMode mode)
{
	this->_sample_mode = mode;
}

void SGPLoader::ResetGraphInputStream(void)
{
	_ifs.clear();
	_ifs.seekg(0, ios_base::beg);
}

bool SGPLoader::isSampled(EDGE& e)
{
	return isEdgeExist(e);
}

void SGPLoader::doSGPStatistic()
{
	Log::logln("====================Partition Statistic======================");

	int cutvalue = 0;
	stringstream str;
	int total_internal_edges = 0, total_external_edges = 0, total_vex=0;
	vector<PartitionStatisticInfo>& stats = GetPartitioner().GetPartitionStatistic();
	for(int i=0; i<_k; i++)
	{
		str.str("");
		str	<<"\n\n>>Partition "<<i<<" Statistic \n"
			<<"Total Vertex number: \t"<<stats.at(i)._vex_number<<"\n"
			<<"Assigned Vertex number: \t"<<stats.at(i)._assign_vex_number<<"\n"
			<<"Internal Edges: \t"<<stats.at(i)._internal_links<<"\n"
			<<"External Edges: \t"<<stats.at(i)._external_links<<"\n";
		Log::log(str.str());
		cutvalue += stats.at(i)._external_links;

		total_internal_edges+=stats.at(i)._internal_links;
		total_external_edges+=stats.at(i)._external_links;
		total_vex += stats.at(i)._vex_number+stats.at(i)._assign_vex_number;
	}

	str.str("");
	cutvalue = cutvalue/2;
	str<<"\n\nPartition Cut Value : \t"<<cutvalue<<"\n" 
		<<"Partition Cut Value in Mem : \t"<<GetPartitioner().ComputeCutValue()<<"\n\n"
		<<"total number of vertex: \t"<<total_vex<<"\n"
		<<"total number of Internal edges:\t"<<total_internal_edges<<"\n"
		<<"total number of External edges:\t"<<total_external_edges<<"\n"
		<<"total number of edges:\t"<<total_internal_edges+total_external_edges/2<<"\n\n"
		<<"Assign Window Size : \t"<<_assign_win_size<<"\n"
		<<"Sample Size Limition: \t"<<_edges_limition<<"\n";
	Log::log(str.str());
	Log::logln("====================Partition Statistic======================");
}

bool SGPLoader::isEdgeExist(EDGE& e)
{
	EdgeID e_id = MakeEdgeID(e._u, e._v);
	map<EdgeID, Edge_Item>::const_iterator iter =	_sample_edge_items.find(e_id);
	if(iter == _sample_edge_items.end())
		return false;
	else
		return true;
}

bool SGPLoader::ReadEdge(EDGE& e)
{
	string buf;

	while(getline(_ifs, buf))//empty line maybe exists
	{	
		if(buf.empty()) continue;
		
		int idx = buf.find_first_of(" ");
		string temp = buf.substr(0, idx);
		int u = stoi(temp);
		temp = buf.substr(idx+1, buf.length()-idx-1);
		int v= stoi(temp);
		e._u = u;
		e._v = v;
		return true;
	}
	return false;
}

//SGLs
bool SGPLoader::doStreamLoadByDBS(PartitionAlgorithm partition_algorithm)
{
	_sample_edge_items.clear();
	_sample_vertex_items.clear();
	_dbs_edges_cache_to_process.clear();
	
	//step1
	if(ReadInitSamples_DBS() < _edges_limition)
	{
		Log::log("the graph is too small!\n");
		return false;
	}
	if(!InitSamples_DBS())
	{
		Log::log("init samples error!!!\n");
		return false;
	}

	//step 2
	doGraphSamplePartition(partition_algorithm);


	//step 4 to step 30 
	vector<ReAdjustPartitionPair> adjust_partitions;
	while(ReadNextEdgesCache_DBS())
	{
		UpdateWeightofEdgesInEs();

		doStreamDBSSample();

		adjust_partitions.clear();

		if(CheckRepartition(adjust_partitions))
			RepartitionSampleGraph(adjust_partitions, partition_algorithm);

		_selected_edges.clear();
		_substituted_edges.clear();
	}
	_assign_manager.Flush();
	
	//step 31 to step 51
	UpdateStorageNode();

	return true;


}

bool SGPLoader::doStreamDBSSample()
{
	//the following codes are copied from SamplingEdgeCache
	vector<EDGE>::iterator iter = _dbs_edges_cache_to_process.begin();
	while(iter != _dbs_edges_cache_to_process.end())
	{
		int min_degree = INT_MAX;
		EDGE e = *iter;
		
		for(int j=0;j<2; j++)
		{
			VERTEX vex = (j==0)?e._u:e._v;
			map<VERTEX, Vertex_Item>::iterator iter_vexs = _sample_vertex_items.find(vex);
			if(iter_vexs == _sample_vertex_items.end())
			{
				Log::log("DBS : get the degree of vertex of edge : read first \\rho edges : error occur!!! ");
				return false;
			}
			else
			{
				if(iter_vexs->second.degree < min_degree)
				{
					min_degree = iter_vexs->second.degree;
				}
			}
		}
		double r = randf(0.0f, 1.0f);
		double key = pow(r, 1.0/min_degree);
		if(key > _min_weight)//substitute occur. step 8 - step 12
		{
			//for SGLs 
			map<EdgeID, Edge_Item>::iterator iter_edges = _sample_edge_items.find(_min_weight_edge_id);
			if(iter_edges == _sample_edge_items.end())
			{
				Log::log("DBS : doStreamDBSSample : find edges error occur!!! ");
				return false;
			}
			_sample_edge_items.erase(iter_edges);
			Edge_Item e_item = {min_degree, r, key};
			EdgeID e_id = MakeEdgeID(e._u, e._v);
			_sample_edge_items.insert(pair<EdgeID, Edge_Item>(e_id, e_item));

			
			map<VERTEX, Vertex_Item>::iterator iter_vex = _sample_vertex_items.find(e._u);
			if(iter_vex == _sample_vertex_items.end())
			{
				Log::log("SGLs : doStreamDBSSample : find vertex 1 error occur!!! ");
				return false;
			}
			iter_vex->second._is_sampled = true;
			iter_vex = _sample_vertex_items.find(e._v);
			if(iter_vex == _sample_vertex_items.end())
			{
				Log::log("SGLs : doStreamDBSSample : find vertex 2 error occur!!! ");
				return false;
			}
			iter_vex->second._is_sampled = true;

			_selected_edges.insert(e_id);
			_substituted_edges.insert(_min_weight_edge_id);
			//

			SearchMinimumKey();//it suffers from low efficiency. to improve it in the future by insert sorting with the index
		}
		else//no substitute occur, then assign edge. step 14- step 24
		{
			StreamAssignEdge(e);
		}
		iter++;
	}

	_graph_sample.UpdateSampleGraph(_selected_edges, _substituted_edges);//- ×¢Òâ£ºÍ¼ÐÞ¸Ä£¡£¡£¡£¡

	return true;
}

bool SGPLoader::StreamAssignEdge(EDGE e)
{
	bool all_vex_sampled = true;
	for(int i=0; i<2; i++)
	{
		VERTEX u = i==0?e._u:e._v;

		map<VERTEX, Vertex_Item>::iterator iter_vex = _sample_vertex_items.find(u);
		if(iter_vex == _sample_vertex_items.end())
		{
			Log::log("SGLs : StreamAssignEdge : find vertex error occur!!! ");
			return false;
		}

		if(!(iter_vex->second._is_sampled))
		{
			all_vex_sampled =  false;
		}
	}
	if(all_vex_sampled)//assign immediately
	{
		assign...//NOTE: modify the partition!!!! TODO
	}
	else//assigning util AC is full.
	{
		_assign_manager.CreateAndAppendContext(
			e, 
			&_partitions_in_memory, 
			&_graph_sample, 
			_assign_win_size);//!!!check, the partition and graph sample varied.

		_assign_manager.AppendEdge(e);//NOTE: modify the partition!!!! TODO
		_assign_manager.doManager();
	}
	return true;
}

bool SGPLoader::CheckRepartition(vector<ReAdjustPartitionPair>& adjust_partitions)
{
	hash_set<VERTEX> changed_vertex;
	//if an edge is in and out, it will not change the partition
	hash_set<EdgeID>::iterator iter_selected =  _selected_edges.begin();
	hash_set<EdgeID>::iterator iter_substituted;
	while(iter_selected != _selected_edges.end())
	{
		iter_substituted = _substituted_edges.find(*iter_selected);
		if(iter_substituted!= _substituted_edges.end())
		{
			iter_selected = _selected_edges.erase(iter_selected);
			_substituted_edges.erase(iter_substituted);
		}
		else
		{
			EDGE e = GetEdgeofID(*iter_selected);
			changed_vertex.insert(e._u);
			changed_vertex.insert(e._v);

			iter_selected++;
		}
	}
	iter_substituted = _substituted_edges.begin();
	while(iter_substituted!=_substituted_edges.end())
	{
		EDGE e = GetEdgeofID(*iter_substituted);
		changed_vertex.insert(e._u);
		changed_vertex.insert(e._v);
		iter_substituted++;
	}

	return _partitions_in_memory.CheckIfAdjust(changed_vertex, adjust_partitions);
}

void SGPLoader::RepartitionSampleGraph(vector<ReAdjustPartitionPair>& adjust_partitions,PartitionAlgorithm partition_algorithm)
{
	switch(partition_algorithm)
	{
	case KL:
		{
			_partitions_in_memory.RepartitionKL(adjust_partitions);
			break;
		}
	case MaxMin:
		{
			_partitions_in_memory.RepartitionMaxMin(adjust_partitions);
			break;
		}
	}
}

bool SGPLoader::UpdateStorageNode()
{
	return false;
}