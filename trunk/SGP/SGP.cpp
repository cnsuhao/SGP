// SGP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <vector>
#include "SGPLoader.h"
#include "Log.h"
#include <sstream>
#include "TimeTicket.h"

using namespace std;

string usage = 
	"sgp -CMD [DFS|BFS|RAND|KL|MaxMin|SGPKL|SGPMaxMin|SGPStreamKL|SGPStreamMaxMin|Test] ...\n"
	"DFS:\n"
	"convert edges order by dfs.\n"
	"params: -i <inputfile> -o <outputfile> -log <log file>\n"
	"BFS:\n"
	"convert edges order by bfs.\n"
	"params: -i <inputfile> -o <outputfile> -log <log file>\n"
	"RAND:\n"
	"convert edges order by random.\n"
	"params: -i <inputfile> -o <outputfile> -log <log file>\n"
	"KL:\n"
	"partitioning a graph by kl algorithm in memory.\n"
	"params: -i <inputfile> -o <outputdir> -k <clusters num> -log <log file>\n"
	"MaxMin:\n"
	"partitioning a graph by max-min algorithm in memory.\n"
	"params: -i <inputfile> -o <outputdir> -k <clusters num> -log <log file>\n"
	"SGPKL:\n"
	"partitioning a graph by sgp of kl algorithm.\n"
	"params: -i <inputfile> -o <outputdir> -k <clusters num> -m <edge order: dfs, bfs, random> -log <log file> -aw <assign window size> -ew <edges limits> -sm <sample mode: eq, uneq,dbs> -maxd <max degree if sample mode is uneq>\n"
	"SGPMaxMin:\n"
	"partitioning a graph by sgp of max-min algorithm\n"
	"params: -i <inputfile> -o <outputdir> -k <clusters num> -m <edge order: dfs, bfs, random> -log <log file> -aw <assign window size> -ew <edges limits> -sm <sample mode: eq, uneq,dbs> -maxd <max degree if sample mode is uneq>\n"
	"SGPStreamKL:\n"
	"partitioning a graph by stream sgp of kl algorithm.\n"
	"params: -i <inputfile> -o <outputdir> -k <clusters num> -m <edge order: dfs, bfs, random> -log <log file> -aw <assign window size> -ew <edges limits> -sm <sample mode: eq, uneq,dbs> -maxd <max degree if sample mode is uneq> -ec <edge cache size for dbs>\n"
	"SGPStreamMaxMin:\n"
	"partitioning a graph by stream sgp  of max-min algorithm.\n"
	"params: -i <inputfile> -o <outputdir> -k <clusters num> -m <edge order: dfs, bfs, random> -log <log file> -aw <assign window size> -ew <edges limits> -sm <sample mode: eq, uneq,dbs> -maxd <max degree if sample mode is uneq>\n"
	"Test:\n"
	"do a test!!!!"
	"params: -i <input file> -log <log file>";
	

string lowercase(const string& s) {
	string lower(s);
	for(size_t i=0; i<s.length(); i++)
		lower[i] = tolower(lower[i]);

	return lower;
}

wstring lowercase(const wstring& s) {
	wstring lower(s);
	for(size_t i=0; i<s.length(); i++)
		lower[i] = tolower(lower[i]);

	return lower;
}

/*
	string 转换为 wstring 
*/
wstring c2w(const char *pc)
{
	std::wstring val = L"";

	if(NULL == pc)
	{
		return val;
	}
	size_t size_of_wc;
	size_t destlen = mbstowcs(0,pc,0);
	if (destlen ==(size_t)(-1))
	{
		return val;
	}
	size_of_wc = destlen+1;
	wchar_t * pw  = new wchar_t[size_of_wc];
	mbstowcs(pw,pc,size_of_wc);
	val = pw;
	delete[] pw;
	return val;
}
/*
	wstring 转换为 string
*/
string w2c(const wchar_t * pw)
{
	std::string val = "";
	if(!pw)
	{
   		return val;
	}
	size_t size= wcslen(pw)*sizeof(wchar_t);
	char *pc = NULL;
	if(!(pc = (char*)malloc(size)))
	{
   		return val;
	}
	size_t destlen = wcstombs(pc,pw,size);
	/*转换不为空时，返回值为-1。如果为空，返回值0*/
	if (destlen ==(size_t)(0))
	{
		return val;
	}
	val = pc;
	delete[] pc;
	return val;
}

