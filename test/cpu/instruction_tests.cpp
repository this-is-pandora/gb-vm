#include "cpu.h"
#include "memory.h"
#include <iostream>

// load C into B, then check if C == B
// perform addition & check flags/result
// 
// 
// 
using namespace std;
bool test() {
    MMU *mmu = new MMU();
    CPU *cpu = new CPU(mmu);
    uint8_t prgm[10];

    for (int i = 0; i < 10; i++) {
        //cout << "executing instrctn: " << prgrm[i] << endl;
        //cpu->execute(prgrm[i]);
    }
}
