#pragma once
/*
This class implements the alogrithm proposed in the paper "FENNEL: streaming graph partitioning for massive scale graph", 
which assigns vetex v to partition i such that $\delta g(v, S_i) \ge \delta g(v, S_j)$, for all $j \in [k]$.
where $\delta g(x)$ is the function of $\alpha \gamma x^{\gamma-1}$, here $\gamma = 3/2, \alpha = sqrt(k) m/(n^{3/2})$ set by the paper
*/
class FENNEL
{
public:
	FENNEL(void);
	~FENNEL(void);
};

