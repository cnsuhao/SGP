#include "StdAfx.h"
#include "GraphDisk.h"
#include "Log.h"
#include <sstream>
#include "TimeTicket.h"
#include <hash_set>

using namespace std;
using namespace stdext; //for hash set

GraphDisk::GraphDisk(void)
{
}


GraphDisk::~GraphDisk(void)
{
}

bool GraphDisk::ReadEdge(EDGE& e)
{
	string buf;

	while(getline(_ifs_graph, buf))//empty line maybe exists
	{	
		if(buf.empty()) continue;

		int idx = buf.find_first_of(" ");
		string temp = buf.substr(0, idx);
		int u = stoi(temp);
		temp = buf.substr(idx+1, buf.length()-idx-1);
		int v= stoi(temp);
		e._u = u;
		e._v = v;
		return true;
	}
	return false;
}


void GraphDisk::InitAdjTable()
{
	_ifs_graph.open(_graph_file);
	_ofs_tmp.open(_tmp_file, ios::trunc|ios::binary);
}

int GraphDisk::BuildAdjTable()
{
	int iread = 0;
	EDGE e;
	while(ReadEdge(e))
	{
		if(InsertEdge(e))
		{
			_graph_data._edge_count++;
		}
		iread++;
	}
	Flush();
	return iread;
}

int GraphDisk::InsertVertex(VERTEX u)
{
	map<VERTEX, int>::iterator iter = _graph_data._vertex_to_pos_idx.find(u);
	if(iter == _graph_data._vertex_to_pos_idx.end())
		return -1;

	gdVertexInfo v_info;
	v_info._degree = 0;
	v_info._status = INDISK;
	v_info._u = u;

	_graph_data._vertex_list.push_back(v_info);
	int pos = _graph_data._vertex_list.size()-1;
	_graph_data._vertex_to_pos_idx.insert(pair<VERTEX,int>(u, pos));

	if(_graph_data._adj_matrix.size()<_graph_data._max_rows)//put the edgelist in the cache
	{
		gdEdgeInfoList edge_info_list;
		_graph_data._adj_matrix.push_back(edge_info_list);

		gdVertexCacheInfo info = {_graph_data._adj_matrix.size()-1, 1, UNLOCK};
		_graph_data._cache_vex_list.insert(pair<VERTEX, gdVertexCacheInfo>(u, info));
	}
	else
	{
		WriteEdgeList(pos, NULL);//write into tmp file
	}
		
	return pos;
}

bool GraphDisk::InsertEdge(EDGE e)
{
	int u_pos = -1, v_pos = -1;
	u_pos = GetVertexPos(e._u);//fix£ºif u == v
	if(u_pos<0){
		u_pos = InsertVertex(e._u);
	}
	v_pos = GetVertexPos(e._v);
	if(v_pos<0){
		v_pos = InsertVertex(e._v);
	}

	if(isConnectbyPos(u_pos, v_pos))//the edge exists
		return false;

	gdEdgeInfo u_adj, v_adj;
	u_adj._adj_vex_pos = v_pos;
	v_adj._adj_vex_pos = u_pos;

	GetAdjEdgeListofPos(u_pos)->push_back(u_adj);
	GetVertexInfoofPos(u_pos)->_degree++;
	GetAdjEdgeListofPos(v_pos)->push_back(v_adj);
	GetVertexInfoofPos(v_pos)->_degree++;
	UnLockVertexofPos(u_pos);
	UnLockVertexofPos(v_pos);
	return true;

}

int GraphDisk::GetVertexPos(VERTEX& u)
{
	map<VERTEX, int>::iterator iter = _graph_data._vertex_to_pos_idx.find(u);
	if(iter == _graph_data._vertex_to_pos_idx.end())
		return -1;
	else
		return iter->second;
}

bool GraphDisk::isConnectbyPos(int vex1_pos, int vex2_pos)
{
	if(vex1_pos == vex2_pos) return true;

	gdEdgeInfoList* vex1_edges = GetAdjEdgeListofPos(vex1_pos);
	for(gdEdgeInfoList::iterator iter = vex1_edges->begin(); iter!=vex1_edges->end(); iter++)
	{
		if(iter->_adj_vex_pos == vex2_pos) return true;
	}
	return false;
}

gdVertexInfo* GraphDisk::GetVertexInfoofPos(VERTEX& u)
{
	int pos = GetVertexPos(u);
	return GetVertexInfoofPos(pos);
}

