#include "Board.h"
#include "BitBoardsUtils.h"
#include "Evaluation.h"
#include "MagicMoves.h"

std::mt19937_64 ZK::rdGen;

Zkey ZK::psq[COLOR_NB][Piece::TOTAL_PIECES][SQUARE_NB];
Zkey ZK::enPassant[FILE_NB];
Zkey ZK::castling[4];
Zkey ZK::side;

void ZK::initZobristKeys(){
	rdGen.seed(1);

	for (Color c = WHITE; c <= BLACK; ++c){
		for (Piece::PieceType pt = Piece::PAWN; pt <= Piece::KING; ++pt){
			for (Square s = SQ_A1; s <= SQ_H8; ++s){
				psq[c][pt][s] = rdGen();
			}
		}
	}

	for (int i = 0; i<3; i++) castling[i] = rdGen();
	for (File f = FILE_A; f <= FILE_H; ++f) enPassant[f] = rdGen();

	side = rdGen();
}

Board::Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"){}

Board::Board(const string fen) : bitBoards(), allPieces(), pinnedPieces(), castling(), 
                                 myHasWhiteCastled(false), myHasBlackCastled(false), atkTo(), kingAttackers(){
    
    vector<string> spaceSplit;
    vector<string> piecesByRank;

    stringstream ss(fen);
    string item;
    while(getline(ss, item, ' ')){
        spaceSplit.push_back(item);
    }

    ss = stringstream(spaceSplit[0]);
    while(getline(ss, item, '/')){
        piecesByRank.push_back(item);
    }

    unsigned int rank = 7;
    for(unsigned int i = 0; i < 8; i++){
        setBitBoards(piecesByRank[i], rank);
        rank--;
    }

    updateConvenienceBitBoards();

    if(spaceSplit[1][0] == 'w') colorToPlay = WHITE;
    else if(spaceSplit[1][0] == 'b') colorToPlay = BLACK;

    Utils::getCastling(spaceSplit[2], castling);

    if((spaceSplit[3][0] != '-') && (getMovesHistory().size() == 0)){
        unsigned int epIndex = getIndexFromChar(spaceSplit[3]);
        Square origin = static_cast<Square>(epIndex + 8 - 16*colorToPlay);
        Square destination = static_cast<Square>(epIndex - 8 + 16*colorToPlay);
        Move lastMove(origin, destination, Move::DOUBLE_PAWN_PUSH_FLAG, Piece::PAWN);
        moves.push_back(lastMove);
    }

    if(spaceSplit[3][0] != '-'){
        unsigned int epIndex = getIndexFromChar(spaceSplit[3]);
        Square epSquare = static_cast<Square>(epIndex);
        epSqaures.push_back(epSquare);
    } else {
        epSqaures.push_back(SQ_NONE);
    }

    if(spaceSplit.size() >= 5){
        movesCounter = Utils::convertStringToInt(spaceSplit[4]);
    } else {
        movesCounter = 0;
    }

    if(spaceSplit.size() >= 6){
        halfMovesCounter = Utils::convertStringToInt(spaceSplit[5]);
    } else {
        halfMovesCounter = 0;
    }


}

Piece::PieceType Board::findBlackPieceType(const Square position) const {
    if(bitBoards[6] & (1LL << position)) return Piece::PAWN;
    else if(bitBoards[7] & (1LL << position)) return Piece::KNIGHT;
    else if(bitBoards[8] & (1LL << position)) return Piece::BISHOP;
    else if(bitBoards[9] & (1LL << position)) return Piece::ROOK;
    else if(bitBoards[10] & (1LL << position)) return Piece::QUEEN;
    else if(bitBoards[11] & (1LL << position)) return Piece::KING;
    else return Piece::NO_PIECE_TYPE;
}

Piece::PieceType Board::findWhitePieceType(const Square position) const {
    if(bitBoards[0] & (1LL << position)) return Piece::PAWN;
    else if(bitBoards[1] & (1LL << position)) return Piece::KNIGHT;
    else if(bitBoards[2] & (1LL << position)) return Piece::BISHOP;
    else if(bitBoards[3] & (1LL << position)) return Piece::ROOK;
    else if(bitBoards[4] & (1LL << position)) return Piece::QUEEN;
    else if(bitBoards[5] & (1LL << position)) return Piece::KING;
    else return Piece::NO_PIECE_TYPE;
}

