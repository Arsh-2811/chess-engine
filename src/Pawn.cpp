#include "Pawn.h"
#include "EvalTables.h"
#include "Evaluation.h"
#include "LookUpTables.h"

Pawn::Entry pawnsTable[Pawn::HASH_SIZE];

void Pawn::initPawnTable(){
    for(U64 i=0; i<Pawn::HASH_SIZE; i++){
    	pawnsTable[i].key = 0;
    	pawnsTable[i].score = 0;
    }
}

int Pawn::getScore(const Board &board, const int gameStage, const int alpha){
    Zkey key = board.pawnsKey;
    int index = key % Pawn::HASH_SIZE;

    if(pawnsTable[index].key == key){
        return pawnsTable[index].score;
    } else {
        int score = calculateScore(board, gameStage, alpha);
        pawnsTable[index].key = key;
        pawnsTable[index].score = score;

        return score;
    }
}

int Pawn::calculateEntryCount(){
	int count = 0;
	for(int i=0; i<HASH_SIZE; i++){
		Pawn::Entry entry = pawnsTable[i];
		if(entry.key !=0){
			count++;
		}
	}
	return count;
}

int Pawn::calculateScore(const Board &board, const int gameStage, const int alpha){
    int doubled = doubledPawns(board);
    int passed = passedPawns(board);
    int isolated = isolatedPawns(board);

    int passedScore = passed*(EvalTables::PawnTable[OPENING][PASSED]*gameStage + EvalTables::PawnTable[ENDGAME][PASSED]*alpha);
    int doubledScore  = doubled*(EvalTables::PawnTable[OPENING][DOUBLED]*gameStage + EvalTables::PawnTable[ENDGAME][DOUBLED]*alpha);
    int isolatedScore = isolated*(EvalTables::PawnTable[OPENING][ISOLATED]*gameStage + EvalTables::PawnTable[ENDGAME][ISOLATED]*alpha);
    int score =  (passedScore + doubledScore + isolatedScore) / Evaluation::TOTAL_MATERIAL;

    return score;
}

int Pawn::supportedPawns(const Board &board){
    unsigned int whiteCount(0);
    unsigned int blackCount(0);
    U64 whitePawns = board.getWhitePawns();
    U64 blackPawns = board.getBlackPawns();

    while (whitePawns){
        Square sq = pop_lsb(&whitePawns);
        Rank rank = getRank(sq);
        File file = getFile(sq);

        bool hasNeighbours = board.getWhitePawns() & LookUpTables::neighbourFiles[file];
        whiteCount += hasNeighbours && (LookUpTables::maskFile[rank-1] & board.getWhitePawns());
    }

    while (blackPawns){
        Square sq = pop_lsb(&blackPawns);
        Rank rank = getRank(sq);
        File file = getFile(sq);

        bool hasNeighbours = board.getBlackPawns() & LookUpTables::neighbourFiles[file];
        blackCount += hasNeighbours && (LOOPUPTABLES_H::maskRank[rank+1] & board.getBlackPawns());
    }

    return static_cast<int>(whiteCount-blackCount);
}

int Pawn::doubledPawns(const Board &board){
	unsigned int whiteCount(0);
	unsigned int blackCount(0);
	unsigned int column;
	for (column=0; column<=7; column++){
        unsigned int count=countPawnsInFile(board,column,WHITE);
		whiteCount += (count>1)*(count-1);
		count=countPawnsInFile(board,column,BLACK);
		blackCount += (count>1)*(count-1);
	}
	return static_cast<int>(whiteCount-blackCount);
}

int Pawn::passedPawns(const Board &board){
    unsigned int whiteCount(0);
	unsigned int blackCount(0);
    U64 whitePawns = board.getWhitePawns();
    U64 blackPawns = board.getBlackPawns();

    while (whitePawns){
		Square positionMsb = pop_lsb(&whitePawns);
        whiteCount += ((LookUpTables::passedPawnMask[WHITE][positionMsb] & blackPawns) == 0);
    }

    whitePawns = board.getWhitePawns();

    while (blackPawns){
		Square positionMsb = pop_lsb(&blackPawns);
        blackCount += ((LookUpTables::passedPawnMask[BLACK][positionMsb] & whitePawns) == 0);
    }
	return static_cast<int>(whiteCount-blackCount);
}

int Pawn::isolatedPawns(const Board &board){
    unsigned int whiteCount(0);
	unsigned int blackCount(0);
	unsigned int column;

	for (column=0; column<=7; column++){
        unsigned int count=countPawnsInFile(board,column,WHITE);
		whiteCount += (count>0)&(!hasNeighbors(board,column,WHITE));
		count=countPawnsInFile(board,column,BLACK);
		blackCount += (count>0)&(!hasNeighbors(board,column,BLACK));
	}

	return static_cast<int>(whiteCount-blackCount);
}