gdVertexInfo* GraphDisk::GetVertexInfoofPos(int pos)
{
	if(pos >= _graph_data._vertex_list.size()||pos<0)
		return NULL;
	
	return &(_graph_data._vertex_list.at(pos));
}

VERTEX GraphDisk::GetVertexAtPos(int pos)
{
	if(pos<0 || pos>_graph_data._vertex_list.size()-1)
		return -1;

	return _graph_data._vertex_list.at(pos)._u;
}

gdEdgeInfoList* GraphDisk::GetAdjEdgeListofVex(VERTEX& u)
{
	int pos = GetVertexPos(u);
	return GetAdjEdgeListofPos(pos);
}

gdEdgeInfoList* GraphDisk::GetAdjEdgeListofPos(int pos)
{
	gdVertexInfo* v_info = GetVertexInfoofPos(pos);
	if(v_info == NULL)
		return NULL;

	switch(v_info->_status)
	{
	case INMEM:
		{
			map<VERTEX, gdVertexCacheInfo>::iterator iter = _graph_data._cache_vex_list.find(v_info->_u);
			if(iter != _graph_data._cache_vex_list.end())
			{
				int pos_in_cache = iter->second._idx_row_adj_matrix;
				iter->second._hit_count++;
				LockVertexofPos(pos);
				return &(_graph_data._adj_matrix.at(pos_in_cache));
			}
			else
			{
				Log::logln("Error::GraphDisk::GetAdjEdgeListofPos::not found the edgelist in the cache");
				return NULL;
			}
			break;
		}
	case INDISK:
		{
			int pos_in_cache = SwitchInEdgeList(pos);
			if(pos_in_cache <= -1)
			{
				Log::logln("Error::GraphDisk::GetAdjEdgeListofPos:: an error thrown on switching the edgelist from the disk");
				return NULL;
			}
			else
			{
				LockVertexofPos(pos);
				return &(_graph_data._adj_matrix.at(pos_in_cache));
			}

			break;
		}
	default:
		{
			Log::logln("Error::GraphDisk::GetAdjEdgeListofPos:: unknown status");
			return NULL;
		}
	}
}

int GraphDisk::SwitchInEdgeList(int switchin_vex_pos)
{
	int switchout_idx_row_adj_matrix = -1, min_hit = INT_MAX, switchout_vex_pos=-1;
	VERTEX switchout_vex = -1, switchin_vex = -1;
	
	//find the switchout
	for (map<VERTEX, gdVertexCacheInfo>::iterator iter = _graph_data._cache_vex_list.begin();
		iter!=_graph_data._cache_vex_list.end();
		iter++)
	{
		if(iter->second._hit_count<min_hit && 
			!isLockVertex(iter->first))
		{
			min_hit = iter->second._hit_count;
			switchout_idx_row_adj_matrix = iter->second._idx_row_adj_matrix;
			switchout_vex = iter->first;
		}
	}
	//error check
	if(switchout_idx_row_adj_matrix == -1)
		return -1;

	switchout_vex_pos = GetVertexPos(switchout_vex);
	if(switchout_vex_pos == -1)
		return -1;
	
	//read and write
	gdEdgeInfoList* switchout_edgelist = &(_graph_data._adj_matrix.at(switchout_idx_row_adj_matrix));
	WriteEdgeList(switchout_vex_pos, switchout_edgelist);
	FillEdgeList(switchin_vex_pos, switchout_edgelist);

	//update _cache_vex_list
	//remove the switchout from the cache
	map<VERTEX, gdVertexCacheInfo>::iterator iter = _graph_data._cache_vex_list.find(switchout_vex);
	_graph_data._cache_vex_list.erase(iter);//don't check if exists
	//insert the switchin
	switchin_vex = GetVertexInfoofPos(switchin_vex_pos)->_u;
	gdVertexCacheInfo switchin_info = {switchout_idx_row_adj_matrix, 1, UNLOCK};
	_graph_data._cache_vex_list.insert(pair<VERTEX, gdVertexCacheInfo>(switchin_vex, switchin_info));

	return switchout_idx_row_adj_matrix;
}

