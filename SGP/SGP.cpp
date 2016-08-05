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
#include "StreamPartiton.h"
#include <unordered_set>
#include "windows.h"

using namespace std;

string usage = 
	"sgp -CMD [DFS|BFS|RAND|KL|SGPKL|SGPStreamKL|StreamPartition|GraphNorm|Test] ...\n"
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
	"SGPKL:\n"
	"partitioning a graph by sgp of kl algorithm.\n"
	"params: -i <inputfile> -o <outputdir> -k <clusters num> -m <edge order: dfs, bfs, random> -log <log file> -aw <assign window size> -ew <edges limits> -sm <sample mode: eq, uneq,dbs> -maxd <max degree if sample mode is uneq>\n"
	"SGPStreamKL:\n"
	"partitioning a graph by stream sgp of kl algorithm.\n"
	"params: -i <inputfile> -o <outputdir> -k <clusters num> -m <edge order: dfs, bfs, random> -log <log file> -aw <assign window size> -ew <edges limits> -sm <sample mode: eq, uneq,dbs> -maxd <max degree if sample mode is uneq> -ec <edge cache size for dbs>\n"
	"StreamPartition:\n"
	"streamly partitioning a graph by a series of vertex assigning measure.\n"
	"params: -i <inputfile> -o <outputdir> -k <clusters num> -log <log file> -asm <assign measure: hash, balance, DG, LDG, EDG, Tri, LTri, EDTri, NN, Fennel> -maxd <max degree> -ew <edges limits>\n"
	"GraphNorm:\n"
	"Normalize the graph file as <nodeid nodeid> and nodeid is in the int32 range.\n"
	"params: -i <inputdir> -o <outputfile> -log <log file> -recode [0|1]<0:false, 1:true> -sep <separator: 9 or 32(space) or self-defined digital> -maxcomments <max comments lines to check> -readercount <reader thread count> -hashreserved <hash bins> -readerbuffer <reader buffer size(lines)>\n"
	"SplitBigFile:\n"
	"split the big file into max_reader files.\n"
	"params: -i <inputfile> -o <outputdir> -log <log file>  -readercount <reader thread count>\n"
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
	Graph graph;
	TimeTicket::reset();
	Log::CreateLog(logfile);
	Log::log(" writing graph by random into "+outputfile+"... \n");
	graph.WriteGraphToFileByRand(inputfile, outputfile);
	Log::log(" writing graph finished! elapse time: ");
	Log::log(TimeTicket::total_elapse());
	Log::log(" sec\n");

	graph.doGraphStatistic();

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
	loader.GetPartitioner().GetStatistic()->SetTotalElapse(TimeTicket::total_elapse());
	loader.doSGPStatistic();

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

void doStreamPartitioning(string inputfile, string outputfile, int k, string logfile, StreamPartitionMeasure measure, int max_d, int max_edges)
{
	TimeTicket::reset();
	Log::CreateLog(logfile);
	Log::logln("Streamingly loading the graph"+inputfile+" by StreamPartitioning");
	StreamPartiton loader;
	loader.SetMaxDegree(max_d);
	loader.SetMaxEdges(max_edges);
	loader.SetGraphFile(inputfile);
	loader.SetOutFile(outputfile);
	loader.SetK(k);
	Log::logln("do graph loading...");
	loader.doStreamPartition(measure);
}

bool isDigitString(string& str, char separator)
{
	for(basic_string <char>::iterator iter = str.begin(); iter!=str.end(); iter++)
	{
		char c = *iter;
		if(c>='0' && c<='9' || c==separator)
			continue;
		else
			return false;
	}
	return true;
}

typedef struct _ReadDataInfo {
	string _file_name;
} ReadDataInfo;

typedef struct _Read_Buf {
	vector<VERTEX*> _buffer;
	int _len; //current rows
	int _total_len;//total space
}Read_Buf;

