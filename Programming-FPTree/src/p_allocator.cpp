#include"utility/p_allocator.h"
#include<iostream>
#include<string.h>

using namespace std;

// the file that store the information of allocator
const string P_ALLOCATOR_CATALOG_NAME = "p_allocator_catalog";
// a list storing the free leaves
const string P_ALLOCATOR_FREE_LIST    = "free_list";

PAllocator* PAllocator::pAllocator = new PAllocator();

PAllocator* PAllocator::getAllocator() {
    if (PAllocator::pAllocator == nullptr) {
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
		this->catalogPmemAddr = (char *)pmem_map_file(allocatorCatalogPath.c_str(), CATALOG_SIZE, PMEM_FILE_CREATE, 0666, NULL, NULL);
		this->freeListPmemAddr = (char *)pmem_map_file(freeListPath.c_str(), FREE_LIST_SIZE, PMEM_FILE_CREATE, 0666, NULL, NULL);
		allocatorCatalog.read((char*)&this->maxFileId, sizeof(uint64_t));
		allocatorCatalog.read((char*)&this->freeNum, sizeof(uint64_t));
		allocatorCatalog.read((char *)&this->startLeaf, sizeof(PPointer));
        for(uint64_t i = 0; i < this->freeNum; i ++) {
			PPointer temp;
			freeListFile.read((char*)&temp.fileId, sizeof(uint64_t));
			freeListFile.read((char*)&temp.offset, sizeof(uint64_t));
			this->freeList.emplace_back(temp);
		}
		persistCatalog();
		persistFreeList();
    } else {
        // not exist, create catalog and free_list file, then open.
        // TODO
		FILE *f1 = fopen(allocatorCatalogPath.c_str(), "wb");
		fclose(f1);
		FILE *f2 = fopen(freeListPath.c_str(), "wb");
		fclose(f2);

		this->catalogPmemAddr = (char *)pmem_map_file(allocatorCatalogPath.c_str(), CATALOG_SIZE, PMEM_FILE_CREATE, 0666, NULL, NULL);
		this->freeListPmemAddr = (char *)pmem_map_file(freeListPath.c_str(), 4096, PMEM_FILE_CREATE, 0666, NULL, NULL);
		this->maxFileId = 1;
		this->freeNum = 0;
		this->startLeaf.fileId = 0;
		this->startLeaf.offset = 0;
		
		persistCatalog();
		persistFreeList();
    }
    this->initFilePmemAddr();
	persistLeafGroup();
	cout << "PAllocator(): " << freeNum << ' ' << freeList.size() << endl;
}

PAllocator::~PAllocator() {
    // TODO
	cout << "before ~PAllocator(): " << freeNum << ' ' << freeList.size() << endl;
	persistCatalog();
	persistFreeList();
	persistLeafGroup();

	pmem_unmap(this->catalogPmemAddr, CATALOG_SIZE);
	pmem_unmap(this->freeListPmemAddr, FREE_LIST_SIZE);
	for(uint64_t i = 1; i < this->maxFileId; i ++){
		pmem_unmap(this->fId2PmAddr[i], LEAF_GROUP_SIZE);
	}

	PAllocator::pAllocator = nullptr;
	cout << "after ~PAllocator(): " << freeNum << ' ' << freeList.size() << endl;
}

// memory map all leaves to pmem address, storing them in the fId2PmAddr
void PAllocator::initFilePmemAddr() {
    // TODO
    this->fId2PmAddr.clear();
    for(uint64_t i = 1; i < getMaxFileId(); i ++) {
        string path;
        path = DATA_DIR + to_string(i);
        this->fId2PmAddr.emplace(i, (char *)pmem_map_file(path.c_str(), LEAF_GROUP_SIZE, PMEM_FILE_CREATE, 0666, NULL, NULL));
    }
}

// get the pmem address of the target PPointer from the map fId2PmAddr
char* PAllocator::getLeafPmemAddr(PPointer p) {
    // TODO
    return this->fId2PmAddr.count(p.fileId) ? (this->fId2PmAddr[p.fileId] + p.offset) : nullptr;
}

