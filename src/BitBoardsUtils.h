#ifndef BITBOARDSUTILS_H
#define BITBOARDSUTILS_H

#include <cstdint>
#include <iostream>

#include "Types.h"
#include "LookUpTables.h"

#define U64 std::uint64_t

#ifdef _MSC_VER
    #include <nmmintrin.h>
    #define __builtin_popcountll _mm_popcnt_u64
#endif

namespace BitBoardsUtils{
    void printBitBoard(std::uint64_t bitBoard);

    inline bool isBitSet(const U64 bitBoard, const unsigned int x, const unsigned int y){
	    unsigned int shift = 8*y + x;
	    return (0 | 1LL << shift) & bitBoard;
    }
};

inline unsigned int popcount(U64 bitboard){
    #ifdef NO_POPCNT
        return popcount64sparse(U64 bitboard);
    #else
	      return __builtin_popcountll(bitboard);
	  #endif
}

inline Square msb(const U64 bitBoard) {
    return static_cast<Square>(__builtin_ctzll(bitBoard));
    // if (bitBoard == 0) return SQ_NONE;  // Return SQ_NONE if no bit is set
    // return static_cast<Square>(63 - __builtin_clzll(bitBoard));  // MSB position
}

inline Square lsb(U64 bitboard) {
    if (bitboard == 0) return SQ_NONE;  // Return SQ_NONE if no bit is set
    return static_cast<Square>(__builtin_ctzll(bitboard));  // LSB position
}

// inline Square msb(const U64 bitBoard){
//     #ifdef _MSC_VER
//         unsigned long idx;
//         _BitScanReverse64(&idx, bitBoard);
//         return (Square) idx;
//     #else
//         U64 idx;
//          __asm__("bsrq %1, %0": "=r"(idx) : "rm"(bitBoard));
//     #endif
//         return (Square) idx;
// }

// inline Square lsb(U64 bitboard) { // Assembly code by Heinz van Saanen
// #ifdef _MSC_VER
// 	unsigned long idx;
// 	_BitScanForward64(&idx, bitboard);
// 	return (Square)idx;

// #else
// 	U64 idx;
// 	__asm__("bsfq %1, %0": "=r"(idx): "rm"(bitboard) );
// 	return (Square)idx;

// #endif
// }

inline Square pop_lsb(U64* b) {
	const Square s = lsb(*b);
	*b &= *b - 1;
	return s;
}

inline bool moreThanOne(U64 bitboard){
	return bitboard & (bitboard-1);
}

inline bool areAligned(Square s1, Square s2, Square s3){
    return LookUpTables::lineBB[s1][s2] & LookUpTables::squareBB[s3];
}

//Operator overload for Bitboard and Square

inline U64 operator&(U64 b, Square s) {
	return b & LookUpTables::squareBB[s];
}

inline U64 operator|(U64 b, Square s) {
  return b | LookUpTables::squareBB[s];
}

inline U64 operator^(U64 b, Square s) {
  return b ^ LookUpTables::squareBB[s];
}

inline U64& operator&=(U64& b, Square s) {
  return b &= LookUpTables::squareBB[s];
}

inline U64& operator|=(U64& b, Square s) {
  return b |= LookUpTables::squareBB[s];
}

inline U64& operator^=(U64& b, Square s) {
  return b ^= LookUpTables::squareBB[s];
}

#endif