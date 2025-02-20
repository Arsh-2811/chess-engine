#ifndef TTENTRY_H
#define TTENTRY_H

#include<iostream>

#include "Types.h"
#include "Utils.h"

using namespace std;

typedef std::uint64_t Zkey;

enum NodeType {NONE = 0, EXACT = 1, LOWER = 2, UPPER = 3};

class TTEntry {
    public :
        const static int K = 2;

        TTEntry() : myZkey(0), myBestMove(), myTTInfo(0), myTTValue(0){};

        inline TTEntry(Zkey zkey, int depth, int score, NodeType node, Move16 bestMove, int moveCounter) : myZkey(zkey), myBestMove(bestMove), myTTValue(K*depth + moveCounter){
            int sign = (score < 0);
            myTTInfo = ((sign & 0x1) << 27 | (std::abs(score) & 0xfffff) << 7) | ((depth & 0x1f) << 2) | (node & 0x3);
        };

        inline Move16 getBestmove() const {return myBestMove;}
        inline int getTTValue() const {return myTTValue;}
        inline int getDepth() const {return ((myTTInfo >> 2) & 0x1f);}
        inline NodeType getNodeType() const {return static_cast<NodeType>(myTTInfo & 0x3);};

        inline Zkey getZkey() const {
            // std::cout << "Zkey : "  << hex << myZkey << dec <<endl;
            return myZkey;
            }

        int getScore() const {
            int sign = (myTTInfo >> 27) & 0x1;
            return (1-2*sign)*((myTTInfo >> 7) & 0xfffff);
        }

    private :
        Zkey myZkey;
        Move16 myBestMove;
        int myTTInfo;
        int myTTValue;
};

inline std::ostream& operator<<(std::ostream &strm, const NodeType &node) {
        if (node == NodeType::NONE) strm << "NONE" << " ";
        else if (node == NodeType::EXACT) strm << "EXACT" << " ";
        else if (node == NodeType::LOWER) strm << "LOWER" << " ";
        else if (node == NodeType::UPPER) strm << "UPPER" << " ";
	return strm;
}

inline std::ostream& operator<<(std::ostream &strm, const TTEntry &entry) {
    strm << " > ";
    strm << entry.getNodeType() << " ";
    strm << "Depth " << entry.getDepth() << " ";
    strm << "Score " << entry.getScore() << " ";
    strm << "Best move " << Utils::Move16ToShortString(entry.getBestmove());
	return strm;
}

#endif