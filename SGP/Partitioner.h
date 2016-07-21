#pragma once
#include "stdafx.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <io.h>
#include <string>
#include <queue>
#include <map>
#include <unordered_set>
#include <set>
#include <hash_set>
#include "Graph.h"

// a node in a cluster
typedef struct _ClusterNode {
	int _pos;//vertex pos in the adj-table
	int _gain; //gain-value of temproary cluster. the final external links should be got by the degree - _internal_cost 
	int _internal_cost;//the internal links.
	int _visited;//to indicate whether the node visited or not at the step of paritioning swap. 0: no, 1:yes
} ClusterNode;

//a set of node
typedef struct _Cluster { 
	vector<ClusterNode> _cluster;
	map<int, int> _cluster_node_idx;//<node_pos_in_adjtable, cluster_node_pos_in_the_cluster>
	//unordered_set<VERTEX> _assign_vex;//the assigned vex, which doesn't exist in adjtable of graph. it is used to sgp algorithm.
} Cluster;

// a set of cluster
typedef vector<Cluster*> Partition;

//statistic information of a partition
typedef struct _PartitionStatisticInfo {
	int	_vex_number; //total number of vertex of a partition
	int _assign_vex_number;//the number of assigned vertex
	int _external_links;
	int _internal_links;
} PartitionStatisticInfo;

//partitioner
class Partitioner {
private:
	// the number of clusters: 2^n
	int _k;
	// a partition
	Partition _aPartition;
	// the corresponding graph data
	Graph* _graph;
	//the stem of out file name
	string _outfile;
	
	//the statistic informations of partitions. these informations will be filled up at the step of writing
	vector<PartitionStatisticInfo> _partitions_statistic;

public:
	Partitioner(){};

	void SetGraph(Graph* g){_graph = g;}
	void SetPartitionNumber(int k);
	int GetPartitionNumber(){return _k;};
	void SetOutFile(string outfile){_outfile = outfile;};
	string GetOutFile(){return _outfile;};
	vector<PartitionStatisticInfo>& GetPartitionStatistic() {return _partitions_statistic;};
	//increase the number of assigned vertex in the partition of cluster_id
	void SetAssignVertexStat(int cluster_id);
	Partition& GetPartition(){return _aPartition;};

	void ClearPartition();
	void ResetPartitionStatistic();

	//append a cluster node at the tail of a cluster
	void AppendClusterNode(Cluster* cluster, ClusterNode& node);
	//overwrite the cluster node at the position of 'at'
	void InsertClusterNode(Cluster* cluster, ClusterNode node, int at);
	//remove the cluster node at the pos of 'at'
	void DeleteClusterNodeAtPos(Cluster* cluster, int at);
	//get the pos of cluster node by node pos in adj_table. return: -1: no found
	int GetClusterNode(Cluster* cluster, ClusterNode& node);
	//get the cluster label of vex. the label is the pos in the partitions. if not found, return -1
	//NOTE: suppose that the pos of cluster in the partitions is kept from changing.
	int GetClusterLabelOfVex(VERTEX u);
	//KL partitioning algorithm
	void doKL();
	void doKLPartition(Cluster* aCluster, Cluster* bCluster);
	//Max-Min partitioning algorithm
	void doMaxMin();

	//compute all nodes's gain in the clusters of a and b,
	void ComputeGainAndSort(Cluster* aCluster, Cluster* bCluster);
	//compute a node's gain according to the cluster of a and b, where the node belongs to the cluster of a
	void ComputeGain(ClusterNode& node_in_aCluster, Cluster* aCluster, Cluster* bCluster);
	//insert the node of end_pos decsendantly into the cluster between the begin_pos and end_pos. the inserted node will be at the location of end_pos
	void InsertSort(Cluster* aCluster, int end_pos, int begin_pos);
	//insert the node of end_pos decsendantly into the cluster between 0 and end_pos. the inserted node will be at the location of end_pos
	void InsertSort(Cluster* aCluster, int end_pos);
	//move the cluster node from the position of 'from' to 'to'. the node information of 'from' will be retained, but its idx will be updated. it is used for insert sort
	void MoveClusterNode(Cluster* cluster, int from, int to);
	int SetIntersectionBetweenEdgeListAndCluster(EdgeInfoArray* vexlist, Cluster* cluster);
	//update d value, exchanged_node_a_pos: the pos of the node exchanged into A,d_changed_list_a: the cluster node list of d value changed
	void UpdateKLDValue(Cluster* aCluster, int exchanged_node_a_pos, unordered_set<int>& d_changed_list_a, 
						Cluster* bCluster, int exchanged_node_b_pos, unordered_set<int>& d_changed_list_b);
	//swap the cluster nodes at the pos of pos_a and pos_b
	void SwapClusterNodebyPos(Cluster* aCluster, int pos_a, Cluster* bCluster, int pos_b);
	//sort cluster nodes of d value changed from the pos of 'from' to the end.
	void SortCluster(Cluster* cluster, unordered_set<int>& node_pos_of_d_changed, int from);
	//return: true: find the exchanged nodes, otherwise, not; if found, exchanged_a and exchanged_b will be set.
	bool FindExchangeClusterNode(Cluster* aCluster,int find_begin_a,int& exchanged_a,
	                                      Cluster* bCluster,int find_begin_b,int& exchanged_b);

