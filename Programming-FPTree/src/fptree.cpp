#include"fptree/fptree.h"

using namespace std;

// Initial the new InnerNode
InnerNode::InnerNode(const int& d, FPTree* const& t, bool _isRoot) {
    // TODO
	tree = t;
	degree = d;
	isLeaf = false;
	isRoot = _isRoot;
	nKeys = 0;
	nChild = 0;
	keys = new Key[2 * d + 1];
	childrens = new Node *[2 * d + 2];
}

// delete the InnerNode
InnerNode::~InnerNode() {
    // TODO
	delete [] keys;
	delete [] childrens;
}

// binary search the first key in the innernode larger than input key
int InnerNode::findIndex(const Key& k) {
    // TODO
	for(int i = 0; i < nKeys; i ++) {
		if(keys[i] > k)
			return i;
	}
	if(nChild == 0){
		return -1;
	}
    return nKeys;
}

// insert the node that is assumed not full
// insert format:
// ======================
// | key | node pointer |
// ======================
// WARNING: can not insert when it has no entry
void InnerNode::insertNonFull(const Key& k, Node* const& node) {
    // TODO
	int index = findIndex(k);
	memcpy(keys + index + 1, keys + index, sizeof(Key) * (nKeys - index));
	memcpy(childrens + index + 2, childrens + index + 1, sizeof(Node *) * (nChild - index - 1));
	keys[index] = k;
	childrens[index + 1] = node;
	nKeys ++;
	nChild ++;
}

// insert func
// return value is not NULL if split, returning the new child and a key to insert
KeyNode* InnerNode::insert(const Key& k, const Value& v) {
    KeyNode* newChild = NULL;
    // 1.insertion to the first leaf(only one leaf)
    if (this->isRoot && this->nKeys == 0) {
        // TODO
		if(this->nChild == 0){
			LeafNode *leafNode = new LeafNode(tree);
			leafNode->insert(k, v);
			childrens[nChild] = leafNode;
			nChild ++;
		}
		else if(nChild == 1) {
			int index = findIndex(k);
			if((newChild = childrens[index]->insert(k, v)) != NULL){
				if(nKeys < 2 * degree - 1){
					insertNonFull(newChild->key, newChild->node);
				}
			}
		}
        return newChild;
    }
    
    // 2.recursive insertion
    // TODO
	int index = findIndex(k);
	childrens[index]->insert(k, v);
    return newChild;
}

// ensure that the leaves inserted are ordered
// used by the bulkLoading func
// inserted data: | minKey of leaf | LeafNode* |
KeyNode* InnerNode::insertLeaf(const KeyNode& leaf) {
    KeyNode* newChild = NULL;
    // first and second leaf insertion into the tree
    if (this->isRoot && this->nKeys == 0) {
        // TODO
		if(this->nChild == 0){
			childrens[nChild] = leaf.node;
			nChild ++;
		}
		else if(this->nChild == 1) {
			if(nKeys < 2 * degree - 1){
				insertNonFull(leaf.key, leaf.node);
			}
		}
        return newChild;
    }
    
    // recursive insert
    // Tip: please judge whether this InnerNode is full
    // next level is not leaf, just insertLeaf
    // TODO

    // next level is leaf, insert to childrens array
    // TODO

    return newChild;
}

KeyNode* InnerNode::split() {
    KeyNode* newChild = new KeyNode();
    // right half entries of old node to the new node, others to the old node. 
    // TODO

    return newChild;
}

// remove the target entry
// return TRUE if the children node is deleted after removement.
// the InnerNode need to be redistributed or merged after deleting one of its children node.
bool InnerNode::remove(const Key& k, const int& index, InnerNode* const& parent, bool &ifDelete) {
    bool ifRemove = false;
    // only have one leaf
    // TODO
    
    // recursive remove
    // TODO
    return ifRemove;
}

// If the leftBro and rightBro exist, the rightBro is prior to be used
void InnerNode::getBrother(const int& index, InnerNode* const& parent, InnerNode* &leftBro, InnerNode* &rightBro) {
    // TODO
}

// merge this node, its parent and left brother(parent is root)
void InnerNode::mergeParentLeft(InnerNode* const& parent, InnerNode* const& leftBro) {
    // TODO
}

// merge this node, its parent and right brother(parent is root)
void InnerNode::mergeParentRight(InnerNode* const& parent, InnerNode* const& rightBro) {
    // TODO
}

// this node and its left brother redistribute
// the left has more entries
void InnerNode::redistributeLeft(const int& index, InnerNode* const& leftBro, InnerNode* const& parent) {
    // TODO
}