typedef struct _GraphReaderParam {
	HANDLE  _hMutex;
	Read_Buf _read_buf;
	vector<ReadDataInfo> _read_data_info;
	char _sep;//separator
	int _max_comments;
	bool _recode;
} GraphReaderParam;

typedef struct _GraphWriterParam {
	GraphReaderParam* _reader_params;
	int _reader_count;
	ofstream* _ofs;
	int _hash_reserved;
}GraphWriterParam;

typedef struct {
	string _file_name;
	int _size;
} File_Info;

//bool GetLine(ifstream& ifs, string& buf)
//{
//	buf.clear();
//	char c = 0;
//	while(!ifs.eof())
//	{
//		ifs.read(&c, 1);
//		if(c!=0x0d && c!=0x0a )
//		{
//			buf.append(&c, 1);
//		}
//		else
//		{
//			break;
//		}
//	}
//	if(c==0 && ifs.eof())
//	{
//		return false;
//	}
//	else
//	{
//		if(c==0x0d)
//			ifs.read(&c, 1);
//		return true;
//	}
//}
int g_dup = 0;
int WriteBuffer(ofstream* ofs, Read_Buf& buffer, map<unsigned int, int>& vex_degree, hash_set<EdgeID>& edges)
{
	int count = 0, total_count=0, write_edge_count=0;
	EDGE e;
	for(vector<VERTEX*>::iterator iter= buffer._buffer.begin(); iter!= buffer._buffer.end(); iter++)
	{
		VERTEX* block = *iter;
		for(count=0; total_count<buffer._len && count<READ_DATA_LEN; count+=2, total_count+=2)
		{
			e._u = block[count];
			e._v = block[count+1];
			EdgeID id = MakeEdgeID(e._u,e._v);
			if(edges.find(id) == edges.end())
			{
				edges.insert(id);

				map<unsigned int, int>::iterator iter = vex_degree.find(e._u);
				if( iter == vex_degree.end())
				{
					vex_degree.insert(pair<unsigned int, int>(e._u, 1));
				}
				else
				{
					iter->second++;
				}
				iter = vex_degree.find(e._v);
				if( iter == vex_degree.end())
				{
					vex_degree.insert(pair<unsigned int, int>(e._v, 1));
				}
				else
				{
					iter->second++;
				}

				ofs->write((char*)(&e._u), sizeof(VERTEX));//TODO: IMPROVE....
				ofs->write((char*)(&e._v), sizeof(VERTEX));
				write_edge_count++;
			}
			else
			{
				g_dup++;
			}
		}
	}
	return write_edge_count;
}

bool ProcessLines(string& buf, bool recode, EDGE& e, char separator)
{
	string temp1, temp2;
	VERTEX u,v;
	int idx = buf.find_first_of(separator);
	temp1 = buf.substr(0, idx);
	int idx2 = buf.find_first_of(separator, idx+1);
	temp2 = buf.substr(idx+1, idx2-idx);

	if(recode)
	{
		if(temp1.length()> VERTEX_ID_MAX_LEN)
		{
			int len = temp1.length();
			temp1 = temp1.substr(len-VERTEX_ID_MAX_LEN-1, VERTEX_ID_MAX_LEN);
		}
		if(temp2.length()> VERTEX_ID_MAX_LEN)
		{
			int len = temp2.length();
			temp2 = temp2.substr(len-VERTEX_ID_MAX_LEN-1, VERTEX_ID_MAX_LEN);
		}
	}
	try{
		e._u = stoul(temp1);
		e._v = stoul(temp2);
	}
	catch(...)
	{
		cout<<"parse buffer error"<<endl;
		return false;
	}
	return true;
}

