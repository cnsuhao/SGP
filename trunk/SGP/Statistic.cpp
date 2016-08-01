#include "StdAfx.h"
#include "Statistic.h"
#include "Log.h"
#include <sstream>

Statistic::Statistic(int k)
{
	_k = k;
	_repartition_count=0;
	_sample_hit = 0;

	for(int i=0; i<_k; i++)
	{
		PartitionStatisticInfo info;
		info._assign_external_links = 0;
		info._assign_internal_links = 0;
		info._assign_vex_number = 0;
		info._partition_external_links = 0;
		info._partition_internal_links = 0;
		info._partition_vex_number = 0;
		_partitions_statistic.push_back(info);
	}
}


Statistic::~Statistic(void)
{
}

void Statistic::IncreasePartitionInternalLinks(int partition)
{
	_partitions_statistic[partition]._partition_internal_links++;
}
void Statistic::IncreasePartitionExternalLinks(int partition)
{
	_partitions_statistic[partition]._partition_external_links++;
}
void Statistic::IncreaseAssignInternalLinks(int partition)
{
	_partitions_statistic[partition]._assign_internal_links++;
}
void Statistic::IncreaseAssignExternalLinks(int partition)
{
	_partitions_statistic[partition]._assign_external_links++;
}
void Statistic::IncreaseRepartitionCount()
{
	_repartition_count++;
}
void Statistic::AppendPartitionExchangeCount(int exchange_count)
{
	_partition_exchange_count_array.push_back(exchange_count);
}
int Statistic::GetPartitionInternalLinks(int partition)
{
	return _partitions_statistic[partition]._partition_internal_links;
}
int Statistic::GetPartitionExternalLinks(int partition)
{
	return _partitions_statistic[partition]._partition_external_links;
}	
int Statistic::GetAssignInternalLinks(int partition)
{
	return _partitions_statistic[partition]._assign_internal_links;
}	
int Statistic::GetAssignExternalLinks(int partition)
{
	return _partitions_statistic[partition]._assign_external_links;
}	
int Statistic::GetRepartitionCount()
{
	return _repartition_count;
}	
int Statistic::GetTotalCutValue()
{
	return GetTotalExternalLinks();
}

int Statistic::GetPartitionCutValue()
{
	int sum = 0;
	for(int i=0; i<_k; i++)
	{
		sum += _partitions_statistic[i]._partition_external_links;
	}
	return sum/2;
}

int Statistic::GetTotalInternalLinks()
{
	int sum = 0;
	for(int i=0; i<_k; i++)
	{
		sum += _partitions_statistic[i]._assign_internal_links+_partitions_statistic[i]._partition_internal_links;
	}
	return sum;
}	
int Statistic::GetTotalExternalLinks()
{
	int sum = 0;
	for(int i=0; i<_k; i++)
	{
		sum += _partitions_statistic[i]._assign_external_links+_partitions_statistic[i]._partition_external_links;
	}
	return sum/2;
}

void Statistic::SetPartitionVexNumber(int partition, int n)
{
	_partitions_statistic[partition]._partition_vex_number = n;
}

void Statistic::SetAssignVexNumber(int partition, int n)
{
	_partitions_statistic[partition]._assign_vex_number = n;
}

