#include <bits/stdc++.h>

#include "LookUpTables.h"
#include "BitBoardsUtils.h"
#include "Utils.h"

U64 LookUpTables::frontSpans[2][SQUARE_NB];
U64 LookUpTables::pawnAttackSpans[2][SQUARE_NB];
U64 LookUpTables::passedPawnMask[2][SQUARE_NB];

U64 LookUpTables::attackTable[Piece::TOTAL_PIECES][Square::SQUARE_NB];
U64 LookUpTables::pawnAttackTable[Color::COLOR_NB][Square::SQUARE_NB];
U64 LookUpTables::lineBB[Square::SQUARE_NB][Square::SQUARE_NB];
U64 LookUpTables::inBetween[SQUARE_NB][SQUARE_NB];
U64 LookUpTables::squareBB[Square::SQUARE_NB];
Square LookUpTables::SQUARE[FILE_NB][RANK_NB];

// U64 LookUpTables::bishopMovementMask[SQUARE_NB];
// vector<vector<U64>> LookUpTables::bishopLookupTableMagicNumbers;

// U64 LookUpTables::rookMovementMask[SQUARE_NB];
// vector<vector<U64>> LookUpTables::rookLookupTableMagicNumbers;

void LookUpTables::init(){
    for(Square square = SQ_A1; square < SQUARE_NB; ++square){
        frontSpans[WHITE][square] = frontBB(square, WHITE);
        frontSpans[BLACK][square] = frontBB(square, BLACK);

        pawnAttackSpans[WHITE][square] = sidesBB(square, WHITE);
        pawnAttackSpans[BLACK][square] = sidesBB(square, BLACK);

        passedPawnMask[WHITE][square] = frontSpans[WHITE][square] | pawnAttackSpans[WHITE][square];
        passedPawnMask[BLACK][square] = frontSpans[BLACK][square] | pawnAttackSpans[BLACK][square];

        attackTable[Piece::PAWN][square] = 0x0;
        attackTable[Piece::KNIGHT][square] = knightAttacks(square);
        attackTable[Piece::BISHOP][square] = bishopAttacks(square);
        attackTable[Piece::ROOK][square] = rookAttacks(square);
        attackTable[Piece::QUEEN][square] = queenAttacks(square);
        attackTable[Piece::KING][square] = kingAttacks(square);
        attackTable[Piece::NO_PIECE_TYPE][square] = 0x0;

        pawnAttackTable[WHITE][square] = pawnAttacks(square, WHITE);
        pawnAttackTable[BLACK][square] = pawnAttacks(square, BLACK);

        squareBB[square] = 1ULL << square;

        File file = getFile(square);
        Rank rank = getRank(square);

        SQUARE[file][rank] = square;
    }

    for(Square square1 = SQ_A1; square1 < SQUARE_NB; ++square1){
        for(Square square2 = SQ_A1; square2 < SQUARE_NB; ++square2){
            File f1 = getFile(square1);
            File f2 = getFile(square2);
            Rank r1 = getRank(square1);
            Rank r2 = getRank(square2);

            Rank rMin = min(r1, r2);
			Rank rMax = max(r1, r2);
			File fMin = min(f1, f2);
			File fMax = max(f1, f2);

			bool AreOnsameFile = f1 == f2;
			bool AreOnsameRank = r1 == r2;
			bool AreOnSameDiag = fMax-fMin == rMax - rMin;

			++rMin;--rMax;++fMin;--fMax;

			U64 b = 0ULL;
			U64 l = 0ULL;

            if(AreOnsameFile){
                for(Rank r = rMin; r <= rMax; ++r){
                    Square s = SQUARE[f1][r];
                    b |= s;
                }
                for (Rank r = RANK_1; r <= RANK_8; ++r){
					l |= SQUARE[f1][r];
				} 
            } else if(AreOnsameRank){
				for (File f = fMin; f<= fMax; ++f){
					Square s = SQUARE[f][r1];
					b |= s;
				}

				for (File f = FILE_A; f<= FILE_H; ++f){
					l |= SQUARE[f][r1];
				}
			} else if(AreOnSameDiag){
				bool positiveDiag = (f2-f1 == r2-r1);

				File f = positiveDiag ? fMin : fMax;
				for (Rank r = rMin; r <= rMax; ++r){
					Square s = SQUARE[f][r];
					b |= s;
					positiveDiag ? ++f : --f;
				}

                

				f = f1; Rank r = r1;

				if(positiveDiag){
					while(isOnBoard(f,r)){
						Square s = SQUARE[f][r];
						l |= s;
						++f; ++r;
					}

					f = f1; r = r1;
					while(isOnBoard(f,r)){
						Square s = SQUARE[f][r];
						l |= s;
						--f; --r;
					}
				} else {
					while(isOnBoard(f,r)){
						Square s = SQUARE[f][r];
						l |= s;
						++f; --r;
					}

					f = f1; r = r1;
					while(isOnBoard(f,r)){
						Square s = SQUARE[f][r];
						l |= s;
						--f; ++r;
					}
				}
			}
            inBetween[square1][square2] = b;
            lineBB[square1][square2] = l;
        }
    }

    // fillBishopMovementMask();
    // initializeBishopLookupTableMagicNumbers();

    // fillRookMovementMask();
    // initializeRookLookupTableMagicNumbers();
}

