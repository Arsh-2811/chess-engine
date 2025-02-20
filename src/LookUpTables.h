#ifndef LOOPUPTABLES_H
#define LOOPUPTABLES_H

#include <bits/stdc++.h>
#include <cstdint>

#include "Pieces.h"
#include "Types.h"
#include "MagicMoves.h"

using namespace std;
#define U64 std::uint64_t

namespace LookUpTables{
    const static U64 all = 0xFFFFFFFFFFFFFFFF;
    const U64 maskRank[8] = {
        0x00000000000000FF,
        0x000000000000FF00,
        0x0000000000FF0000,
        0x00000000FF000000,
        0x000000FF00000000,
        0x0000FF0000000000,
        0x00FF000000000000,
        0xFF00000000000000
    };

    const U64 maskFile[8] = {
        0x0101010101010101,
        0x0202020202020202,
        0x0404040404040404,
        0x0808080808080808,
        0x1010101010101010,
        0x2020202020202020,
        0x4040404040404040,
        0x8080808080808080
    };

    const U64 clearRank[8] = {
        0xFFFFFFFFFFFFFF00,
        0xFFFFFFFFFFFF00FF,
        0xFFFFFFFFFF00FFFF,
        0xFFFFFFFF00FFFFFF,
        0xFFFFFF00FFFFFFFF,
        0xFFFF00FFFFFFFFFF,
        0xFF00FFFFFFFFFFFF,
        0x00FFFFFFFFFFFFFF
    };

    const U64 clearFile[8] = {
        0xFEFEFEFEFEFEFEFE,
        0xFDFDFDFDFDFDFDFD,
        0xFBFBFBFBFBFBFBFB,
        0xF7F7F7F7F7F7F7F7,
        0xEFEFEFEFEFEFEFEF,
        0xDFDFDFDFDFDFDFDF,
        0xBFBFBFBFBFBFBFBF,
        0x7F7F7F7F7F7F7F7F
    };

    const U64 rookInitialPos = {0x8100000000000081};

    const U64 neighbourFiles[]={0x202020202020202,
                                0x505050505050505,
                                0xa0a0a0a0a0a0a0a,
                                0x1414141414141414,
                                0x2828282828282828,
                                0x5050505050505050,
                                0xa0a0a0a0a0a0a0a0,
                                0x4040404040404040};

    // const vector<int> bishopShiftValues = {
    //     58, 59, 59, 59, 59, 59, 59, 58, 
    //     58, 59, 59, 59, 59, 59, 59, 58, 
    //     59, 59, 56, 56, 57, 57, 59, 59, 
    //     59, 59, 57, 53, 52, 56, 59, 59, 
    //     59, 59, 57, 53, 54, 56, 59, 59, 
    //     59, 59, 56, 57, 56, 57, 59, 59, 
    //     59, 59, 59, 59, 59, 59, 59, 59, 
    //     58, 59, 59, 59, 59, 58, 59, 58
    // };
    
    // const unsigned int bishopShiftValues[64] = {
    //         58, 59, 59, 59, 59, 59, 59, 58,
	// 		59, 59, 59, 59, 59, 59, 59, 59,
	// 		59, 59, 57, 57, 57, 57, 59, 59,
	// 		59, 59, 57, 55, 55, 57, 59, 59,
	// 		59, 59, 57, 55, 55, 57, 59, 59,
	// 		59, 59, 57, 57, 57, 57, 59, 59,
	// 		59, 59, 59, 59, 59, 59, 59, 59,
	// 		58, 59, 59, 59, 59, 59, 59, 58
    // };

