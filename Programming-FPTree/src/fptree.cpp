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
	if(nChild == 0){
		return -1;
	}
	for(int i = 0; i < nKeys; i ++) {
		if(keys[i] > k)
			return i;
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
				insertNonFull(newChild->key, newChild->node);
				return NULL;
			}
		}
        return newChild;
    }

    // 2.recursive insertion
    // TODO
	if((newChild = this->childrens[findIndex(k)]->insert(k, v)) != NULL){
		if(this->nKeys >= 2 * degree){
			insertNonFull(newChild->key, newChild->node);
			newChild = split();
			if(this->isRoot == true){
				InnerNode *newRoot = new InnerNode(degree, tree, true);
				newRoot->childrens[newRoot->nChild] = this;
				newRoot->nChild ++;
				newRoot->insertNonFull(newChild->key, newChild->node);
				this->isRoot = false;
				tree->root = newRoot;
				return NULL;
			}
		}
		else{
			insertNonFull(newChild->key, newChild->node);
			return NULL;
		}
	}

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
			insertNonFull(leaf.key, leaf.node);
		}
        return newChild;
    }
    // recursive insert
    // Tip: please judge whether this InnerNode is full
    // next level is not leaf, just insertLeaf
    // TODO

    // next level is leaf, insert to childrens array
    // TODO
	if(this->childrens[0]->ifLeaf() == false){
		if((newChild = dynamic_cast<InnerNode *>(this->childrens[findIndex(leaf.key)])->insertLeaf(leaf)) != NULL){
			if(this->nKeys >= 2 * degree){
				insertNonFull(newChild->key, newChild->node);
				newChild = split();
				if(this->isRoot == true){
					InnerNode *newRoot = new InnerNode(degree, tree, true);
					newRoot->childrens[newRoot->nChild] = this;
					newRoot->nChild ++;
					newRoot->insertNonFull(newChild->key, newChild->node);
					this->isRoot = false;
					tree->root = newRoot;
					return NULL;
				}
			}
			else{
				insertNonFull(newChild->key, newChild->node);
				return NULL;
			}
		}
	}
	else{
		if(this->nKeys >= 2 * degree){
			insertNonFull(leaf.key, leaf.node);
			newChild = split();
			if(this->isRoot == true){
				InnerNode *newRoot = new InnerNode(degree, tree, true);
				newRoot->childrens[newRoot->nChild] = this;
				newRoot->nChild ++;
				newRoot->insertNonFull(newChild->key, newChild->node);
				this->isRoot = false;
				tree->root = newRoot;
				return NULL;
			}
		}
		else{
			insertNonFull(leaf.key, leaf.node);
			return NULL;
		}
	}
    return newChild;
}

KeyNode* InnerNode::split() {
    KeyNode* newChild = new KeyNode();
	InnerNode *innerNode = new InnerNode(degree, tree, false);
    // right half entries of old node to the new node, others to the old node.
    // TODO
	newChild->node = innerNode;
	innerNode->nKeys = degree;
	innerNode->nChild = degree + 1;
	memcpy(innerNode->keys, keys + degree + 1, sizeof(Key) * degree);
	memcpy(innerNode->childrens, childrens + degree + 1, sizeof(Node *) * (degree + 1));


	nKeys = degree;
	nChild = degree + 1;

	newChild->key = keys[nKeys];

	memset(keys +  nKeys, 0, sizeof(Key) * (degree + 1));
	memset(childrens +  nChild, 0, sizeof(Node *) * (degree + 1));

    return newChild;
}

