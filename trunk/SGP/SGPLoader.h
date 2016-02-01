#pragma once
#include "Graph.h"
#include "Partitioner.h"
#include "AssignContext.h"

int rand(int range_min, int range_max);
float randf(float range_min, float range_max);

typedef enum _EdgeOrderMode { 
	RANDOM, DFS, BFS 
} EdgeOrderMode;

typedef enum _PartitionAlgorithm {
	KL, MaxMin
} PartitionAlgorithm;

typedef enum _SampleMode {
	FIX_RATIO, //degree * _sample_ratio. the samples size not fixed. it is for BFS
	FIX_MEM_EQ, //fixed samples's size of _edges_limition. the replaced likelihood is equal for each edge in sampling cache
	FIX_MEM_UNEQ, //fixed samples's size of _edges_limition. the replaced likelihood is unequal for each edge in sampling cache
	RESERVOIR_DBS // DBS-sample : weighted sample with reservoir design like 
} SampleMode;

class SGPLoader
{
public:
	SGPLoader(void);
	~SGPLoader(void);

private:
	EdgeOrderMode			_edge_order_mode;
	Graph					_graph_sample;//samples graph
	string					_graph_file; //graph file input
	string					_outfile;//output file;
	ifstream				_ifs;
	int						_k;
	float					_sample_ratio;//sample's ratio, it maybe not used.
	int						_edges_limition;//samples size
	Partitioner				_partitions_in_memory;
	map<EdgeID, int>		_sampled_edges_idx;//to search the sampled edges
	SampleMode				_sample_mode;
	vector<EDGE>			_samples_cache; // the cache of sampled edges
	AssignContextManager	_assign_manager;
	int                     _assign_win_size;

	//data for sampling with unequal prob.
	map<EdgeID, float>      _sample_removed_probs;//<edge, removed-prob>.here the removed-prob is the max of degreem, which isn't normalized
	float                   _removed_probs_sum;//the sum of the max degree for each in _sample_removed_probs
	map<VERTEX, int>        _vertex_degree_in_sample;//<vex, degree>

	void doGraphSamplingByFixRatioMode();
	void doGraphSamplingByFixMemEq();
	void doGraphSamplingByFixMemUneq();
	void doGraphSampleingByDBS();
	
	// read the fist samples of _edges_limition from the begin of _graph_file. return the read lines
	int ReadInitSamples();
	//read an edge from current pos of ifs
	bool ReadEdge(EDGE& e);
	//append an edge into sample cache at the position of pos without checking if it exists. the prob of removed isn't computed
	void AppendEdgeSample(EDGE e, int pos);
	//at the end
	void AppendEdgeSample(EDGE e);
	//remove an edge from sample cache. just remove it from index.
	void RemoveEdgeSampleOfPos(int pos);

	//compute the prob of a edge for unequal sampling. if return < 0, error. (1-d/n)/sum(1-d/n) = (1-d/n)/{n-sum(d)/n}
	float ComputeProbOfEdgeRemoved(EDGE e);
	//re-compute the probs of edge to be removed when the degrees of v1 and v2 have been changed.
	//NOTE: only the operation of append edge will cause the degree changing
	void UpdateProbsOfEdgeRemoved(VERTEX changed_v1, VERTEX changed_v2);
	//update all probs of edges
	void UpdateProbsOfEdgeRemoved();
	//get the unnormalized prob of edge to be removed. if not found, return -1.0f
	float GetProbOfEdge(EDGE e);
	//set the unnormalized prob of edge to be removed
	void SetProbOfEdge(EDGE e, float prob);
	//select an edge to remove by unequal sampling. the return is the pos of selected edge. if -1 returned, error.
	int SelectEdgeToRemoveByUneq();

	void ResetGraphInputStream(void);
	bool isSampled(EDGE& e);

public:
	//sampling the _graph_file and construct _graph_sample
	void doGraphSampling();
	//partitioning the graph sample into _partitions_in_memory of k clusters
	void doGraphSamplePartition(PartitionAlgorithm partition_algorithm);
	// assign the reminder of edges
	void doAssignReminderEdges();


	void SetEdgeOrderMode(EdgeOrderMode mode);
	void SetGraphFile(string graphfile);
	void SetK(int k);
	void SetSampleRation(float ratio);
	void SetEdgesLimition(int limit);
	void SetSampleMode(SampleMode mode);
	void SetAssignWindowSize(int assign_win_size) {_assign_win_size = assign_win_size;};

	void SetOutputFile(string outfile) {
		_outfile = outfile; 
		_partitions_in_memory.SetOutFile(_outfile);
	};

	//if not found, return -1.
	int GetEdgePosInCache(EDGE e);
	int GetEdgePosInCache(EdgeID e_id);

	Partitioner& GetPartitioner() { return _partitions_in_memory;};

	//log the informations of the size of each partition, the cutvalue
	void doSGPStatistic();
	//check if the edge exists
	bool isEdgeExist(EDGE& e);

	//debug
	bool check_cache_idx(void);
	void check_degree_distribution(void);
};