    // const vector<U64> bishopMagicNumbers = {
    //     0x6ea0342c1802a7e3, 0x727a7c080a9f0042, 0xf0b013cfa731e6e7, 0x29020a0a03b537fa,
    //     0xa2f5104082a625d0, 0x6c3f037840951149, 0xd56bed53d006e4e0, 0xc86101011120460d,
    //     0x3fba84e4f0122df4, 0x8f75948f8211d914, 0x978ca113030601e1, 0xabd3181a0660d014,
    //     0xa22a2410c46c2fcd, 0x55e43e083219b91e, 0xd2969c0b1410358b, 0xaaf4b6f4125606bf,
    //     0x646d7860140fca61, 0x98f85b1ce7e80758, 0x9af90da6d1bb8f1d, 0x7bc13801bdbfd8f9,
    //     0xa1ac02af83a00bb9, 0xef2200be41100121, 0xc15e0b0e882f282c, 0x7412078342028c49,
    //     0xf690652c28381056, 0xce9838210e704709, 0x59fc060770068472, 0x172bc06a3eb9aff1,
    //     0xd3abe0fbb8a4a633, 0x47536304319994bd, 0x79751b0b65c81833, 0xc3fb6a0217808c01,
    //     0xafec8fad1a413409, 0x65bb0fa9c726e7df, 0xc40f80c311f01402, 0xc60fe36676491465,
    //     0x4e68593feff81ffc, 0x157dc09536de2239, 0xce9838210e704709, 0xf4970f0a26b30406,
    //     0xe7ec02208c36b184, 0x67d11c073c136022, 0x989655f010843649, 0xf3136c141401d801,
    //     0xb1f6c31d75d7a6e4, 0x99eb02098e015100, 0xf4e058022887d7c8, 0xfcbc02b2020065c7,
    //     0xd56bed53d006e4e0, 0x44492d7828285bd4, 0x3d233e0146580c45, 0xeaea663308c8008e,
    //     0xd81aacb9cf340038, 0xc1ad4c201c2c423d, 0x3171722a341c06bc, 0x727a7c080a9f0042,
    //     0xc86101011120460d, 0xaaf4b6f4125606bf, 0x71f2e4fe45cc1042, 0x817bffb25a420a13,
    //     0xdb0ca18530206605, 0x7c3242f91f87505f, 0x3fba84e4f0122df4, 0x6ea0342c1802a7e3
    // };

    // const U64 bishopMagicNumbers[64] = {
    //         0x0002020202020200, 0x0002020202020000, 0x0004010202000000, 0x0004040080000000,
	// 		0x0001104000000000, 0x0000821040000000, 0x0000410410400000, 0x0000104104104000,
	// 		0x0000040404040400, 0x0000020202020200, 0x0000040102020000, 0x0000040400800000,
	// 		0x0000011040000000, 0x0000008210400000, 0x0000004104104000, 0x0000002082082000,
	// 		0x0004000808080800, 0x0002000404040400, 0x0001000202020200, 0x0000800802004000,
	// 		0x0000800400A00000, 0x0000200100884000, 0x0000400082082000, 0x0000200041041000,
	// 		0x0002080010101000, 0x0001040008080800, 0x0000208004010400, 0x0000404004010200,
	// 		0x0000840000802000, 0x0000404002011000, 0x0000808001041000, 0x0000404000820800,
	// 		0x0001041000202000, 0x0000820800101000, 0x0000104400080800, 0x0000020080080080,
	// 		0x0000404040040100, 0x0000808100020100, 0x0001010100020800, 0x0000808080010400,
	// 		0x0000820820004000, 0x0000410410002000, 0x0000082088001000, 0x0000002011000800,
	// 		0x0000080100400400, 0x0001010101000200, 0x0002020202000400, 0x0001010101000200,
	// 		0x0000410410400000, 0x0000208208200000, 0x0000002084100000, 0x0000000020880000,
	// 		0x0000001002020000, 0x0000040408020000, 0x0004040404040000, 0x0002020202020000,
	// 		0x0000104104104000, 0x0000002082082000, 0x0000000020841000, 0x0000000000208800,
	// 		0x0000000010020200, 0x0000000404080200, 0x0000040404040400, 0x0002020202020200
    // };

    // const unsigned int rookShiftValues[64] = {
    //     50, 51, 51, 51, 51, 51, 52, 50,
    //     52, 53, 53, 53, 52, 53, 53, 52,
    //     52, 53, 53, 53, 53, 53, 53, 52,
    //     52, 53, 53, 53, 53, 53, 53, 52,
    //     52, 53, 53, 53, 52, 53, 53, 53,
    //     52, 51, 53, 53, 53, 53, 53, 52,
    //     52, 53, 53, 54, 53, 52, 53, 52,
    //     50, 51, 52, 52, 52, 52, 53, 52
    // };

