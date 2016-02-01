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

using namespace std;

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

class Graph {
private:
	AdjTable _graph_data;
	//record the vexs labeled as REMOVED
	RemoveVertexList _removed_vex_list;
public:
	Graph(){};
	
	void ClearGraph();

	int GetVertexNumber(){return _graph_data._vex_table.size();}
	int GetEdgesNumber();
	
	AdjTable* GetAdjTableRef(){return &_graph_data;}
	
	EdgeInfoArray* GetAdjEdgeListofVertex(VERTEX& u);
	EdgeInfoArray* GetAdjEdgeListofPos(int pos);
	VertexInfo* GetVertexInfoofVertex(VERTEX& u);
	VertexInfo* GetVertexInfoofPos(int pos);
	//-1: not found. return the vertex's pos
	int GetVertexPos(VERTEX& u);
	
	//check if vex1 is connected by vex2 according to their pos
	bool isConnectbyPos(int vex1_pos, int vex2_pos);
	//check if vex1 is connected by vex2
	bool isConnect(VERTEX v1, VERTEX v2);
	//NOTE: to ensure the u does't exist by calling getvertexpos.
	int InsertVertex(VERTEX u);
	//the vertex u will set as REMOVED, and its idx will be kept
	void DeleteVertex(VERTEX& u);
	//if the e exists, do nothing
	void InsertEdge(EDGE e);
	//the edge will be deleted, but the ends will be kept
	void DeleteEdge(EDGE& e);
	//remove all the nodes labeled by REMOVED
	void RemoveDeletedVertex();

	void BuildGraphFromDir(string& dir);
	void BuildGraphFromFile(string& file);
	void BuildGraphFromEdgesCache(vector<EDGE>& edges_cache);

	void WriteGraphToFileByBFS(string& file);
	void WriteGraphToFileByDFS(string& file);
	
	//compute the lengths of the shortest paths from u to all the others . if no path, it's set by INT_MAX. 
	//the return vector is corresponding to the vector of graph vex table
	void ComputeShortestPathsFromVertex(VERTEX u, vector<int>& shortest_path_lens);

	//log the statistic of graph
	void doGraphStatistic();


};