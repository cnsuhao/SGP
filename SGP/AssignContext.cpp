#include "StdAfx.h"
#include "AssignContext.h"
#include "SGPLoader.h"
//
AssignContext::AssignContext(void)
{

}

AssignContext::~AssignContext(void)
{

}

AssignContext::AssignContext(EDGE e, Partitioner* partitioner, Graph* graph)
{
	this->_assign_edge = e;
	this->_partitioner = partitioner;
	this->_graph = graph;
	_bAssigned = false;
}

void AssignContext::SetContextSizeLimit(int size)
{
	this->_context_size_limit = size;
}

void AssignContext::AppendEdge(EDGE& e)
{
	if(_bAssigned) return;

	_context_cache.push_back(e);
	if(_context_cache.size() == _context_size_limit)
	{
		Assign();
		_bAssigned = true;
	}
}

void AssignContext::Assign()
{
	Graph cache_graph;

	cache_graph.BuildGraphFromEdgesCache(_context_cache);

	typedef struct {
		VERTEX vex;
		int cluster;
	} AssignInfo;

	AssignInfo assign_info[2];

	for(int i=0; i<2; i++)
	{
		VERTEX vex_to_partition = i==0?_assign_edge._u : _assign_edge._v;
		assign_info[i].vex = vex_to_partition;
		assign_info[i].cluster = _partitioner->GetClusterLabelOfVex(vex_to_partition);
		if(assign_info[i].cluster == -1)
		{
			assign_info[i].cluster = AssignContextManager::GetAssignManager()->GetAssignVertexPartition(vex_to_partition);
			if(assign_info[i].cluster == -1)
			{
				assign_info[i].cluster = EvaluateVexCluster(cache_graph, vex_to_partition);
				//将assign的顶点及其partition保存在manager中，不保存AC。如果sampled，则标记。在最终阶段assign节点不在重新评估，可能cut会变大，但均值不会太高，因为绝大多数顶点会被采样。
				AssignContextManager::GetAssignManager()->SaveAssignVertex(vex_to_partition, assign_info[i].cluster);
				//_partitioner->AppendAssignVertex(assign_info[i].vex, assign_info[i].cluster);
				//statistic: 注意此部分的统计数据由writeassignverticeofpartitions函数计算。
				//_partitioner->SetAssignVertexStat(assign_info[i].cluster);
			}
		}
	}

	_partitioner->WriteAssignEdge(assign_info[0].vex, assign_info[0].cluster, assign_info[1].vex, assign_info[1].cluster);
}

int AssignContext::EvaluateVexCluster(Graph& cache_graph, VERTEX& vex_to_partition)
{
	vector<int> len_shortest_paths;
	cache_graph.ComputeShortestPathsFromVertex(vex_to_partition, len_shortest_paths);

	//init
	vector<float> assign_weights;
	for(int i=0; i<_partitioner->GetPartitionNumber(); i++)
	{
		assign_weights.push_back(0);
	}

	//compute weight
	int vex_pos = 0;
	VertexInfoArray& vex_table = cache_graph.GetAdjTableRef()->_vex_table;
	VertexInfoArray::const_iterator iter_vextable = vex_table.begin();
	while(iter_vextable != vex_table.end())
	{
		VERTEX vex = iter_vextable->_u;
		int cluster = _partitioner->GetClusterLabelOfVex(vex);
		if(cluster == -1)//not partitioned
		{
			iter_vextable++;
			vex_pos++;
			continue;
		}

		//partitioned
		int degree = _graph->GetVertexInfoofVertex(vex)->_degree;
		int degree_to_cluster = _partitioner->ComputeLinkstoClusterFromVex(vex, cluster);
		int len_shortest_path = len_shortest_paths.at(vex_pos);
		if(len_shortest_path>0 && len_shortest_path!=INT_MAX)
		{
			float w = assign_weights.at(cluster);
			w += 1.0f*degree_to_cluster/(len_shortest_path*degree);
			assign_weights.at(cluster) = w;
		}

		iter_vextable++;
		vex_pos++;
	}

	//find max weight
	int max_cluster = -1, cur_cluster=0;
	float max_weight = 0;
	vector<float>::const_iterator iter_weights = assign_weights.begin();
	while(iter_weights!=assign_weights.end())
	{
		if(*iter_weights > max_weight)
		{
			max_cluster = cur_cluster;
			max_weight = *iter_weights;
		}

		iter_weights++;
		cur_cluster++;
	}

	//if the vex doesn't connect any others
	if(max_cluster==-1)
	{
		max_cluster = rand(0, _partitioner->GetPartitionNumber()-1);
	}

	return max_cluster;
}

bool AssignContext::isAssigned()
{
	return _bAssigned;
}

AssignContextManager::AssignContextManager()
{
}

AssignContextManager::~AssignContextManager()
{
}

void AssignContextManager::AppendEdge(EDGE& e)
{
	vector<AssignContext*>::iterator iter = _assign_context_list.begin();
	while(iter!=_assign_context_list.end())
	{
		(*iter)->AppendEdge(e);
		iter++;
	}

}

void AssignContextManager::doManager()
{
	vector<AssignContext*>::iterator iter = _assign_context_list.begin();
	while(iter!=_assign_context_list.end())
	{
		if((*iter)->isAssigned()) {
			delete (*iter);
			iter = _assign_context_list.erase(iter);//if erase iter, then iter++, error since iter has been del, iter++ will error. 
		}
		else
		{
			iter++;
		}
	}
}

void AssignContextManager::CreateAndAppendContext(EDGE e, Partitioner* partitioner, Graph* graph, int sizelimit)
{
	AssignContext* ctx = new AssignContext(e, partitioner, graph);
	ctx->SetContextSizeLimit(sizelimit);
	_assign_context_list.push_back(ctx);
}

void AssignContextManager::Flush()
{
	vector<AssignContext*>::iterator iter = _assign_context_list.begin();
	while(iter!=_assign_context_list.end())
	{
		if(!((*iter)->isAssigned())) {
			(*iter)->Assign();
		}

		delete (*iter);
		iter = _assign_context_list.erase(iter);
	}
}

int AssignContextManager::GetAssignVertexPartition(VERTEX& vex)
{
	map<VERTEX, int>::iterator iter = _assign_vex_info.find(vex);
	if(iter != _assign_vex_info.end())
		return iter->second;
	else
		return -1;
}

void AssignContextManager::SaveAssignVertex(VERTEX& vex, int partition)
{
	_assign_vex_info.insert(pair<VERTEX, int>(vex, partition));
}

bool AssignContextManager::LabelAssignVertexSample(VERTEX& vex)
{
	map<VERTEX, int>::iterator iter = _assign_vex_info.find(vex);
	if(iter != _assign_vex_info.end())
	{
		iter->second = -1;
		return true;
	}
	else
	{
		return false;
	}
}

void  AssignContextManager::UpdateAssignManager(Partitioner& partitions)
{
	for (map<VERTEX, int>::iterator iter =  _assign_vex_info.begin(); iter!= _assign_vex_info.end(); iter++)
	{
		VERTEX u = iter->first;
		int cluster = partitions.GetClusterLabelOfVex(u);
		if(cluster == -1) 
		{
			continue;//该节点不在当前partitions中，该节点可能为assign或之前被删除的采样节点（此时，保留删除前的cluster label）
		}
		else
		{
			iter->second = cluster;
		}
	}
}

AssignContextManager* AssignContextManager::GetAssignManager()
{
	return &_assign_manager;
}