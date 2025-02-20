#include "TT.h"

typedef std::uint64_t Zkey;

TT globalTT = TT();

void TT::init_TT_size(int sizeMBytes){
    U64 sizeBytes = sizeMBytes*1024*1024;
    std::size_t entrySize = sizeof(TTEntry);
    U64 nEntries = sizeBytes / entrySize;

    delete [] myTTTable;
    myTTSize = nEntries;

    try{
        myTTTable = new TTEntry [myTTSize];
    } catch (std::bad_alloc&){
        std::cerr << "Failed to allocate transposition hash table" << std::endl;
        exit(EXIT_FAILURE);
    }
    clearTT();
}

void TT::setTTEntry(const Zkey zkey, const int depth, const int score, const NodeType node, const Move16 bestMove, const int moveCounter){
    U64 index = zkey % myTTSize;
    if (TTEntry::K * depth + moveCounter >= myTTTable[index].getTTValue()){
        TTEntry newEntry(zkey, depth, score, node, bestMove, moveCounter);
        myTTTable[index] = newEntry ;
    }
}

TTEntry* TT::probeTT(const Zkey zkey, const int depth){
    U64 index = zkey % myTTSize;
    // cout << "Index : " << index << endl;
    if (myTTTable[index].getZkey() == zkey && myTTTable[index].getNodeType() != NodeType::NONE && myTTTable[index].getDepth() >= depth){
        return &myTTTable[index];
    } else {
        return nullptr;
    }
}

U64 TT::countEntries(){
	U64 count = 0;
	for(U64 i=0; i<myTTSize; i++){
		count += myTTTable[i].getNodeType() != NodeType::NONE;
	}

	return count;
}