Piece::PieceType Board::findPieceType(const Square position, const Color color) const{
    if(color == WHITE) return findWhitePieceType(position);
    else return findBlackPieceType(position);
}

Piece::Piece Board::findPieceType(const Square position) const {
    Piece::PieceType whiteType = findWhitePieceType(position);
    Piece::PieceType blackType = findBlackPieceType(position);

    return static_cast<Piece::Piece>(whiteType + blackType*(whiteType == 6));
}

bool Board::isMoveLegal(Move &move, bool isCheckb){

    bool isMoveLegal = true;
    bool isEnPassant = move.getFlags() == Move::EP_CAPTURE_FLAG;
    Color color = colorToPlay;

    Square origin = move.getOrigin();
    U64 oribb = 1ULL << origin;

    bool isPinned = oribb & getPinnedPieces();
    bool isKingMove = move.getPieceType() == Piece::KING;

    if(isPinned){
        Square destination = move.getDestination();
        Square ksq = getKingSquare(colorToPlay);
		bool isAreAligned = areAligned(origin, destination, ksq);
        return isAreAligned;
    } else if (isKingMove){
		// cout << "Move : " << move.toShortString() << " ,";
        return !isSquareAttacked(move.getDestination(), colorToPlay);
    } if(isCheckb || isEnPassant){
        executerMove(move);
        updateKingAttackers(color);
        if(isCheck()) isMoveLegal = false;
        undoMove(move);
    }
    return isMoveLegal;
}

void Board::updateKingAttackers(const Color color){
    U64 kingPosition = getKing(color);

    Square kingSquare = msb(kingPosition);

    kingAttackers = getAttackersTo(kingSquare, color);
}

bool Board::isSquareAttacked(Square square, Color color) const {
	// cout << "Square << " << square << endl;
    Color enemyColor = Utils::getOppositeColor(color);

    if(LookUpTables::pawnAttackTable[color][square] & getPawns(enemyColor)) return true;
    else if(LookUpTables::attackTable[Piece::KNIGHT][square] & getKnights(enemyColor)) return true;
    else if(LookUpTables::attackTable[Piece::KING][square] & getKing(enemyColor)) return true;

    //U64 validBishopMoves = LookUpTables::generateBishopPseudoLegalMoves(square, getPieces(color), getPieces(enemyColor));
	U64 validBishopMoves = MagicMoves::Bmagic(square, getAllPieces());
    if(validBishopMoves & (getBishops(enemyColor) | getQueens(enemyColor))) return true;
	// cout << "Bishop cannot reach here\n";


    //U64 validRookMoves = LookUpTables::generateRookPseudoLegalMoves(square, getPieces(color), getPieces(enemyColor));
	U64 validRookMoves = MagicMoves::Rmagic(square, getAllPieces());
    if(validRookMoves & (getRooks(enemyColor) | getQueens(enemyColor))) return true;
	// cout << "Rook cannot reach here\n";

    return false;
}

U64 Board::getAttackersTo(Square sq, Color color) const {
    U64 atkTo = 0;

    Color enemyColor = Utils::getOppositeColor(color);
    atkTo |= (LookUpTables::pawnAttackTable[color][sq] & getPawns(enemyColor));
    atkTo |= (LookUpTables::attackTable[Piece::KNIGHT][sq] & getKnights(enemyColor));
    atkTo |= (LookUpTables::attackTable[Piece::KING][sq] & getKing(enemyColor));

    // U64 potentialAttackers = LookUpTables::generateBishopPseudoLegalMoves(sq, getPieces(color), getPieces(enemyColor));
	U64 potentialAttackers = MagicMoves::Bmagic(sq, getAllPieces()) & ~getPieces(color);
    atkTo |= (potentialAttackers & (getBishops(enemyColor) | getQueens(enemyColor)));

    // potentialAttackers = LookUpTables::generateRookPseudoLegalMoves(sq, getPieces(color), getPieces(enemyColor));
    potentialAttackers = MagicMoves::Rmagic(sq, getAllPieces()) & ~getPieces(color);
	atkTo |= (potentialAttackers & (getRooks(enemyColor) | getQueens(enemyColor)));


    return atkTo;
}

