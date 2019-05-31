#include "fptree/fptree.h"
#include <leveldb/db.h>
#include <string>

#define KEY_LEN 8
#define VALUE_LEN 8
using namespace std;

const string workload = "../../workloads/"; // TODO: the workload folder filepath

const string load = workload + "220w-rw-50-50-load.txt"; // TODO: the workload_load filename
const string run  = workload + "220w-rw-50-50-run.txt"; // TODO: the workload_run filename

const int READ_WRITE_NUM = 2200000; // TODO: amount of operations

int main()
{        
    FPTree fptree(1028);
    uint64_t inserted;
	uint64_t queried;
    uint64_t *keys = new uint64_t[READ_WRITE_NUM];
	uint64_t key;
    bool* ifInsert = new bool[READ_WRITE_NUM];
	FILE *ycsb, *ycsb_read;
	char op[8];
    struct timespec start, finish;
    double single_time;

    printf("===================FPtreeDB===================\n");
    printf("Load phase begins \n");

    // TODO: read the ycsb_load
    ycsb_read = fopen(load.c_str(), "r");
	for(int i = 0; i < READ_WRITE_NUM; i ++){
		if(fscanf(ycsb_read, "%s %lu", op, &keys[i]) == EOF)
			break;
		ifInsert[i] = (op[0] == 'I');
	}
    fclose(ycsb_read);          // close the file

    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: load the workload in the fptree
	inserted = 0;	
	queried = 0;
    for (int i = 0; i < READ_WRITE_NUM; ++ i)
    {
		key = keys[i];
        if (ifInsert[i])
        {
			inserted ++;
            fptree.insert(key, key);
        }
        else
        {
            fptree.find(key);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);

	single_time = (finish.tv_sec - start.tv_sec) * 1000000000.0 + (finish.tv_nsec - start.tv_nsec);
    printf("Load phase finishes: %lu items are inserted \n", inserted);
    printf("Load phase used time: %fs\n", single_time / 1000000000.0);
    printf("Load phase single insert time: %fns\n", single_time / inserted);

	printf("Run phase begins\n");
	
    // TODO: read the ycsb_run
    ycsb = fopen(run.c_str(), "r");
	for(int i = 0; i < READ_WRITE_NUM; i ++){
		if(fscanf(ycsb, "%s %lu", op, &keys[i]) == EOF)
			break;
		ifInsert[i] = (op[0] == 'I');
	}
	fclose(ycsb);       // close the file

 	clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: load the workload in the fptree
	inserted = 0;	
	queried = 0;
    for (int i = 0; i < READ_WRITE_NUM; ++ i)
    {
		key = keys[i];
        if (ifInsert[i])
        {
			inserted ++;
            fptree.insert(key, key);
        }
        else
        {
			queried ++;
            fptree.find(key);
        }
    }

	clock_gettime(CLOCK_MONOTONIC, &finish);
	single_time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Run phase finishes: %lu/%lu items are inserted/searched\n", inserted, queried);
    printf("Run phase throughput: %f operations per second \n", READ_WRITE_NUM/single_time);	
    
    // LevelDB
    printf("===================LevelDB====================\n");
    const string filePath = ""; // data storing folder(NVM)

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
	char (*leveldbKeys)[40] = new char[READ_WRITE_NUM][40];
	char *leveldKey;

    printf("Load phase begins \n");
    // TODO: read the ycsb_read
    ycsb_read = fopen(load.c_str(), "r");

	for(int i = 0; i < READ_WRITE_NUM; i ++){
		if(fscanf(ycsb_read, "%s %s", op, leveldbKeys[i]) == EOF)
			break;
		ifInsert[i] = (op[0] == 'I');
	}
    fclose(ycsb_read);          // close the file

    clock_gettime(CLOCK_MONOTONIC, &start);
    // TODO: load the levelDB
	inserted = 0;	
	queried = 0;
    for (int i = 0; i < READ_WRITE_NUM; ++ i)
    {	
		leveldKey = leveldbKeys[i];
        if (ifInsert[i])
        {	
			inserted ++;
            status = db->Put(write_options, leveldKey, leveldKey);
        }
        else
        {
			queried ++;
            status = db->Get(read_options, leveldKey, &res);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);
	single_time = (finish.tv_sec - start.tv_sec) * 1000000000.0 + (finish.tv_nsec - start.tv_nsec);
    printf("Load phase finishes: %lu items are inserted \n", inserted);
    printf("Load phase used time: %fs\n", single_time / 1000000000.0);
    printf("Load phase single insert time: %fns\n", single_time / inserted);

	printf("Run phase begin\n");
		
    // TODO: read the ycsb_run
    ycsb = fopen(run.c_str(), "r");

	for(int i = 0; i < READ_WRITE_NUM; i ++){
		if(fscanf(ycsb_read, "%s %s", op, leveldbKeys[i]) == EOF)
			break;
		ifInsert[i] = (op[0] == 'I');
	}
    fclose(ycsb);       // close the file

    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: run the workload_run in levelDB
	inserted = 0;	
	queried = 0;
    for (int i = 0; i < READ_WRITE_NUM; ++ i)
    {	
		leveldKey = leveldbKeys[i];
        if (ifInsert[i])
        {	
			inserted ++;
            status = db->Put(write_options, leveldKey, leveldKey);
        }
        else
        {
			queried ++;
            status = db->Get(read_options, leveldKey, &res);
        }
    }
	clock_gettime(CLOCK_MONOTONIC, &finish);
    fclose(ycsb);
	single_time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Run phase finishes: %lu/%lu items are inserted/searched\n", inserted, queried);
    printf("Run phase throughput: %f operations per second \n", READ_WRITE_NUM/single_time);	
    return 0;

	delete [] keys;
	delete [] ifInsert;
	delete [] leveldbKeys;
}