void GraphDisk::LockVertex(VERTEX& u)
{
	map<VERTEX, gdVertexCacheInfo>::iterator iter = _graph_data._cache_vex_list.find(u);
	if(iter != _graph_data._cache_vex_list.end())
	{
		iter->second._status = LOCKED;
	}
	else
	{
		Log::logln("Error::GraphDisk::LockVertex::the vertex in cache not found");
	}

}
void GraphDisk::LockVertexofPos(int pos)
{
	VERTEX u = GetVertexAtPos(pos);
	if(u == -1)
	{
		Log::logln("Error::GraphDisk::LockVertexofPos::the vertex info not found");
		return;
	}

	LockVertex(u);

}
void GraphDisk::UnLockVertex(VERTEX& u)
{
	map<VERTEX, gdVertexCacheInfo>::iterator iter = _graph_data._cache_vex_list.find(u);
	if(iter != _graph_data._cache_vex_list.end())
	{
		iter->second._status = UNLOCK;
	}
	else
	{
		Log::logln("Error::GraphDisk::UnLockVertex::the vertex in cache not found");
	}

}
void GraphDisk::UnLockVertexofPos(int pos)
{
	VERTEX u = GetVertexAtPos(pos);
	if(u == -1)
	{
		Log::logln("Error::GraphDisk::UnLockVertexofPos::the vertex info not found");
		return;
	}

	UnLockVertex(u);
}

bool GraphDisk::isLockVertex(VERTEX u)
{
	map<VERTEX, gdVertexCacheInfo>::iterator iter = _graph_data._cache_vex_list.find(u);
	if(iter != _graph_data._cache_vex_list.end())
	{
		if(iter->second._status == LOCKED)
			return true;
		else
			return false;
	}
	else
	{
		Log::logln("Error::GraphDisk::isLockVertex::the vertex in cache not found");
		return false;
	}
}

bool GraphDisk::isLockVertexofPos(int pos)
{
	VERTEX u = GetVertexAtPos(pos);
	if(u == -1)
	{
		Log::logln("Error::GraphDisk::isLockVertexofPos::the vertex info not found");
		return false;
	}
	return isLockVertex(u);
}


void GraphDisk::WriteEdgeList(int vex_pos, gdEdgeInfoList* edges)
{
	int max_pos = GetVertexNum();//maybe append at the end with the condition of max_pos == vex_pos
	if(vex_pos > max_pos)
	{
		Log::logln("Error::GraphDisk::WriteEdgeList::Error Position");
		return;
	}
	gdVertexInfo* v_info = GetVertexInfoofPos(vex_pos);
	if(v_info == NULL)
	{
		Log::logln("Error::GraphDisk::WriteEdgeList::Not found the vertex");
		return;
	}

	VERTEX u =v_info->_u;
	int d = v_info->_degree;
	int write_pos = vex_pos*GetAdjLineSize();
	_ofs_tmp.seekp(write_pos, ios::beg);
	
	_ofs_tmp<<u<<d;
	for(int i=0; i<GetMaxDegree(); i++)
	{
		int adj = -1;
		if(edges == NULL || i>edges->size())
		{
			adj = -1;
		}
		else
		{
			adj = edges->at(i)._adj_vex_pos;
		}
		_ofs_tmp<<adj;
	}
}

void GraphDisk::FillEdgeList(int vex_pos,gdEdgeInfoList* edges)
{
	int max_pos = GetVertexNum()-1;
	if(vex_pos > max_pos)
	{
		Log::logln("Error::GraphDisk::WriteEdgeList::Error Position");
		return;
	}

	edges->clear();
	
	VERTEX u = -1;
	int d = -1;
	gdEdgeInfo adj_info = {-1};
	int read_pos = vex_pos*GetAdjLineSize();
	_ofs_tmp.seekg(read_pos, ios::beg);

	_ofs_tmp>>u;
	_ofs_tmp>>d;
	for(int i=0; i<GetMaxDegree(); i++)
	{
		_ofs_tmp>>adj_info._adj_vex_pos;
		edges->push_back(adj_info);
	}
}

void GraphDisk::Flush()
{
	for (map<VERTEX, gdVertexCacheInfo>::iterator iter = _graph_data._cache_vex_list.begin();
		iter!=_graph_data._cache_vex_list.end();
		iter++)
	{
		VERTEX u = iter->first;
		int u_pos = GetVertexPos(u);
		if(u_pos == -1)
		{
			Log::logln("Error::GraphDisk::Flush::the pos of the vex in the cache not found");
		}
		else
		{
			int idx_row_adj_matrix = iter->second._idx_row_adj_matrix;
			gdEdgeInfoList* edge_info_list = &(_graph_data._adj_matrix.at(idx_row_adj_matrix));
			WriteEdgeList(u_pos, edge_info_list);
		}
	}
}