U64 Board::getAttackersTo(Square sq, Color color, U64 occupancy) const {
    U64 atkTo = 0;

    Color enemyColor = Utils::getOppositeColor(color);
    atkTo |= (LookUpTables::pawnAttackTable[color][sq] & getPawns(enemyColor));
    atkTo |= (LookUpTables::attackTable[Piece::KNIGHT][sq] & getKnights(enemyColor));
    atkTo |= (LookUpTables::attackTable[Piece::KING][sq] & getKing(enemyColor));

    U64 potentialAttacks = MagicMoves::Bmagic(sq, occupancy) & ~getPieces(color);
    atkTo |= (potentialAttacks& (getBishops(enemyColor) | getQueens(enemyColor)));

    potentialAttacks = MagicMoves::Rmagic(sq, occupancy) & ~getPieces(color);
    atkTo |= (potentialAttacks& (getRooks(enemyColor) | getQueens(enemyColor)));

    return atkTo;
} 

void Board::executerMove(Move &move){
	// cout << "I was here" << endl;
    Square origin = move.getOrigin();
	Square destination = move.getDestination();
	unsigned int pieceType = move.getPieceType();
	Color oppositeColor = Utils::getOppositeColor(colorToPlay);

	if(move.isQuiet()){
		// cout << "Quiet Move" << endl;
		movePiece(origin, destination, pieceType, colorToPlay);
	} else{
		if(move.isCastling()){
			movePiece(origin, destination, pieceType, colorToPlay);

			Square rookOrigin = SQ_NONE;
			Square rookDestination = SQ_NONE;

			if(move.isKingSideCastling()){
				if(colorToPlay == WHITE){
					// cout << "White king side catling move" << endl;
					myHasWhiteCastled = true;
					rookOrigin = SQ_H1;
					rookDestination = SQ_F1;
				}
				else{
					// cout << "Black king side castling move" << endl;
					myHasBlackCastled = true;
					rookOrigin = SQ_H8;
					rookDestination = SQ_F8;
				}
			}
			else{
				if(colorToPlay == WHITE){
					// cout << "White queen side catling move" << endl;
					myHasWhiteCastled = true;
					rookOrigin = SQ_A1;
					rookDestination = SQ_D1;
				}
				else{
					// cout << "Black queen side catling move" << endl;
					myHasBlackCastled = true;
					rookOrigin = SQ_A8;
					rookDestination = SQ_D8;
				}
			}
			movePiece(rookOrigin, rookDestination, Piece::ROOK, colorToPlay);
		} else if (move.isPromotion()){
			unsigned int promotedType = move.getPromotedPieceType();

			if(move.isCapture()){
				unsigned int capturedPieceType = move.getCapturedPieceType();
				removePiece(destination, capturedPieceType, oppositeColor);
				// cout << "Promotion with capture. Captured Piece : " << capturedPieceType << endl;
			}

			removePiece(origin, Piece::PAWN, colorToPlay);
			addPiece(destination, promotedType, colorToPlay);
		} else {
			if (move.isEnPassant()) {
				Square capturedPawnIndex = static_cast<Square>(move.getDestination() - 8 + 16*colorToPlay);
				removePiece(capturedPawnIndex, Piece::PAWN, oppositeColor);

				// cout << "Enpassant Move" << endl;
			}
			else {
				unsigned int type(move.getCapturedPieceType());
				removePiece(destination, type, oppositeColor);

				// cout << "Normal Capture Move : " << move.toShortString() << ", Piece Moved : " << pieceType << ", Captured Piece : " << type << endl;
			}
			movePiece(origin, destination, pieceType, colorToPlay);
		}
	}
    Square lastEpSquare = getLastEpSquare();

	if(lastEpSquare != SQ_NONE) key ^= ZK::enPassant[Utils::getFile(lastEpSquare)];

    if (move.isDoublePawnPush()){
        epSqaures.push_back(static_cast<Square>(destination-8+16*colorToPlay));
		key ^= ZK::enPassant[Utils::getFile(destination)];
    } else {
        epSqaures.push_back(SQ_NONE) ;
    }
	updateCastlingRights(move);

	movesCounter += colorToPlay;
	halfMovesCounter++;

	colorToPlay = oppositeColor;

	key ^= ZK::side;
	pawnsKey ^= ZK::side;

	moves.push_back(move);
	myKeys.push_back(key);

    updateConvenienceBitBoards();
}