// this node and its right brother redistribute
// the right has more entries
void InnerNode::redistributeRight(const int& index, InnerNode* const& rightBro, InnerNode* const& parent) {
    // TODO
}

// merge all entries to its left bro, delete this node after merging.
void InnerNode::mergeLeft(InnerNode* const& leftBro, const Key& k) {
    // TODO
}

// merge all entries to its right bro, delete this node after merging.
void InnerNode::mergeRight(InnerNode* const& rightBro, const Key& k) {
    // TODO
}

// remove a children from the current node, used by remove func
void InnerNode::removeChild(const int& keyIdx, const int& childIdx) {
    // TODO
	delete childrens[childIdx];
	nKeys --;
	nChild --;
	memcpy(keys + keyIdx, keys + keyIdx + 1, sizeof(Key) * (nKeys - keyIdx - 1));
	memcpy(childrens + childIdx, childrens + childIdx + 1, sizeof(Node *) * (nChild - childIdx - 1));
}

// update the target entry, return true if the update succeed.
bool InnerNode::update(const Key& k, const Value& v) {
    // TODO
	int index = findIndex(k);
    return index != -1 && childrens[index]->update(k, v);
}

// find the target value with the search key, return MAX_VALUE if it fails.
Value InnerNode::find(const Key& k) {
    // TODO
	if(!this->isLeaf){
		int index = findIndex(k);
		return childrens[index]->find(k);
	}
	else{
		return find(k);
	}
}

// get the children node of this InnerNode
Node* InnerNode::getChild(const int& idx) {
    // TODO
	if(idx < this->nChild && idx >= 0){
		return this->childrens[idx];
	}
    return NULL;
}

// get the key of this InnerNode
Key InnerNode::getKey(const int& idx) {
    if (idx < this->nKeys && idx >= 0) {
        return this->keys[idx];
    } else {
        return MAX_KEY;
    }
}

// print the InnerNode
void InnerNode::printNode() {
    cout << "||#|";
    for (int i = 0; i < this->nKeys; i++) {
        cout << " " << this->keys[i] << " |#|";
    }
    cout << "|" << "    ";
}

// print the LeafNode
void LeafNode::printNode() {
    cout << "||";
    for (int i = 0; i < 2 * this->degree; i++) {
        if (this->getBit(i)) {
            cout << " " << this->kv[i].k << " : " << this->kv[i].v << " |";
        }
    }
    cout << "|" << " ====>> ";
}

// new a empty leaf and set the valuable of the LeafNode
LeafNode::LeafNode(FPTree* t) {
	// TODO	
	tree = t;
	isLeaf = true;
	degree = LEAF_DEGREE;
	
	n = 0;
	prev = next = nullptr;
	PAllocator::getAllocator()->getLeaf(pPointer, pmem_addr);
	filePath = DATA_DIR + to_string(pPointer.fileId);
	bitmapSize = (2 * LEAF_DEGREE + 7) / 8;

	bitmap = new Byte[bitmapSize];
	pNext = new PPointer({0, 0});
	fingerprints = new Byte[2 * LEAF_DEGREE];
	kv = new KeyValue[2 * LEAF_DEGREE];
	memset((char *)bitmap, 0, bitmapSize * sizeof(Byte));
	memset((char *)fingerprints, 0, 2 * LEAF_DEGREE * sizeof(Byte));
	memset((char *)kv, 0, 2 * LEAF_DEGREE * sizeof(KeyValue));

}

// reload the leaf with the specific Persistent Pointer
// need to call the PAllocator
LeafNode::LeafNode(PPointer p, FPTree* t) {
    // TODO
	tree = t;
	isLeaf = true;
	degree = LEAF_DEGREE;

	n = 0;
	pPointer = p;
	pmem_addr = PAllocator::getAllocator()->getLeafPmemAddr(p);
	filePath = DATA_DIR + to_string(p.fileId);
	bitmapSize = (2 * LEAF_DEGREE + 7) / 8;

	bitmap = new Byte[bitmapSize];
	pNext = new PPointer({0, 0});
	fingerprints = new Byte[2 * LEAF_DEGREE];
	kv = new KeyValue[2 * LEAF_DEGREE];
	memcpy((char *)bitmap, pmem_addr, bitmapSize * sizeof(Byte));
	memcpy((char *)pNext, pmem_addr + bitmapSize * sizeof(Byte), sizeof(PPointer));
	memcpy((char *)fingerprints, pmem_addr + bitmapSize * sizeof(Byte) + sizeof(PPointer),2 * LEAF_DEGREE * sizeof(Byte));
	memcpy((char *)kv, pmem_addr + bitmapSize * sizeof(Byte) + sizeof(PPointer) + 2 * LEAF_DEGREE * sizeof(Byte), 2 * LEAF_DEGREE * (sizeof(Key) + sizeof(Value)));
	
	for(int i = 0; i < bitmapSize; i ++){
		n += countOneBits(bitmap[i]);
	}
	
}

