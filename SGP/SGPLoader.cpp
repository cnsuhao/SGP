#include "StdAfx.h"
#include "SGPLoader.h"
#include <time.h>
#include "Log.h"
#include <sstream>


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

void SGPLoader::doGraphSampling()
{
	_ifs.open(_graph_file.c_str());
	_samples_cache.clear();
	_sampled_edges_idx.clear();

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

}

void SGPLoader::doGraphSamplingByFixMemEq()
{
	int iread = ReadInitSamples();
	if(iread < _edges_limition)
	{
		Log::log("the graph is too small!\n");
		return;
	}

	srand( (unsigned)time( NULL ) );
	EDGE e;
	while(ReadEdge(e))
	{
		if(isEdgeExist(e)) continue;

		iread++;
		int i = rand(1, iread);
		if(i<=_edges_limition)
		{
			int replace = rand(1, _edges_limition);
			RemoveEdgeSampleOfPos(replace);
			AppendEdgeSample(e, replace);
		}
 	}
}

void SGPLoader::doGraphSamplingByFixRatioMode()
{

}

void SGPLoader::doGraphSamplingByFixMemUneq()
{
	_removed_probs_sum = 0;

	int iread = ReadInitSamples();
	if(iread < _edges_limition)
	{
		Log::log("the graph is too small!\n");
		return;
	}

	UpdateProbsOfEdgeRemoved();	
	
	srand( (unsigned)time( NULL ) );
	
	EDGE e;
	while(ReadEdge(e))
	{
		if(isEdgeExist(e)) 
			continue;

		int i = rand(1, iread);
		if(i>_edges_limition)
			continue;
		
		//check_degree_distribution();

		int replace = SelectEdgeToRemoveByUneq();//not check return value -1...

		//debug
		/*
		EDGE e_replace = _samples_cache.at(replace); 
		map<VERTEX, int>::iterator iter_vexs = _vertex_degree_in_sample.find(e_replace._u);
		Log::log(replace);
		Log::log(" : ( ");
		Log::log(iter_vexs->second);
		Log::log(" ");
		iter_vexs = _vertex_degree_in_sample.find(e_replace._v);
		Log::log(iter_vexs->second);
		Log::log(" ) ");
		*/

		RemoveEdgeSampleOfPos(replace);
		AppendEdgeSample(e, replace);


		//update all the others related to e
		UpdateProbsOfEdgeRemoved(e._u, e._v);
 	}
}

