#include <leveldb/db.h>
#include <string>

#define KEY_LEN 8
#define VALUE_LEN 8
using namespace std;

const string workload = "workloads/";

const string load = workload + "10w-rw-0-100-load.txt"; // TODO: the workload_load filename
const string run  = workload + "10w-rw-0-100-run.txt"; // TODO: the workload_run filename

const string filePath = "";

const int READ_WRITE_NUM = 10000; // TODO: how many operations

int main()
{        
    leveldb::DB* db;
    leveldb::Options options;
    leveldb::WriteOptions write_options;
    // TODO: open and initial the levelDB
    options.create_if_missing = true;
    leveldb::ReadOptions read_options;
    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    assert(status.ok());

    uint64_t inserted = 0, queried = 0, t = 0;
    uint64_t* key = new uint64_t[2200000]; // the key and value are same
    bool* ifInsert = new bool[2200000]; // the operation is insertion or not
    FILE *ycsb_load, *ycsb_run; // the files that store the ycsb operations
    char *buf = NULL;
    size_t len = 0;
    struct timespec start, finish; // use to caculate the time
    double single_time; // single operation time

    printf("Load phase begins \n");
    // TODO: read the ycsb_load and store
    // load the file
    ycsb_load = fopen(load.c_str(), "r");
    int index = 0;
    char line[100];
    while (fgets(line, 100, ycsb_load) != NULL)
    {
        string str = line;
        string op, value;
        bool judge = true;
        for (int i = 0; line[i] != '\0'; ++ i)
        {
            if (line[i] == ' ')
            {
                judge = false;
                continue;
            }
            if (judge)  //split the message
                op += line[i];
            else
                value += line[i];
        }
        // store the message
        if (op == "INSERT")
        {
            ifInsert[index] = true;
            inserted ++;    //count the operations
        }
        else
        {
            ifInsert[index] = false;
            queried ++;     //count the operations
        }
        key[index ++] = atoll(value.c_str());
    }

    fclose(ycsb_load);          // close the file

    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: load the workload in LevelDB
    // LevelDB operations: Put
    string res;
    char tmp[15];
    for (int i = 0; i < 100000; ++ i)
    {
        memcpy(tmp, key + i, 8);
        if (ifInsert[i])
        {
            status = db->Put(write_options, tmp, tmp);
        }
        else
        {
            status = db->Get(read_options, tmp, &res);
        }
    }
    
    //calculate the time
    clock_gettime(CLOCK_MONOTONIC, &finish);
    single_time = (finish.tv_sec - start.tv_sec) * 1000000000.0 + (finish.tv_nsec - start.tv_nsec);

    printf("Load phase finishes: %lu items are inserted \n", inserted);
    printf("Load phase used time: %fs\n", single_time / 1000000000.0);
    printf("Load phase single insert time: %fns\n", single_time / inserted);

    int operation_num = 0;
    inserted = 0;       

    // TODO:read the ycsb_run and store
    // load the file
    ycsb_run = fopen(run.c_str(), "r");
    index = 0;
    while (fgets(line, 100, ycsb_run) != NULL)
    {
        string str = line;
        string op, value;
        bool judge = true;
        for (int i = 0; line[i] != '\0'; ++ i)
        {
            if (line[i] == ' ')
            {
                judge = false;
                continue;
            }
            // split the message
            if (judge)
                op += line[i];
            else
                value += line[i];
        }
        if (op == "INSERT")
        {
            ifInsert[index] = true;
            inserted ++;
        }
        else
        {
            ifInsert[index] = false;
        }
        key[index ++] = atoll(value.c_str());
        operation_num ++;       //count the operations
    }

    fclose(ycsb_run);       // close the file

    clock_gettime(CLOCK_MONOTONIC, &start);

    // TODO: operate the levelDB
    // LevelDB operations
    for (int i = 0; i < 10000; ++ i)
    {
        memcpy(tmp, key + i, 8);
        if (ifInsert[i])
        {
            status = db->Put(write_options, tmp, tmp);
        }
        else
        {
            status = db->Get(read_options, tmp, &res);
        }
    }

    //calculate the time
    clock_gettime(CLOCK_MONOTONIC, &finish);
    single_time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Run phase finishes: %lu/%lu items are inserted/searched\n", inserted, operation_num - inserted);
    printf("Run phase throughput: %f operations per second \n", READ_WRITE_NUM/single_time);    
    return 0;
}