LeafNode::~LeafNode() {
    // TODO
	updatePmem();
	persist();
	delete [] bitmap;
	delete [] fingerprints;
	delete [] kv;
}

// insert an entry into the leaf, need to split it if it is full
KeyNode* LeafNode::insert(const Key& k, const Value& v) {
    KeyNode *newChild = NULL;
    // TODO
	if(update(k, v)){
		return newChild;
	}
	if(n >= 2 * degree - 1){
		insertNonFull(k, v);
		newChild = split();
	}
	else{
		insertNonFull(k, v);
	}
    return newChild;
}

// insert into the leaf node that is assumed not full
void LeafNode::insertNonFull(const Key& k, const Value& v) {
    // TODO
	n ++;
	int index = findFirstZero();
	bitmap[index / 8] |= 1 << index % 8;
	fingerprints[index] = keyHash(k);
	kv[index] = {k, v};

	updatePmem();
}

// split the leaf node


KeyNode* LeafNode::split() {

	Byte fingerprint_temp;
	KeyValue kv_temp;
	for(int i = 0; i < 2 * degree - 1; i ++){
		for(int j = 0; j < 2 * degree - 1 - i; j ++){
			if(kv[j].k > kv[j + 1].k){
				kv_temp = kv[j];
				kv[j] = kv[j + 1];
				kv[j + 1] = kv_temp;

				fingerprint_temp = fingerprints[j];
				fingerprints[j] = fingerprints[j + 1];
				fingerprints[j + 1] = fingerprint_temp;
			}
		}
	}

	updatePmem();

    KeyNode* newChild = new KeyNode();
    // TODO
	LeafNode *splitNode = new LeafNode(tree);
	splitNode->n = n + 1 / 2;
	LeafNode *prev_temp = this;
	splitNode->prev = prev_temp;
	for(int i = 0; i < splitNode->bitmapSize; i++){
		for(int j = 0; j < 8; j ++){
			if(j + i * 8 >= splitNode->n){
				break;
			}
			splitNode->bitmap[i] |= 1 << j;
		}
	}
	memcpy((char *)splitNode->fingerprints, (char *)fingerprints + sizeof(Byte) * (n / 2), splitNode->n * sizeof(Byte));
	memcpy((char *)splitNode->kv, (char *)kv + sizeof(KeyValue) * (n / 2), splitNode->n * sizeof(KeyValue));
	splitNode->updatePmem();
	
	n = n / 2;
	next = splitNode;
	*pNext = splitNode->getPPointer();
	for(int i = bitmapSize - 1; i >= 0; i --){
		for(int j = 7; j >= 0; j --){
			if(j + i * 8 < n){
				break;
			}
			splitNode->bitmap[i] &= 1 << j;
		}
	}
	memset((char *)fingerprints + sizeof(Byte) * n, 0, sizeof(Byte) * (2 * LEAF_DEGREE - n));
	memset((char *)kv + sizeof(KeyValue) * n, 0, sizeof(KeyValue) * (2 * LEAF_DEGREE - n));
	updatePmem();

	newChild->key = splitNode->kv[0].k;
	newChild->node = splitNode;
    return newChild;
}

// use to find a mediant key and delete entries less then middle
// called by the split func to generate new leaf-node
// qsort first then find
Key LeafNode::findSplitKey() {
    Key midKey = 0;
    // TODO
    return midKey;
}

// get the targte bit in bitmap
// TIPS: bit operation
int LeafNode::getBit(const int& idx) {
    // TODO
    return ((bitmap[idx / 8] >> (idx % 8)) & 1);
}

Key LeafNode::getKey(const int& idx) {
    return this->kv[idx].k;
}

Value LeafNode::getValue(const int& idx) {
    return this->kv[idx].v;
}

PPointer LeafNode::getPPointer() {
    return this->pPointer;
}

// remove an entry from the leaf
// if it has no entry after removement return TRUE to indicate outer func to delete this leaf.
// need to call PAllocator to set this leaf free and reuse it
bool LeafNode::remove(const Key& k, const int& index, InnerNode* const& parent, bool &ifDelete) {
    bool ifRemove = false;
    // TODO
    return ifRemove;
}

// update the target entry
// return TRUE if the update succeed
bool LeafNode::update(const Key& k, const Value& v) {
    bool ifUpdate = false;
    // TODO
	for(int i = 0; i < bitmapSize; i ++){
		for(int j = 0; j < 8; j ++) {
			if((bitmap[i] >> j) & 1){
				if(k == kv[j + i * 8].k){
					kv[j + i * 8].v = v;
					updatePmem();
					ifUpdate = true;
					return ifUpdate;
				}
			}
		}
	}	
    return ifUpdate;
}