void SGPLoader::doGraphSamplingByDBS()
{
	_dbs_edge_items.clear();
	_dbs_vertex_items.clear();
	
	EDGE e;
	DBS_Edge_Item e_item = {0,0,0};
	int iread = 0;
	while(iread<_edges_limition && ReadEdge(e))
	{
		if(isEdgeExist(e))
			continue;

		AppendEdgeSample(e);

		EdgeID e_id = MakeEdgeID(e._u,e._v);
		_dbs_edge_items.insert(pair<EdgeID, DBS_Edge_Item>(e_id, e_item));

		DBS_Vertex_Item v_item = {0};
		map<VERTEX, DBS_Vertex_Item>::iterator iter;
		for(int j=0;j<2; j++)
		{
			VERTEX vex = (j==0)?e._u:e._v;
			iter = _dbs_vertex_items.find(vex);
			if(iter == _dbs_vertex_items.end())
			{
				v_item.degree = 1;
				_dbs_vertex_items.insert(pair<VERTEX, DBS_Vertex_Item>(vex, v_item));
			}
			else
			{
				(iter->second).degree++;
			}
		}
		
		iread++;
	}
	if(iread < _edges_limition)
	{
		Log::log("the graph is too small!\n");
		return;
	}

	map<EdgeID, DBS_Edge_Item>::iterator iter_edges = _dbs_edge_items.begin();
	while(iter_edges != _dbs_edge_items.end())
	{
		EDGE e = GetEdgeofID(iter_edges->first);

		int min_degree = INT_MAX;
		map<VERTEX, DBS_Vertex_Item>::iterator iter_vexs;
		for(int j=0;j<2; j++)
		{
			VERTEX vex = (j==0)?e._u:e._v;
			iter_vexs = _dbs_vertex_items.find(vex);
			if(iter_vexs == _dbs_vertex_items.end())
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
		iter_edges->second._weight = min_degree;
		iter_edges->second._random = rand();
		iter_edges->second._weight = pow(iter_edges->second._random, 1.0/iter_edges->second._weight);
	}

	//TODO:...

}

void SGPLoader::doGraphSamplePartition(PartitionAlgorithm partition_algorithm)
{
	_partitions_in_memory.ClearPartition();
	_partitions_in_memory.SetPartitionNumber(_k);

	_graph_sample.BuildGraphFromEdgesCache(_samples_cache);
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

int SGPLoader::ReadInitSamples()
{
	int i = 0;
	EDGE e;
	while(i<_edges_limition && ReadEdge(e))
	{
		if(isEdgeExist(e))
			continue;

		AppendEdgeSample(e);
		i++;
	}
	return i;
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
void SGPLoader::AppendEdgeSample(EDGE e)
{
	_samples_cache.push_back(e);
	AppendEdgeSample(e, _samples_cache.size()-1);
}

void SGPLoader::AppendEdgeSample(EDGE e, int pos)
{
	EdgeID e_id = MakeEdgeID(e._u,e._v);
	_samples_cache.at(pos) = e;
	_sampled_edges_idx.insert(pair<EdgeID, int>(e_id, pos));
	
	//TODO:
	//move these codes to the independent function of unequal sampling
	/*
	if(_sample_mode == FIX_MEM_UNEQ)
	{
		map<VERTEX, int>::iterator iter_vexs = _vertex_degree_in_sample.find(e._u);
		if(iter_vexs == _vertex_degree_in_sample.end())
		{
			_vertex_degree_in_sample.insert(pair<VERTEX, int>(e._u,1));
		}
		else
		{
			++(iter_vexs->second);//degree++
		}
		iter_vexs = _vertex_degree_in_sample.find(e._v);
		if(iter_vexs == _vertex_degree_in_sample.end())
		{
			_vertex_degree_in_sample.insert(pair<VERTEX, int>(e._v,1));
		}
		else
		{
			++(iter_vexs->second);//degree++
		}

		float e_prob = ComputeProbOfEdgeRemoved(e);
		_sample_removed_probs.insert(pair<EdgeID, float>(e_id, e_prob));
		_removed_probs_sum += e_prob;
	}
	*/
}

float SGPLoader::GetProbOfEdge(EDGE e)
{
	map<EdgeID, float>::iterator iter = _sample_removed_probs.find(MakeEdgeID(e._u, e._v));
	if(iter == _sample_removed_probs.end())
	{
		return -1.0f;
	}
	else
	{
		float prob=iter->second;
		return prob;
	}
}

void SGPLoader::SetProbOfEdge(EDGE e, float prob)
{
	EdgeID e_id = MakeEdgeID(e._u, e._v);
	map<EdgeID, float>::iterator iter = _sample_removed_probs.find(e_id);
	if(iter == _sample_removed_probs.end())
	{
		_sample_removed_probs.insert(pair<EdgeID, float>(e_id, prob));
		//int iret = GetEdgePosInCache(e);
	}
	else
	{
		iter->second = prob;
	}
}

float SGPLoader::ComputeProbOfEdgeRemoved(EDGE e)
{

	int u_degree, v_degree;
	map<VERTEX, int>::iterator iter_vexs = _vertex_degree_in_sample.find(e._u);
	if(iter_vexs == _vertex_degree_in_sample.end())
	{
		return -1.0f;
	}
	else
	{
		u_degree = iter_vexs->second;
	}
	iter_vexs = _vertex_degree_in_sample.find(e._v);
	if(iter_vexs == _vertex_degree_in_sample.end())
	{
		return -1.0f;
	}
	else
	{
		v_degree = iter_vexs->second;
	}

	//int vex_num = _vertex_degree_in_sample.size();
	int max_degree = u_degree>v_degree?u_degree:v_degree;
	//float e_prob = 1.0f-max_degree*1.0f/vex_num;

	return max_degree;
}


void SGPLoader::UpdateProbsOfEdgeRemoved()
{
	_removed_probs_sum = 0;
	vector<EDGE>::iterator iter = _samples_cache.begin();
	while(iter!=_samples_cache.end())
	{
		VERTEX u=iter->_u, v=iter->_v;
		EDGE e = {u, v};
		float prob_new = ComputeProbOfEdgeRemoved(e);
		SetProbOfEdge(e, prob_new);
		_removed_probs_sum = _removed_probs_sum+prob_new;
		iter++;
	}
}

void SGPLoader::UpdateProbsOfEdgeRemoved(VERTEX changed_v1, VERTEX changed_v2)
{
	//int vex_num = _vertex_degree_in_sample.size();
	//find all the related edges and compute their probs
	vector<EDGE>::iterator iter = _samples_cache.begin();
	while(iter!=_samples_cache.end())
	{
		VERTEX u=iter->_u, v=iter->_v;
		if(u==changed_v1 ||u==changed_v2 || v==changed_v1 ||v==changed_v2)
		{
			EDGE e = {u, v};
			float prob_old = GetProbOfEdge(e);
			float prob_new = ComputeProbOfEdgeRemoved(e);
			SetProbOfEdge(e, prob_new);
			_removed_probs_sum = _removed_probs_sum-prob_old+prob_new;
		}
		iter++;
	}
}

void SGPLoader::RemoveEdgeSampleOfPos(int pos)
{
	if(pos>=_samples_cache.size() || pos<0)
	{
		Log::log("!!!ERR: remove edge sample of pos: ");
		Log::logln(pos);
		return;
	}


	EDGE e = _samples_cache.at(pos);
	EdgeID e_id = MakeEdgeID(e._u,e._v);
	map<EdgeID, int>::iterator iter = _sampled_edges_idx.find(e_id);
	if(iter!=_sampled_edges_idx.end())
	{
		_sampled_edges_idx.erase(iter);
	}
	else
	{
		Log::log("!!!ERR: remove the idx of edge sample of pos");
		return;
	}

	//FIX_MEM_UNEQ: don't decrease the degree
	if(_sample_mode == FIX_MEM_UNEQ)
	{
		float prob=GetProbOfEdge(e);
		_removed_probs_sum -= prob;
		map<EdgeID, float>::iterator iter = _sample_removed_probs.find(e_id);
		_sample_removed_probs.erase(iter);
	}
}

int SGPLoader::SelectEdgeToRemoveByUneq()
{
/*
	float min = INT_MAX; 
	EdgeID e_id;
	map<EdgeID, float>::const_iterator iter = _sample_removed_probs.begin();
	while(iter!=_sample_removed_probs.end())
	{
		if(iter->second<min)
		{
			e_id = iter->first;
			min = iter->second;
		}

		iter++;
	}

	return GetEdgePosInCache(e_id);
*/	
	
	float hit = randf(0.0f, 1.0f);
	float accum = 0.0f;
	int vex_num = _vertex_degree_in_sample.size();
	map<EdgeID, float>::const_iterator iter = _sample_removed_probs.begin();
	while(iter!=_sample_removed_probs.end() && accum<hit)
	{
		float max_d = iter->second;
		//float prob = (1-max_d/vex_num)/(vex_num - _removed_probs_sum/vex_num);
		float prob = (max_d)/( _removed_probs_sum);
		accum += prob;

		iter++;
	}

	if(iter !=_sample_removed_probs.end())
	{
		EdgeID e_id = iter->first;
		return GetEdgePosInCache(e_id);
	}
	else
	{
		return _samples_cache.size()-1;
	}

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

int SGPLoader::GetEdgePosInCache(EDGE e)
{
	EdgeID e_id = MakeEdgeID(e._u, e._v);
	return GetEdgePosInCache(e_id);
}

int SGPLoader::GetEdgePosInCache(EdgeID e_id)
{
	map<EdgeID, int>::const_iterator iter =	_sampled_edges_idx.find(e_id);
	if(iter == _sampled_edges_idx.end())
		return -1;
	else
		return iter->second;
}

bool SGPLoader::isEdgeExist(EDGE& e)
{
	EdgeID e_id = MakeEdgeID(e._u, e._v);
	map<EdgeID, int>::const_iterator iter =	_sampled_edges_idx.find(e_id);
	if(iter == _sampled_edges_idx.end())
		return false;
	else
		return true;
}


bool SGPLoader::check_cache_idx(void)
{
	//debug
	for(int j=0; j<_samples_cache.size();j++){
		EdgeID e_id = MakeEdgeID(_samples_cache.at(j)._u,_samples_cache.at(j)._v);
		map<EdgeID, int>::iterator iter1 = _sampled_edges_idx.find(e_id);
		if(iter1 ==_sampled_edges_idx.end())
		{
			Log::logln("not found!");
			return false;
		}
		if(j != iter1->second)
		{
			Log::logln("pos is wrong");
			return false;
		}
	}
	return true;
}


void SGPLoader::check_degree_distribution(void)
{
	map<int, int> d_dist;
	map<VERTEX, int>::iterator it_d = _vertex_degree_in_sample.begin();
	while(it_d != _vertex_degree_in_sample.end())
	{
		int d = it_d->second;
		map<int, int>::iterator iter = d_dist.find(d);
		if(iter==d_dist.end())
		{
			d_dist.insert(pair<int,int>(d, 1));
		}
		else
		{
			iter->second++;
		}
		it_d++;
	}
	float avg_d = 0;
	int total = 0;
	stringstream log_str;
	map<int, int>::iterator iter = d_dist.begin();
	while(iter!=d_dist.end())
	{
		//log_str.str("");
		//log_str<<"degree:\t"<<iter->first<<"\t count:\t"<<iter->second;
		//Log::logln(log_str.str());
		avg_d += iter->first * iter->second;
		total += iter->second;
		iter++;
	}

	log_str.str("");
	log_str<<"avg degree:\t"<<avg_d/total;
	Log::logln(log_str.str());

}
