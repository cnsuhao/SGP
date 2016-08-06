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
	int _max_edges;
	int _edges_count_in_mem;
	int _total_edge_count;
} gdAdjTable;

typedef struct _gdTempFileInfo
{
	string _tmp_file;
	vector<fstream*> _tmp_file_handle_list;
	int _line_bytes;//the len of each line(byte);
	int _max_size;//max bytes for each tmp file;
	int _max_line;//max line for each tmp : _max_line = floor(_max_size/_line_bytes);
} gdTempFileInfo;

typedef struct _gdFilePhysicalPos
{
	fstream* _fs;
	int _pos;
} gdFilePhysicalPos;

class GraphDisk
{
private:
	gdAdjTable _graph_data;

	string _graph_file;
	ifstream _ifs_graph;
	//adjtable temp file (binary): vertex|degree|adj_pos1....adj_pos_{max_d}
	gdTempFileInfo _temp_file_info;
	

	/*******temp file management********/
	//write the vex at the vex_pos in the adjtable, and its edgelist
	//if edges is null, write a default value list of -1
	void WriteEdgeList(int vex_pos, gdEdgeInfoList* edges);
	void FillEdgeList(int vex_pos,gdEdgeInfoList* edges);
	void Flush();
	//check if all temp files reach max size
	bool isTempFull();
	void CreateNewTempFile();
	//To confirm the input parameter is legal.
	gdFilePhysicalPos GetPhysicalPos(int vex_pos);

	int GetAdjLineSize(){
		return 
		sizeof(VERTEX)					//vertex
		+sizeof(int)					//degree
		+_graph_data._max_d*sizeof(int);//adj_pos list
	};

	//read the edgelist at the pos in the temp file into the cache
	//return the pos of the switchin edgelist in the cache after switching
	int SwitchInEdgeList(int switchin_vex_pos);

public:
	GraphDisk(void);
	~GraphDisk(void);

	void SetMaxDegree(int d) {_graph_data._max_d = d;};
	int GetMaxDegree() {return _graph_data._max_d;};

	//void SetMaxRows(int r) {_graph_data._max_rows = r;};
	//int GetMaxRows() {return _graph_data._max_rows;};
	void SetMaxEdges(int r) {_graph_data._max_edges = r;};
	int GetMaxEdges() {return _graph_data._max_edges;};
	
	void SetGraphFile(string f) {_graph_file = f;};

	int GetVertexNum() {return _graph_data._vertex_list.size();};
	int GetEdgeNum() {return _graph_data._total_edge_count;};

	void InitAdjTable();

	//temp file management
	void InitTempFile(string& filename, int file_max_bytes);

	//return the count of edges read
	int BuildAdjTable();

	//return the pos after inserted
	int InsertVertex(VERTEX u);
	//if the e exists or self-edge, do nothing
	bool InsertEdge(EDGE e);
	int GetVertexPos(VERTEX& u);
	VERTEX GetVertexAtPos(int pos);
	gdVertexInfo* GetVertexInfoofPos(VERTEX& u);
	gdVertexInfo* GetVertexInfoofPos(int pos);
	bool isConnectbyPos(int vex1_pos, int vex2_pos);
	bool isConnectbyVex(VERTEX u, VERTEX v);
	//NOTE: the corresponding edgelist will be locked to avoid swithing out.
	//the caller is reponsible for unlocking it by calling unlockvertex.
	gdEdgeInfoList* GetAdjEdgeListofPos(int pos);
	gdEdgeInfoList* GetAdjEdgeListofVex(VERTEX& u);

	VertexInfoList* GetVertexList() {return &(_graph_data._vertex_list);};
	void LockVertex(VERTEX& u);
	void LockVertexofPos(int pos);
	void UnLockVertex(VERTEX& u);
	void UnLockVertexofPos(int pos);
	bool isLockVertex(VERTEX u);
	bool isLockVertexofPos(int pos);

	bool ReadEdge(EDGE& e);
};