// if the entry can not be found, return the max Value
Value LeafNode::find(const Key& k) {
    // TODO
	for(int i = 0; i < bitmapSize; i ++){
		for(int j = 0; j < 8; j ++) {
			if((bitmap[i] >> j) & 1){
				if(k == kv[j + i * 8].k){
					return kv[j + i * 8].v;
				}
			}
		}
	}			
    return MAX_VALUE;
}

// find the first empty slot
int LeafNode::findFirstZero() {
    // TODO
	for(int i = 0; i < bitmapSize; i ++){
		for(int j = 0; j < 8; j ++) {
			if(!((bitmap[i] >> j) & 1)) {
				return (j + i * 8);
			}
		}
	}
    return -1;
}

void LeafNode::updatePmem() {
	memcpy(pmem_addr, (char *)bitmap, bitmapSize * sizeof(Byte));
	memcpy(pmem_addr + bitmapSize * sizeof(Byte), (char *)pNext, sizeof(PPointer));
	memcpy(pmem_addr + bitmapSize * sizeof(Byte) + sizeof(PPointer), (char *)fingerprints, 2 * LEAF_DEGREE * sizeof(Byte));
	memcpy(pmem_addr + bitmapSize * sizeof(Byte) + sizeof(PPointer) + 2 * LEAF_DEGREE * sizeof(Byte), (char *)kv, 2 * LEAF_DEGREE * (sizeof(Key) + sizeof(Value)));
}

// persist the entire leaf
// use PMDK
void LeafNode::persist() {
    // TODO
	if(pmem_is_pmem(this->pmem_addr, LEAF_SIZE)){
		pmem_persist(this->pmem_addr, LEAF_SIZE);
	}
	else{
		pmem_msync(this->pmem_addr, LEAF_SIZE);
	}
}

// call by the ~FPTree(), delete the whole tree
void FPTree::recursiveDelete(Node* n) {
    if (n->isLeaf) {
        delete n;
    } else {
        for (int i = 0; i < ((InnerNode*)n)->nChild; i++) {
            recursiveDelete(((InnerNode*)n)->childrens[i]);
        }
        delete n;
    }
}

FPTree::FPTree(uint64_t t_degree) {
    FPTree* temp = this;
    this->root = new InnerNode(t_degree, temp, true);
    this->degree = t_degree;
    bulkLoading();
}

FPTree::~FPTree() {
    recursiveDelete(this->root);
}

// get the root node of the tree
InnerNode* FPTree::getRoot() {
    return this->root;
}

// change the root of the tree
void FPTree::changeRoot(InnerNode* newRoot) {
    this->root = newRoot;
}

void FPTree::insert(Key k, Value v) {
    if (root != NULL) {
        root->insert(k, v);
    }
}

bool FPTree::remove(Key k) {
    if (root != NULL) {
        bool ifDelete = false;
        InnerNode* temp = NULL;
        return root->remove(k, -1, temp, ifDelete);
    }
    return false;
}

bool FPTree::update(Key k, Value v) {
    if (root != NULL) {
        return root->update(k, v);
    }
    return false;
}

Value FPTree::find(Key k) {
    if (root != NULL) {
        return root->find(k);
    }
}

// call the InnerNode and LeafNode print func to print the whole tree
// TIPS: use Queue
void FPTree::printTree() {
    // TODO
}

// bulkLoading the leaf files and reload the tree
// need to traverse leaves chain
// if no tree is reloaded, return FALSE
// need to call the PALlocator
bool FPTree::bulkLoading() {
	// TODO
	PPointer pPointer = PAllocator::getAllocator()->getStartPointer();
	if(pPointer.fileId == 0){
    	return false;
	}
	FPTree *tree_temp = this;
	LeafNode *leafNode = new LeafNode(pPointer, tree_temp);
	KeyNode leaf = {leafNode->getKey(0), leafNode};
	root->insertLeaf(leaf);
	std::cout << "bulkLoading() " << pPointer.fileId << ' ' << pPointer.offset << endl;
	pPointer = *leafNode->pNext;
	std::cout << "bulkLoading()2 " << pPointer.fileId << ' ' << pPointer.offset << endl;
	if(pPointer.fileId != 0){
		FPTree *tree_temp2 = this;
		LeafNode *leafNode2 = new LeafNode(pPointer, tree_temp2);
		KeyNode leaf2 = {leafNode2->getKey(0), leafNode2};
		root->insertLeaf(leaf2);
	}
}