// remove the target entry
// return TRUE if the children node is deleted after removement.
// the InnerNode need to be redistributed or merged after deleting one of its children node.
bool InnerNode::remove(const Key& k, const int& index, InnerNode* const& parent, bool &ifDelete) {
    bool ifRemove = false;
    int keyIndex = this->findIndex(k);
    ifRemove = childrens[keyIndex]->remove(k, keyIndex, this, ifDelete);
    //child node is deleted
    if(ifDelete == true){
        cout << "child node is deleted" << endl;
        this->removeChild(keyIndex, keyIndex);
        // only have one leaf
        // TODO
        if(this->isRoot == true && this->nChild == 1 && this->childrens[0]->ifLeaf() == true){
            cout << "only have one leaf" << endl;
            this->tree->root = (InnerNode*)childrens[0];
            this->tree->root->isRoot = true;
            ifDelete = true;
            return ifRemove;
        }
        // recursive remove
        // TODO

        //entries not enough
        if(this->nChild < degree + 1 && this->isRoot == false){
            cout << "entries not enough" << endl;
            InnerNode* leftBro;
            InnerNode* rightBro;
            this->getBrother(index, parent, leftBro, rightBro);

            //Case 2: redistributed with left brother
            if(leftBro != NULL && leftBro->nChild > this->degree + 1){
                cout << "Case 2: redistributed with left brother" << endl;
                redistributeLeft(index, leftBro, parent);
                ifDelete = false;
                return ifRemove;
            }
            //Case 3: redistributed with right brother
            if(rightBro != NULL && rightBro->nChild > this->degree + 1){
                cout << "Case 3: redistributed with right brother" << endl;
                redistributeRight(index, rightBro, parent);
                ifDelete = false;
                return ifRemove;
            }
            //Case 4:merge with left brother
            if(leftBro != NULL){
                cout << "Case 4: merge with left brother" << endl;
                mergeLeft(leftBro, k);
                cout << "Case 4: Finish!" << endl;
                /*
                for(int i = index; i < parent->nChild - 1; i ++){
                    parent->childrens[i] = parent->childrens[i + 1];
                }
                cout << "Case 4: children Finish!" << endl;
                for(int i = index - 1; i < parent->nKeys - 1; i ++){
                    parent->keys[i] = parent->keys[i + 1];
                }
                cout << "Case 4: key Finish!" << endl;
                parent->nChild --;
                parent->nKeys --;
                */
                ifDelete = true;
                return ifRemove;
            }
            //Case 5:merge with right brother
            if(rightBro != NULL){
                cout << "Case 5:merge with right brother" << endl;
                mergeRight(rightBro, k);
                /*
                for(int i = index; i < parent->nChild - 1; i ++){
                    parent->childrens[i] = parent->childrens[i + 1];
                }
                for(int i = index; i < parent->nKeys - 1; i ++){
                    parent->keys[i] = parent->keys[i + 1];
                }
                parent->nChild --;
                parent->nKeys --;
                */
                parent->keys[index + 1] = rightBro->keys[0];
                ifDelete = true;
                return ifRemove;
            }
            //Case 6: parent only has two children and is root
            if(parent->isRoot == true && parent->nChild == 2){
                cout << "Case 6: parent only has two children and is root" << endl;
                //merge left
                if(leftBro != NULL){
                    mergeParentLeft(parent, leftBro);
                    ifDelete = true;
                }
                //merge right
                else if(rightBro != NULL){
                    mergeParentRight(parent, rightBro);
                    ifDelete = true;
                }
                return ifRemove;
            }
            return false;
        }
        ifDelete = false;
        return ifRemove;
    }
    //child node is not deleted
     cout << "child node is not deleted" << endl;
    return ifRemove;
}

// If the leftBro and rightBro exist, the rightBro is prior to be used
void InnerNode::getBrother(const int& index, InnerNode* const& parent, InnerNode* &leftBro, InnerNode* &rightBro) {
    // TODO
    if (parent != NULL && index > 0){
        leftBro = (InnerNode*)(parent->childrens[index - 1]);
    }
    if (parent != NULL && index + 1 < parent->nChild){
        rightBro = (InnerNode*)(parent->childrens[index + 1]);
    }
}

// merge this node, its parent and left brother(parent is root)
void InnerNode::mergeParentLeft(InnerNode* const& parent, InnerNode* const& leftBro) {
    cout << "Merge parent left" << endl;
    // TODO
    for(int i = 0; i < this->nChild; i ++){
        leftBro->childrens[leftBro->nChild ++] = this->childrens[i];
    }
    for(int i = 0; i < this->nKeys; i ++){
        leftBro->keys[leftBro->nKeys ++] = this->keys[i];
    }
    tree->root = leftBro;
    leftBro->isRoot = true;
    /*
    if(parent->nChild == 2){
        tree->root = leftBro;
        leftBro->isRoot = true;
    }
    else{
        int ChildPostion = findIndex(this->keys[0]);
        int KeyPostion = Childpostion - 1;
        for(int i = ChildPostion + 1; i < nChild; i ++){
            parent->childrens[i - 1] = parent->childrens[i];
        }
        for(int i = KeyPostion - 1; i < nKeys; i ++){
            parent->keys[i - 1] = parent->keys[i];
        }
        parent->nChild --;
        parent->nKeys --;
    }
    */
}

