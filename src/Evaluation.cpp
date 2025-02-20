#include "Evaluation.h"
#include "EvalTables.h"
#include "BitBoardsUtils.h"
#include "LookUpTables.h"
#include "Types.h"

int Evaluation::POSITIONAL_GAIN_PERCENT = 100;
int Evaluation::MOBILITY_GAIN_PERCENT = 100;
int Evaluation::PAWN_GAIN_PERCENT = 100;

Evaluation::Evaluation(shared_ptr<Board> boardPtr){
    myBoard = boardPtr;
    init();
}

int Evaluation::calculateKingSafety(){
    int hasWhiteCastled = myBoard->hasWhiteCastled();
	int hasBlackCastled = myBoard->hasBlackCastled();
	int kingSafetyScore = 45*(hasWhiteCastled - hasBlackCastled);

    if(hasWhiteCastled && getRank(myBoard->getWhiteKingSquare()) <= RANK_4){   
        Square kSq = myBoard->getWhiteKingSquare();
        File kingFile = getFile(kSq);
        Rank kingRank = getRank(kSq);
        
        if(kingFile >= FILE_G){   
            //king side castling
            U64 safeArea = 14737632ULL;

            U64 pawnShelter = LookUpTables::maskRank[kingRank+1] & myBoard->getWhitePawns() & safeArea;
            U64 farAwayPawnShelter = LookUpTables::maskRank[kingRank+2] & myBoard->getWhitePawns() & safeArea;
            kingSafetyScore += 5*popcount(pawnShelter);
            kingSafetyScore += 3*popcount(farAwayPawnShelter);
        }
        else if(kingFile <= FILE_C){
            //queen side castling
            U64 safeArea = 1799ULL;

            U64 pawnShelter = LookUpTables::maskRank[kingRank+1] & myBoard->getWhitePawns() & safeArea;
            kingSafetyScore += 5*popcount(pawnShelter);
        }
    }

    //Black king safety
    if(hasBlackCastled && getRank(myBoard->getBlackKingSquare()) >= RANK_5){
        Square kSq = myBoard->getBlackKingSquare();
        File kingFile = getFile(kSq);
        Rank kingRank = getRank(kSq);

        if(kingFile >= FILE_G){
            //king side castling
            U64 safeArea = 16204197749883666432ULL;

            U64 pawnShelter = LookUpTables::maskRank[kingRank-1] & myBoard->getBlackPawns() & safeArea;
            U64 farAwayPawnShelter = LookUpTables::maskRank[kingRank-2] & myBoard->getBlackPawns() & safeArea;
            kingSafetyScore -= 5*popcount(pawnShelter);
            kingSafetyScore -= 3*popcount(farAwayPawnShelter);
        }
        else if(kingFile <= FILE_C){
            //queen side castling
            U64 safeArea = 506373483102470144ULL;

            U64 pawnShelter = LookUpTables::maskRank[kingRank-1] & myBoard->getBlackPawns() & safeArea;;
            kingSafetyScore -= 5*popcount(pawnShelter);
        }
    }
	return kingSafetyScore;
}

