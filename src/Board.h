#ifndef BOARD_H
#define BOARD_H

#include <bits/stdc++.h>
#include <cstdint>
#include <iostream>

#include "BitBoardsUtils.h"
#include "LookUpTables.h"
#include "Move.h"
#include "Pieces.h"
#include "Types.h"
#include "Utils.h"
#include "MagicMoves.h"

using namespace std;
using namespace LookUpTables;

#define U64 std::uint64_t
typedef std::uint64_t Zkey;

namespace ZK {
    extern std::mt19937_64 rdGen;

    extern Zkey psq[COLOR_NB][Piece::TOTAL_PIECES][SQUARE_NB];
    extern Zkey enPassant[FILE_NB];
    extern Zkey castling[4];
    extern Zkey side;

    void initZobristKeys();
}

class Board {
    public :
        Board();
        Board(std::string fen);

        inline U64 getWhitePawns() const{return bitBoards[0];}
        inline U64 getWhiteKnights() const{return bitBoards[1];}
	    inline U64 getWhiteBishops() const{return bitBoards[2];}
	    inline U64 getWhiteRooks() const{return bitBoards[3];}
	    inline U64 getWhiteQueens() const{return bitBoards[4];}
	    inline U64 getWhiteKing() const{return bitBoards[5];}
	    inline Square getWhiteKingSquare() const{return msb(getWhiteKing());}

	    inline U64 getBlackPawns() const{return bitBoards[6];}
	    inline U64 getBlackKnights() const{return bitBoards[7];}
	    inline U64 getBlackBishops() const{return bitBoards[8];}
	    inline U64 getBlackRooks() const{return bitBoards[9];}
	    inline U64 getBlackQueens() const{return bitBoards[10];}
	    inline U64 getBlackKing() const{return bitBoards[11];}
	    inline Square getBlackKingSquare() const{return msb(getBlackKing());}

        inline U64 getPawns(Color color) const{return getBitBoard(Piece::PAWN, color);}
	    inline U64 getKnights(Color color) const{return getBitBoard(Piece::KNIGHT, color);}
	    inline U64 getBishops(Color color) const{return getBitBoard(Piece::BISHOP, color);}
	    inline U64 getRooks(Color color) const{return getBitBoard(Piece::ROOK, color);}
	    inline U64 getQueens(Color color) const{return getBitBoard(Piece::QUEEN, color);}
	    inline U64 getKing(Color color) const{return getBitBoard(Piece::KING, color);}
	    inline Square getKingSquare(Color color) const{return msb(getKing(color));}

        inline U64 getWhitePieces() const{return bitBoards[12];}
	    inline U64 getBlackPieces() const{return bitBoards[13];}
	    inline U64 getAllPieces() const{return allPieces;}
	    inline U64 getPieces(const Color color) const {return bitBoards[12+color];}
	    inline U64 getPinnedPieces() const{return pinnedPieces;}

        inline U64 getAllPawns() const{return bitBoards[14];}
	    inline U64 getAllKnights() const{return bitBoards[15];}
	    inline U64 getAllBishops() const{return bitBoards[16];}
	    inline U64 getAllRooks() const{return bitBoards[17];}
	    inline U64 getAllQueens() const{return bitBoards[18];}
	    inline U64 getAllKings() const{return bitBoards[19];}

        inline U64 getBitBoard(Piece::PieceType pieceType, Color color) const{return bitBoards[pieceType+6*color];};

        inline U64 getAtkTo(Square sq) const {return atkTo[sq];};
        inline U64 getKingAttackers() const {return kingAttackers;};

        U64 getAttackersTo(Square sq, Color color) const;
        U64 getAttackersTo(Square sq, Color color, U64 occ) const;

        inline Color getColorToPlay() const {return colorToPlay;};
        inline vector<Move> getMovesHistory() const {return moves;};
        inline vector<Zkey> getKeysHistory() const {return myKeys;};

        void setBitBoards(const string piecesString, const unsigned int rank);
        void updateConvenienceBitBoards();
        void updatePinnedPieces();

        bool isMoveLegal(Move &move, bool isCheck);
        void executerMove(Move &move);
        void undoMove(Move &move);
        void executeNullMove();
	    void undoNullMove();
        const Move* getEnemyLastMove() const;

        inline bool isQueenSideCastlingAllowed(const Color color) const {return (castling >> (1+2*color)) & 0x1;}
        inline bool isKingSideCastlingAllowed(const Color color) const {return (castling >> 2*color) & 0x1;}
        void updateCastlingRights(Move &move);
        void rewindCastlingRights(const Move&move);

        inline Square getLastEpSquare() const {return epSqaures.back();}

        Piece::Piece findPieceType(const Square position) const;
        Piece::PieceType findPieceType(const Square position, const Color color) const;
        Piece::PieceType findWhitePieceType(const Square position) const;
        Piece::PieceType findBlackPieceType(const Square postion) const;

        void updateKingAttackers(const Color color);

        inline void updateKingAttackers(){ return updateKingAttackers(colorToPlay); };
        inline bool isCheck() const { return kingAttackers; };

