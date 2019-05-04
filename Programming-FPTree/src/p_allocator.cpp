#include"utility/p_allocator.h"
#include<iostream>
using namespace std;

// the file that store the information of allocator
const string P_ALLOCATOR_CATALOG_NAME = "p_allocator_catalog";
// a list storing the free leaves
const string P_ALLOCATOR_FREE_LIST    = "free_list";

PAllocator* PAllocator::pAllocator = new PAllocator();

PAllocator* PAllocator::getAllocator() {
    if (PAllocator::pAllocator == NULL) {
        PAllocator::pAllocator = new PAllocator();
    }
    return PAllocator::pAllocator;
}

/* data storing structure of allocator
   In the catalog file, the data structure is listed below
   | maxFileId(8 bytes) | freeNum = m | treeStartLeaf(the PPointer) |
   In freeList file:
   | freeList{(fId, offset)1,...(fId, offset)m} |
*/
PAllocator::PAllocator() {
    string allocatorCatalogPath = DATA_DIR + P_ALLOCATOR_CATALOG_NAME;
    string freeListPath         = DATA_DIR + P_ALLOCATOR_FREE_LIST;
    ifstream allocatorCatalog(allocatorCatalogPath, ios::in|ios::binary);
    ifstream freeListFile(freeListPath, ios::in|ios::binary);
    // judge if the catalog exists
    if (allocatorCatalog.is_open() && freeListFile.is_open()) {
        // exist
        // TODO

        //read allocatorCatalog
        string catalog;
        getline(in, catalog);
        string maxFileID_s, freeNum_s, startID_s, startOffset_s;
        char *maxFileID_c;
        char *freeNum_c;
        char *startID_c;
        char *startOffset_c;

        //load maxFileID
        for(int i = 0; i < 64; i ++){
            maxFileID_s += catalog[i];
        }
        maxFileID_c =(char*)maxFileID_s.data();
        maxFileId = strtoull(maxFileID_c, NULL, 2);

        //load freeNum
        for(int i = 64; i < 128; i ++){
            freeNum_s += catalog[i];
        }
        freeNum_c =(char*)freeNum_s.data();
        freeNum = strtoull(freeNum_c, NULL, 2);

        //load startLeaf fileID
        for(int i = 128; i < 192; i ++){
            startID_s += catalog[i];
        }
        startID_c =(char*)startID_s.data();
        startLeaf.fileId = strtoull(startID_c, NULL, 2);

        //load startLeaf offset
        for(int i = 192; i < 256; i ++){
            startOffset_s += catalog[i];
        }
        startOffset_c =(char*)startOffset_s.data();
        startLeaf.offset = strtoull(startOffset_c, NULL, 2);

        allocatorCatalog.close();

        //read freeListFile
        string oneFreeLeaf;
        while(getline(freeListFile, oneFreeLeaf)){
            PPointer temp;
            string leafFileId, leafOffset;
            for(int i = 0; i < 128; i ++){
                leafFileId += oneFreeLeaf[i];
            }
            char *leafFileId_c = (char*)leafFileId.data();
            temp.fileId = strtoull(leafFileId_c, NULL, 2);
            for(int i = 128; i < 256; i ++){
                leafOffset += oneFreeLeaf[i];
            }
            char *leafOffset_c = (char*)leafOffset.data();
            temp.offset = strtoull(leafOffset_c, NULL, 2);
            freeList.push_back(temp);
        }
        freeListFile.close();

    } else {
        // not exist, create catalog and free_list file, then open.
        // TODO
        ofstream allocatorCatalogOut;
        allocatorCatalogOut.open(allocatorCatalogPath, ios::out|ios::binary);
        ofstream freeListFileOut;
        freeListFileOut.open(freeListPath, ios::out|ios::binary);
        maxFileId = 1;
        freeNum = 0;
        startLeaf.fileId = 0;
        startLeaf.offset = 0;
        bitset<64> b1(maxFileId);
        bitset<64> b2(freeNum);
        bitset<64> b3(startLeaf.fileId);
        bitset<64> b4(startLeaf.offset);
        allocatorCatalogOut << b1 << b2 << b3 << b4;
        allocatorCatalogOut.close();
        freeListFileOut.close();
    }
    this->initFilePmemAddr();
}

PAllocator::~PAllocator() {
    // TODO
    ofstream allocatorCatalogOut(allocatorCatalogPath, ios::out|ios::binary);
    ofstream freeListFileOut(freeListPath, ios::out|ios::binary);
    bitset<64> b1(maxFileId);
    bitset<64> b2(freeNum);
    bitset<64> b3(startLeaf.fileId);
    bitset<64> b4(startLeaf.offset);
    allocatorCatalogOut << b1 << b2 << b3 << b4;
    allocatorCatalogOut.close();
    for(int i = 0; i < freeNum; i ++){
        bitset<64> b5(freeList[i].fileId);
        bitset<64> b6(freeList[i].offset);
        freeListFileOut << b5 << b6 << endl;
    }
    freeListFileOut.close();
    pAllocator == NULL
}

