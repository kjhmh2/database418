#include <leveldb/db.h>
#include <string>

#define KEY_LEN 8
#define VALUE_LEN 8
using namespace std;

const string workload = "../../workloads/";

const string load = workload + "220w-rw-50-50-load.txt"; // TODO: the workload_load filename
const string run  = workload + "220w-rw-50-50-run.txt"; // TODO: the workload_run filename

const string filePath = "";

const int READ_WRITE_NUM = 10000; // TODO: how many operations

int main()
{        
    uint64_t inserted = 0, queried = 0;
    uint64_t* keys = new uint64_t[READ_WRITE_NUM];
    bool* ifInsert = new bool[READ_WRITE_NUM];
	FILE *ycsb, *ycsb_read;
    struct timespec start, finish;
    double single_time;
    printf("===================LevelDB====================\n");
    const string filePath = ""; // data storing folder(NVM)

    memset(keys, 0, READ_WRITE_NUM);
    memset(ifInsert, 0, READ_WRITE_NUM);

    leveldb::DB* db;
    leveldb::Options options;
    leveldb::WriteOptions write_options;
    // TODO: initial the levelDB
    options.create_if_missing = true;
    leveldb::ReadOptions read_options;
    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    assert(status.ok());
	string res;

    inserted = 0;
	queried = 0;
    printf("Load phase begins \n");
    // TODO: read the ycsb_read
    ycsb_read = fopen(load.c_str(), "r");
    char op3[8];
	uint64_t key3;
	for(int i = 0; i < READ_WRITE_NUM; i ++){
		if(fscanf(ycsb_read, "%s %lu", op3, &key3) == EOF)
			break;
		keys[i] = key3;
		ifInsert[i] = (op3[0] == 'I');
	}
    fclose(ycsb_read);          // close the file

    clock_gettime(CLOCK_MONOTONIC, &start);
    // TODO: load the levelDB
    char _key3[40];
    for (int i = 0; i < READ_WRITE_NUM; ++ i)
    {	
		sprintf(_key3,"%lu", keys[i]);
        if (ifInsert[i])
        {	
			inserted ++;
            status = db->Put(write_options, _key3, _key3);
        }
        else
        {
			queried ++;
            status = db->Get(read_options, _key3, &res);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);
	single_time = (finish.tv_sec - start.tv_sec) * 1000000000.0 + (finish.tv_nsec - start.tv_nsec);
    printf("Load phase finishes: %lu items are inserted \n", inserted);
    printf("Load phase used time: %fs\n", single_time / 1000000000.0);
    printf("Load phase single insert time: %fns\n", single_time / inserted);

	printf("Run phase begin\n");
    inserted = 0;		
    // TODO: read the ycsb_run
    ycsb = fopen(run.c_str(), "r");
    char op4[8];
	uint64_t key4;
	for(int i = 0; i < READ_WRITE_NUM; i ++){
		if(fscanf(ycsb_read, "%s %lu", op4, &key4) == EOF)
			break;
		keys[i] = key4;
		ifInsert[i] = (op4[0] == 'I');
	}
    fclose(ycsb);       // close the file

    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: run the workload_run in levelDB
	char _key4[40];
    for (int i = 0; i < READ_WRITE_NUM; ++ i)
    {	
		sprintf(_key4,"%lu", keys[i]);
        if (ifInsert[i])
        {	
			inserted ++;
            status = db->Put(write_options, _key4, _key4);
        }
        else
        {
			queried ++;
            status = db->Get(read_options, _key4, &res);
        }
    }
	clock_gettime(CLOCK_MONOTONIC, &finish);
    fclose(ycsb);
	single_time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Run phase finishes: %lu/%lu items are inserted/searched\n", inserted, queried);
    printf("Run phase throughput: %f operations per second \n", READ_WRITE_NUM/single_time);	
    return 0;
}