void Evaluation::init(){
    myMaterialScore = getWhitePiecesValue() - getBlackPiecesValue();
    myGameStage = (getWhitePiecesValue() + getBlackPiecesValue());

    int whiteOpeningValue(0); int blackOpeningValue(0);
    int whiteEndgameValue(0); int blackEndgameValue(0);

    U64 myPos(myBoard->getWhitePawns());
    while(myPos){
		int myIndex = msb(myPos);
		myPos = myPos ^ ( 0 | 1LL << myIndex);
		whiteOpeningValue+=EvalTables::AllPieceSquareTables[WHITE][0][0][myIndex];
		whiteEndgameValue+=EvalTables::AllPieceSquareTables[WHITE][1][0][myIndex];
	}

	myPos=myBoard->getWhiteKnights();
    while(myPos){
		int myIndex = msb(myPos);
        myPos = myPos ^ ( 0 | 1LL << myIndex);
		whiteOpeningValue+=EvalTables::AllPieceSquareTables[WHITE][0][1][myIndex];
		whiteEndgameValue+=EvalTables::AllPieceSquareTables[WHITE][1][1][myIndex];
    }

	myPos=myBoard->getWhiteBishops();
    while(myPos){
		int myIndex = msb(myPos);
        myPos = myPos ^ ( 0 | 1LL << myIndex);
		whiteOpeningValue+=EvalTables::AllPieceSquareTables[WHITE][0][2][myIndex];
		whiteEndgameValue+=EvalTables::AllPieceSquareTables[WHITE][1][2][myIndex];
	}

	myPos=myBoard->getWhiteRooks();
    while(myPos){
		int myIndex = msb(myPos);
		myPos = myPos ^ ( 0 | 1LL << myIndex);
		whiteOpeningValue+=EvalTables::AllPieceSquareTables[WHITE][0][3][myIndex];
		whiteEndgameValue+=EvalTables::AllPieceSquareTables[WHITE][1][3][myIndex];
	}

	myPos=myBoard->getWhiteQueens();
    while(myPos){
		int myIndex = msb(myPos);
		myPos = myPos ^ ( 0 | 1LL << myIndex);
        whiteOpeningValue+=EvalTables::AllPieceSquareTables[WHITE][0][4][myIndex];
		whiteEndgameValue+=EvalTables::AllPieceSquareTables[WHITE][1][4][myIndex];
	}

	myPos=myBoard->getWhiteKing();
    while(myPos){
		int myIndex = msb(myPos);
		myPos = myPos ^ ( 0 | 1LL << myIndex);
        whiteOpeningValue+=EvalTables::AllPieceSquareTables[WHITE][0][5][myIndex];
		whiteEndgameValue+=EvalTables::AllPieceSquareTables[WHITE][1][5][myIndex];
	}

    myPos=myBoard->getBlackPawns();
    while(myPos){
		int myIndex = msb(myPos);
		myPos = myPos ^ ( 0 | 1LL << myIndex);
        blackOpeningValue+=EvalTables::AllPieceSquareTables[BLACK][0][0][myIndex];
		blackEndgameValue+=EvalTables::AllPieceSquareTables[BLACK][1][0][myIndex];
	}

	myPos=myBoard->getBlackKnights();
    while(myPos){
		int myIndex = msb(myPos);
		myPos = myPos ^ ( 0 | 1LL << myIndex);
        blackOpeningValue+=EvalTables::AllPieceSquareTables[BLACK][0][1][myIndex];
		blackEndgameValue+=EvalTables::AllPieceSquareTables[BLACK][1][1][myIndex];
	}

	myPos=myBoard->getBlackBishops();
    while(myPos){
		int myIndex = msb(myPos);
		myPos = myPos ^ ( 0 | 1LL << myIndex);
        blackOpeningValue+=EvalTables::AllPieceSquareTables[BLACK][0][2][myIndex];
		blackEndgameValue+=EvalTables::AllPieceSquareTables[BLACK][1][2][myIndex];	}

	myPos=myBoard->getBlackRooks();
    while(myPos){
		int myIndex = msb(myPos);
		myPos = myPos ^ ( 0 | 1LL << myIndex);
        blackOpeningValue+=EvalTables::AllPieceSquareTables[BLACK][0][3][myIndex];
		blackEndgameValue+=EvalTables::AllPieceSquareTables[BLACK][1][3][myIndex];	}

	myPos=myBoard->getBlackQueens();
    while(myPos){
		int myIndex = msb(myPos);
		myPos = myPos ^ ( 0 | 1LL << myIndex);
        blackOpeningValue+=EvalTables::AllPieceSquareTables[BLACK][0][4][myIndex];
		blackEndgameValue+=EvalTables::AllPieceSquareTables[BLACK][1][4][myIndex];	}

	myPos=myBoard->getBlackKing();
    while(myPos){
		int myIndex = msb(myPos);
		myPos = myPos ^ ( 0 | 1LL << myIndex);
        blackOpeningValue+=EvalTables::AllPieceSquareTables[BLACK][0][5][myIndex];
		blackEndgameValue+=EvalTables::AllPieceSquareTables[BLACK][1][5][myIndex];	}

    myOpeningPSQValue = whiteOpeningValue - blackOpeningValue;
    myEndgamePSQValue = whiteEndgameValue - blackEndgameValue;
}

