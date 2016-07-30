#include "StdAfx.h"
#include <time.h>
#include "Log.h"
#include <sstream>
#include "TimeTicket.h"
#include <hash_set>
#include "StreamPartiton.h"

using namespace std;
using namespace stdext; //for hash set

StreamPartiton::StreamPartiton(void)
{
}


StreamPartiton::~StreamPartiton(void)
{
}

bool StreamPartiton::ReadEdge(EDGE& e)
{
	string buf;

	while(getline(_ifs, buf))//empty line maybe exists
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

int hash_stream_partition_find_partition(hash_set<VERTEX>* partitions, int k, VERTEX u)
{
	for(int i=0; i<k; i++)
	{
		if(partitions[i].find(u) != partitions[i].end())
		{
			return i;
		}
	}
	int c = rand(0, k-1);
	return c;
}
void StreamPartiton::doHashStreamPartition()
{
	Log::logln("doHashStreamPartition.........");

	hash_set<VERTEX>* partitions = new hash_set<VERTEX>[_k];
	ofstream* ofs = new ofstream[_k];
	stringstream str;
	for(int i=0; i<_k; i++)
	{
		str.str("");
		str<<_outfile<<"_edge."<<i;
		ofs[i].open(str.str(), ios_base::trunc);
	}
	_ifs.open(_outfile);

	int iread = 0, interlinks =0, exterlinks=0, vex_num=0;
	EDGE e;
	while(ReadEdge(e))
	{
		int u_partition, v_partition;
		u_partition = hash_stream_partition_find_partition(partitions, _k, e._u);
		partitions[u_partition].insert(e._u);
		v_partition = hash_stream_partition_find_partition(partitions, _k, e._v);
		partitions[u_partition].insert(e._v);

		if(u_partition == v_partition)
		{
			ofs[u_partition]<<e._u<<" "<<e._v<<endl;
			interlinks++;
		}
		else
		{
			ofs[u_partition]<<e._u<<" "<<e._v<<endl;
			ofs[v_partition]<<e._u<<" "<<e._v<<endl;
			exterlinks++;
		}

		iread++;
	}
	//write the vertices
	ofstream* ofs_v = new ofstream;
	for(int i=0; i<_k; i++)
	{
		str.str("");
		str<<_outfile<<"_vertices."<<i;
		ofs_v->open(str.str(), ios_base::trunc);
		for(hash_set<VERTEX>::iterator iter = partitions[i].begin(); iter!=partitions[i].end(); iter++)
		{
			(*ofs_v)<<*iter<<endl;
			vex_num++;
		}
		ofs_v->close();
	}
	for(int i=0; i<_k; i++)
	{
		ofs[i].close();
	}

	delete[] ofs;
	delete ofs_v;
	delete[] partitions;
	_ifs.close();

	str.str("");
	str<<"k : \t"<<_k<<"\n"
		<<"Total Cut Value: \t"<<exterlinks<<"\n"
		<<"Total Edges: \t"<<exterlinks+interlinks<<"\n"
		<<"Total Vex: \t"<<vex_num<<"\n"
		<<"Total Elapse : \t"<<TimeTicket::total_elapse()<<"\n";
	Log::logln(str.str());

}

int balance_stream_partition_find_partition(hash_set<VERTEX>* partitions, int k, VERTEX u)
{
	for(int i=0; i<k; i++)
	{
		if(partitions[i].find(u) != partitions[i].end())
		{
			return i;
		}
	}

	int min_c =-1, min_size = INT_MAX;
	for(int i=0; i<k; i++)
	{
		if(partitions[i].size()<min_size)
		{
			min_c = i;
			min_size = partitions[i].size();
		}
	}
	return min_c;
}

void StreamPartiton::doBalanceStreamPartition()
{
	Log::logln("doBalanceStreamPartition.........");

	hash_set<VERTEX>* partitions = new hash_set<VERTEX>[_k];
	ofstream* ofs = new ofstream[_k];
	stringstream str;
	for(int i=0; i<_k; i++)
	{
		str.str("");
		str<<_outfile<<"_edge."<<i;
		ofs[i].open(str.str(), ios_base::trunc);
	}
	_ifs.open(_outfile);

	int iread = 0, interlinks =0, exterlinks=0, vex_num=0;
	EDGE e;
	while(ReadEdge(e))
	{
		int u_partition, v_partition;
		u_partition = balance_stream_partition_find_partition(partitions, _k, e._u);
		partitions[u_partition].insert(e._u);
		v_partition = hash_stream_partition_find_partition(partitions, _k, e._v);
		partitions[u_partition].insert(e._v);

		if(u_partition == v_partition)
		{
			ofs[u_partition]<<e._u<<" "<<e._v<<endl;
			interlinks++;
		}
		else
		{
			ofs[u_partition]<<e._u<<" "<<e._v<<endl;
			ofs[v_partition]<<e._u<<" "<<e._v<<endl;
			exterlinks++;
		}

		iread++;
	}
	//write the vertices
	ofstream* ofs_v = new ofstream;
	for(int i=0; i<_k; i++)
	{
		str.str("");
		str<<_outfile<<"_vertices."<<i;
		ofs_v->open(str.str(), ios_base::trunc);
		for(hash_set<VERTEX>::iterator iter = partitions[i].begin(); iter!=partitions[i].end(); iter++)
		{
			(*ofs_v)<<*iter<<endl;
			vex_num++;
		}
		ofs_v->close();
	}
	for(int i=0; i<_k; i++)
	{
		ofs[i].close();
	}

	delete[] ofs;
	delete ofs_v;
	delete[] partitions;
	_ifs.close();

	str.str("");
	str<<"k : \t"<<_k<<"\n"
		<<"Total Cut Value: \t"<<exterlinks<<"\n"
		<<"Total Edges: \t"<<exterlinks+interlinks<<"\n"
		<<"Total Vex: \t"<<vex_num<<"\n"
		<<"Total Elapse : \t"<<TimeTicket::total_elapse()<<"\n";
	Log::logln(str.str());

}
void StreamPartiton::doDeterministicGreadyStreamPartition()
{
}
void StreamPartiton::doLinearWeightedDeterministicGreadyStreamPartition()
{
}
void StreamPartiton::doExponentialWeightedDeterministicGreadyStreamPartition()
{
}
void StreamPartiton::doTriangleStreamPartition()
{
}
void StreamPartiton::doLinearTriangleStreamPartition()
{
}
void StreamPartiton::doExponentDeterministicTriangleStreamPartition()
{
}
void StreamPartiton::doNonNeighborStreamPartition()
{
}
