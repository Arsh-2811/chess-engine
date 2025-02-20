#ifndef TT_H
#define TT_H

#include "Board.h"
#include "TTEntry.h"
#include "Move.h"
#include "Types.h"

typedef std::uint64_t Zkey;

using namespace std;

class TT {
    public :
        const static int TEST_MB_SIZE = 16;
        TT(){}
        ~TT(){delete []myTTTable;}

        inline TTEntry operator[](const U64 x) const {
            // cout << "Asking for index : " << x << " from the transposition table..." << endl;
            if(x > myTTSize){std::cout << "TT out of bounds" << std::endl;return myTTTable[0];}
            return myTTTable[x];
        }

        inline void clearTT(){
    	    for(U64 i=0; i<myTTSize; i++){
    		    myTTTable[i] = TTEntry();
    	    }
        }

        void init_TT_size(int sizeMBytes);
        void setTTEntry(const Zkey zkey, const int depth, const int score, const NodeType node, const Move16 bestMove, const int moveCounter);
        TTEntry* probeTT(const Zkey zkey, const int depth);

        U64 countEntries();
        inline int getTTUsage(){return countEntries()/myTTSize*100;}
        inline U64 getTTSize() const {return myTTSize;}

    private :
        U64 myTTSize;
        TTEntry* myTTTable;
};

extern TT globalTT;

inline std::ostream& operator<<(std::ostream&strm, const TT &tt){
    for(unsigned long int entry = 0; entry < tt.getTTSize(); entry++){
        if(tt[entry].getNodeType() != NodeType::NONE){
            strm << entry << " > ";
            strm << tt[entry].getNodeType() << " ";
		    strm << "Depth " << tt[entry].getDepth() << " ";
		    strm << "Score " << tt[entry].getScore() << " ";
		    strm << "Best move " << tt[entry].getBestmove() << " ";
		    strm << std::endl;
        }
    }
    strm << std::endl;
	return strm;
}

#endif