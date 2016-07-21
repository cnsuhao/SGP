#pragma once
#include "Graph.h"
#include "Partitioner.h"
#include "Graph.h"
#include <vector>
#include <map>

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
	map<VERTEX, int> _assign_vex_info;//key: vertex, value: partition

public:
	AssignContextManager();
	~AssignContextManager();

	void AppendEdge(EDGE& e);
	//release the context of assigned edge
	void doManager();
	void CreateAndAppendContext(EDGE e, Partitioner* partitioner, Graph* graph, int sizelimit);
	//if reach the end, assign the edge by current cache
	void Flush();

	//return the partition of vertex, if not found, or the vex is sampled, return -1;
	int GetAssignVertexPartition(VERTEX& vex);
	//save assigned vertex and its partition into info. if the vertex exists, override it.
	void SaveAssignVertex(VERTEX& vex, int partition);
	//Label the vertex as sampled. that is, the assigned partition is -1; if not found, return false;
	bool LabelAssignVertexSample(VERTEX& vex);
	//get and set
	map<VERTEX, int>* GetAssignVexInfo() {return &_assign_vex_info;};

	static AssignContextManager* GetAssignManager();
	
};
//global assign manager
static AssignContextManager _assign_manager;