void ProcessFile(string& file, char separator, bool recode, Read_Buf& read_buf, int max_comments )
{
	ifstream ifs(file, ios::in|ios::binary);
	ifs.seekg(0, ios::end);
	int size = ifs.tellg();
	char* tmp_buf = new char[size];
	ifs.seekg(0);
	ifs.read(tmp_buf, size);
	ifs.close();

	char c;
	int start=0, end=0;
	int file_pos=0;
	bool line_finished = false;
	int file_comments = 0;
	EDGE e;
	while(file_pos<size)
	{
		c = tmp_buf[file_pos];
		switch(c)
		{
		case 0x0d:
			end = file_pos;
			file_pos += 2;//skip 0x0a
			line_finished = true;
			break;
		case 0x0a:
			end = file_pos;
			file_pos ++;
			line_finished = true;
			break;
		default:
			file_pos++;
			line_finished = false;
			break;
		}
		if(!line_finished) continue;

		string str(tmp_buf+start, end-start);
		if(file_comments++<max_comments)
		{
			if(!isDigitString(str, separator))
			{
				start = file_pos;
				continue;
			}
		}

		if(ProcessLines(str, recode, e, separator))
		{
			if(read_buf._len > read_buf._total_len-2)//full
			{
				VERTEX* block = new VERTEX[READ_DATA_LEN];
				read_buf._buffer.push_back(block);
				read_buf._total_len += READ_DATA_LEN;
			}

			VERTEX* last_block = read_buf._buffer.back();
			int pos = read_buf._len - READ_DATA_LEN * (read_buf._buffer.size()-1);
			last_block[pos]=e._u;
			last_block[pos+1]=e._v;
			read_buf._len += 2;
		}
		start = file_pos;
	}
	delete[] tmp_buf;
}

DWORD WINAPI GraphNormReaderThread( LPVOID lpParam )
{
	GraphReaderParam* param = (GraphReaderParam*) lpParam;
	cout<<"GraphNormReaderThread: "<<param->_read_data_info[0]._file_name<<endl;
	DWORD dwWaitResult; 
	while(true)
	{
		dwWaitResult = WaitForSingleObject(param->_hMutex,INFINITE);
		if(dwWaitResult == WAIT_OBJECT_0)
		{
			break;
		}
		else
		{
			cout<<"Reader WAIT_FAIL, Wait again"<<endl;
			continue;
		}
	}

	for(int i=0; i<param->_read_data_info.size(); i++)
	{
		ProcessFile(param->_read_data_info[i]._file_name, param->_sep, param->_recode, param->_read_buf, param->_max_comments );
	}
	ReleaseMutex(param->_hMutex);
	cout<<"GraphNormReaderThread: "<<param->_read_data_info[0]._file_name<<"<<<END"<<endl;
	return 1;
}

DWORD WINAPI GraphNormWriterThread( LPVOID lpParam )
{
	DWORD dwWaitResult; 
	GraphWriterParam* param = (GraphWriterParam*) lpParam;
	map<unsigned int, int> vex_degree;
	hash_set<EdgeID> edges;
	edges.rehash(param->_hash_reserved); 
	int total_edges =0;
	
	for(int i=0; i<param->_reader_count; i++)
	{
		while(true)
		{
			dwWaitResult = WaitForSingleObject(param->_reader_params[i]._hMutex,INFINITE);
			if(dwWaitResult == WAIT_OBJECT_0)
			{
				if(param->_reader_params[i]._read_buf._len>0)
				{
					int lines = WriteBuffer(param->_ofs, param->_reader_params[i]._read_buf, vex_degree, edges);
					total_edges += lines;;
					cout<<"Write Edge : Thread Reader : "<< i <<"Total Edges: "<<total_edges<<" : "<<TimeTicket::total_elapse()<<endl;
					ReleaseMutex(param->_reader_params[i]._hMutex);
					break;//while
				}
			}
			else
			{
				ReleaseMutex(param->_reader_params[i]._hMutex);
				Sleep(10);
			}
		}
	}

	stringstream str;
	str<<"Total Vex Num: \t"<<vex_degree.size()
		<<"\nTotal Edges Num (Valid): \t"<<total_edges
		<<"\nTotal Edges Num (Dup): \t"<<g_dup
		<<"\nElapse: \t"<<TimeTicket::total_elapse()
		<<"\nDegree Distribution <degree : count>";
	
	map<int, int> degree_distribution;
	for(map<unsigned int, int>::iterator iter = vex_degree.begin(); iter!= vex_degree.end(); iter++)
	{
		map<int, int>::iterator iter_d = degree_distribution.find(iter->second);
		if(iter_d == degree_distribution.end())
		{
			degree_distribution.insert(pair<int,int>(iter->second,1));
		}
		else
		{
			iter_d->second++;
		}
	}
	for(map<int, int>::iterator iter_d = degree_distribution.begin(); iter_d!= degree_distribution.end(); iter_d++)
	{
		str<<"\n"<<iter_d->first<<":"<<iter_d->second;
	}

	Log::logln(str.str());
	return 1;
}

