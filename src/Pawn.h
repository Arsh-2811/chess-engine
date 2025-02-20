#ifndef PAWN_H
#define PAWN_H

#include "Types.h"
#include "Board.h"

namespace Pawn{
    const static int HASH_SIZE = 16384;
    enum Status{
        PASSED = 0,
        DOUBLED = 1,
        ISOLATED = 2
    };

    struct Entry{
        Zkey key;
        int score;
    };

    void initPawnTable();

    int getScore(const Board &board, const int gameStage, const int alpha);
    int calculateEntryCount();
	int calculateScore(const Board &board, const int gameStage, const int alpha);

	int doubledPawns(const Board &board);
	int passedPawns(const Board &board);
    int isolatedPawns(const Board &board);
    int supportedPawns(const Board &board);

    inline unsigned int countPawnsInFile(const Board &board, const unsigned int file, const Color color){return popcount(board.getBitBoard(Piece::PAWN,color) & LookUpTables::maskFile[file]);};
	inline unsigned int countPawns(const Board &board, const Color color){return popcount(board.getPawns(color));};
    inline bool hasNeighbors(const Board &board, const unsigned int file, const Color color){return (board.getPawns(color) & LookUpTables::neighbourFiles[file]);};
};

inline std::ostream& operator<<(std::ostream &strm, const Pawn::Entry &entry) {

    strm << " > ";
    strm << entry.key << " ";
    strm << "Score " << entry.score;
	return strm;
}

#endif