// merge this node, its parent and right brother(parent is root)
void InnerNode::mergeParentRight(InnerNode* const& parent, InnerNode* const& rightBro) {
    cout << "Merge parent right" << endl;
    // TODO
    for(int i = 0; i < rightBro->nChild; i ++){
        this->childrens[this->nChild ++] = rightBro->childrens[i];
    }
    for(int i = 0; i < rightBro->nKeys; i ++){
        this->keys[this->nKeys ++] = rightBro->keys[i];
    }
    tree->root = this;
    this->isRoot = true;
}

// this node and its left brother redistribute
// the left has more entries
void InnerNode::redistributeLeft(const int& index, InnerNode* const& leftBro, InnerNode* const& parent) {
    // TODO
    for(int i = this->nChild; i >= 0; i --){
        this->childrens[i + 1] = this->childrens[i];
    }
    for(int i = this->nKeys; i >= 0; i --){
        this->keys[i + 1] = this->keys[i];
    }
    this->childrens[0] = leftBro->childrens[leftBro->nChild - 1];
    this->keys[0] = leftBro->keys[leftBro->nKeys - 1];
    leftBro->nChild --;
    leftBro->nKeys --;
    this->nChild ++;
    this->nKeys ++;
    parent->keys[index - 1] = this->keys[0];
}

// this node and its right brother redistribute
// the right has more entries
void InnerNode::redistributeRight(const int& index, InnerNode* const& rightBro, InnerNode* const& parent) {
    // TODO
    this->childrens[this->nChild] = rightBro->childrens[0];
    this->keys[this->nKeys] = rightBro->keys[0];
    for(int i = 0; i < rightBro->nChild - 1; i ++){
        rightBro->childrens[i] = rightBro->childrens[i + 1];
    }
    for(int i = 0; i < rightBro->nKeys - 1; i ++){
        rightBro->keys[i] = rightBro->keys[i + 1];
    }
    this->nChild ++;
    this->nKeys ++;
    rightBro->nChild --;
    rightBro->nKeys --;
    parent->keys[index] = rightBro->keys[0];
}

// merge all entries to its left bro, delete this node after merging.
void InnerNode::mergeLeft(InnerNode* const& leftBro, const Key& k) {
    // TODO
    cout << "InnerNode::mergeLeft begin" << endl;
    for(int i = leftBro->nChild; i < leftBro->nChild + this->nChild; i ++){
        leftBro->childrens[i] = this->childrens[i - leftBro->nChild];
    }
    leftBro->nChild += this->nChild;
    for(int i = leftBro->nKeys; i < leftBro->nKeys + this->nKeys; i ++){
        leftBro->keys[i] = this->keys[i - leftBro->nKeys];
    }
    leftBro->nKeys += this->nKeys;
    cout << "InnerNode::mergeLeft finish" << endl;
}

// merge all entries to its right bro, delete this node after merging.
void InnerNode::mergeRight(InnerNode* const& rightBro, const Key& k) {
    cout << "Merge right" << endl;
    // TODO
    for(int i = rightBro->nChild - 1; i >= 0; i --){
        rightBro->childrens[i + this->nChild] = rightBro->childrens[i];
    }
    for(int i = 0; i < this->nChild; i ++){
        rightBro->childrens[i] = this->childrens[i];
    }
    rightBro->nChild += this->nChild;

    for(int i = rightBro->nKeys - 1; i >= 0; i --){
        rightBro->keys[i + this->nKeys] = rightBro->keys[i];
    }
    for(int i = 0; i < this->nKeys; i ++){
        rightBro->keys[i] = this->keys[i];
    }
    rightBro->nKeys += this->nKeys;
}

