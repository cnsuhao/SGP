#pragma once
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
public:
	StreamPartiton(void);
	~StreamPartiton(void);
};