void doDFSConvert(string inputfile, string outputfile, string logfile)
{
	Graph graph;
	TimeTicket::reset();
	Log::CreateLog(logfile);
	Log::log(" building graph from dir "+inputfile+"... \n");
	graph.BuildGraphFromDir(inputfile);
	Log::log("\n building elaplse time: ");
	Log::log(TimeTicket::check());
	Log::log(" sec\n");

	Log::log(" writing graph into by dfs "+outputfile+"... \n");
	graph.WriteGraphToFileByDFS(outputfile);
	Log::log(" writing graph finished! elapse time: ");
	Log::log(TimeTicket::total_elapse());
	Log::log(" sec\n");

	graph.doGraphStatistic();
}

void doBFSConvert(string inputfile, string outputfile, string logfile)
{
	Graph graph;
	TimeTicket::reset();
	Log::CreateLog(logfile);
	Log::log(" building graph from dir "+inputfile+"... \n");
	graph.BuildGraphFromDir(inputfile);
	Log::log("\n building elaplse time: ");
	Log::log(TimeTicket::check());
	Log::log(" sec\n");

	Log::log(" writing graph by bfs into "+outputfile+"... \n");
	graph.WriteGraphToFileByBFS(outputfile);
	Log::log(" writing graph finished! elapse time: ");
	Log::log(TimeTicket::total_elapse());
	Log::log(" sec\n");

	graph.doGraphStatistic();
}

void doRANDConvert(string inputfile, string outputfile, string logfile)
{
}

void doKLPartitioning(string inputfile, string outputfile, int k, string logfile)
{
	Graph graph;
	TimeTicket::reset();
	Log::CreateLog(logfile);
	Log::log(" building graph from dir "+inputfile+"... \n");
	graph.BuildGraphFromDir(inputfile);
	Log::log(" graph vertex:\t");
	Log::log(graph.GetVertexNumber());
    Log::log("\n graph edges:\t");
    Log::log(graph.GetEdgesNumber());
	Log::log("\n building elaplse time: ");
	Log::log(TimeTicket::check());
	Log::log(" sec\n");

	Log::log(" partitioning the graph by KL... \n");
	Partitioner partitioner;
	partitioner.SetGraph(&graph);
	partitioner.SetPartitionNumber(k);
	partitioner.doKL();
	partitioner.SetOutFile(outputfile);
	partitioner.WriteVerticesOfPartitions();
	Log::log(" partitioning the graph finished! elapse time: ");
	Log::log(TimeTicket::total_elapse());
	Log::log(" sec\n");
	Log::log(" Cut Value: ");
	Log::log(partitioner.ComputeCutValue());
}

void doMaxMinPartitioning(string inputfile, string outputfile, int k, string logfile)
{
}

void doSGPKLPartitioning(string inputfile, string outputfile, int k, string logfile,EdgeOrderMode ordermode, int assign_win_size, int edges_limits, SampleMode sample_mode, int max_d)
{
	TimeTicket::reset();
	Log::CreateLog(logfile);
	Log::logln("Partitioning the graph"+inputfile+" by SGP KL");
	SGPLoader loader;
	loader.SetEdgeOrderMode(ordermode);
	loader.SetAssignWindowSize(assign_win_size);
	loader.SetOutputFile(outputfile);
	loader.SetEdgesLimition(edges_limits);
	loader.SetGraphFile(inputfile);
	loader.SetK(k);
	loader.SetSampleMode(sample_mode);
	loader.SetMaxDegree(max_d);

	Log::logln("do graph sampling...");
	loader.doGraphSampling();
	Log::log("do graph sampling elapse : \t");
	Log::logln(TimeTicket::check());

	Log::logln("do partitioning on graph samples...");
	loader.doGraphSamplePartition(KL);
	Log::log("do partitioning on graph samples : \t");
	Log::logln(TimeTicket::check());

	Log::logln("do assigning edges...");
	loader.doAssignReminderEdges();
	Log::log("do assigning edges : \t");
	Log::logln(TimeTicket::check());


	Log::logln(" writing the vertices of each partition in memory ");
	loader.GetPartitioner().WriteVerticesOfPartitions();
	Log::log("do WriteVerticesOfPartitions : \t");
	Log::logln(TimeTicket::check());

	Log::logln(" writing the edges of each partition in memory ");
	loader.GetPartitioner().WriteClusterEdgesOfPartitions();
	Log::log("do WriteClusterEdgesOfPartitions : \t");
	Log::logln(TimeTicket::check());

	Log::log(" partitioning the graph finished! elapse time: ");
	Log::log(TimeTicket::total_elapse());
	Log::log(" sec\n");

	loader.doSGPStatistic();

}