// remove a children from the current node, used by remove func
void InnerNode::removeChild(const int& keyIdx, const int& childIdx) {
    cout << "InnerNode::removeChild" << endl;
    // TODO
	delete childrens[childIdx];
	nKeys --;
	nChild --;
	memcpy(keys + keyIdx, keys + keyIdx + 1, sizeof(Key) * (nKeys - keyIdx - 1));
	memcpy(childrens + childIdx, childrens + childIdx + 1, sizeof(Node *) * (nChild - childIdx - 1));
    cout << "InnerNode::removeChild  finish" << endl;
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
		if(index < 0 || index >= nChild){
			return MAX_VALUE;
		}
		else{
			return childrens[index]->find(k);
		}
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

	bitmap = new Byte[(2 * LEAF_DEGREE + 7) / 8];
	pNext = PPointer({0, 0});
	fingerprints = new Byte[2 * LEAF_DEGREE];
	kv = new KeyValue[2 * LEAF_DEGREE];
	memset(bitmap, 0, bitmapSize * sizeof(Byte));
	memset(fingerprints, 0, 2 * LEAF_DEGREE * sizeof(Byte));
	memset(kv, 0, 2 * LEAF_DEGREE * sizeof(KeyValue));

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
	pmem_addr = PAllocator::getAllocator()->getLeafPmemAddr(pPointer);
	filePath = DATA_DIR + to_string(pPointer.fileId);
	bitmapSize = (2 * LEAF_DEGREE + 7) / 8;

	bitmap = new Byte[(2 * LEAF_DEGREE + 7) / 8];
	pNext = PPointer({0, 0});
	fingerprints = new Byte[2 * LEAF_DEGREE];
	kv = new KeyValue[2 * LEAF_DEGREE];
	memcpy(bitmap, pmem_addr, bitmapSize * sizeof(Byte));
	memcpy(&pNext, pmem_addr + bitmapSize * sizeof(Byte), sizeof(PPointer));
	memcpy(fingerprints, pmem_addr + bitmapSize * sizeof(Byte) + sizeof(PPointer), 2 * LEAF_DEGREE * sizeof(Byte));
	memcpy(kv, pmem_addr + bitmapSize * sizeof(Byte) + sizeof(PPointer) + 2 * LEAF_DEGREE * sizeof(Byte), 2 * LEAF_DEGREE * (sizeof(Key) + sizeof(Value)));

	for(uint64_t i = 0; i < bitmapSize; i ++){
		n += countOneBits(bitmap[i]);
	}

}