	//write the vertices of each partitioning results into outfile_vertices.1
	void WriteVerticesOfPartitions();
	//write the edges of each partitioning results into outfile_edges.1
	void WriteClusterEdgesOfPartitions();
	//write an cluster edge of u, v belonging to u_cluster and v_cluster repectively. outfile_edges.1
	void WriteClusterEdge(VERTEX u, int u_cluster, VERTEX v, int v_cluster);
	//write the assigned vertices that saved in the _assign_vex of cluster. NOTE: for SGLs, the vex in _assign_vex maybe sampled and partitioned after it assigned. So, SGLs will 
	//re-adjust the vex in the assigned file according to the corresponding cluster.
	void WriteAssignVerticesOfPartitions();
	//write an assigned edge of u, v belonging to u_cluster and v_cluster repectively. outfile_assign.1
	void WriteAssignEdge(VERTEX u, int u_cluster, VERTEX v, int v_cluster);
	//compute the cut value
	int ComputeCutValue();
	//compute the links from the vex to the cluster of pos
	int ComputeLinkstoClusterFromVex(VERTEX vex, int cluster_pos);


	/*void AppendAssignVertex(VERTEX vex, int partition_id);*/
	/*int GetAssignedLabelOfVex(VERTEX vex);*/

	/*********************************************************************************/
	//SGLs:将整个划分过程看做二叉树（完全），但最终只保存了叶子节点，更新实际上在叶子上开始处理的
	//adjust_partitions contains the partions to be adjust, i.e the gain is inversed, if null , no adjust.
	//change_vexs contains the vertex whose edges is changed.partitions_change_vex is the partition of vex that begin with 0
	//NOTE: the number of partition adjusted begin with the root of partitioning binary tree.
	bool CheckIfAdjust(vector<VERTEX>& change_vexs, vector<int>& partitions_change_vex,vector<ReAdjustPartitionPair>& adjust_partitions);
	//partition_u:the leaf set of the node containing u in BT at the level. begin with 0
	//partition_not_u:the leaf set of the node's sibling. begin with 0
	bool CheckClusterAdjust(VERTEX u, vector<int>& partition_u, vector<int>& partition_not_u);
	int GetConnectionsToClusterSet(VERTEX u, vector<int>& partitions);
	//repartition
	void RepartitionKL(vector<ReAdjustPartitionPair>& adjust_partitions);
	void RepartitionMaxMin(vector<ReAdjustPartitionPair>& adjust_partitions);

	Cluster* MergeLeafofNode(int bt_node);
	
	//if the vex in _assign_vex of the partition is also in the cluster, the vex will be removed from _assign_vex since it has been selected and partitioned
	//To sure that the partition number is in the legal range, the function doesn't check the parameter.
	//void UpdateAssignVertices(int partition);
	
	//For current partitions, remove and insert vex. NOTE: for inserted vertices, the smaller size of partition will be preferred, otherwise, if all equal, random
	//the following functions will be used on one sampling process finished, and affect the partitioned vertices, not assigned vertices
	void RemoveClusterNode(vector<VERTEX>& vexs);
	void RandomInsertNewVertices(vector<VERTEX>& vexs);
	//insert a vertex into cluster. to be sure the vex exists in adj-table of sample graph
	void InsertNewVertexInCluster(Cluster* cluster, VERTEX& vex);

	//for debug
	int GetClusterNodeNumber();
};