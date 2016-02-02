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

#include "commondef.h"

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
	void BuildGraphFromEdgesCache(map<EdgeID, DBS_Edge_Item>& edges_cache);

	void WriteGraphToFileByBFS(string& file);
	void WriteGraphToFileByDFS(string& file);
	
	//compute the lengths of the shortest paths from u to all the others . if no path, it's set by INT_MAX. 
	//the return vector is corresponding to the vector of graph vex table
	void ComputeShortestPathsFromVertex(VERTEX u, vector<int>& shortest_path_lens);

	//log the statistic of graph
	void doGraphStatistic();


};