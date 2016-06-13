#pragma once

#include "Graph.h"
#include "Partitioner.h"
#include "AssignContext.h"
#include "commondef.h"

int rand(int range_min, int range_max);
float randf(float range_min, float range_max);


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
	int						_edges_limition;//samples size, $\rho$
	Partitioner				_partitions_in_memory;
	
	//assign context data
	AssignContextManager	_assign_manager;
	int                     _assign_win_size;

	SampleMode				_sample_mode;

	/***********************************************************************************************************************************/
	//data structure for sampling
	map<VERTEX, Vertex_Item>		_sample_vertex_items;//vertex --> degree; Vertex_Item represents V_s
	map<EdgeID, Edge_Item>			_sample_edge_items;//edgeid --> weight, rand, key; Edge_Item represents E_s
	/***********************************************************************************************************************************/

	/***********************************************************************************************************************************/
	//data for eq sample
	vector<EdgeID>	_sample_reserior;//auxiliary variable
	/***********************************************************************************************************************************/
	//the function for eq sample
	bool doGraphSamplingByFixMemEq();
	/***********************************************************************************************************************************/

	/***********************************************************************************************************************************/
	//data for dbs
	int								_edges_cache_limitation;// $\eta$, $\eta$<_edges_limition
	vector<EDGE>					_dbs_edges_cache_to_process;//E'
	EdgeID				_min_weight_edge_id;//the subtituted edge with the minimum key
	double				_min_weight; // minimum key
	/***********************************************************************************************************************************/
	//the funtion for dbs sampling
	//the entry to dbs sampling
	bool doGraphSamplingByDBS();
	//read the first \rho edges into _sample_edge_items
	int ReadInitSamples_DBS();
	//compute the sample weight by the first \rho edges
	bool InitSamples_DBS();
	//read the next \eta edges into _dbs_edges_cache_to_process and update _sample_vertex_items at the same time
	bool ReadNextEdgesCache_DBS();
	//update the weight of edge in E_s
	void UpdateWeightofEdgesInEs();
	//sampling the edges in E'(_dbs_edges_cache_to_process)
	bool SamplingEdgeCache();
	//insert a edge into _sample_edge_items
	bool AppendEdgeSample_DBS(EDGE e);
	//find the minimum key in the current E_s
	void SearchMinimumKey();
	/***********************************************************************************************************************************/
		
	/***********************************************************************************************************************************/
	//the data for SGLs
	//indicator of repartition
	bool _is_repartition;
	//cache of selected edges by dbs
	vector<EdgeID> _selected_edges;
	//cache of substituted edges by dbs
	vector<EdgeID> _substituted_edges;

	//the funtion for streaming load with dbs sampling - SGLs
	// streaming dbs
	bool doStreamDBSSample();
	//check if repartition
	bool isRepartition();
	//repartition sample graph
	void RepartitionSampleGraph();
	//substitute edges in sample graph
	bool StreamAssignEdge(EDGE e);
	//update storage node on parallel
	bool UpdateStorageNode();
	/***********************************************************************************************************************************/
	
	/***********************************************************************************************************************************/
	//data for uneq sampling
	int _max_d;//auxiliary variable. NOTE: set the value before sampling
	int _sum_weight;//auxiliary variable
	//NOTE:only the item weight is used as the min degree in _sample_edge_items
	/***********************************************************************************************************************************/
	//the function for uneq sampling	
	bool doGraphSamplingByFixMemUneq();
	// read the fist samples of _edges_limition from the begin of _graph_file. return the read lines
	int ReadInitSamples_Uneq();
	bool AppendEdgeSample_Uneq(EDGE e);
	//compute the min degree of edge after the first _edges_limition edges read
	bool InitEdgeWeightInEs();
	bool UpdateVertexWeight_Uneq(EDGE e);
	bool SelectNewEdge_Uneq(EDGE e);
	void SetMaxDegree(int d) {_max_d = d;};
	/***********************************************************************************************************************************/

	bool doGraphSamplingByFixRatioMode();

	/***********************************************************************************************************************************/
	// the functions in common
	//build the graph on the _sample_edge_items
	void BuildSampleGraph();
	//read an edge from current pos of ifs
	bool ReadEdge(EDGE& e);
	void ResetGraphInputStream(void);
	bool isSampled(EDGE& e);
	/***********************************************************************************************************************************/

public:
	//sampling the _graph_file and construct _graph_sample
	bool doGraphSampling();
	//partitioning the graph sample into _partitions_in_memory of k clusters
	void doGraphSamplePartition(PartitionAlgorithm partition_algorithm);
	// assign the reminder of edges for the algorithm of scaning graph on disk twice
	void doAssignReminderEdges();

	void SetEdgeOrderMode(EdgeOrderMode mode);
	void SetGraphFile(string graphfile);
	void SetK(int k);
	void SetSampleRation(float ratio);
	void SetEdgesLimition(int limit);
	void SetSampleMode(SampleMode mode);

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

	/***********************************************************************************************************************************/
	//the funtion for streaming load with dbs sampling - SGLs
	bool doStreamLoadByDBS(PartitionAlgorithm partition_algorithm);
	void SetAssignWindowSize(int assign_win_size) {_assign_win_size = assign_win_size;};
	/***********************************************************************************************************************************/


};

