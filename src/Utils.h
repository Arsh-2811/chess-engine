#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>

#include "Types.h"

using namespace std;

class Board;

namespace Utils{
    void getCastling(string const& castleString, unsigned int &castlingRights);
    unsigned int convertStringToInt(string const &fenMoveCounter);
    
    inline constexpr Color getOppositeColor(const Color color){return Color(color ^ 1);};
    inline unsigned int getFile(const Square positionIndex){return positionIndex%8;};
    string Move16ToShortString(const Move16 shortMove);
};

#endif