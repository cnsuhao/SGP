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
	_ofs_tmp.open(_tmp_file);
}

int GraphDisk::BuildAdjTable()
{
	int iread = 0;
	EDGE e;
	while(ReadEdge(e))
	{

		iread++;
	}

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
		return;

	gdEdgeInfo u_adj, v_adj;
	u_adj._adj_vex_pos = v_pos;
	v_adj._adj_vex_pos = u_pos;

	GetAdjEdgeListofPos(u_pos)->push_back(u_adj);
	GetVertexInfoofPos(u_pos)->_degree++;
	GetAdjEdgeListofPos(v_pos)->push_back(v_adj);
	GetVertexInfoofPos(v_pos)->_degree++;
	UnLockVertexofPos(u_pos);
	UnLockVertexofPos(v_pos);

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

int GraphDisk::SwitchInEdgeList(int pos)
{

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
	gdVertexInfo* v_info = GetVertexInfoofPos(pos);
	if(v_info == NULL)
	{
		Log::logln("Error::GraphDisk::LockVertexofPos::the vertex info not found");
		return;
	}

	LockVertex(v_info->_u);

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
	gdVertexInfo* v_info = GetVertexInfoofPos(pos);
	if(v_info == NULL)
	{
		Log::logln("Error::GraphDisk::UnLockVertexofPos::the vertex info not found");
		return;
	}

	UnLockVertex(v_info->_u);

}
