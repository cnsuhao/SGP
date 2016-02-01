#pragma once
#include "Graph.h"
#include "Partitioner.h"
#include "Graph.h"
#include <vector>

class AssignContext
{
private:
	int				_context_size_limit; // the max size of context
	vector<EDGE>	_context_cache;
	EDGE			_assign_edge;// the edge to assign
	bool			_bAssigned;
	Partitioner*	_partitioner;
	Graph*			_graph;
public:
	AssignContext(void);
	AssignContext(EDGE e, Partitioner* partitioner, Graph* graph);

	~AssignContext(void);

	void SetContextSizeLimit(int size);
	void AppendEdge(EDGE& e);
	void Assign();
	int EvaluateVexCluster(Graph& cache_graph, VERTEX& vex_to_partition);

	bool isAssigned();
};

class AssignContextManager
{
private:
	vector<AssignContext*>		_assign_context_list;
public:
	AssignContextManager();
	~AssignContextManager();

	void AppendEdge(EDGE& e);
	void doManager();
	void CreateAndAppendContext(EDGE e, Partitioner* partitioner, Graph* graph, int sizelimit);
	//if reach the end, assign the edge by current cache
	void Flush();
};