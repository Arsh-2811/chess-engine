#ifndef EVALUATION_H
#define EVALUATION_H

#include<bits/stdc++.h>

#include "Board.h"

#include "Pieces.h"
#include "Move.h"
#include "Pawn.h"

using namespace std;

class Evaluation {
    public :
        Evaluation(shared_ptr<Board> boardPtr);
        
        const static int TOTAL_MATERIAL = 2*Piece::QUEEN_VALUE + 4*Piece::ROOK_VALUE +
                        4*Piece::BISHOP_VALUE + 4*Piece::KNIGHT_VALUE + 16*Piece::PAWN_VALUE;

        const static int DRAW_SCORE = 0;
        const static int CHECKMATE_SCORE = 100000;

        static int POSITIONAL_GAIN_PERCENT;
        static int PAWN_GAIN_PERCENT;
        static int MOBILITY_GAIN_PERCENT;

        void updateEvalAttributes(const Move &move);
        void rewindEvalAttributes(const Move &move);

        int calcMobilityScore(const int64_t alpha) const;
        int calcMaterialAdjustments(const int64_t alpha) const;

        int evaluate();

        int getWhitePiecesValue() const;
        int getBlackPiecesValue() const;

        static int pieceTypeToValue(int type);

    private :
        shared_ptr<Board> myBoard;
        int64_t myGameStage;
        int myOpeningPSQValue;
        int myEndgamePSQValue;
        int myMaterialScore;

        void init();
        int calculateKingSafety();
};

#endif