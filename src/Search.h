#ifndef SEARCH_H
#define SEARCH_H

#include "Board.h"
#include "Evaluation.h"
#include "MoveOrdering.h"

using namespace std;

class Search{
    public :
        const static int MAX_DEPTH = 64;

        Search(std::shared_ptr<Board> boardPtr);

        void clearSearchData();

        Move16 myBestMove;
        int evaluation;

        int DEPTH;

        void generateRandomMove();
        int negaMaxRoot(const int depth);
        int negaMaxRootIterativeDeepening(const unsigned int allocatedTimeMS);

        int negaMaxRootIterativeDeepeningLazySMP(const unsigned int allocatedTimeMS);

        bool isInsufficentMatingMaterial() const;

        inline Move getPVMove(const unsigned int ply, const unsigned int depth) const {return myPvTable[ply][depth];}
        void printPvTable(const unsigned numLines);

        U64 nodesSearched;
        int TTTableHits;

        U64 myMovesSearched;
        unsigned int mySearchDurationMS;
        unsigned int myDepth;

        unsigned int initialDepth = 1;
        void setInitialDepth(int iDepth){
            this->initialDepth = iDepth;
        }

        int DELTA_MARGIN = 200;
        void setDeltaMargin(int deltaMargin){
            this->DELTA_MARGIN = deltaMargin;
        }

    private:
        shared_ptr<Board> myBoard;
        Evaluation myEval;
        MoveOrdering myMoveOrder;
        unsigned int myPly;

        int historyTable[COLOR_NB][SQUARE_NB][SQUARE_NB];

        int negaMax(const int depth, int alpha, const int beta);
        int evaluate();
        int qSearch(int alpha, const int beta);

        void sendInfoToUCI(int alpha);

        bool isEndGame() const;
        void updateHistoryTable(const Move &move, int depth);

        Move myPvTable[MAX_DEPTH][MAX_DEPTH];
        unsigned int myPvLength[MAX_DEPTH];
};

#endif