U64 LookUpTables::frontBB(const Square pos, Color color){
    U64 shift = 0xffffffffffffffff << (pos+1);
	if (color == BLACK){shift = (~shift) >> 1;}
	return  shift & maskFile[Utils::getFile(pos)];
}

U64 LookUpTables::sidesBB(const Square pos, Color color){
    unsigned int file = Utils::getFile(pos);
	U64 leftSide(0);
	U64 rightSide(0);
	if (file > 0){leftSide = frontBB(static_cast<Square>(pos-1), color);}
	if (file < 7){rightSide = frontBB(static_cast<Square>(pos+1), color);}
	return leftSide | rightSide;
}

U64 LookUpTables::kingAttacks(const Square pos){
    U64 kingLocation = 0 | 1LL << pos;

    U64 kingClipFileA = kingLocation & clearFile[FILE_A];
    U64 kingClipFileH = kingLocation & clearFile[FILE_H];

    U64 spot1 = kingClipFileH >> 7;
    U64 spot2 = kingLocation >> 8;
    U64 spot3 = kingClipFileH << 9;
    U64 spot4 = kingClipFileH << 1;

    U64 spot5 = kingClipFileA << 7;
    U64 spot6 = kingLocation << 8;
    U64 spot7 = kingClipFileA >> 9;
    U64 spot8 = kingClipFileA >> 1;

    return (spot1 | spot2 | spot3 | spot4 | spot5 | spot6 | spot7 | spot8);
}

U64 LookUpTables::knightAttacks(const Square pos){
    U64 knightLocation = 0 | 1LL << pos;

    U64 spot1clip = clearFile[0] & clearFile[1];
    U64 spot2clip = clearFile[0];
    U64 spot3clip = clearFile[7];
    U64 spot4clip = clearFile[6] & clearFile[7];

    U64 spot1 = (knightLocation & spot1clip) << 6;
    U64 spot2 = (knightLocation & spot2clip) << 15;
    U64 spot3 = (knightLocation & spot3clip) << 17;
    U64 spot4 = (knightLocation & spot4clip) << 10;

    U64 spot5 = (knightLocation & spot4clip) >> 6;
    U64 spot6 = (knightLocation & spot3clip) >> 15;
    U64 spot7 = (knightLocation & spot2clip) >> 17;
    U64 spot8 = (knightLocation & spot1clip) >> 10;

    return (spot1 | spot2 | spot3 | spot4 | spot5 | spot6 | spot7 | spot8);
}

U64 LookUpTables::pawnAttacks(const Square pos, Color color){
    U64 pawnLocation = 0 | 1LL << pos;

    if(color == WHITE){
        U64 leftAttack = (pawnLocation & clearFile[0]) << 7;
        U64 rightAttack = (pawnLocation & clearFile[7]) << 9;

        return (leftAttack | rightAttack);
    } else if (color == BLACK){
        U64 leftAttack = (pawnLocation & clearFile[0]) >> 9;
        U64 rightAttack = (pawnLocation & clearFile[7]) >> 7;

        return (leftAttack | rightAttack);
    }
    return 0;
}