// get and use a leaf for the fptree leaf allocation
// return 
bool PAllocator::getLeaf(PPointer &p, char* &pmem_addr) {
	cout << "before getLeaf(): " << freeNum << ' ' << freeList.size() << endl;
    // TODO
	if(this->freeNum == 0){
		if(newLeafGroup() == false){
			return false;
		}
	}

	p = this->freeList.back();
	this->freeList.pop_back();
	this->freeNum --;

	uint64_t usedNum_temp;
	Byte bitmap_temp;
	memcpy(&usedNum_temp, this->fId2PmAddr[p.fileId], sizeof(uint64_t));
	usedNum_temp ++;
	bitmap_temp = 1;
	memcpy(this->fId2PmAddr[p.fileId], &usedNum_temp, sizeof(uint64_t));
	memcpy(this->fId2PmAddr[p.fileId] + sizeof(uint64_t) + (p.offset - LEAF_GROUP_HEAD)/LEAF_SIZE, &bitmap_temp, sizeof(Byte));

	pmem_addr = this->fId2PmAddr[p.fileId] + p.offset;

	if(this->startLeaf.fileId == 0){
		this->startLeaf = p;
	}
	
	persistCatalog();
	persistFreeList();
	persistLeafGroup();
	cout << "after getLeaf(): " << freeNum << ' ' << freeList.size() << endl;
	return true;
}

bool PAllocator::ifLeafUsed(PPointer p) {
    // TODO
	if(ifLeafExist(p)){
		Byte ifUsed;
		memcpy(&ifUsed, this->fId2PmAddr[p.fileId] + sizeof(uint64_t) + (p.offset - LEAF_GROUP_HEAD)/LEAF_SIZE, sizeof(Byte));
		return (ifUsed == 1 ? true : false);
			
	}
    return false;
}

bool PAllocator::ifLeafFree(PPointer p) {
    // TODO
    if(ifLeafExist(p)){
		Byte ifUsed;
		memcpy(&ifUsed, this->fId2PmAddr[p.fileId] + sizeof(uint64_t) + (p.offset - LEAF_GROUP_HEAD)/LEAF_SIZE, sizeof(Byte));
		return (ifUsed == 1 ? false : true);
			
	}
    return false;
}

// judge whether the leaf with specific PPointer exists. 
bool PAllocator::ifLeafExist(PPointer p) {
    // TODO
	if(p.fileId <= 0 || p.fileId >= this->maxFileId){
		return false;
	}
	if((p.offset - LEAF_GROUP_HEAD) % LEAF_SIZE != 0){
		return false;
	}
	return true;
}

// free and reuse a leaf
bool PAllocator::freeLeaf(PPointer p) {
	cout << "before freeLeaf(): " << freeNum << ' ' << freeList.size() << endl;
    // TODO
	if(ifLeafUsed(p)){
		PPointer freeLeaf_temp{p.fileId, p.offset};
		this->freeList.emplace_back(freeLeaf_temp);
		this->freeNum ++;

		uint64_t usedNum_temp;
		Byte bitmap_temp;
		memcpy(&usedNum_temp, this->fId2PmAddr[p.fileId], sizeof(uint64_t));
		usedNum_temp --;
		bitmap_temp = 0;
		memcpy(this->fId2PmAddr[p.fileId], &usedNum_temp, sizeof(uint64_t));
		memcpy(this->fId2PmAddr[p.fileId] + sizeof(uint64_t) + (p.offset - LEAF_GROUP_HEAD)/LEAF_SIZE, &bitmap_temp, sizeof(Byte));
		
		persistCatalog();
		persistFreeList();
		persistLeafGroup();
		cout << "after freeLeaf(): " << freeNum << ' ' << freeList.size() << endl;
		return true;
	}
    return false;
}

