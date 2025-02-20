#ifndef MOVEORDERING_H
#define MOVEORDERING_H

#include "Move.h"
#include "Board.h"

using namespace std;

class MoveOrdering{
    public :
        const static int KILLER1_BONUS = 20;
        const static int KILLER2_BONUS = 10;

        inline MoveOrdering() : myKiller1(), myKiller2(){}

        inline Move getKiller1(const unsigned int ply) const {return myKiller1[ply];}
        inline Move getKiller2(const unsigned int ply) const {return myKiller2[ply];}

        void setNewKiller(const Move& move,const unsigned int ply);
        void clearKillers();

        // void rateMoves(std::vector<Move>& moveList, shared_ptr<Board> board);
        // void rateMoves(std::vector<Move>& moveList, const unsigned int ply);
        void rateMoves(std::vector<Move>& moveList, std::shared_ptr<Board> board, const unsigned int ply, const bool isSee);
        // void rateMovesH(std::vector<Move>& moveList, std::shared_ptr<Board> board, const unsigned int ply);

        void sortMoves(std::vector<Move>& moveList);

        vector<Move>::iterator getBestCandidate(vector<Move>& moveList);

    private : 
        Move myKiller1[64];
        Move myKiller2[64];
};

#endif