// U64 LookUpTables::bishopAttacks(Square square){
//     U64 mask = 0ULL;
//     int r = square / 8;
//     int c = square % 8;
//     for(int i = r + 1, j = c + 1; i < 8 && j < 8; i++, j++){
//         mask |= 1ULL << (i * 8 + j);
//     }
//     for(int i = r + 1, j = c - 1; i < 8 && j >= 0; i++, j--){
//         mask |= 1ULL << (i * 8 + j);
//     }
//     for(int i = r - 1, j = c + 1; i >= 0 && j < 8; i--, j++){
//         mask |= 1ULL << (i * 8 + j);
//     }
//     for(int i = r - 1, j = c - 1; i >= 0 && j >= 0; i--, j--){
//         mask |= 1ULL << (i * 8 + j);
//     }
//     return mask;
// }

// U64 LookUpTables::rookAttacks(Square square){
//     U64 mask = 0ULL;
//     int r = square / 8;
//     int c = square % 8;
//     for (int j = r + 1; j < 8; j++){
//         mask |= 1ULL << (j * 8 + c);
//     }
//     for (int j = r - 1; j >= 0; j--){
//         mask |= 1ULL << (j * 8 + c);
//     }
//     for (int j = c + 1; j < 8; j++){
//         mask |= 1ULL << (r * 8 + j);
//     }
//     for (int j = c - 1; j >= 0; j--){
//         mask |= 1ULL << (r * 8 + j);
//     }
//     return mask;
// }

// U64 LookUpTables::queenAttacks(Square square){
//     return bishopAttacks(square) | rookAttacks(square);
// }

// void LookUpTables::fillBishopMovementMask(){
//     for(Square square = SQ_A1; square < SQUARE_NB; ++square){
//         U64 mask = 0ULL;
//         int r = square / 8;
//         int c = square % 8;
//         for(int i = r + 1, j = c + 1; i < 7 && j < 7; i++, j++){
//             mask |= 1ULL << (i * 8 + j);
//         }
//         for(int i = r + 1, j = c - 1; i < 7 && j >= 1; i++, j--){
//             mask |= 1ULL << (i * 8 + j);
//         }
//         for(int i = r - 1, j = c + 1; i >= 1 && j < 7; i--, j++){
//             mask |= 1ULL << (i * 8 + j);
//         }
//         for(int i = r - 1, j = c - 1; i >= 1 && j >= 1; i--, j--){
//             mask |= 1ULL << (i * 8 + j);
//         }
//         bishopMovementMask[square] = mask;
//     }
// }

// void LookUpTables::fillRookMovementMask(){
//     for (Square square = SQ_A1; square < SQUARE_NB; ++square){
//         U64 mask = 0ULL;
//         int r = square / 8;
//         int c = square % 8;
//         for (int j = r + 1; j < 7; j++){
//             mask |= 1ULL << (j * 8 + c);
//         }
//         for (int j = r - 1; j >= 1; j--){
//             mask |= 1ULL << (j * 8 + c);
//         }
//         for (int j = c + 1; j < 7; j++){
//             mask |= 1ULL << (r * 8 + j);
//         }
//         for (int j = c - 1; j >= 1; j--){
//             mask |= 1ULL << (r * 8 + j);
//         }
//         rookMovementMask[square] = mask;
//     }
// }

// void LookUpTables::initializeBishopLookupTableMagicNumbers(){
//     for (Square square = SQ_A1; square < SQUARE_NB; ++square){
//         int shift = bishopShiftValues[square];
//         U64 magicNumber = bishopMagicNumbers[square];
//         vector<U64> lookupTable(UINT64_MAX >> shift, 0);
//         vector<U64> blockerBitBoards = createAllBlockerBitBoards(bishopMovementMask[square]);
//         for (U64 blockerBitBoard : blockerBitBoards){
//             U64 index = (blockerBitBoard * magicNumber) >> shift;
//             lookupTable[index] = createBishopLegalMoves(square, blockerBitBoard);
//         }
//         bishopLookupTableMagicNumbers.push_back(lookupTable);
//     }
// }