    // const vector<int> rookShiftValues = {
    //         52, 53, 53, 53, 53, 53, 53, 52,
	// 		53, 54, 54, 54, 54, 54, 54, 53,
	// 		53, 54, 54, 54, 54, 54, 54, 53,
	// 		53, 54, 54, 54, 54, 54, 54, 53,
	// 		53, 54, 54, 54, 54, 54, 54, 53,
	// 		53, 54, 54, 54, 54, 54, 54, 53,
	// 		53, 54, 54, 54, 54, 54, 54, 53,
	// 		53, 54, 54, 53, 53, 53, 53, 53
    // };

    // const U64 rookMagicNumbers[64] = {
    //     0x1edb4e0126dbfb64, 0x4d3562feea5046e2, 0xe8e3c554bf880e9e, 0x84ce1e51167e5cfd,
    //     0xc7b7250c3f13d53a, 0x699e80bd5e4b10a3, 0xe8a01af0e67b2aea, 0x6d31fe520cff3b98,
    //     0x793e150a831f4dbf, 0xdc895dbbb6ac7f08, 0xf5373afee143a5b2, 0x4d0a7db1cf9f7864,
    //     0x10be767f9affcf37, 0x6aa57ff27f79f658, 0x71b04fc51b5af355, 0x80ed71cbb7160021,
    //     0x719b3e65cd19d145, 0x63d369fdaca6148f, 0x3e720acff50b1f94, 0xff2af28544f284a4,
    //     0xb9a13f12ba726f13, 0xb137f2d172d15cbb, 0xa25a05d7ff2bf417, 0xe32766d35ce4ac4b,
    //     0xf3efa0e5ffee38ac, 0x1e48c002c410351a, 0x667d6c2f3cfc1ddd, 0x9565ba2ba2b8505d,
    //     0xdc382453ac823c53, 0xcc871ff8dfc805e0, 0x4761b24960076699, 0x940f4f833941c820,
    //     0xeced0f9b72c91b98, 0x196e4f1f0c42057c, 0xcaa1414b7e453282, 0x4e5fe3a5019994cd,
    //     0x16f864f5fddb647f, 0xb11720396f1ffd28, 0xa8486c1fc5f02c21, 0x1632dc048600154f,
    //     0xf99d1bc30763f144, 0x152bd62d66c357b7, 0xb53a01c94a91fff7, 0x6f8e52abbb847bd4,
    //     0x2b5e47fd8d980115, 0xbdf369edc381edcf, 0x5281d9c078ffee0b, 0xa2c5bb3b0f606513,
    //     0x52d45fc17d4c7600, 0x8ce221dc001d6890, 0x4f1d678eca8b32d0, 0x841710210a014200,
    //     0x1d0f8d406a148fff, 0x6fe8d18d2e41df0e, 0xc49dee8b731610e1, 0x2efb1a2ca19c7c60,
    //     0xa5d4b28b56c4ae3e, 0xa5d4b28b56c4ae3e, 0x4cf705fef599b1d2, 0x6022b541ffc65166,
    //     0x850e11ffe3e4eb3a, 0x850e11ffe3e4eb3a, 0xd45e481623102184, 0x9a9674009746431e
    // };