void doSGPMaxMinPartitioning(string inputfile, string outputfile, int k, string logfile,EdgeOrderMode ordermode, int assign_win_size, int edges_limits, SampleMode sample_mode, int max_d)
{
	
}

void doSGPStreamKLPartitioning(string inputfile, string outputfile, int k, string logfile,EdgeOrderMode ordermode, int assign_win_size, int edges_limits, SampleMode sample_mode, int max_d, int edge_cache_size)
{
	TimeTicket::reset();
	Log::CreateLog(logfile);
	Log::logln("Streamingly loading the graph"+inputfile+" by SGPs KL");
	SGPLoader loader;
	loader.SetEdgeOrderMode(ordermode);
	loader.SetAssignWindowSize(assign_win_size);
	loader.SetOutputFile(outputfile);
	loader.SetEdgesLimition(edges_limits);
	loader.SetGraphFile(inputfile);
	loader.SetK(k);
	loader.SetSampleMode(sample_mode);
	loader.SetMaxDegree(max_d);
	loader.SetEdgeCacheSize(edge_cache_size);

	Log::logln("do graph loading...");
	loader.doStreamLoadByDBS(KL);
	Log::log("do graph loading elapse : \t");
	Log::logln(TimeTicket::total_elapse());

	loader.doSGPStatistic();
}

void doSGPStreamMaxMinPartitioning(string inputfile, string outputfile, int k, string logfile,EdgeOrderMode ordermode, int assign_win_size, int edges_limits, SampleMode sample_mode, int max_d)
{
}

void doTest(string inputfile, string logfile)
{
	Log::CreateLog(logfile);

	Log::logln("TEST : shortest path");
	Log::logln("inputfile : a graph");

	Graph graph;
	graph.BuildGraphFromFile(inputfile);
	int begin_vex_pos = 0;
	VERTEX vex = graph.GetVertexInfoofPos(begin_vex_pos)->_u;
	vector<int> shortest_path_lens;
	graph.ComputeShortestPathsFromVertex(vex, shortest_path_lens);
	
	stringstream log_str;
	log_str<<" the start vertex : \t" <<vex;
	Log::logln(log_str.str());
	int vex_pos = 0;
	for(vector<int>::iterator iter = shortest_path_lens.begin(); iter!=shortest_path_lens.end(); iter++)
	{
		VERTEX vex = graph.GetVertexInfoofPos(vex_pos)->_u;
		int len = *iter;
		log_str.str("");
		log_str<<"the shortest path to \t"<<vex<<" : "<<len;
		Log::logln(log_str.str());
		vex_pos++;
	}
}

bool GetParam(map<string, string>& command_params, string& cmd, string& param)
{
	map<string, string>::iterator iter;
	iter = command_params.find(cmd);
	if(iter == command_params.end())
		return false;

	param = iter->second;
	return true;
}