void doGraphNorm(string& inputdir, string& outputfile, string& logfile, bool recode, char separator, int max_comments, int reader_count, int hash_reserved, int reader_buffer_size)
{
	std::cout<<">>>>>GraphNorm Start";	
	TimeTicket::reset();
	Log::CreateLog(logfile);
	ofstream ofs(outputfile, ios::out|ios::trunc|ios::binary);
	if(!ofs.is_open())
	{
		cout<<"open output file error :"<<outputfile<<endl;
		return;
	}

	vector<File_Info> file_info_list;
	int total_file_size = 0;

	
	string path = inputdir.substr(0, inputdir.find_last_of('\\'));
	_finddata_t file;
	long lf;
	if((lf = _findfirst(inputdir.c_str(), &file))==-1l)
	{
		return;
	}
	else
	{
		while( _findnext( lf, &file ) == 0 )
		{
			if(file.attrib == _A_NORMAL || file.attrib == _A_ARCH)
			{
				File_Info f_info;
				f_info._file_name = path+"\\"+string(file.name);
				f_info._size = file.size;
				file_info_list.push_back(f_info);
			}
		}
	}
	_findclose(lf);

	GraphReaderParam* reader_params = new GraphReaderParam[reader_count];
	int reader_file_count = file_info_list.size()/reader_count;
	vector<File_Info>::iterator iter_file = file_info_list.begin();
	for(int i=0; i<reader_count; i++)
	{
		reader_params[i]._max_comments = max_comments;
		reader_params[i]._recode = recode;
		reader_params[i]._sep = separator;
		reader_params[i]._read_buf._len = 0;
		reader_params[i]._read_buf._total_len = 0;
		//init mutex
		reader_params[i]._hMutex = CreateMutex( 
			NULL,                       // default security attributes
			FALSE,                      // initially not owned
			NULL);                      // unnamed mutex

		//init ReaderDataInfo
		int cur_count = 0;
		if(i<reader_count-1)
		{
			for(; iter_file!= file_info_list.end(); iter_file++)
			{
				cur_count += 1;
				if(cur_count <= reader_file_count)
				{
					ReadDataInfo rdi;
					rdi._file_name = iter_file->_file_name;
					reader_params[i]._read_data_info.push_back(rdi);
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			for(; iter_file!= file_info_list.end(); iter_file++)
			{
				ReadDataInfo rdi;
				rdi._file_name = iter_file->_file_name;
				reader_params[i]._read_data_info.push_back(rdi);
			}
		}
	}

	//create reader thread
	DWORD dwThreadId;
	HANDLE* hReaderThreads = new HANDLE[reader_count];
	for(int i=0; i<reader_count; i++)
	{
		hReaderThreads[i] = CreateThread( 
			NULL,                       // default security attributes 
			0,                          // use default stack size  
			GraphNormReaderThread,      // thread function 
			&reader_params[i],			// argument to thread function 
			0,                          // use default creation flags 
			&dwThreadId); 

	}

	//create writer thread
	GraphWriterParam writer_param;
	writer_param._hash_reserved=hash_reserved;
	writer_param._ofs = new ofstream(outputfile, ios::out|ios::binary);
	writer_param._reader_count = reader_count;
	writer_param._reader_params = reader_params;
	HANDLE hWriterThreads;

	hWriterThreads = CreateThread( 
		NULL,                       // default security attributes 
		0,                          // use default stack size  
		GraphNormWriterThread,      // thread function 
		&writer_param,				// argument to thread function 
		0,                          // use default creation flags 
		&dwThreadId); 

	//wait writer
	DWORD dwWaitResult = WaitForSingleObject(hWriterThreads, INFINITE);
	//uninitialization
	delete[] reader_params;
	writer_param._ofs->flush();
	writer_param._ofs->close();
	delete writer_param._ofs;
	delete[] hReaderThreads;

	cout<<"Total Elapse: \t"<<TimeTicket::total_elapse()<<endl;
	cout<<">>>>>GraphNorm Finish";
}

void doSplitBigFile(string& inputfile, string& outputdir, string& logfile, int reader_count)
{
	cout<<"do Spliting Big File Start"<<endl;

	ifstream ifs(inputfile, ios::in|ios::binary);
	ifs.seekg(0, ios::end);
	int file_size = ifs.tellg();
	int block_size = file_size/reader_count;
	int reader_data_remain = 0;//approximate
	int file_remain = file_size;
	int g_file_pos=0;
	for(int i=0; i<reader_count && file_remain>0; i++)
	{
		if(i==reader_count-1)
		{
			reader_data_remain = file_size-block_size*i;
		}
		else
		{
			reader_data_remain = block_size;
		}
		int start = 0, end=0;
		if(reader_data_remain >= file_remain)
		{
			start = g_file_pos;
			end = file_size;//g_file_pos+file_remain
			//reader_data_remain -= file_remain;
			reader_data_remain = 0;//only one file
			file_remain = 0;
		}
		else //reader_data_remain < file_remain
		{
			start = g_file_pos;
			//reset the rdi.end as the end of a line
			end = g_file_pos +reader_data_remain ;

			ifs.seekg(end);
			char c;
			do{
				ifs.read(&c, 1);
			}
			while(c!=0x0d && c!=0x0a && !ifs.eof());
			
			end = ifs.tellg();
			
			if(c==0x0d) end ++;//0a

			g_file_pos = end;
			file_remain -= end-start;
			reader_data_remain = 0;
		}
		
		stringstream str;
		str<<outputdir<<".s"<<i;

		cout<<"Create Split File: "<<str.str()<<" size: "<<end-start<<endl;
		
		ofstream ofs(str.str(), ios::out|ios::binary|ios::trunc);
		ifs.seekg(start);
		char* buf = new char[end-start];
		ifs.read(buf, end-start);
		ofs.write(buf, end-start);
		ofs.close();
		delete[] buf;
	}
	cout<<"do Spliting Big File Over"<<endl;
}

void doTest(string inputfile, string logfile)
{
	ifstream ifs("D:\\workspace\\data\\test\\20060293.edges");
	string buf;
	int c=0;
	while(getline(ifs, buf))
	{
		cout<<"file pos:"<<ifs.tellg()<<":"<<buf<<endl;
		c++;
	}
	cout<<"total lines: "<<c<<endl;
	ifs.close();
/*
	ofstream ofs(inputfile, ios::out|ios::binary|ios::trunc);
	char c = 'a';
	ofs.write(&c, 1);
	c = 0x0a;
	ofs.write(&c, 1);
	ofs.close();

	ifstream ifs1(inputfile);
	string buf1;
	while(getline(ifs1, buf1)) 
	{
		int i = ifs1.tellg();
		cout<<i<<endl;
	}
*/
	/*
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
	*/
	/*
	hash_set<EdgeInfo, EdgeInfoCompare> edges;
	EdgeInfo e;
	for(int i=0; i<10000;i++)
	{
	e._adj_vex_pos = i;
	edges.insert(e);
	}
	for(int i=0; i<10000;i++)
	{
	e._adj_vex_pos = i;
	if(edges.find(e) == edges.end())
	{
	cout<<"not found :"<<i<<endl;
	}
	else
	{
	cout<<"found :"<<i<<endl;
	}
	}
	e._adj_vex_pos = 10000;
	if(edges.find(e) == edges.end())
	{
	cout<<"not found 10000"<<endl;
	}
	else
	{
	cout<<"found 10000"<<endl;
	}
	*/
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

	if(cmd == "streampartition")
	{
		//inputfile, outputfile
		string inputfile, outputfile, logfile, k_str, assign_measure, max_d_str, max_edges_str;
		if(GetParam(command_params, string("-i"), inputfile) && 
			GetParam(command_params, string("-o"), outputfile) &&
			GetParam(command_params, string("-log"), logfile)&&
			GetParam(command_params, string("-k"), k_str)&&
			GetParam(command_params, string("-asm"), assign_measure)&&
			GetParam(command_params, string("-maxd"), max_d_str)&&
			GetParam(command_params, string("-ew"), max_edges_str))
		{
			int k = stoi(k_str); 
			int max_d = stoi(max_d_str);
			int max_edges = stoi(max_edges_str);

			StreamPartitionMeasure measure;
			if(assign_measure.compare("hash")==0) measure = HASH;
			if(assign_measure.compare("balance")==0) measure = BALANCE;
			if(assign_measure.compare("dg")==0) measure = DG;
			if(assign_measure.compare("ldg")==0) measure = LDG;
			if(assign_measure.compare("edg")==0) measure = EDG;
			if(assign_measure.compare("tri")==0) measure = Tri;
			if(assign_measure.compare("ltri")==0) measure = LTri;
			if(assign_measure.compare("edtri")==0) measure = EDTri;
			if(assign_measure.compare("nn")==0) measure = NN;
			if(assign_measure.compare("fennel")==0) measure = FENNEL;

			doStreamPartitioning(inputfile, outputfile, k, logfile, measure, max_d, max_edges);
			return true;
		}
		else
		{
			return false;
		}
	}

	if(cmd == "graphnorm")
	{
		//inputfile, outputfile
		string inputdir, outputfile, logfile, recode_str, sep_str, maxcomments_str, reader_count_str, hash_reserved_str, reader_buffer_size_str;
		if(GetParam(command_params, string("-i"), inputdir) && 
			GetParam(command_params, string("-o"), outputfile) &&
			GetParam(command_params, string("-log"), logfile) &&
			GetParam(command_params, string("-recode"), recode_str) &&
			GetParam(command_params, string("-sep"), sep_str) &&
			GetParam(command_params, string("-maxcomments"), maxcomments_str)&&
			GetParam(command_params, string("-readercount"), reader_count_str)&&
			GetParam(command_params, string("-hashreserved"), hash_reserved_str)&&
			GetParam(command_params, string("-readerbuffer"), reader_buffer_size_str))
		{
			bool recode;
			if(recode_str=="0") recode = false;
			if(recode_str=="1") recode = true;
			char separator = stoi(sep_str);
			int maxc = stoi(maxcomments_str);
			int reader_count = stoi(reader_count_str);
			int	hash_reserved = stoi(hash_reserved_str); 
			int	reader_buffer_size = stoi(reader_buffer_size_str);

			doGraphNorm(inputdir, outputfile, logfile, recode, separator, maxc, reader_count, hash_reserved, reader_buffer_size);
			return true;
		}
		else
		{
			return false;
		}
	}

	if(cmd == "splitbigfile")
	{
		string inputfile, outputdir, logfile, reader_count_str;
		if(GetParam(command_params, string("-i"), inputfile) && 
			GetParam(command_params, string("-o"), outputdir) &&
			GetParam(command_params, string("-log"), logfile) &&
			GetParam(command_params, string("-readercount"), reader_count_str))
		{
			int reader_count = stoi(reader_count_str);
			doSplitBigFile(inputfile, outputdir, logfile, reader_count);
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