void Board::undoMove(Move &move){
    Square origin = move.getOrigin();
    Square destination = move.getDestination();
    unsigned int pieceType = move.getPieceType();
    Color oppositeColor = Utils::getOppositeColor(colorToPlay);

    rewindCastlingRights(move);

	if(move.isQuiet()){
		movePiece(destination, origin, pieceType, oppositeColor);
	} else{
		if(move.isCastling()){
			movePiece(destination, origin, pieceType, oppositeColor);

			Square rookOrigin = SQ_NONE;
			Square rookDestination = SQ_NONE;

			if(move.isKingSideCastling()){
				if(oppositeColor == WHITE){
					myHasWhiteCastled = false;
					rookOrigin = SQ_H1;
					rookDestination = SQ_F1;
				} else {
					myHasBlackCastled = false;
					rookOrigin = SQ_H8;
					rookDestination = SQ_F8;
				}
			}
			else{
				if(oppositeColor == WHITE){
					myHasWhiteCastled = false;
					rookOrigin = SQ_A1;
					rookDestination = SQ_D1;
				} else {
					myHasBlackCastled = false;
					rookOrigin = SQ_A8;
					rookDestination = SQ_D8;
				}
			}
			movePiece(rookDestination, rookOrigin, Piece::ROOK, oppositeColor);
		} else if(move.isPromotion()) {
			unsigned int promotedType = move.getFlags() - Move::PROMOTION_FLAG +1;

			if(move.isCapture()) {
				promotedType -= Move::CAPTURE_FLAG;
				unsigned int type(move.getCapturedPieceType());
				addPiece(destination, type, colorToPlay);
			}

			removePiece(destination, promotedType, oppositeColor);
			addPiece(origin, Piece::PAWN, oppositeColor);
		} else {
			movePiece(destination, origin, pieceType, oppositeColor);

			if (move.isEnPassant()){
				Square capturedPawnIndex = static_cast<Square>(move.getDestination() - 8 + 16*oppositeColor);
				addPiece(capturedPawnIndex, Piece::PAWN, colorToPlay);
			}
			else {
				unsigned int type(move.getCapturedPieceType());
				addPiece(destination, type, colorToPlay);
			}
		}
	}

	if(move.isDoublePawnPush()) key ^= ZK::enPassant[Utils::getFile(destination)];
    epSqaures.pop_back();

	Square lastEpSquare = getLastEpSquare();
    if (lastEpSquare != SQ_NONE) key ^= ZK::enPassant[Utils::getFile(lastEpSquare)];

	movesCounter += colorToPlay - 1;
	halfMovesCounter--;

	colorToPlay = Utils::getOppositeColor(colorToPlay);

	key ^= ZK::side;
	pawnsKey ^= ZK::side;

	moves.pop_back();
	myKeys.pop_back();

	updateConvenienceBitBoards();
}

void Board::executeNullMove(){
	Color oppositeColor = Utils::getOppositeColor(colorToPlay);
	movesCounter += colorToPlay;
	halfMovesCounter++;
	colorToPlay = oppositeColor;
	key ^= ZK::side;
	pawnsKey ^= ZK::side;
	moves.push_back(Move());
	myKeys.push_back(key);
}

void Board::undoNullMove(){
	Color oppositeColor = Utils::getOppositeColor(colorToPlay);
	movesCounter += colorToPlay - 1;
	halfMovesCounter--;
	colorToPlay = oppositeColor;
	key ^= ZK::side;
	pawnsKey ^= ZK::side;
	moves.pop_back();
	myKeys.pop_back();
}