bool ParseCommand(map<string, string> &command_params)
{
	map<string, string>::iterator iter;

	iter = command_params.find("-cmd");
	if(iter == command_params.end())
		return false;

	string cmd = iter->second;
	if(cmd == "bfs")
	{
		//inputfile, outputfile
		string inputfile, outputfile, logfile;
		if(GetParam(command_params, string("-i"), inputfile) && 
			GetParam(command_params, string("-o"), outputfile) &&
			GetParam(command_params, string("-log"), logfile))
		{
			doBFSConvert(inputfile, outputfile, logfile);
			return true;
		}
		else
		{
			return false;
		}
	}

	if(cmd == "dfs")
	{
		//inputfile, outputfile
		string inputfile, outputfile, logfile;
		if(GetParam(command_params, string("-i"), inputfile) && 
			GetParam(command_params, string("-o"), outputfile) &&
			GetParam(command_params, string("-log"), logfile))
		{
			doDFSConvert(inputfile, outputfile,logfile);
			return true;
		}
		else
		{
			return false;
		}
	}

	if(cmd == "rand")
	{
		//inputfile, outputfile
		string inputfile, outputfile, logfile;
		if(GetParam(command_params, string("-i"), inputfile) && 
			GetParam(command_params, string("-o"), outputfile) &&
			GetParam(command_params, string("-log"), logfile))
		{
			doRANDConvert(inputfile, outputfile, logfile);
			return true;
		}
		else
		{
			return false;
		}
	}

	if(cmd == "kl")
	{
		//inputfile, outputfile
		string inputfile, outputfile, logfile, k_str;
		if(GetParam(command_params, string("-i"), inputfile) && 
			GetParam(command_params, string("-o"), outputfile) &&
			GetParam(command_params, string("-log"), logfile)&&
			GetParam(command_params, string("-k"), k_str))
		{
			int k = stoi(k_str); 
			doKLPartitioning(inputfile, outputfile, k, logfile);
			return true;
		}
		else
		{
			return false;
		}
	}

	if(cmd == "maxmin")
	{
		//inputfile, outputfile
		string inputfile, outputfile, logfile, k_str;
		if(GetParam(command_params, string("-i"), inputfile) && 
			GetParam(command_params, string("-o"), outputfile) &&
			GetParam(command_params, string("-log"), logfile)&&
			GetParam(command_params, string("-k"), k_str))
		{
			int k = stoi(k_str); 
			doMaxMinPartitioning(inputfile, outputfile, k, logfile);
			return true;
		}
		else
		{
			return false;
		}
	}

	if(cmd == "sgpkl")
	{
		//inputfile, outputfile
		string inputfile, outputfile, logfile, k_str, order_mode, assign_win, edges_limit, sample_mode, max_d_str;
		if(GetParam(command_params, string("-i"), inputfile) && 
			GetParam(command_params, string("-o"), outputfile) &&
			GetParam(command_params, string("-log"), logfile)&&
			GetParam(command_params, string("-k"), k_str)&&
			GetParam(command_params, string("-m"), order_mode)&&
			GetParam(command_params, string("-aw"), assign_win)&&
			GetParam(command_params, string("-ew"), edges_limit)&&
			GetParam(command_params, string("-sm"), sample_mode))
		{
			int k = stoi(k_str); 
			int aw_size = stoi(assign_win);
			int ew_size = stoi(edges_limit);
			
			SampleMode mode;
			if(sample_mode.compare("eq")==0) mode = FIX_MEM_EQ;
			if(sample_mode.compare("uneq")==0) mode = FIX_MEM_UNEQ;
			if(sample_mode.compare("dbs")==0) mode = RESERVOIR_DBS;

			EdgeOrderMode ordermode;
			if(order_mode.compare("dfs") == 0) ordermode = DFS;
			if(order_mode.compare("bfs") == 0) ordermode = BFS;
			if(order_mode.compare("random") == 0) ordermode = RANDOM;

			int max_d = 0;
			if(mode == FIX_MEM_UNEQ)
			{
				GetParam(command_params, string("-maxd"), max_d_str);
				max_d = stoi(max_d_str);
			}

			doSGPKLPartitioning(inputfile, outputfile, k, logfile, ordermode, aw_size, ew_size, mode, max_d);
			return true;
		}
		else
		{
			return false;
		}
	}

	if(cmd == "sgpmaxmin")
	{
		//inputfile, outputfile
		string inputfile, outputfile, logfile, k_str, order_mode, assign_win, edges_limit, sample_mode, max_d_str;
		if(GetParam(command_params, string("-i"), inputfile) && 
			GetParam(command_params, string("-o"), outputfile) &&
			GetParam(command_params, string("-log"), logfile)&&
			GetParam(command_params, string("-k"), k_str)&&
			GetParam(command_params, string("-m"), order_mode)&&
			GetParam(command_params, string("-aw"), assign_win)&&
			GetParam(command_params, string("-ew"), edges_limit)&&
			GetParam(command_params, string("-sm"), sample_mode))
		{
			int k = stoi(k_str); 
			int aw_size = stoi(assign_win);
			int ew_size = stoi(edges_limit);

			SampleMode mode;
			if(sample_mode.compare("eq")==0) mode = FIX_MEM_EQ;
			if(sample_mode.compare("uneq")==0) mode = FIX_MEM_UNEQ;
			if(sample_mode.compare("dbs")==0) mode = RESERVOIR_DBS;

			EdgeOrderMode ordermode;
			if(order_mode.compare("dfs") == 0) ordermode = DFS;
			if(order_mode.compare("bfs") == 0) ordermode = BFS;
			if(order_mode.compare("random") == 0) ordermode = RANDOM;

			int max_d = 0;
			if(mode == FIX_MEM_UNEQ)
			{
				GetParam(command_params, string("-maxd"), max_d_str);
				max_d = stoi(max_d_str);
			}


			doSGPMaxMinPartitioning(inputfile, outputfile, k, logfile, ordermode, aw_size, ew_size, mode, max_d);
			return true;
		}
		else
		{
			return false;
		}
	}

	if(cmd == "sgpstreamkl")
	{
		//inputfile, outputfile
		string inputfile, outputfile, logfile, k_str, order_mode, assign_win, edges_limit, sample_mode, max_d_str, edge_cache_size_str;
		if(GetParam(command_params, string("-i"), inputfile) && 
			GetParam(command_params, string("-o"), outputfile) &&
			GetParam(command_params, string("-log"), logfile)&&
			GetParam(command_params, string("-k"), k_str)&&
			GetParam(command_params, string("-m"), order_mode)&&
			GetParam(command_params, string("-aw"), assign_win)&&
			GetParam(command_params, string("-ew"), edges_limit)&&
			GetParam(command_params, string("-sm"), sample_mode))
		{
			int k = stoi(k_str); 
			int aw_size = stoi(assign_win);
			int ew_size = stoi(edges_limit);

			SampleMode mode;
			if(sample_mode.compare("eq")==0) mode = FIX_MEM_EQ;
			if(sample_mode.compare("uneq")==0) mode = FIX_MEM_UNEQ;
			if(sample_mode.compare("dbs")==0) mode = RESERVOIR_DBS;

			EdgeOrderMode ordermode;
			if(order_mode.compare("dfs") == 0) ordermode = DFS;
			if(order_mode.compare("bfs") == 0) ordermode = BFS;
			if(order_mode.compare("random") == 0) ordermode = RANDOM;

			int max_d = 0;
			if(mode == FIX_MEM_UNEQ)
			{
				GetParam(command_params, string("-maxd"), max_d_str);
				max_d = stoi(max_d_str);
			}
			int edge_cache_size = 0;
			if(mode == RESERVOIR_DBS)
			{
				GetParam(command_params, string("-ec"), edge_cache_size_str);
				edge_cache_size = stoi(edge_cache_size_str);
			}

			doSGPStreamKLPartitioning(inputfile, outputfile, k, logfile, ordermode, aw_size, ew_size, mode, max_d,edge_cache_size);
			return true;
		}
		else
		{
			return false;
		}
	}

	if(cmd == "sgpstreammaxmin")
	{
		//inputfile, outputfile
		string inputfile, outputfile, logfile, k_str, order_mode,assign_win, edges_limit, sample_mode, max_d_str;
		if(GetParam(command_params, string("-i"), inputfile) && 
			GetParam(command_params, string("-o"), outputfile) &&
			GetParam(command_params, string("-log"), logfile)&&
			GetParam(command_params, string("-k"), k_str)&&
			GetParam(command_params, string("-m"), order_mode)&&
			GetParam(command_params, string("-aw"), assign_win)&&
			GetParam(command_params, string("-ew"), edges_limit)&&
			GetParam(command_params, string("-sm"), sample_mode))
		{
			int k = stoi(k_str); 
			int aw_size = stoi(assign_win);
			int ew_size = stoi(edges_limit);

			SampleMode mode;
			if(sample_mode.compare("eq")==0) mode = FIX_MEM_EQ;
			if(sample_mode.compare("uneq")==0) mode = FIX_MEM_UNEQ;
			if(sample_mode.compare("dbs")==0) mode = RESERVOIR_DBS;

			EdgeOrderMode ordermode;
			if(order_mode.compare("dfs") == 0) ordermode = DFS;
			if(order_mode.compare("bfs") == 0) ordermode = BFS;
			if(order_mode.compare("random") == 0) ordermode = RANDOM;

			int max_d = 0;
			if(mode == FIX_MEM_UNEQ)
			{
				GetParam(command_params, string("-maxd"), max_d_str);
				max_d = stoi(max_d_str);
			}

			doSGPStreamMaxMinPartitioning(inputfile, outputfile, k, logfile, ordermode, aw_size, ew_size, mode, max_d);
			return true;
		}
		else
		{
			return false;
		}
	}

	if(cmd == "test")
	{
		//inputfile, outputfile
		string inputfile, logfile;
		if(	GetParam(command_params, string("-i"), inputfile) &&
			GetParam(command_params, string("-log"), logfile))
		{
			doTest(inputfile, logfile);
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc < 3)
	{
		cout<<usage<<endl;
		return 0;
	}

	map<string, string> command_params;

	for(int i=1; i<argc-1; i++)
	{
		wstring str1(argv[i]);
		string cmd = lowercase(w2c(str1.c_str()));
		wstring str2(argv[i+1]);
		string param = lowercase(w2c(str2.c_str()));
		command_params.insert(pair<string, string>(cmd,param));
	}

	if(!ParseCommand(command_params))
		cout<<usage<<endl;

	getchar();

	return 0;
}