LeafNode::~LeafNode() {
    // TODO
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
	if(n >= 2 * LEAF_DEGREE - 1){
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
	bitmap[index / 8] |= (1 << (index % 8));
	fingerprints[index] = keyHash(k);
	kv[index] = {k, v};

	persist();
}

// split the leaf node


KeyNode* LeafNode::split() {
	Byte fingerprint_temp;
	KeyValue kv_temp;
	for(int i = 0; i < 2 * LEAF_DEGREE - 1; i ++){
		for(int j = 0; j < 2 * LEAF_DEGREE - 1 - i; j ++){
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

    KeyNode *newChild = new KeyNode();
	LeafNode *leafNode = new LeafNode(tree);
    // TODO
	newChild->key = findSplitKey();
	newChild->node = leafNode;
	leafNode->n = LEAF_DEGREE;
	leafNode->prev = this;
	for(uint64_t i = 0; i < (2 * LEAF_DEGREE + 7) / 8; i++){
		for(uint64_t j = 0; j < 8; j ++){
			if(i * 8 + j < LEAF_DEGREE){
				leafNode->bitmap[i] |= (1 << j);
			}
			else{
				leafNode->bitmap[i] &= ~(1 << j);
			}
		}
	}
	memcpy(leafNode->fingerprints, fingerprints + LEAF_DEGREE, sizeof(Byte) * LEAF_DEGREE);
	memcpy(leafNode->kv, kv +  LEAF_DEGREE, sizeof(KeyValue) * LEAF_DEGREE);
	leafNode->persist();

	n = LEAF_DEGREE;
	next = leafNode;
	pNext = leafNode->getPPointer();
	for(uint64_t i = 0; i < (2 * LEAF_DEGREE + 7) / 8; i ++){
		for(uint64_t j = 0; j < 8; j ++){
			if(j + i * 8 < LEAF_DEGREE){
				bitmap[i] |= (1 << j);
			}
			else{
				bitmap[i] &= ~(1 << j);
			}
		}
	}
	memset(fingerprints + LEAF_DEGREE, 0, sizeof(Byte) * LEAF_DEGREE);
	memset(kv + LEAF_DEGREE, 0, sizeof(KeyValue) * LEAF_DEGREE);
	persist();

    return newChild;
}

// use to find a mediant key and delete entries less then middle
// called by the split func to generate new leaf-node
// qsort first then find
Key LeafNode::findSplitKey() {
    Key midKey = 0;
    // TODO
	if(n == 2 *  LEAF_DEGREE){
		midKey = kv[LEAF_DEGREE].k;
	}
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
    // TODO
    const Byte hashValue = keyHash(k);
    int keyIndex = -1;
    for(uint64_t i = 0; i < bitmapSize; i ++){
        for(int j = 0; j < 8; j ++){
            if(this->getBit(i * 8 + j) && this->fingerprints[i * 8 + j] == hashValue && this->kv[i * 8 + j].k == k){
                keyIndex = i * 8 + j;
                break;
            }
        }
    }
    cout << keyIndex << endl;
    if(keyIndex == -1){
        //cout << "LeafNode::remove: KeyIndex = -1" << endl;
        return false;
    }
    // bitmap[keyIndex] set 0;
    bitmap[keyIndex / 8] &= ~(1 << (keyIndex % 8));
    this->n --;
    if(this->n == 0){
        cout << "LeafNode::remove: n = 0" << endl;
        if(this->prev != NULL){
            this->prev->next = this->next;
            if(this->next != NULL){
                this->prev->pNext = this->next->pPointer;
            }
            else{
                this->prev->pNext.fileId = 0;
                this->prev->pNext.offset = 0;
            }
            this->prev->persist();
        }
        else{
            PAllocator::getAllocator()->setStartLeafPointer(this->pNext);
        }
        if(this->next != NULL){
            this->next->prev = this->prev;
        }
        PAllocator::getAllocator()->freeLeaf(this->pPointer);
        ifDelete = true;
    }
    else{
        cout << "LeafNode::remove: n != 0" << endl;
        this->persist();
        ifDelete = false;
    }
    return true;
}

// update the target entry
// return TRUE if the update succeed
bool LeafNode::update(const Key& k, const Value& v) {
    bool ifUpdate = false;
    // TODO
	for(uint64_t i = 0; i < bitmapSize; i ++){
		for(uint64_t j = 0; j < 8; j ++) {
			if((bitmap[i] >> j) & 1){
				if(k == kv[j + i * 8].k){
					kv[j + i * 8].v = v;
					persist();
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
	for(uint64_t i = 0; i < bitmapSize; i ++){
		for(uint64_t j = 0; j < 8; j ++) {
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
	for(uint64_t i = 0; i < bitmapSize; i ++){
		for(uint64_t j = 0; j < 8; j ++) {
			if(!((bitmap[i] >> j) & 1)) {
				return (j + i * 8);
			}
		}
	}
    return -1;
}

// persist the entire leaf
// use PMDK
void LeafNode::persist() {
	memcpy(pmem_addr, bitmap, bitmapSize * sizeof(Byte));
	memcpy(pmem_addr + bitmapSize * sizeof(Byte), &pNext, sizeof(PPointer));
	memcpy(pmem_addr + bitmapSize * sizeof(Byte) + sizeof(PPointer), fingerprints, 2 * LEAF_DEGREE * sizeof(Byte));
	memcpy(pmem_addr + bitmapSize * sizeof(Byte) + sizeof(PPointer) + 2 * LEAF_DEGREE * sizeof(Byte), kv, 2 * LEAF_DEGREE * (sizeof(Key) + sizeof(Value)));
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
    queue<Node*> level;
    level.push(root);
    while(level.empty() == false){
        Node* toPrint = level.front();
        level.pop();
        toPrint->printNode();
        if(toPrint->isLeaf == false){
            for(int i = 0; i < ((InnerNode*)toPrint)->nChild; i ++ ){
                level.push( ((InnerNode*)toPrint)->childrens[i]);
            }
        }
        cout << endl;
    }
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
	vector<LeafNode *> leafNodes;
	KeyNode leaf;
	while(pPointer.fileId != 0){
		leafNodes.push_back(new LeafNode(pPointer, tree_temp));
		pPointer = leafNodes.back()->pNext;
	}
	for(uint64_t i = 0; i < leafNodes.size(); i ++){
		leaf = {leafNodes[i]->getKey(0), leafNodes[i]};
		root->insertLeaf(leaf);
	}
	return true;
}