void Board::updateConvenienceBitBoards(){
	bitBoards[12] = bitBoards[0] ^ bitBoards[1] ^ bitBoards[2] ^ bitBoards[3] ^ bitBoards[4] ^ bitBoards[5];
	bitBoards[13] = bitBoards[6] ^ bitBoards[7] ^ bitBoards[8] ^ bitBoards[9] ^ bitBoards[10] ^ bitBoards[11];
	bitBoards[14] = bitBoards[0] ^ bitBoards[6];
	bitBoards[15] = bitBoards[1] ^ bitBoards[7];
	bitBoards[16] = bitBoards[2] ^ bitBoards[8];
	bitBoards[17] = bitBoards[3] ^ bitBoards[9];
	bitBoards[18] = bitBoards[4] ^ bitBoards[10];
	bitBoards[19] = bitBoards[5] ^ bitBoards[11];
	allPieces = bitBoards[12] ^ bitBoards[13];
}

char Board::getChar(const unsigned int file, const unsigned int rank) const{
    char c;

	if (BitBoardsUtils::isBitSet(getWhitePawns(), file, rank)) c = 'P';
	else if (BitBoardsUtils::isBitSet(getWhiteKnights(), file, rank)) c = 'N';
	else if (BitBoardsUtils::isBitSet(getWhiteBishops(), file, rank)) c = 'B';
	else if (BitBoardsUtils::isBitSet(getWhiteRooks(), file, rank)) c = 'R';
	else if (BitBoardsUtils::isBitSet(getWhiteQueens(), file, rank)) c = 'Q';
	else if (BitBoardsUtils::isBitSet(getWhiteKing(), file, rank)) c = 'K';
	else if (BitBoardsUtils::isBitSet(getBlackPawns(), file, rank)) c = 'p';
	else if (BitBoardsUtils::isBitSet(getBlackKnights(), file, rank)) c = 'n';
	else if (BitBoardsUtils::isBitSet(getBlackBishops(), file, rank)) c = 'b';
	else if (BitBoardsUtils::isBitSet(getBlackRooks(), file, rank)) c = 'r';
	else if (BitBoardsUtils::isBitSet(getBlackQueens(), file, rank)) c = 'q';
	else if (BitBoardsUtils::isBitSet(getBlackKing(), file, rank)) c = 'k';
	else c = '*';

	return c;
}

string Board::getUnicode(const unsigned int file, const unsigned int rank) const {
    const string blackPawn = "♙";
    const string blackKnight = "♘";
    const string blackBishop = "♗";
    const string blackRook = "♖";
    const string blackQueen = "♕";
    const string blackKing = "♔";
    const string whitePawn = "♟";
    const string whiteKnight = "♞";
    const string whiteBishop = "♝";
    const string whiteRook = "♜";
    const string whiteQueen = "♛";
    const string whiteKing = "♚";

    if (BitBoardsUtils::isBitSet(getWhitePawns(), file, rank)) return whitePawn;
    else if (BitBoardsUtils::isBitSet(getWhiteKnights(), file, rank)) return whiteKnight;
    else if (BitBoardsUtils::isBitSet(getWhiteBishops(), file, rank)) return whiteBishop;
    else if (BitBoardsUtils::isBitSet(getWhiteRooks(), file, rank)) return whiteRook;
    else if (BitBoardsUtils::isBitSet(getWhiteQueens(), file, rank)) return whiteQueen;
    else if (BitBoardsUtils::isBitSet(getWhiteKing(), file, rank)) return whiteKing;
    else if (BitBoardsUtils::isBitSet(getBlackPawns(), file, rank)) return blackPawn;
    else if (BitBoardsUtils::isBitSet(getBlackKnights(), file, rank)) return blackKnight;
    else if (BitBoardsUtils::isBitSet(getBlackBishops(), file, rank)) return blackBishop;
    else if (BitBoardsUtils::isBitSet(getBlackRooks(), file, rank)) return blackRook;
    else if (BitBoardsUtils::isBitSet(getBlackQueens(), file, rank)) return blackQueen;
    else if (BitBoardsUtils::isBitSet(getBlackKing(), file, rank)) return blackKing;

    return "*";
}