void Statistic::printStatistic()
{
	int cutvalue = 0;
	stringstream str;
	int max_size = 0;
	for(int i=0; i<_k; i++)
	{
		int size = GetPartitionVexNumber(i)+GetAssignVexNumber(i);
		if(size > max_size)
			max_size = size;

		str.str("");
		str	<<"\n\n>>Partition "<<i<<" Statistic \n"
			<<"Partition Vertex number: \t"<<GetPartitionVexNumber(i)<<"\n"
			<<"Assigned Vertex number: \t"<<GetAssignVexNumber(i)<<"\n"
			<<"Partition Internal Edges: \t"<<GetPartitionInternalLinks(i)<<"\n"
			<<"Partition External Edges: \t"<<GetPartitionExternalLinks(i)<<"\n"
			<<"Assign Internal Edges: \t"<<GetAssignInternalLinks(i)<<"\n"
			<<"Assign External Edges: \t"<<GetAssignExternalLinks(i)<<"\n";
		Log::log(str.str());
	}

	str.str("");
	str<<"\n>>In Total \n Total Cut Value: \t"<<GetTotalCutValue()<<"\n"
		<<"Parititoner Cut Value: \t"<<GetPartitionCutValue()<<"\n"
		<<"Total Partition Internal Edges: \t"<<GetTotalPartitionInternalLinks()<<"\n"
		<<"Total Partition External Edges: \t"<<GetTotalPartitionExternalLinks()<<"\n"
		<<"Total Assign Internal Edges: \t"<<GetTotalAssignInternalLinks()<<"\n"
		<<"Total Assign External Edges: \t"<<GetTotalAssignExternalLinks()<<"\n"
		<<"Total Internal Edges: \t"<<GetTotalInternalLinks()<<"\n"
		<<"Total External Edges: \t"<<GetTotalExternalLinks()<<"\n"
		<<"Total Edges: \t"<<GetTotalInternalLinks()+GetTotalExternalLinks()<<"\n"
		<<"Total Partition Vex: \t"<<GetTotalPartitionVexNumber()<<"\n"
		<<"Total Assign Vex: \t"<<GetTotalAssignVexNumber()<<"\n"
		<<"Total  Vex: \t"<<GetTotalVexNumber()<<"\n"
		<<"Repartition Count: \t"<<GetRepartitionCount()<<"\n"
		<<"Sample Hit Count: \t"<<GetSampleHit()<<"\n"
		<<"Exchange Count In Each Partition:\n";
	for(vector<int>::iterator iter = _partition_exchange_count_array.begin();
		iter!=_partition_exchange_count_array.end();
		iter++)
	{
		str<<*iter<<"\t";
	}
	str<<"\nCut/Edges: \t"<<GetTotalCutValue()*1.0f/(GetTotalInternalLinks()+GetTotalExternalLinks())<<"\n"
	<<"MaxParitionSize/AvgSize: \t"<<max_size*_k*1.0f/GetTotalVexNumber()<<"\n";
	str<<"\nTotal Elapse : \t"<<GetTotalElapse()<<"\n";
	Log::logln(str.str());
}

void Statistic::ResetPartitionInfos()
{
	for(int i=0; i<_k; i++)
	{
		_partitions_statistic[i]._assign_external_links = 0;
		_partitions_statistic[i]._assign_internal_links = 0;
		_partitions_statistic[i]._assign_vex_number = 0;
		_partitions_statistic[i]._partition_external_links = 0;
		_partitions_statistic[i]._partition_internal_links = 0;
		_partitions_statistic[i]._partition_vex_number = 0;
	}
	_repartition_count=0;
	_sample_hit = 0;
	_partition_exchange_count_array.clear();
}

void Statistic::ResetPartitionInfoofAssignLinks()
{
	for(int i=0; i<_k; i++)
	{
		_partitions_statistic[i]._assign_external_links = 0;
		_partitions_statistic[i]._assign_internal_links = 0;
	}
}

int Statistic::GetTotalAssignVexNumber()
{
	int sum = 0;
	for(int i=0; i<_k; i++)
	{
		sum += _partitions_statistic[i]._assign_vex_number;
	}
	return sum;
}
int Statistic::GetTotalPartitionVexNumber()
{
	int sum = 0;
	for(int i=0; i<_k; i++)
	{
		sum += _partitions_statistic[i]._partition_vex_number;
	}
	return sum;
}
int Statistic::GetTotalVexNumber()
{
	return GetTotalPartitionVexNumber()+GetTotalAssignVexNumber();
}

int Statistic::GetTotalPartitionInternalLinks()
{
	int sum = 0;
	for(int i=0; i<_k; i++)
	{
		sum += _partitions_statistic[i]._partition_internal_links;
	}
	return sum;
}
int Statistic::GetTotalPartitionExternalLinks()
{
	int sum = 0;
	for(int i=0; i<_k; i++)
	{
		sum += _partitions_statistic[i]._partition_external_links;
	}
	return sum/2;
}
int Statistic::GetTotalAssignInternalLinks()
{
	int sum = 0;
	for(int i=0; i<_k; i++)
	{
		sum += _partitions_statistic[i]._assign_internal_links;
	}
	return sum;
}
int Statistic::GetTotalAssignExternalLinks()
{
	int sum = 0;
	for(int i=0; i<_k; i++)
	{
		sum += _partitions_statistic[i]._assign_external_links;
	}
	return sum/2;
}
