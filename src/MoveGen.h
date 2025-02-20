#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <bits/stdc++.h>

#include "Board.h"
#include "Move.h"
#include "Utils.h"
#include "LookUpTables.h"

#define U64 std::uint64_t
using namespace std;

class MoveGen{
    public :
        MoveGen(Board board);
        MoveGen(shared_ptr<Board> boardPtr);

        // Get Attributes
        inline vector<Move> generateMoves() { return generateLegalMoves(); };

        // Special Moves
        void appendWhiteEnPassantMoves(vector<Move> &moves, U64 target = LookUpTables::all) const;
        void appendBlackEnPassantMoves(vector<Move> &moves, U64 target = LookUpTables::all) const;

        //Pseudo Legal Moves
        void appendQueenPseudoLegalMoves(const Color color, vector<Move> &moves, U64 target = LookUpTables::all) const;
        void appendKnightPseudoLegalMoves(const Color color, vector<Move> &moves, U64 target = LookUpTables::all) const;
        void appendKingPseudoLegalMoves(const Color color, vector<Move> &moves, U64 target = LookUpTables::all) const;
        void appendRookPseudoLegalMoves(const Color color, vector<Move> &moves, U64 target = LookUpTables::all) const;
        void appendBishopPseudoLegalMoves(const Color color, vector<Move> &moves, U64 target = LookUpTables::all) const;
        void appendPawnPseudoLegalMoves(const Color color, vector<Move> &moves, U64 target = LookUpTables::all) const;
        void appendWhitePawnPseudoLegalMoves(vector<Move> &moves, U64 target = LookUpTables::all) const;
        void appendBlackPawnPseudoLegalMoves(vector<Move> &moves, U64 target = LookUpTables::all) const;  

        vector<Move> getRookPseudoLegalMoves(const Color color) const {
            vector<Move> moves;
            appendRookPseudoLegalMoves(color, moves);
            return moves;
        }

        vector<Move> getQueenPseudoLegalMoves(const Color color) const {
            vector<Move> moves;
            appendQueenPseudoLegalMoves(color, moves);
            return moves;
        }

        vector<Move> getKingPseudoLegalMoves(const Color color) const {
            vector<Move> moves;
            appendKingPseudoLegalMoves(color, moves);
            return moves;
        }

        vector<Move> getBishopPesudoLegalmoves(const Color color) const {
            vector<Move> moves;
            appendBishopPseudoLegalMoves(color, moves);
            return moves;
        }

        vector<Move> getKinghtPseudoLegalMoves(const Color color) const {
            vector<Move> moves;
            appendKnightPseudoLegalMoves(color, moves);
            return moves;
        }

        vector<Move> getPawnPseudoLegalMoves(const Color color) const {
            vector<Move> moves;
            appendKingPseudoLegalMoves(color, moves);
            return moves;
        }

        vector<Move> getWhitePawnPseudoLegalMoves(){
            vector<Move> moves;
            appendWhitePawnPseudoLegalMoves(moves);
            return moves;
        }

        vector<Move> getBlackPawnPseudoLegalMoves(){
            vector<Move> moves;
            appendBlackPawnPseudoLegalMoves(moves);
            return moves;
        }

        // Moves
        vector<Move> generatePseudoLegalMoves();
        vector<Move> generatePseudoLegalMoves(const Color color);
        vector<Move> generateEvasionMoves(const Color color);
        vector<Move> generateLegalMoves();
        vector<Move> generateLegalMoves(const Color color);

        // Castling
        void addKingSideCastlingMove(Color color, Square kingIndex, vector<Move> &moves) const;
        void addQueenSideCastlingMove(Color color, Square kingIndex, vector<Move> &moves) const;

        bool isQueenSideCastlingPossible(const Color color) const;
        bool isKingSideCastlingPossible(const Color color) const;

    private :
        shared_ptr<Board> myBoard;

        void addQuietMoves(U64 quietDestinations, Square pieceIndex, vector<Move>& moves, Piece::PieceType pieceType) const;
	    void addDoublePawnPushMoves(U64 pawnDestinations, Square pieceIndex, vector<Move>& moves) const;
	    void addCaptureMoves(U64 captureDestinations, Square pieceIndex, vector<Move>& moves, Piece::PieceType pieceType) const;
	    void addPromotionMoves(U64 promotionDestinations, Square pieceIndex, vector<Move>& moves) const;
	    void addPromotionCaptureMoves(U64 promotionDestinations, Square pieceIndex, vector<Move>& moves) const;
};

#endif