int Evaluation::calcMobilityScore(const int64_t alpha) const {
    int64_t score(0);
    U64 currentBB(0ULL);
    int pieceMobility(0);
    Square square;

    for (Color color = WHITE; color<COLOR_NB; ++color){
        pieceMobility = 0;

        // KNIGHT
        currentBB = myBoard->getBitBoard(Piece::KNIGHT, color);
        while(currentBB){
            square = pop_lsb(&currentBB);
            U64 attacks = myBoard->getKnightAttacks(square, color);
            pieceMobility += popcount(attacks);
        }

        score += pieceMobility*(EvalTables::MobilityScaling[OPENING][Piece::KNIGHT]*myGameStage +
        EvalTables::MobilityScaling[ENDGAME][Piece::KNIGHT]*alpha)*(-2*color + 1);

        // BISHOP
        pieceMobility = 0 ;
        currentBB = myBoard->getBitBoard(Piece::BISHOP, color);
        while(currentBB)
        {
            square = pop_lsb(&currentBB);
            U64 attacks = myBoard->getBishopAttacks(square, color);
            pieceMobility += popcount(attacks);
         }

        score += pieceMobility*(EvalTables::MobilityScaling[OPENING][Piece::BISHOP]*myGameStage +
        EvalTables::MobilityScaling[ENDGAME][Piece::BISHOP]*alpha)*(-2*color + 1); 

        // ROOK
        pieceMobility = 0 ; 
        currentBB = myBoard->getBitBoard(Piece::ROOK, color);
        while(currentBB)
        {
            square = pop_lsb(&currentBB);
            U64 attacks = myBoard->getRookAttacks(square, color);
            pieceMobility += popcount(attacks);
        }

        score += pieceMobility*(EvalTables::MobilityScaling[OPENING][Piece::ROOK]*myGameStage +
        EvalTables::MobilityScaling[ENDGAME][Piece::ROOK]*alpha)*(-2*color + 1); 

        // QUEEN
        pieceMobility = 0 ; 
        currentBB = myBoard->getBitBoard(Piece::QUEEN, color);
        while(currentBB)
        {
            square = pop_lsb(&currentBB);
            U64 attacks = myBoard->getQueenAttacks(square, color);
            pieceMobility += popcount(attacks); 
       }

        score += pieceMobility*(EvalTables::MobilityScaling[OPENING][Piece::QUEEN]*myGameStage +
        EvalTables::MobilityScaling[ENDGAME][Piece::QUEEN]*alpha)*(-2*color + 1); 
	}
    return score / TOTAL_MATERIAL;
}

int Evaluation::calcMaterialAdjustments(const int64_t alpha) const{
    unsigned int whitePawns = Pawn::countPawns(*myBoard, WHITE);
    unsigned int blackPawns = Pawn::countPawns(*myBoard, BLACK);
    unsigned int whiteKnights = popcount(myBoard->getWhiteKnights());
    unsigned int blackKnights = popcount(myBoard->getBlackKnights());
    unsigned int whiteBishops = popcount(myBoard->getWhiteBishops());
    unsigned int blackBishops = popcount(myBoard->getBlackBishops());
    int knightBonus = EvalTables::KnightValue*static_cast<int>(whiteKnights*whitePawns-blackKnights*blackPawns)/8;
    int bishopBonus = EvalTables::BishopValue*static_cast<int>(whiteBishops*whitePawns-blackBishops*blackPawns)/8;

    /* Bishop pair */
    unsigned int whiteBishopCount = popcount(myBoard->getWhiteBishops());
    unsigned int blackBishopCount = popcount(myBoard->getBlackBishops());
    int pairCount = (whiteBishopCount > 1) - (blackBishopCount > 1);

    int bishopPairBonus = pairCount*(EvalTables::BishopPair[OPENING]*myGameStage +
    EvalTables::BishopPair[ENDGAME]*alpha)/Evaluation::TOTAL_MATERIAL;

    return knightBonus + bishopBonus + bishopPairBonus;
}

int Evaluation::evaluate(){
    int64_t alpha = TOTAL_MATERIAL - myGameStage;
    int64_t openingValue = myOpeningPSQValue*myGameStage;
	int64_t endGameValue = myEndgamePSQValue*alpha;
	int64_t diff = openingValue + endGameValue;
    int positionScore =  diff/TOTAL_MATERIAL;

    int mobilityScore = calcMobilityScore(alpha);
    int materialScore = myMaterialScore;

    int kingSafetyScore = myGameStage*calculateKingSafety()/TOTAL_MATERIAL;
    int pawnScore = Pawn::getScore(*myBoard, myGameStage, alpha);

    return calcMaterialAdjustments(alpha) + materialScore + 1.16*positionScore + 0.62*mobilityScore + kingSafetyScore + pawnScore/4;
}   

int Evaluation::getWhitePiecesValue() const {
    int whitePiecesValue(0);
    whitePiecesValue += popcount(myBoard->getWhitePawns())*Piece::PAWN_VALUE
                     + popcount(myBoard->getWhiteKnights())*Piece::KNIGHT_VALUE
                     + popcount(myBoard->getWhiteBishops())*Piece::BISHOP_VALUE
                     + popcount(myBoard->getWhiteRooks())*Piece::ROOK_VALUE
                     + popcount(myBoard->getWhiteQueens())*Piece::QUEEN_VALUE;
    return whitePiecesValue;
}

