#pragma once
#include<vector>
using namespace std;

//statistic information of a partition
typedef struct _PartitionStatisticInfo {
	int	_partition_vex_number; //the number of vertex of a partition
	int _assign_vex_number;//the number of assigned vertex
	int _partition_external_links; // external links in the partition
	int _partition_internal_links; // internal links in the partition
	int _assign_external_links; //external links in assigned partition
	int _assign_internal_links; //internal links in assigned partition
} PartitionStatisticInfo;

class Statistic
{
public:
	Statistic(int k);
	~Statistic(void);
private:
	//the statistic informations of partitions. these informations will be filled up at the step of writing
	vector<PartitionStatisticInfo> _partitions_statistic;
	int _repartition_count;
	vector<int> _partition_exchange_count_array;
	int _k;
	int _sample_hit;//采样置换次数
	double _total_elapse;

public:
	void IncreasePartitionInternalLinks(int partition);
	void IncreasePartitionExternalLinks(int partition);
	void IncreaseAssignInternalLinks(int partition);
	void IncreaseAssignExternalLinks(int partition);
	int GetPartitionInternalLinks(int partition);
	int GetPartitionExternalLinks(int partition);
	int GetAssignInternalLinks(int partition);
	int GetAssignExternalLinks(int partition);

	void IncreaseSampleHit() {_sample_hit++;};
	int GetSampleHit(){return _sample_hit;};

	void SetTotalElapse(double elapse){_total_elapse = elapse;};
	double GetTotalElapse(){return _total_elapse;};

	void IncreaseRepartitionCount();
	int GetRepartitionCount();

	void AppendPartitionExchangeCount(int exchange_count);
	
	int GetTotalCutValue();
	int GetPartitionCutValue();
	int GetTotalInternalLinks();
	int GetTotalExternalLinks();
	int GetTotalPartitionInternalLinks();
	int GetTotalPartitionExternalLinks();
	int GetTotalAssignInternalLinks();
	int GetTotalAssignExternalLinks();

	void SetPartitionVexNumber(int partition, int n);
	void SetAssignVexNumber(int partition, int n);
	void IncreaseAssignVexNumber(int partition){_partitions_statistic[partition]._assign_vex_number++;};
	int GetPartitionVexNumber(int partition){return _partitions_statistic[partition]._partition_vex_number;};
	int GetAssignVexNumber(int partition){return _partitions_statistic[partition]._assign_vex_number;};
	int GetTotalAssignVexNumber();
	int GetTotalPartitionVexNumber();
	int GetTotalVexNumber();
	

	void ResetPartitionInfos();
	void ResetPartitionInfoofAssignLinks();

	void printStatistic();
};

