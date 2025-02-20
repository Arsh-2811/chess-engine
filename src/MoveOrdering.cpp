#include "MoveOrdering.h"
#include "Evaluation.h"
#include "Pieces.h"
#include "TT.h"
#include "TTEntry.h"

void MoveOrdering::setNewKiller(const Move& move, const unsigned int ply){
    if (!move.isPromotion() && !move.isCapture()) {
        myKiller2[ply] = myKiller1[ply];
        myKiller1[ply] = move;
    } 
}

void MoveOrdering::clearKillers(){
    for (unsigned int i=0 ; i<64 ; i++){
        myKiller1[i] = Move();
        myKiller2[i] = Move();
    } 
}

void MoveOrdering::rateMoves(vector<Move> &moveList, shared_ptr<Board> board, const unsigned int ply, const bool isSEE){
    TTEntry* ttEntry = globalTT.probeTT(board->key, 0);
    Move hashMove = Move();
    if(ttEntry) hashMove = ttEntry->getBestmove();

    for(Move &move : moveList){
        unsigned int score = 0;
        if(move == hashMove) score += 100000;
        if(move.isPromotion()) score += Evaluation::pieceTypeToValue(move.getPromotedPieceType()) - Piece::PAWN_VALUE;
        if(move.isCapture()){
            if(isSEE) score += board->seeCapture(move, Utils::getOppositeColor(board->getColorToPlay()));
            else score += Evaluation::pieceTypeToValue(move.getCapturedPieceType())-move.getPieceType();
        }

        if(move == myKiller1[ply]) score += KILLER1_BONUS;
        if(move == myKiller2[ply]) score += KILLER2_BONUS;

        move.setMoveRating(score);
    }
}

void MoveOrdering::sortMoves(vector<Move> &moveList){
    std::sort(moveList.rbegin(), moveList.rend());
}