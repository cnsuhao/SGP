#pragma once
#include "commondef.h"
#include <fstream>

typedef struct _gdEdgeInfo {
	int			_adj_vex_pos;
} gdEdgeInfo;

typedef vector<gdEdgeInfo> gdEdgeInfoList;

typedef enum _gdVertexStatus {
	INMEM=0,//the edge list is in the memory
	INDISK  //the edge list is in the disk
} gdVertexStatus;

typedef struct _gdVertexInfo {
	VERTEX				_u;
	gdEdgeInfoList*		_edge_list;
	gdVertexStatus		_status;
	int					_degree;
} gdVertexInfo;

typedef enum _gdVertexCacheStatus {
	LOCKED = 0,
	UNLOCK
} gdVertexCacheStatus;

typedef struct _gdVertexCacheInfo {
	int _idx_row_adj_matrix;//idx of row of adj_matrix
	int _hit_count;//the access count of the edgelist of vex
	gdVertexCacheStatus _status;
} gdVertexCacheInfo;

typedef vector<gdVertexInfo> VertexInfoList;

typedef struct _gdAdjTable {
	VertexInfoList			_vertex_list;//all the vertices will be saved
	map<VERTEX, int>		_vertex_to_pos_idx;//vex ->the pos in _vertex_list
	vector<gdEdgeInfoList>	_adj_matrix; //the adj matrix of the partial graph with the limition of mem size _max_rows*_max_d in the memory
	map<VERTEX, gdVertexCacheInfo>	_cache_vex_list;//the vex list in the cache
	int _max_d;
	int _max_rows;
} AdjTable;

class GraphDisk
{
private:
	AdjTable _graph_data;
	string _tmp_file;
	string _graph_file;
	ofstream _ofs_tmp;
	ifstream _ifs_graph;

public:
	GraphDisk(void);
	~GraphDisk(void);

	void SetMaxDegree(int d) {_graph_data._max_d = d;};
	int GetMaxDegree() {return _graph_data._max_d;};

	void SetMaxRows(int r) {_graph_data._max_rows = r;};
	int GetMaxRows() {return _graph_data._max_rows;};

	void SetTmpFile(string f) { _tmp_file = f;};
	void SetGraphFile(string f) {_graph_file = f;};

	void InitAdjTable();

	//return the count of edges read
	int BuildAdjTable();

	//return the pos after inserted
	int InsertVertex(VERTEX u);
	//if the e exists or self-edge, do nothing
	bool InsertEdge(EDGE e);
	int GetVertexPos(VERTEX& u);
	gdVertexInfo* GetVertexInfoofPos(VERTEX& u);
	gdVertexInfo* GetVertexInfoofPos(int pos);
	bool isConnectbyPos(int vex1_pos, int vex2_pos);
	//NOTE: the corresponding edgelist will be locked to avoid swithing out.
	//the caller is reponsible for unlocking it by calling unlockvertex.
	gdEdgeInfoList* GetAdjEdgeListofPos(int pos);

	int SwitchInEdgeList(int pos);
	void LockVertex(VERTEX& u);
	void LockVertexofPos(int pos);
	void UnLockVertex(VERTEX& u);
	void UnLockVertexofPos(int pos);

	bool ReadEdge(EDGE& e);
};