bool PAllocator::persistCatalog() {
	memset(catalogPmemAddr, 0, CATALOG_SIZE);
	memcpy(catalogPmemAddr, &this->maxFileId, sizeof(uint64_t));
	memcpy(catalogPmemAddr + sizeof(uint64_t), &this->freeNum, sizeof(uint64_t));
	memcpy(catalogPmemAddr + sizeof(uint64_t) + sizeof(uint64_t), (char *)&this->startLeaf, sizeof(PPointer));
    // TODO
	if(pmem_is_pmem(this->catalogPmemAddr, CATALOG_SIZE)){
		pmem_persist(this->catalogPmemAddr, CATALOG_SIZE);
	}
	else{
		pmem_msync(this->catalogPmemAddr, CATALOG_SIZE);
	}
    return true;
}

bool PAllocator::persistFreeList() {
	cout << "before persistFreeList(): " << freeNum << ' ' << freeList.size() << endl;
	memset(freeListPmemAddr, 0, FREE_LIST_SIZE);
	for(uint64_t i = 0; i < this->freeList.size(); i ++){
		memcpy(freeListPmemAddr + sizeof(uint64_t) * 2 * i, &this->freeList[i].fileId, sizeof(uint64_t));
		memcpy(freeListPmemAddr + sizeof(uint64_t) + sizeof(uint64_t) * 2 * i, &this->freeList[i].offset, sizeof(uint64_t));
	}

	if(pmem_is_pmem(this->freeListPmemAddr, FREE_LIST_SIZE)){
		pmem_persist(this->freeListPmemAddr, FREE_LIST_SIZE);
	}
	else{
		pmem_msync(this->freeListPmemAddr, FREE_LIST_SIZE);
	}
	cout << "after persistFreeList(): " << freeNum << ' ' << freeList.size() << endl;
    return true;
}

bool PAllocator::persistLeafGroup() {
	for(uint64_t i = 1; i < this->maxFileId; i ++){
		if(pmem_is_pmem(this->fId2PmAddr[i], LEAF_GROUP_SIZE)){
			pmem_persist(this->fId2PmAddr[i], LEAF_GROUP_SIZE);
		}
		else{
			pmem_msync(this->fId2PmAddr[i], LEAF_GROUP_SIZE);
		}
	}
	return true;
}
/*
  Leaf group structure: (uncompressed)
  | usedNum(8b) | bitmap(n * byte) | leaf1 |...| leafn |
*/
// create a new leafgroup, one file per leafgroup
bool PAllocator::newLeafGroup() {
	cout << "before newLeafGroup(): " << freeNum << ' ' << freeList.size() << endl;
    // TODO
	string path;
	path = DATA_DIR + to_string(this->maxFileId);
	FILE *f = fopen(path.c_str(), "wb");
	fclose(f);

	char *fId2PmAddr_temp = (char *)pmem_map_file(path.c_str(), LEAF_GROUP_SIZE, PMEM_FILE_CREATE, 0666, NULL, NULL);
	if(!fId2PmAddr_temp){
    	return false;
	}

	uint64_t usedNum_temp = 0;
	memset(fId2PmAddr_temp, 0, LEAF_GROUP_SIZE);
	memcpy(fId2PmAddr_temp, &usedNum_temp, sizeof(uint64_t));

	this->fId2PmAddr.emplace(this->maxFileId, fId2PmAddr_temp);

	
	for(int i = 0; i < LEAF_GROUP_AMOUNT; i ++){
		PPointer freeLeaf_temp{this->maxFileId, LEAF_GROUP_HEAD + LEAF_SIZE * i};
		this->freeList.emplace_back(freeLeaf_temp);
	}

	this->maxFileId++;
	this->freeNum += LEAF_GROUP_AMOUNT;
	
	persistCatalog();
	persistFreeList();
	persistLeafGroup();
	cout << "after newLeafGroup(): " << freeNum << ' ' << freeList.size() << endl;
	return true;
}
void PAllocator::setStartLeafPointer(const PPointer p1){
    this->startLeaf = p1;
    updateCatalog();
}
