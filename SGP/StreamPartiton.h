#pragma once
#include "commondef.h"
#include <fstream>

/*
This class implements the algorithm in the paper of " streaming graph partitioning for large distributed graphs ", including:
1. hash partitoin: place a vertex to a cluster chosen uniformly at random
2. Balance: place a vertex to the cluster with minimal size
3. Deterministic Greedy: place a vertex to the cluster that maximizes |N(v) intersect Si|, N(v) is the neighbors of v, Si is the cluster i.
4. Linear Weighted Deterministic Greedy: place a vertex to the cluster that maximizes |N(v) intersect Si| * (1-|Si|/(n/k))
5. Exponentially Weighted Deterministic Greedy: place a vertex to the cluster that maximizes |N(v) intersect Si| * (1-exp(|Si|-(n/k)))
6. Triangles: place a vertex to the cluster that maximizes tSi(v)
7. Linear Weighted  Triangles: place a vertex to the cluster that maximizes tSi(v) * (1-|Si|/(n/k))
8. Exponentially  Deterministic Triangles: place a vertex to the cluster that maximizes tSi(v) * (1-exp(|Si|-(n/k)))
9. Non-Neighbors: place a vertex to the cluster that minimizes |Si \ N(v)|
*/
class StreamPartiton
{
private:
	string		_graph_file; //graph file input
	string		_outfile;//output file;
	ifstream	_ifs;
	int			_k;
	//graph disk parameter
	int			_max_d;
	int			_max_rows;

	bool ReadEdge(EDGE& e);
	//hash
	void doHashStreamPartition();
	//balance
	void doBalanceStreamPartition();
	//DG
	void doDeterministicGreadyStreamPartition();
	//LDG
	void doLinearWeightedDeterministicGreadyStreamPartition();
	//EDG
	void doExponentialWeightedDeterministicGreadyStreamPartition();
	//Tri
	void doTriangleStreamPartition();
	//LTri
	void doLinearTriangleStreamPartition();
	//EDTri
	void doExponentDeterministicTriangleStreamPartition();
	//NN
	void doNonNeighborStreamPartition();

public:
	StreamPartiton(void);
	~StreamPartiton(void);

	void SetMaxDegree(int d) {_max_d = d;};
	void SetMaxRows(int r) {_max_rows = r;};
	void SetGraphFile(string f) {_graph_file = f;};
	void SetOutFile(string f) {_outfile = f;};
	void SetK(int k) {_k = k;};

	void doStreamPartition(StreamPartitionMeasure measure);
	
};

