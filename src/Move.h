#ifndef MOVE_H
#define MOVE_H

#include <bits/stdc++.h>

#include "Types.h"
#include "Pieces.h"

using namespace std;
#define U64 std::uint64_t

class Move{
    private :
        unsigned int myMove;
        unsigned int myMoveRating;

    public:

        const static unsigned int QUIET_FLAG = 0b0000;
        const static unsigned int EP_CAPTURE_FLAG = 0b0101;
        const static unsigned int DOUBLE_PAWN_PUSH_FLAG =0b0001;
	    const static unsigned int CAPTURE_FLAG = 0b0100;
	    const static unsigned int PROMOTION_FLAG = 0b1000;
	    const static unsigned int KING_SIDE_CASTLING = 	0b0010;
	    const static unsigned int QUEEN_SIDE_CASTLING = 0b0011;
        const static unsigned int NULL_MOVE = 0;

        inline Move() : myMove(0), myMoveRating(0){}

        inline Move(int move) : myMove(move), myMoveRating(0){}

        inline Move(Square origin, Square destination, unsigned int flags, Piece::PieceType pieceType) : myMoveRating(0){
            int capturedPiece = Piece::NO_PIECE_TYPE;
            myMove = ((capturedPiece & 0x7) << 19) | ((pieceType & 0x7) << 16) | ((flags & 0xf) << 12) | ((origin & 0x3f) << 6) | (destination & 0x3f);
        }

        inline Square getDestination() const{
            return static_cast<Square>(myMove & 0x3f);
        }

        inline Square getOrigin() const{
            return static_cast<Square>((myMove >> 6) & 0x3f);
        }

        inline unsigned int getFlags() const{
            return (myMove >> 12) & 0xf;
        }

        inline unsigned int getPieceType() const{
		    return (myMove >> 16) & 0x7;
	    }

	    inline unsigned int getPromotedPieceType() const{
		    return (getFlags() & 0b11) + 1;
	    }

        inline unsigned int getCapturedPieceType() const{
		    return (myMove >> 19) & 0x7;
	    }

	    inline unsigned int getPreviousCastlingRights() const{
	        return (myMove >> 22) & 0xf;
	    }

        inline unsigned int getMove() const{
            return myMove;
        }

        inline Move16 getMove16() const {
            return static_cast<Move16>(myMove & 0xffff);
        }

        inline void setDestination(const Square destination){
            myMove &= ~0x3f;
            myMove |= (destination & 0x3f);
        }

        inline void setOrigin(const Square origin){
            myMove &= ~0xfc0; myMove |= ((origin & 0x3f) << 6);
        }

        /*  POOSIBLE FLAG VALUES :
            0b0000  ->  quiet move
            0b0001  ->  double pawn push
            0b0010  ->  king castle
            0b0011  ->  queen castle
            0b0100  ->  captures
            0b0101  ->  en passant capture
            0b0111  ->  nothing
            0b1000  ->  knight promotion
            0b1001  ->  bishop promotion
            0b1010  ->  rook promotion
            0b1011  ->  queen promotion
            0b1100  ->  knight promotion with capture
            0b1101  ->  bishop promotion with capture
            0b1110  ->  rook promotion with capture
            0b1111  ->  queen promotion with capture
        */
	    inline void setFlags(const unsigned int flag){
	        myMove &= ~0xf000; myMove |= ((flag & 0xf) << 12);
	    }

        inline void setCapturedPieceType(const Piece::PieceType type){
	        myMove &= ~0x380000; myMove |= ((type & 0x7) << 19);
	    }

        inline void setPreviousCastlingRights(const unsigned int state){
            myMove &= ~0x3c00000; myMove |= ((state & 0xf) << 22);
        }

        inline bool isQuiet() const{
            bool isNotQuite = getFlags() & 0b1110;
            return !isNotQuite;
        }

        inline bool isCapture() const {return getFlags() & CAPTURE_FLAG;}
        inline bool isEnPassant() const {return getFlags() == EP_CAPTURE_FLAG;}
	    inline bool isPromotion() const {return getFlags() & PROMOTION_FLAG;}
	    inline bool isQueenSideCastling() const {return getFlags() == QUEEN_SIDE_CASTLING;}
	    inline bool isKingSideCastling() const {return getFlags() == KING_SIDE_CASTLING;}
	    inline bool isCastling() const {return isKingSideCastling() || isQueenSideCastling();}
        inline bool isDoublePawnPush() const {return getFlags() == DOUBLE_PAWN_PUSH_FLAG;}
        inline bool isNullMove() const {return myMove == NULL_MOVE;}

        inline unsigned int getMoveRating() const {return myMoveRating;}
        inline void setMoveRating(const unsigned int rating) {myMoveRating = rating;}

        inline bool operator==(const Move &otherMove) const {
            return (getOrigin() == otherMove.getOrigin()) && (getDestination() == otherMove.getDestination()) && (getFlags() == otherMove.getFlags());
        };

        inline bool operator<(const Move &otherMove) const {return getMoveRating() < otherMove.getMoveRating();};
        inline bool operator>(const Move &otherMove) const {return getMoveRating() > otherMove.getMoveRating();};
        inline bool operator<=(const Move &otherMove) const {return getMoveRating() <= otherMove.getMoveRating();};
        inline bool operator>=(const Move &otherMove) const {return getMoveRating() >= otherMove.getMoveRating();};

        string toShortString() const;
};

inline ostream& operator<<(ostream &strm, const Move &move){
    bitset<4> flags(move.getFlags());
    unsigned int xOrigin = move.getOrigin() % 8;
	unsigned int yOrigin = move.getOrigin() / 8;

	unsigned int xDestination = move.getDestination() % 8;
	unsigned int yDestination = move.getDestination() / 8;

    unsigned int moveOrderingScore = move.getMoveRating();

	strm << move.toShortString() << " "     
            << "Origin: [" << xOrigin << ", " << yOrigin << "] Dest: ["
			<< xDestination << ", " << yDestination << "] Flags: " << flags
            << " MO score : " << moveOrderingScore
			<< std::endl;

	return strm;
}

#endif