#include"fptree/fptree.h"

using namespace std;

// Initial the new InnerNode
InnerNode::InnerNode(const int& d, FPTree* const& t, bool _isRoot) {
    // TODO
    degree = d;
    tree = t;
    nKeys = 0;
    nChild = 0;
    isLeaf = false;
    isRoot = _isRoot;
    new Key[2 * d + 2];
    children = new Node*[2 * d + 2];
}

// delete the InnerNode
InnerNode::~InnerNode() {
    // TODO
    degree = 0;
    delete tree;
    tree = NULL;
    delete keys;
    keys = NULL;
    delete childrens;
    childrens = NULL;
}

// binary search the first key in the innernode larger than input key
int InnerNode::findIndex(const Key& k) {
    // TODO
    return 0;
    int leftIndex = 0;
    int rightIndex = getKeyNum() - 1;
    int midIndex;
    while(leftIndex != rightIndex){
        midIndex = (leftIndex + rightIndex) / 2;
        Key midKey = getKey(midIndex);
        if(k > midKey){
            leftIndex = midIndex + 1;
        }
        else{
            rightIndex = midIndex;
        }
    }
    return leftIndex;
}

// insert the node that is assumed not full
// insert format:
// ======================
// | key | node pointer |
// ======================
// WARNING: can not insert when it has no entry
void InnerNode::insertNonFull(const Key& k, Node* const& node) {
    // TODO
    int inserIndex = 0;
    bool isBreak = false;
    for(int i = 0; i < nKeys; i ++){
        if(k < keys[i]){
            inserIndex = i;
            isBreak = true;
            break;
        }
    }
    if(isBreak == false){
        childrens[nChild] = node;
        keys[nKeys] = k;
    }
    else{
        for(int i = nKeys; i > inserIndex; i --){
            keys[i] = keys[i - 1];
            childrens[i + 1] = childrens[i];
        }
        keys[inserIndex] = k;
        childrens[inserIndex + 1] = node;
    }
    nChild ++;
    nKeys ++;
}

// insert func
// return value is not NULL if split, returning the new child and a key to insert
KeyNode* InnerNode::insert(const Key& k, const Value& v) {
    KeyNode* newChild = NULL;
    newChild->key = k;

    // 1.insertion to the first leaf(only one leaf)
    if (this->isRoot && this->nKeys == 0) {
        LeafNode* tmp = new LeafNode(tree);
        tmp->insert(k, v);
        insertNonFull(k, tmp);
        return newChild;
    }
    
    // 2.recursive insertion
    KeyNode key_node;
    int index = findIndex(k);
    if (index == 0)
    {
        key_node = childrens[0]->insert(k, v);
        keys[0] = childrens[0]->keys[0];
    }
    else
        key_node = childrens[index - 1]->insert(k, v);
    if (nKeys < 2 * degree + 1)
        insertNonFull(key_node.key, key_node.node);
    else    // if is full
    {
        KeyNode key_node2 = split();
        if (key_node.key < key_node2.key)
            insertNonFull(key_node.key, key_node.node);
        else
            ((InnerNode *)key_node2.node)->insertNonFull(key_node.key, key_node.node);

        if (isRoot)
        {
            isRoot = false;
            key_node2.node->isRoot = false;
            InnerNode * tmp = new InnerNode(tree->degree, tree, true);
            tmp->insertNonFull(keys[0], this);
            tmp->insertNonFull((InnerNode *)key_node2.node->keys[0], (InnerNode *)key_node2.node);
            tree->root = tmp;
        }
        return key_node2;
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
}

// update the target entry, return true if the update succeed.
bool InnerNode::update(const Key& k, const Value& v) {
    // TODO
    return false;
}

// find the target value with the search key, return MAX_VALUE if it fails.
Value InnerNode::find(const Key& k) {
    // TODO
    return MAX_VALUE;
}

// get the children node of this InnerNode
Node* InnerNode::getChild(const int& idx) {
    // TODO
    return NULL;
}

// get the key of this InnerNode
Key InnerNode::getKey(const int& idx) {
    if (idx < this->nKeys) {
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
    PAllocator::getAllocator()->getLeaf(pPointer, pmem_addr);
    //bitmap = 
    kv = pmem_addr;

    n = 0;
    pre = next = NULL;//
    filePath = DATA_DIR + pPointer.fileId;

    bitmapSize = 2*degree;
}

// reload the leaf with the specific Persistent Pointer
// need to call the PAllocator
LeafNode::LeafNode(PPointer p, FPTree* t) {
    // TODO
    tree = t;
    isLeaf = true;
    degree = LEAF_DEGREE;
    pmem_addr = PAllocator::getAllocator()->getLeafPmemAddr(p);
    //bitmap = 
    kv = pmem_addr;

    n  = 0;
    pre = next = NULL;//
    pPointer = p;
    filePath = DATA_DIR + pPointer.fileId;

    bitmapSize = 2*degree;
}

LeafNode::~LeafNode() {
    // TODO
    PAllocator::getAllocator()->freeLeaf(pPointer);
}

// insert an entry into the leaf, need to split it if it is full
KeyNode* LeafNode::insert(const Key& k, const Value& v) {
    KeyNode* newChild = NULL;
    // TODO
    newChild->key = k;
    newChild->node = NULL;
    if(update(k, v)) return newChild;
    if(n >= 2*degree){
        newChild = split();
        if(k < newChild.key){
            insertNonFull(k ,v);
        }
        else{
            newChild->node->insertNonFull(k, v);
        }
    }
    else{
        insertNonFull(k, v);
    }
    return newChild;
}

// insert into the leaf node that is assumed not full
void LeafNode::insertNonFull(const Key& k, const Value& v) {
    // TODO
}

// split the leaf node
KeyNode* LeafNode::split() {
    KeyNode* newChild = new KeyNode();
    // TODO
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
    return 0;
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
    return ifUpdate;
}

// if the entry can not be found, return the max Value
Value LeafNode::find(const Key& k) {
    // TODO
    return MAX_VALUE;
}

// find the first empty slot
int LeafNode::findFirstZero() {
    // TODO
    return -1;
}

// persist the entire leaf
// use PMDK
void LeafNode::persist() {
    // TODO
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
    return false;
}