void Board::setBitBoards(const std::string piecesString, const unsigned int rank){
	unsigned int x = -1;
	for (const char& pieceChar : piecesString) {
		if(isdigit(pieceChar)) x += pieceChar - '0';
		else{
			x++;
			if (pieceChar == 'K') bitBoards[5] |= 1LL << (8*rank + x);
            else if (pieceChar == 'R') bitBoards[3] |= 1LL << (8*rank + x);
			else if (pieceChar == 'B') bitBoards[2] |= 1LL << (8*rank + x);
            else if (pieceChar == 'Q') bitBoards[4] |= 1LL << (8*rank + x);
			else if (pieceChar == 'N') bitBoards[1] |= 1LL << (8*rank + x);
			else if (pieceChar == 'P') bitBoards[0] |= 1LL << (8*rank + x);
			else if (pieceChar == 'k') bitBoards[11] |= 1LL << (8*rank + x);
			else if (pieceChar == 'r') bitBoards[9] |= 1LL << (8*rank + x);
			else if (pieceChar == 'b') bitBoards[8] |= 1LL << (8*rank + x);
			else if (pieceChar == 'q') bitBoards[10] |= 1LL << (8*rank + x);
			else if (pieceChar == 'n') bitBoards[7] |= 1LL << (8*rank + x);
			else if (pieceChar == 'p') bitBoards[6]|= 1LL << (8*rank + x);
		}
	}
}

const Move* Board::getEnemyLastMove() const {
    if(moves.size() > 0){
        const Move* lastMovePtr = &moves.back();
        return lastMovePtr;
    } else return nullptr;
}

void Board::updateCastlingRights(Move &move){
    move.setPreviousCastlingRights(castling);

	unsigned int isKingMove(move.getPieceType() == Piece::KING);
	castling &= ~((isKingMove*3) << (colorToPlay*2));

	Square origin = move.getOrigin();


	if (((1LL << origin)&LookUpTables::rookInitialPos)!=0){
		unsigned int shift(((~origin)&0b0001) + 2*((origin&0b1000)>>3));
		unsigned int mask = ~(0b0001 << shift);
		castling &= mask;
	}

	Square destination = move.getDestination();
	if (((1LL << destination)&LookUpTables::rookInitialPos)!=0){
		unsigned int shift(((~destination)&0b0001) + 2*((destination&0b1000)>>3));
		unsigned int mask = ~(0b0001 << shift);
		castling &= mask;
	}

    unsigned int hasChanged = (castling^move.getPreviousCastlingRights()) & 0b1111;

	if(hasChanged & 0b0001) key^=(ZK::castling[0]);
	if(hasChanged & 0b0010) key^=(ZK::castling[1]);
    if(hasChanged & 0b0100) key^=(ZK::castling[2]);
    if(hasChanged & 0b1000) key^=(ZK::castling[3]);
}

void Board::rewindCastlingRights(const Move &move){
	unsigned int hasChanged = (castling^move.getPreviousCastlingRights()) & 0b1111;

    if (hasChanged & 0b0001) key^=(ZK::castling[0]);
    if (hasChanged & 0b0010) key^=(ZK::castling[1]);
    if (hasChanged & 0b0100) key^=(ZK::castling[2]);
    if (hasChanged & 0b1000) key^=(ZK::castling[3]);

    castling = move.getPreviousCastlingRights();
}

void Board::updatePinnedPieces(){
    pinnedPieces = 0ULL;
	Color color = getColorToPlay();
	Color oppositeColor = Utils::getOppositeColor(color);
	U64 occ = getAllPieces();
    Square kingSquare = getKingSquare(color);

	U64 rookWise = MagicMoves::Rmagic(kingSquare, occ);
	U64 potPinned = rookWise & getPieces(color);
	U64 xrays = rookWise ^ MagicMoves::Rmagic(kingSquare, occ ^ potPinned);
	U64 possiblePinners = getRooks(oppositeColor) | getQueens(oppositeColor);
	U64 pinners = xrays & possiblePinners;

	while ( pinners ){
		unsigned int pinnerSq = pop_lsb(&pinners);
		pinnedPieces  |= potPinned & LookUpTables::inBetween[pinnerSq][kingSquare];
	}

	U64 bishopWise = MagicMoves::Bmagic(kingSquare, occ);
	potPinned = bishopWise & getPieces(color);
	xrays = bishopWise ^ MagicMoves::Bmagic(kingSquare, occ ^ potPinned);
	//xrays = rookWise ^ bmagic(kingSquare, occ ^ potPinned);
	possiblePinners = getBishops(oppositeColor) | getQueens(oppositeColor);
	pinners = xrays & possiblePinners;

	while ( pinners ){
		unsigned int pinnerSq = pop_lsb(&pinners);
		pinnedPieces  |= potPinned & LookUpTables::inBetween[pinnerSq][kingSquare];
	}
}