// void LookUpTables::initializeRookLookupTableMagicNumbers(){
//     for (Square square = SQ_A1; square < SQUARE_NB; ++square){
//         int shift = rookShiftValues[square];
//         U64 magicNumber = rookMagicNumbers[square];
//         vector<U64> lookupTable(UINT64_MAX >> shift, 0);
//         vector<U64> blockerBitBoards = createAllBlockerBitBoards(rookMovementMask[square]);
//         for (U64 blockerBitBoard : blockerBitBoards)
//         {
//             U64 index = (blockerBitBoard * magicNumber) >> shift;
//             lookupTable[index] = createRookLegalMoves(square, blockerBitBoard);
//         }
//         rookLookupTableMagicNumbers.push_back(lookupTable);
//     }
// }
// /* GENERATING PSEUDO LEGAL MOVES */

// U64 LookUpTables::generateKingPseudoLegalMoves(Square square, U64 friendlyPieces){
//     return attackTable[Piece::KING][square] & ~friendlyPieces;
// }

// U64 LookUpTables::generateKnightPseudoLegalMoves(Square square, U64 friendlyPieces){
//     return attackTable[Piece::KNIGHT][square] & ~friendlyPieces;
// }

// U64 LookUpTables::generatePawnPseudoLegalMoves(Square square, U64 friendlyPieces, U64 enemyPieces, Color color){
//     U64 pawnLocation = 1ULL << square;
//     U64 allPieces = friendlyPieces | enemyPieces;
//     if(color == WHITE){
//         U64 whitePawnOneStep = (pawnLocation << 8) & ~allPieces;
//         U64 whitePawnTwoStep = ((whitePawnOneStep & maskRank[2]) << 8) & ~allPieces;
//         U64 whitePawnAttacks = pawnAttackTable[WHITE][square] & enemyPieces;
//         return (whitePawnOneStep | whitePawnTwoStep | whitePawnAttacks);
//     } else if (color == BLACK){
//         U64 blackPawnOneStep = (pawnLocation >> 8) & ~allPieces;
//         U64 blackPawnTwoStep = ((blackPawnOneStep & maskRank[5]) >> 8) & ~allPieces;
//         U64 blackPawnAttacks = pawnAttackTable[BLACK][square] & enemyPieces;
//         return (blackPawnOneStep | blackPawnTwoStep | blackPawnAttacks);
//     } else{
//         return 0ULL;
//     }
// }

// U64 LookUpTables::generateBishopPseudoLegalMoves(Square square, U64 friendlyPieces, U64 enemyPieces){
//     U64 allPieces = friendlyPieces | enemyPieces;
//     if(allPieces & bishopMovementMask[square]){
//         U64 magicNumber = bishopMagicNumbers[square];
//         int shift = bishopShiftValues[square];
//         U64 index = ((allPieces & bishopMovementMask[square]) * magicNumber) >> shift;
//         return bishopLookupTableMagicNumbers[square][index] & ~friendlyPieces;
//     } else {
//         return attackTable[Piece::BISHOP][square] & ~friendlyPieces;
//     }
// }

// U64 LookUpTables::generateRookPseudoLegalMoves(Square square, U64 friendlyPieces, U64 enemyPieces){
//     U64 allPieces = friendlyPieces | enemyPieces;
//     if(allPieces & rookMovementMask[square]){
//         U64 magicNumber = rookMagicNumbers[square];
//         int shift = rookShiftValues[square];
//         U64 index = ((allPieces & rookMovementMask[square]) * magicNumber) >> shift;
//         return rookLookupTableMagicNumbers[square][index] & ~friendlyPieces;
//     } else {
//         return attackTable[Piece::ROOK][square] & ~friendlyPieces;
//     }
// }

// U64 LookUpTables::generateQueenPseudoLegalMoves(Square square, U64 friendlyPieces, U64 enemyPieces){
//     return generateBishopPseudoLegalMoves(square, friendlyPieces, enemyPieces) | generateRookPseudoLegalMoves(square, friendlyPieces, enemyPieces);
// }