    // const vector<U64> rookMagicNumbers = {
    //         0x0080001020400080, 0x0040001000200040, 0x0080081000200080, 0x0080040800100080,
	// 		0x0080020400080080, 0x0080010200040080, 0x0080008001000200, 0x0080002040800100,
	// 		0x0000800020400080, 0x0000400020005000, 0x0000801000200080, 0x0000800800100080,
	// 		0x0000800400080080, 0x0000800200040080, 0x0000800100020080, 0x0000800040800100,
	// 		0x0000208000400080, 0x0000404000201000, 0x0000808010002000, 0x0000808008001000,
	// 		0x0000808004000800, 0x0000808002000400, 0x0000010100020004, 0x0000020000408104,
	// 		0x0000208080004000, 0x0000200040005000, 0x0000100080200080, 0x0000080080100080,
	// 		0x0000040080080080, 0x0000020080040080, 0x0000010080800200, 0x0000800080004100,
	// 		0x0000204000800080, 0x0000200040401000, 0x0000100080802000, 0x0000080080801000,
	// 		0x0000040080800800, 0x0000020080800400, 0x0000020001010004, 0x0000800040800100,
	// 		0x0000204000808000, 0x0000200040008080, 0x0000100020008080, 0x0000080010008080,
	// 		0x0000040008008080, 0x0000020004008080, 0x0000010002008080, 0x0000004081020004,
	// 		0x0000204000800080, 0x0000200040008080, 0x0000100020008080, 0x0000080010008080,
	// 		0x0000040008008080, 0x0000020004008080, 0x0000800100020080, 0x0000800041000080,
	// 		0x00FFFCDDFCED714A, 0x007FFCDDFCED714A, 0x003FFFCDFFD88096, 0x0000040810002101,
	// 		0x0001000204080011, 0x0001000204000801, 0x0001000082000401, 0x0001FFFAABFAD1A2
    // };
    extern U64 frontSpans[2][SQUARE_NB];
    extern U64 pawnAttackSpans[2][SQUARE_NB];
    extern U64 passedPawnMask[2][SQUARE_NB];

    extern U64 attackTable[Piece::TOTAL_PIECES][Square::SQUARE_NB];
    extern U64 pawnAttackTable[Color::COLOR_NB][Square::SQUARE_NB];
    extern U64 lineBB[SQUARE_NB][SQUARE_NB];
    extern U64 inBetween[SQUARE_NB][SQUARE_NB];
    extern U64 squareBB[SQUARE_NB];
    extern Square SQUARE[FILE_NB][RANK_NB];

    // extern U64 bishopMovementMask[SQUARE_NB];
    // extern vector<vector<U64>> bishopLookupTableMagicNumbers;

    // extern U64 rookMovementMask[SQUARE_NB];
    // extern vector<vector<U64>> rookLookupTableMagicNumbers;

    void init();

    U64 frontBB(const Square pos,Color color);
    U64 sidesBB(const Square pos,Color color);

    // void fillBishopMovementMask();
    // void initializeBishopLookupTableMagicNumbers();

    // void fillRookMovementMask();
    // void initializeRookLookupTableMagicNumbers();

    U64 kingAttacks(const Square pos);
    U64 knightAttacks(const Square pos);
    U64 pawnAttacks(const Square pos, Color color);

    inline U64 bishopAttacks(const Square pos){return MagicMoves::Bmagic(pos, 0x0);};
    inline U64 rookAttacks(const Square pos){return MagicMoves::Rmagic(pos, 0x0);};
    inline U64 queenAttacks(const Square pos){return (bishopAttacks(pos) | rookAttacks(pos));};
    // U64 bishopAttacks(Square pos);
    // U64 rookAttacks(Square pos);
    // U64 queenAttacks(Square pos);

    //Generating Pseudo Legal Moves
    // U64 generateKingPseudoLegalMoves(Square square, U64 friendlyPieces);
    // U64 generateKnightPseudoLegalMoves(Square square, U64 friendlyPieces);
    // U64 generatePawnPseudoLegalMoves(Square square, U64 friendlyPieces, U64 enemyPieces, Color color);
    // U64 generateBishopPseudoLegalMoves(Square square, U64 friendlyPieces, U64 enemyPieces);
    // U64 generateRookPseudoLegalMoves(Square square, U64 friendlyPieces, U64 enemyPieces);
    // U64 generateQueenPseudoLegalMoves(Square square, U64 friendlyPieces, U64 enemyPieces);

    // // Helper Functions
    // vector<U64> createAllBlockerBitBoards(U64 movementMask);
    // U64 createBishopLegalMoves(Square square, U64 blockerBitBoard);
    // U64 createRookLegalMoves(Square square, U64 blockerBitBoard);

    // U64 bmagic(Square square, U64 occupancy);
    // U64 rmagic(Square square, U64 occupancy);
}

#endif