int Board::see(const Square square, Color color){
	int score = 0;
	U64 attackers;
	Piece::PieceType pieceType = getSmallestAttacker(square, color, attackers);

	if (pieceType != Piece::PieceType::NO_PIECE_TYPE){
		Square origin = msb(attackers);
		Move captureMove = Move(origin, square, Move::CAPTURE_FLAG, pieceType);
		Piece::PieceType capturedType = findPieceType(square, color);

		captureMove.setCapturedPieceType(capturedType);

		executerMove(captureMove);

		score = Evaluation::pieceTypeToValue(captureMove.getCapturedPieceType()) - see(square, Utils::getOppositeColor(color));
		score = std::max(0, score);

		undoMove(captureMove);
	}
	return score;
}

int Board::seeCapture(Move captureMove, Color color){
	executerMove(captureMove);

	int score = Evaluation::pieceTypeToValue(captureMove.getCapturedPieceType()) - see(captureMove.getDestination(), Utils::getOppositeColor(color));
	undoMove(captureMove);

   return score;
}

int Board::seeCapture2(Move captureMove, Color color){
	Square toSq = captureMove.getDestination();
	int gain[32], d = 0;
	U64 mayXray =  getAllPawns() | getAllKnights() | getAllRooks() | getAllQueens();
	U64 fromSet = 1ULL << captureMove.getOrigin();
	U64 occ     = getAllPieces();
	U64 attadef = getAttackersTo(toSq, color, occ) | getAttackersTo(toSq, Utils::getOppositeColor(color), occ); 
	gain[d]     = Evaluation::pieceTypeToValue(captureMove.getCapturedPieceType());
	
	do {
		d++;
		gain[d]  = Evaluation::pieceTypeToValue(captureMove.getPieceType()) - gain[d-1];
		if (std::max(-gain[d-1], gain[d]) < 0) break;
		attadef ^= fromSet;
		occ     ^= fromSet;
		
		if (fromSet & mayXray){
			attadef = getAttackersTo(toSq, color, occ) | getAttackersTo(toSq, Utils::getOppositeColor(color), occ); 
		}

		U64 attackers = attadef;
		color = Utils::getOppositeColor(color);

		getSmallestAttacker(toSq, Utils::getOppositeColor(color), attackers);

		fromSet = attackers ? 1ULL << attackers : 0;
		std::cout << d << std::endl;

	} while (fromSet);

	while (--d){
	  gain[d-1]= -std::max (-gain[d-1], gain[d]);
	}

	return gain[0];
}

Piece::PieceType Board::getSmallestAttacker(const Square square, Color color, U64 &attackers){
	attackers = 0;
	Color enemyColor = Utils::getOppositeColor(color);

	attackers = LookUpTables::pawnAttackTable[color][square] & getPawns(enemyColor);
	if (attackers) return Piece::PieceType::PAWN;

	attackers = LookUpTables::attackTable[Piece::KNIGHT][square] & getKnights(enemyColor);
	if (attackers) return Piece::PieceType::KNIGHT;

	U64 potentialBishopAttackers = MagicMoves::Bmagic(square, getAllPieces());
	attackers = potentialBishopAttackers & getBishops(enemyColor);
	if (attackers) return Piece::PieceType::BISHOP;

	U64 potentialRookAttackers = MagicMoves::Rmagic(square, getAllPieces());
	attackers = potentialRookAttackers & getRooks(enemyColor);
	if (attackers) return Piece::PieceType::ROOK;

	attackers = potentialRookAttackers & getQueens(enemyColor);
	if (attackers) return Piece::PieceType::QUEEN;

	attackers = potentialBishopAttackers & getQueens(enemyColor);
	if (attackers) return Piece::PieceType::QUEEN;

	attackers = LookUpTables::attackTable[Piece::KING][square] & getKing(enemyColor);
	if (attackers) return Piece::PieceType::KING;

	return Piece::PieceType::NO_PIECE_TYPE;
}