        U64 getAttacksFromSq(const Square position) const;

        inline U64 getKnightAttacks(const Square pos, const Color color) const {
            return LookUpTables::attackTable[Piece::KNIGHT][pos] & ~getPieces(color);
        };

        inline U64 getPawnAttacks(const Square square, const Color color) const {
            return LookUpTables::pawnAttackTable[color][square] & ~getPieces(color);
        };

        inline U64 getBishopAttacks(const Square square, const Color color) const {
            return MagicMoves::Bmagic(square, getAllPieces()) & ~getPieces(color);
        };
        
        inline U64 getRookAttacks(const Square square, const Color color) const {
            return MagicMoves::Rmagic(square, getAllPieces()) & ~getPieces(color);
        };

        inline U64 getQueenAttacks(const Square square, const Color color) const {
            return getBishopAttacks(square, color) | getRookAttacks(square, color);
        };

        inline U64 getKingAttacks(const Square square, const Color color) const {
            return LookUpTables::attackTable[Piece::KING][square] & ~getPieces(color);
        };

        inline U64 getKingAttacks(const Square square) const {
            return LookUpTables::attackTable[Piece::KING][square];
        };

        inline bool hasBlackCastled() const {
            return myHasBlackCastled;
        }

        inline bool hasWhiteCastled() const {
            return myHasWhiteCastled;
        }

        bool isSquareAttacked(Square square, Color color) const;
        char getChar(const unsigned int file, const unsigned int rank) const;
        string getUnicode(const unsigned int file, const unsigned int rank) const;

        inline unsigned int getIndexFromChar(const string position) const {
            return (position[0] - 'a' + 8 * (position[1] - '0' - 1));

        }

        int see(const Square square, Color color);
        int seeCapture(Move captureMove, Color color);
        int seeCapture2(Move captureMove, Color color);
        Piece::PieceType getSmallestAttacker(const Square square, Color side, U64 &attackers);

        inline unsigned int getply() const {return halfMovesCounter;}

        Zkey key;
        Zkey pawnsKey;

    private:
        array<U64, 20> bitBoards;
        U64 allPieces;
        U64 pinnedPieces;

        Color colorToPlay;
        /*
            bin    dec
            0001   1      white king can castle to the king side
            0010   2      white king can castle to the queen side
            0100   4      black king can castle to the king side
            1000   8      black king can castle to the queen side

            examples
            1111   15     both kings can castle to both sides
            1001   9      black king => queen side
                          white king => king side
        */
        unsigned int castling;
        
        bool myHasWhiteCastled;
        bool myHasBlackCastled;

        U64 atkTo[SQUARE_NB];    // Locations of pieces that attack to the square
        U64 kingAttackers;

        unsigned int movesCounter;
        unsigned int halfMovesCounter;

        vector<Move> moves;
        vector<Zkey> myKeys;
        vector<Square> epSqaures;

        inline void movePiece(const Square origin, const Square destination, const unsigned int pieceType, const Color color){
            movePiece(origin, destination, bitBoards[pieceType+color*6]);

            key ^= ZK::psq[color][pieceType][origin];
		    key ^= ZK::psq[color][pieceType][destination];
		    if(pieceType == Piece::PieceType::PAWN){
		    	pawnsKey ^= ZK::psq[color][Piece::PieceType::PAWN][origin];
		    	pawnsKey ^= ZK::psq[color][Piece::PieceType::PAWN][destination];
		    }
        }

        inline void removePiece(const Square index, const unsigned int pieceType, const Color color){
            removePiece(index, bitBoards[pieceType+color*6]);

            key ^= ZK::psq[color][pieceType][index];
            if(pieceType == Piece::PieceType::PAWN) pawnsKey ^= ZK::psq[color][Piece::PieceType::PAWN][index];
        }

        inline void addPiece(const Square index, const unsigned int pieceType, const Color color){
            addPiece(index, bitBoards[pieceType+color*6]);

            key ^= ZK::psq[color][pieceType][index];
            if(pieceType == Piece::PieceType::PAWN) pawnsKey ^= ZK::psq[color][Piece::PieceType::PAWN][index];
        }



        inline void removePiece(const Square index, U64 &bitBoard){
            bitBoard &= ~(1ULL << index);
        }

        inline void addPiece(const Square index, U64 &bitBoard){
            bitBoard |= (1ULL << index);
        }

        inline void movePiece(const Square origin, const Square destination, U64 &bitBoard){
            removePiece(origin, bitBoard);
            addPiece(destination, bitBoard);
        }
};

inline ostream& operator<<(ostream &strm, const Board &Board){
    for(int rank = 7; rank >= 0; rank--){
        strm << rank+1 << "| ";
        for(int file = 0; file < 8; file++){
            strm << Board.getUnicode(file, rank) << " ";
        }
        strm << endl;
    }
    strm << "   ________________" << endl;
    strm << "   a b c d e f g h" << endl;

    return strm;
}

inline ostream& operator<<(ostream &strm, const vector<Move> &moves){
    for(unsigned int n = 0; n < moves.size(); n++){
        strm << moves[n];
    }
    return strm;
}

#endif