// /* HELPER FUNCTIONS */
// vector<U64> LookUpTables::createAllBlockerBitBoards(U64 movementMask){
//     vector<int> moveSquareIndices;
//     for (int i = 0; i < 64; i++){
//         if (((movementMask >> i) & 1) == 1) moveSquareIndices.push_back(i);
//     }
//     vector<U64> blockerBitBoards(1ULL << moveSquareIndices.size(), 0);
//     for (int i = 0; i < (1 << moveSquareIndices.size()); i++){
//         for (int j = 0; j < moveSquareIndices.size(); j++){
//             U64 bit = (i >> j) & 1;
//             blockerBitBoards[i] |= bit << moveSquareIndices[j];
//         }
//     }
//     return blockerBitBoards;
// }

// U64 LookUpTables::createBishopLegalMoves(Square square, U64 blockerBitBoard){
//     U64 legalMoves = 0ULL;
//     int r = square / 8;     // 7
//     int c = square % 8;     // 5

//     // i = 8, j = 6, i < 8, j < 8
//     // NE
//     for (int i = r + 1, j = c + 1; i < 8 && j < 8; i++, j++){
//         int sq = i * 8 + j;
//         legalMoves |= 1ULL << sq;
//         if (blockerBitBoard & (1ULL << sq)) break;
//     }

//     // i = 8, j = 4, i < 8, j >= 0
//     // NW
//     for (int i = r + 1, j = c - 1; i < 8 && j >= 0; i++, j--){
//         int sq = i * 8 + j;
//         legalMoves |= 1ULL << sq;
//         if (blockerBitBoard & (1ULL << sq)) break;
//     }

//     // i = 6, j =  6, i >= 0, j < 6
//     // SE
//     for (int i = r - 1, j = c + 1; i >= 0 && j < 8; i--, j++){
//         int sq = i * 8 + j;
//         legalMoves |= 1ULL << sq;
//         if (blockerBitBoard & (1ULL << sq)) break;
//     }

//     // i = 6, j = 4, i >= 0, j >= 0
//     //SW
//     for (int i = r - 1, j = c - 1; i >= 0 && j >= 0; i--, j--){
//         int sq = i * 8 + j;
//         legalMoves |= 1ULL << sq;
//         if (blockerBitBoard & (1ULL << sq)) break;
//     }
//     return legalMoves;
// }

// U64 LookUpTables::createRookLegalMoves(Square square, U64 blockerBitBoard){
//     U64 legalMoves = 0ULL;
//     int r = square / 8;
//     int c = square % 8;

//     for (int i = r + 1; i < 8; i++){
//         int sq = i * 8 + c;
//         legalMoves |= 1ULL << sq;
//         if (blockerBitBoard & (1ULL << sq)) break;
//     }

//     for (int i = r - 1; i >= 0; i--){
//         int sq = i * 8 + c;
//         legalMoves |= 1ULL << sq;
//         if (blockerBitBoard & (1ULL << sq)) break;
//     }

//     for (int i = c + 1; i < 8; i++){
//         int sq = r * 8 + i;
//         legalMoves |= 1ULL << sq;
//         if (blockerBitBoard & (1ULL << sq)) break;
//     }

//     for (int i = c - 1; i >= 0; i--){
//         int sq = r * 8 + i;
//         legalMoves |= 1ULL << sq;
//         if (blockerBitBoard & (1ULL << sq)) break;
//     }
//     return legalMoves;
// }

// U64 LookUpTables::bmagic(Square square, U64 occupancy){
//     U64 magicNumber = bishopMagicNumbers[square];
//     int shift = bishopShiftValues[square];
//     U64 index = ((occupancy & bishopMovementMask[square]) * magicNumber) >> shift;
//     return bishopLookupTableMagicNumbers[square][index];
// }

// U64 LookUpTables::rmagic(Square square, U64 occupancy){
//     U64 magicNumber = rookMagicNumbers[square];
//     int shift = rookShiftValues[square];
//     U64 index = ((occupancy & rookMovementMask[square]) * magicNumber) >> shift;
//     return rookLookupTableMagicNumbers[square][index];
// }