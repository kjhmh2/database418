#include <iostream>
#include <time.h>
#include"fptree/fptree.h"
#define INSERT_NUM 10000

using namespace std;

int main() {;
    clock_t start, end;
	double dur;
    FPTree *tree = new FPTree(32);
	cout << "Inserting..." << endl;
	start = clock();
    for (int i = 1; i <= INSERT_NUM; i++) {
        tree->insert(i, i* 10);
    }
    end = clock();
	dur = end - start;
	cout << "Insert num: " << INSERT_NUM << endl;
    cout << "Insert Time: " << (dur / CLOCKS_PER_SEC) << endl;
    delete tree;
    tree = NULL;
    PAllocator::getAllocator()->~PAllocator();

    cout << "Bulkloding..." << endl;
    start = clock();
    FPTree *t_tree = new FPTree(32);
    end = clock();
	dur = end - start;
	cout << "Bulkload Time: " << (dur / CLOCKS_PER_SEC) << endl;

	cout << "Searching..." << endl;
	start = clock();
	for (int i = 1; i <= INSERT_NUM; i++) {
        if(t_tree->find(i) != i * 10){
			cout << "Searching error!" << endl;
			break;
		}
    }
	end = clock();
	cout << "Searching successfully!" << endl;
    dur = end - start;
    cout << "Search Time: " << (dur / CLOCKS_PER_SEC) << endl;
}