int Evaluation::getBlackPiecesValue() const {
    int blackPiecesValue(0);
    blackPiecesValue += popcount(myBoard->getBlackPawns())*Piece::PAWN_VALUE
                     + popcount(myBoard->getBlackKnights())*Piece::KNIGHT_VALUE
                     + popcount(myBoard->getBlackBishops())*Piece::BISHOP_VALUE
                     + popcount(myBoard->getBlackRooks())*Piece::ROOK_VALUE
                     + popcount(myBoard->getBlackQueens())*Piece::QUEEN_VALUE;
    return blackPiecesValue;
}

void Evaluation::updateEvalAttributes(const Move &move){
    int origin = move.getOrigin();
    int destination = move.getDestination();
    int pieceType = move.getPieceType();
    Color nextToPlay = myBoard->getColorToPlay();
    Color color = Utils::getOppositeColor(nextToPlay);

    myOpeningPSQValue += (-2*color + 1)*
                        (EvalTables::AllPieceSquareTables[color][0][pieceType][destination]
                        -EvalTables::AllPieceSquareTables[color][0][pieceType][origin]);
    myEndgamePSQValue += (-2*color + 1)*
                        (EvalTables::AllPieceSquareTables[color][1][pieceType][destination]
                        -EvalTables::AllPieceSquareTables[color][1][pieceType][origin]);
    if (move.isCapture()){
        int capturedPieceType = move.getCapturedPieceType();
        int pieceValue = pieceTypeToValue(capturedPieceType);
        myGameStage -= pieceValue;
        myMaterialScore +=  (-2*color + 1)*pieceValue;
        myOpeningPSQValue += (-2*color + 1)*EvalTables::AllPieceSquareTables[nextToPlay][0][capturedPieceType][destination];
        myEndgamePSQValue += (-2*color + 1)*EvalTables::AllPieceSquareTables[nextToPlay][1][capturedPieceType][destination];
    }

    if (move.isPromotion()){
        int promotedPieceType = move.getPromotedPieceType();
        myMaterialScore += (-2*color + 1)*(pieceTypeToValue(promotedPieceType)-Piece::PAWN_VALUE);
        myOpeningPSQValue += (-2*color + 1)*EvalTables::AllPieceSquareTables[color][0][promotedPieceType][destination];
        myEndgamePSQValue += (-2*color + 1)*EvalTables::AllPieceSquareTables[color][1][promotedPieceType][destination];
    }
}

void Evaluation::rewindEvalAttributes(const Move &move){
    int origin=move.getOrigin();
    int destination=move.getDestination();
    int pieceType=move.getPieceType();
    Color color=myBoard->getColorToPlay();
    Color oppositeColor = Utils::getOppositeColor(color);

    myOpeningPSQValue -= (-2*color + 1)*
                        (EvalTables::AllPieceSquareTables[color][0][pieceType][destination]
                        -EvalTables::AllPieceSquareTables[color][0][pieceType][origin]);
    myEndgamePSQValue -= (-2*color + 1)*
                        (EvalTables::AllPieceSquareTables[color][1][pieceType][destination]
                        -EvalTables::AllPieceSquareTables[color][1][pieceType][origin]);
    if (move.isCapture()){
        int capturedPieceType = move.getCapturedPieceType();
        int pieceValue = pieceTypeToValue(capturedPieceType);
        myGameStage += pieceValue;
        myMaterialScore -=  (-2*color + 1)*pieceValue;
        myOpeningPSQValue -= (-2*color + 1)*EvalTables::AllPieceSquareTables[oppositeColor][0][capturedPieceType][destination];
        myEndgamePSQValue -= (-2*color + 1)*EvalTables::AllPieceSquareTables[oppositeColor][1][capturedPieceType][destination];
    }

    if (move.isPromotion()){
        int promotedPieceType = move.getPromotedPieceType();
        myMaterialScore -= (-2*color + 1)*(pieceTypeToValue(promotedPieceType)-Piece::PAWN_VALUE);
        myOpeningPSQValue -= (-2*color + 1)*EvalTables::AllPieceSquareTables[color][0][promotedPieceType][destination];
        myEndgamePSQValue -= (-2*color + 1)*EvalTables::AllPieceSquareTables[color][1][promotedPieceType][destination];
    }
}

int Evaluation::pieceTypeToValue(int type){
    switch (type){
        case 0: return Piece::PAWN_VALUE;
        case 1: return Piece::KNIGHT_VALUE;
        case 2: return Piece::BISHOP_VALUE;
        case 3: return Piece::ROOK_VALUE;
        case 4: return Piece::QUEEN_VALUE;
        case 5: return Piece::KING_VALUE;
        default: return 0;
    }
}