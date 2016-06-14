#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <unordered_set>
#include <set>

using namespace std;

/**
Graph
*/
typedef unsigned __int32 VERTEX;

typedef struct _EDGE {
	VERTEX _u;
	VERTEX _v;
} EDGE, *PEDGE;

typedef struct _EdgeInfo {
	int			_adj_vex_pos;
} EdgeInfo;

typedef std::vector <EdgeInfo> EdgeInfoArray;

typedef enum _VertexStatus {
	NORM=0,
	REMOVED
} VertexStatus;

typedef struct _VertexInfo {
	VERTEX			_u;
	EdgeInfoArray	_edge_list;
	VertexStatus	_indicator;
	int				_degree;
	bool            _visisted; //dfs, bfs
} VertexInfo;

typedef std::vector <VertexInfo> VertexInfoArray;

typedef struct _AdjTable{
	VertexInfoArray _vex_table;
	map<int, int> _vertex_to_pos_idx;
} AdjTable;

typedef pair <int, int> Int_Pair;

typedef set<int, less<int>> RemoveVertexList;

typedef unsigned __int64 EdgeID;//this is used to cache the edges of sampling or dfs, bfs, to check if it sampled or visited

EdgeID MakeEdgeID(VERTEX u, VERTEX v);

EDGE GetEdgeofID(EdgeID eid);//u,v

/*
 vertex_v2 and edge_v2 are the types used by metis, fennel, dbs, sgp etc.
the var param is reserved for the extend by the application
*/
typedef struct _VERTEXV2 {
	VERTEX _vex;
	void* param;
} Vertex_v2, *PVertex_v2;

typedef struct _EDGEV2 {
	EDGE _e;
	void* param;
} Edge_v2, *PEdge_v2;

/**
SGPLoader
*/
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

typedef struct _EDGE_ITEM {
	double _weight;
	double _random;
	double _key;
} Edge_Item, PEdge_Item;

typedef struct _VERTEX_ITEM{
	int degree;
	bool _is_sampled;
} Vertex_Item, PVertex_Item;

//for SGLs - repartitioning the sample graph
//the whole steps of partitioning can be considered as a binary tree. so for the k=2^h partitions,
//the root's index is 0, and so on. if two partitions should be repartitioned, they should be siblings
//that is, their index satisfies floor(_part1/2) == floor(_part2/2).
typedef struct _ReAdjustPartitionPair {
	int _part1;
	int _part2;
	vector<VERTEX> _part1_changed_vex;
	vector<VERTEX> _part2_changed_vex;
} ReAdjustPartitionPair;