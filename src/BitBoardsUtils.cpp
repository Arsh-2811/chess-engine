#include <bits/stdc++.h>
#include "BitBoardsUtils.h"

using namespace std;

#define U64 std::uint64_t

void BitBoardsUtils::printBitBoard(U64 bitBoard){
    for (int rank = 7; rank >= 0; --rank){
        for (int file = 0; file < 8; ++file){
            int square = rank * 8 + file;
            int bit = (bitBoard >> square) & 1U;
            cout << bit << " ";
        }
        cout << endl;
    }
    cout << endl;
}