#define LEAF_GROUP_LEN 28672
#include <sstream>
// memory map all leaves to pmem address, storing them in the fId2PmAddr
void PAllocator::initFilePmemAddr() {
    // TODO
    for(uint64_t i = 1; i < this->maxFileID; ++i){
        //get path name
        stringstream ss;
        ss << DATA_DIR;
        ss << i;
        ss << ".pmem";
        char str[70];
        int index = 0;
        char c;
        while(ss >> c){
            str[index] = c;
        }
        const char*path = str;
        //get addr
        char *pmemaddr = pmem_map_file(path, LEAF_GROUP_LEN, 
            PMEM_FILE_CREATE|PMEM_FILE_EXCL, 0666, NULL, NULL);
        fId2PmAddr.insert(make_pair(i, pmemaddr));
    }
}

// get the pmem address of the target PPointer from the map fId2PmAddr
char* PAllocator::getLeafPmemAddr(PPointer p) {
    // TODO
    map<uint64_t, char*>::iterator it;
    it = fId2PmAddr.find(p.fileId);
    if(it != fId2PmAddr.end()){
        uint64_t startAddr =  strtoull(it->second, NULL, 10);
        uint64_t targetAdd = startAddr + p.offset;
        bitset<64> tmpAddr(targetAdd);
        string s = tmpAddr.to_string();
        char *t = (char*)s.data();
        return t;
    }
    return NULL;
}

// get and use a leaf for the fptree leaf allocation
// return 
bool PAllocator::getLeaf(PPointer &p, char* &pmem_addr) {
    // TODO
    if(freeNum <= 0)
        return false;
    PPointer front = freeList[0];
    freeList.erase(freeList.begin());
    --freeNum;
    p.fileId = front.fileId;
    p.offset = front.offset;
    pmem_addr = getLeafPmemAddr(p);
    return true;
}

bool PAllocator::ifLeafUsed(PPointer p) {
    // TODO
    for(int i = 0; i < freeList.size(); i ++){
        if(p == freeList[i]){
            return false;
        }
    }
    return true;
}

bool PAllocator::ifLeafFree(PPointer p) {
    // TODO
    for(int i = 0; i < freeList.size(); i ++){
        if(p == freeList[i]){
            return true;
        }
    }
    return false;
}

// judge whether the leaf with specific PPointer exists. 
bool PAllocator::ifLeafExist(PPointer p) {
    // TODO
    map<uint64_t, char*>::iterator it;
    it = fId2PmAddr.find(p.fileId);
    if(it == fId2PmAddr.end()){
        return false;
    }
    else{
        if( (p.offset - 192) % 128 != 0 || p.offset < 192 || p.offset > 192 + 15 * 128){
            return false;
        }
        else return true;
    }
}

// free and reuse a leaf
bool PAllocator::freeLeaf(PPointer p) {
    // TODO
    freeList.push_back(p);
    ++freeNum;
    return true;
}

bool PAllocator::persistCatalog() {
    // TODO
    //get catalog path
    string allocatorCatalogPath = DATA_DIR + P_ALLOCATOR_CATALOG_NAME;
    stringstream ss;
    ss << allocatorCatalogPath;
    char catalogPath[100];
    char c;
    int index = 0;
    while(ss >> c){
        catalogPath[index] = c;
        ++index;
    }
    const char* fName = catalogPath;
    //get catalog pmemaddr
    char *pmemaddr = pmem_map_file(fName, LEAF_GROUP_LEN, 
            PMEM_FILE_CREATE|PMEM_FILE_EXCL, 0666, NULL, NULL);
    if(pmemaddr == NULL)
        return false;
    //persist
    pmem_persist(pmemaddr, 32); //catalog 32 bytes
    return true;
}

/*
  Leaf group structure: (uncompressed)
  | usedNum(8bytes) | bitmap(n * bytes) | leaf1 |...| leafn | n=16
  each leaf, 64bits-value, 64bits-key, 128bits in all
*/
// create a new leafgroup, one file per leafgroup
#include <bitset>
bool PAllocator::newLeafGroup() {
    // TODO
    //update freeNum
    freeNum += 16;
    //get file name
    stringstream ss;
    ss << DATA_DIR;
    ss << maxFileId;
    ss << ".pmem";
    char fileName[64];
    char tmp;
    int index = 0;
    while(ss >> tmp){
        fileName[index] = tmp;
        ++index;
    }
    const char*fName = fileName;
    //update maxFileId
    ++maxFileId;
    //create and init file
    ofstream fs(fName, ios::binary);
    if(!fs.is_open())
        return false;
    bitset<64> usedNum(0);
    bitset<128>  bitmap(0);
    fs << usedNum << bitmap;
    //init each leaf and update freeList
    for(int i = 0; i < 16; ++i){
        bitset<128> leaf(0);
        fs << leaf;
        PPointer leafPointer;
        leafPointer.fileId = maxFileId;
        bitset<64> offset(192+i*128);
        leafPointer.offset = offset;
        freeList.push_back(leafPointer);
    }
    fs.close();
    //update fId2PmAddr
    char *pmemaddr = pmem_map_file(fName, LEAF_GROUP_LEN, 
            PMEM_FILE_CREATE|PMEM_FILE_EXCL, 0666, NULL, NULL);
    if(pmemaddr == NULL)
        return false;
    fId2PmAddr.insert(make_pair(maxFileId, pmemaddr));
    return true;
}
