#ifndef LAZY_SMP_H
#define LAZY_SMP_H

#include <bits/stdc++.h>
#include <thread>

#include "Move.h"
#include "Search.h"
#include "Utils.h"

using namespace std;

struct SearchResult {
    int score;
    Move16 bestMove;
    int searchDepth;
};

class LazySMP {
    public :
        LazySMP(int num_threads, unsigned int allocatedTimeMS, std::shared_ptr<Board> boardPtr){
            this->num_threads = num_threads;
            this->allocatedTimeMS = allocatedTimeMS;
            this->myBoard = boardPtr;
        }

        Move16 search();
        
    private :
        int num_threads;
        unsigned int allocatedTimeMS;
        shared_ptr<Board> myBoard;
};

#endif