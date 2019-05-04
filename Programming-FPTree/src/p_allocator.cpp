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
}

// memory map all leaves to pmem address, storing them in the fId2PmAddr
void PAllocator::initFilePmemAddr() {
    // TODO
}

// get the pmem address of the target PPointer from the map fId2PmAddr
char* PAllocator::getLeafPmemAddr(PPointer p) {
    // TODO
    return NULL;
}

// get and use a leaf for the fptree leaf allocation
// return 
bool PAllocator::getLeaf(PPointer &p, char* &pmem_addr) {
    // TODO
    return false;
}

bool PAllocator::ifLeafUsed(PPointer p) {
    // TODO
    return false;
}

bool PAllocator::ifLeafFree(PPointer p) {
    // TODO
    return false;
}

// judge whether the leaf with specific PPointer exists. 
bool PAllocator::ifLeafExist(PPointer p) {
    // TODO
}

// free and reuse a leaf
bool PAllocator::freeLeaf(PPointer p) {
    // TODO
    return false;
}

bool PAllocator::persistCatalog() {
    // TODO
    return false;
}

/*
  Leaf group structure: (uncompressed)
  | usedNum(8b) | bitmap(n * byte) | leaf1 |...| leafn |
*/
// create a new leafgroup, one file per leafgroup
bool PAllocator::newLeafGroup() {
    // TODO
    return false;
}
