#include "MoveGen.h"
#include "BitBoardsUtils.h"
#include "MagicMoves.h"

MoveGen::MoveGen(Board board) : myBoard(make_shared<Board> (board)) {}
MoveGen::MoveGen(shared_ptr<Board> boardPtr) { myBoard = boardPtr; }

void MoveGen::addQuietMoves(U64 quietDestinations, Square pieceIndex, vector<Move>& moves, Piece::PieceType pieceType) const {
    while(quietDestinations){
        Square positionMsb = pop_lsb(&quietDestinations);
        Move move = Move(pieceIndex, positionMsb, 0, pieceType);
        moves.push_back(move);
    }
}

void MoveGen::addDoublePawnPushMoves(U64 pawnDestinations, Square pieceIndex, std::vector<Move>& moves) const{
	while (pawnDestinations){
		Square positionMsb = pop_lsb(&pawnDestinations);
		Move move = Move(pieceIndex, positionMsb, Move::DOUBLE_PAWN_PUSH_FLAG, Piece::PAWN);
		moves.push_back(move);
	}
}

void MoveGen::addCaptureMoves(U64 captureDestinations, Square pieceIndex, std::vector<Move>& moves, Piece::PieceType pieceType) const{
	while (captureDestinations){
		Square positionMsb = pop_lsb(&captureDestinations);
		Move move = Move(pieceIndex, positionMsb, Move::CAPTURE_FLAG, pieceType);
		Piece::PieceType capturedType(myBoard->findPieceType(positionMsb,Utils::getOppositeColor(myBoard->getColorToPlay())));
		move.setCapturedPieceType(capturedType);
		moves.push_back(move);

	}
}

void MoveGen::addPromotionMoves(U64 promotionDestinations, Square pieceIndex, std::vector<Move>& moves) const{
	while (promotionDestinations){
		Square positionMsb = pop_lsb(&promotionDestinations);
		Move move = Move(pieceIndex, positionMsb, Move::PROMOTION_FLAG, Piece::PAWN);
		moves.push_back(move);
		move.setFlags(Move::PROMOTION_FLAG+1);
		moves.push_back(move);
		move.setFlags(Move::PROMOTION_FLAG+2);
		moves.push_back(move);
		move.setFlags(Move::PROMOTION_FLAG+3);
		moves.push_back(move);
	}
}

void MoveGen::addPromotionCaptureMoves(U64 promotionDestinations, Square pieceIndex, std::vector<Move>& moves) const{
	while (promotionDestinations){
		Square positionMsb = pop_lsb(&promotionDestinations);
		unsigned int flag = Move::PROMOTION_FLAG+Move::CAPTURE_FLAG;
		Move move = Move(pieceIndex, positionMsb, flag, Piece::PAWN);
		Piece::PieceType capturedType(myBoard->findPieceType(positionMsb, Utils::getOppositeColor(myBoard->getColorToPlay())));
		move.setCapturedPieceType(capturedType);

		moves.push_back(move);
		move.setFlags(flag+1);
		moves.push_back(move);
		move.setFlags(flag+2);
		moves.push_back(move);
		move.setFlags(flag+3);
		moves.push_back(move);
	}
}

void MoveGen::appendKingPseudoLegalMoves(const Color color, std::vector<Move> &moves, U64 target) const {
	U64 kingPos = myBoard->getKing(color);
	Square kingIndex = msb(kingPos);
	U64 kingValidDestinations = myBoard->getKingAttacks(kingIndex, color);

	Color ennemyColor = Utils::getOppositeColor(color);
	U64 kingCaptureDestinations = kingValidDestinations & myBoard->getPieces(ennemyColor);
	U64 kingQuietDestinations = kingValidDestinations ^ kingCaptureDestinations;

	addQuietMoves(kingQuietDestinations, kingIndex, moves, Piece::KING);
	addCaptureMoves(kingCaptureDestinations, kingIndex, moves, Piece::KING);

	addKingSideCastlingMove(color, kingIndex, moves);
	addQueenSideCastlingMove(color, kingIndex, moves);
}

void MoveGen::addKingSideCastlingMove(Color color, Square kingIndex, vector<Move> &moves) const{
	if(isKingSideCastlingPossible(color)){
		Square destination = static_cast<Square>(color == WHITE ? 6 : 62);
		Move move = Move(kingIndex, destination, Move::KING_SIDE_CASTLING, Piece::KING);
		moves.push_back(move);
	}
}

void MoveGen::addQueenSideCastlingMove(Color color, Square kingIndex, vector<Move> &moves) const{
	if(isQueenSideCastlingPossible(color)){
		Square destination = static_cast<Square>(color == WHITE ? 2 : 58);
		Move move = Move(kingIndex, destination, Move::QUEEN_SIDE_CASTLING, Piece::KING);
		moves.push_back(move);
	}
}

void MoveGen::appendQueenPseudoLegalMoves(const Color color, std::vector<Move>& moves, U64 target) const{
	U64 queenPositions = myBoard->getQueens(color);

	while(queenPositions){
		Square queenIndex = pop_lsb(&queenPositions);

		Color ennemyColor = Utils::getOppositeColor(color);

		U64 bishopDestinations = MagicMoves::Bmagic(queenIndex, myBoard->getAllPieces()) & ~myBoard->getPieces(color);
		U64 rookDestinations = MagicMoves::Rmagic(queenIndex, myBoard->getAllPieces()) & ~myBoard->getPieces(color);
		U64 queenDestinations = (bishopDestinations ^ rookDestinations) & target ;

		U64 queenCaptureDestinations = queenDestinations & myBoard->getPieces(ennemyColor);
		U64 queenQuietDestinations = queenDestinations ^ queenCaptureDestinations;

		addQuietMoves(queenQuietDestinations, queenIndex, moves, Piece::QUEEN);
		addCaptureMoves(queenCaptureDestinations, queenIndex, moves, Piece::QUEEN);
	}
}

void MoveGen::appendBishopPseudoLegalMoves(const Color color, std::vector<Move>& moves, U64 target) const{
	U64 bishopPositions = myBoard->getBishops(color);

	//loop through the bishops:
	while(bishopPositions){
		Square bishopIndex = pop_lsb(&bishopPositions);

		Color ennemyColor = Utils::getOppositeColor(color);

		U64 bishopDestinations = MagicMoves::Bmagic(bishopIndex, myBoard->getAllPieces()) & ~myBoard->getPieces(color) & target;

		U64 bishopCaptureDestinations = bishopDestinations & myBoard->getPieces(ennemyColor);
		U64 bishopQuietDestinations = bishopDestinations ^ bishopCaptureDestinations;

		addQuietMoves(bishopQuietDestinations, bishopIndex, moves, Piece::BISHOP);
		addCaptureMoves(bishopCaptureDestinations, bishopIndex, moves, Piece::BISHOP);
	}
}

void MoveGen::appendRookPseudoLegalMoves(const Color color, std::vector<Move>& moves, U64 target) const{
	U64 rookPositions = myBoard->getRooks(color);

	//loop through the rooks:
	while(rookPositions){
		Square rookIndex = pop_lsb(&rookPositions);

		Color ennemyColor = Utils::getOppositeColor(color);

		U64 rookDestinations = MagicMoves::Rmagic(rookIndex, myBoard->getAllPieces()) & ~myBoard->getPieces(color) & target;

		U64 rookCaptureDestinations = rookDestinations & myBoard->getPieces(ennemyColor);
		U64 rookQuietDestinations = rookDestinations ^ rookCaptureDestinations;

		addQuietMoves(rookQuietDestinations, rookIndex, moves, Piece::ROOK);
		addCaptureMoves(rookCaptureDestinations, rookIndex, moves, Piece::ROOK);
	}
}

void MoveGen::appendPawnPseudoLegalMoves(const Color color, std::vector<Move>& moves, U64 target) const{
	if(color == WHITE){
        appendWhitePawnPseudoLegalMoves(moves, target);
	} else{
		appendBlackPawnPseudoLegalMoves(moves, target);
	}
}

void MoveGen::appendKnightPseudoLegalMoves(const Color color, std::vector<Move>& moves, U64 target) const{
	U64 knightPositions = myBoard->getKnights(color) & ~myBoard->getPinnedPieces();

	//loop through the knights:
	while(knightPositions){
		const Square knightIndex = pop_lsb(&knightPositions);
		U64 knightValidDestinations = myBoard->getKnightAttacks(knightIndex, color) & target;

		Color ennemyColor = Utils::getOppositeColor(color);

		U64 knightCaptureDestinations = knightValidDestinations & myBoard->getPieces(ennemyColor);
		U64 knightQuietDestinations = knightValidDestinations ^ knightCaptureDestinations;

		addQuietMoves(knightQuietDestinations, knightIndex, moves, Piece::KNIGHT);
		addCaptureMoves(knightCaptureDestinations, knightIndex, moves, Piece::KNIGHT);
	}
}

void MoveGen::appendWhitePawnPseudoLegalMoves(std::vector<Move>& moves, U64 target) const{
	appendWhiteEnPassantMoves(moves, LookUpTables::all);
	U64 pawnPositions = myBoard->getWhitePawns();

	while(pawnPositions){
		Square pawnIndex = pop_lsb(&pawnPositions);
		U64 pawnPos = 0 | 1LL << pawnIndex;

		U64 firstStep = (pawnPos << 8) & ~myBoard->getAllPieces() ;
		U64 twoSteps = ((firstStep & LookUpTables::maskRank[2]) << 8) & ~myBoard->getAllPieces() & target;
		U64 validAttacks = LookUpTables::pawnAttackTable[WHITE][pawnIndex] & myBoard->getBlackPieces() & target;

		addQuietMoves(firstStep & target & LookUpTables::clearRank[7], pawnIndex, moves, Piece::PAWN);
		addDoublePawnPushMoves(twoSteps & LookUpTables::clearRank[7], pawnIndex, moves);
		addPromotionMoves(firstStep & LookUpTables::maskRank[7], pawnIndex, moves);
		addCaptureMoves(validAttacks & LookUpTables::clearRank[7], pawnIndex, moves, Piece::PAWN);
		addPromotionCaptureMoves(validAttacks & LookUpTables::maskRank[7], pawnIndex, moves);
	}
}

void MoveGen::appendBlackPawnPseudoLegalMoves(std::vector<Move>& moves, U64 target) const{
	appendBlackEnPassantMoves(moves, LookUpTables::all);
	U64 pawnPositions = myBoard->getBlackPawns();

	while(pawnPositions){
		Square pawnIndex = pop_lsb(&pawnPositions);
		U64 pawnPos = 0 | 1LL << pawnIndex;

		U64 firstStep = (pawnPos >> 8) & ~myBoard->getAllPieces();
		U64 twoSteps = ((firstStep & LookUpTables::maskRank[5]) >> 8) & ~myBoard->getAllPieces() & target;
		U64 validAttacks = LookUpTables::pawnAttackTable[BLACK][pawnIndex] & myBoard->getWhitePieces() & target;

		addQuietMoves(firstStep  & target & LookUpTables::clearRank[0], pawnIndex, moves, Piece::PAWN);
		addDoublePawnPushMoves(twoSteps & LookUpTables::clearRank[0], pawnIndex, moves);
		addPromotionMoves(firstStep & LookUpTables::maskRank[0], pawnIndex, moves);
		addCaptureMoves(validAttacks & LookUpTables::clearRank[0], pawnIndex, moves, Piece::PAWN);
		addPromotionCaptureMoves(validAttacks & LookUpTables::maskRank[0], pawnIndex, moves);
	}
}

vector<Move> MoveGen::generatePseudoLegalMoves(){
    return generateLegalMoves(myBoard->getColorToPlay());
}

vector<Move> MoveGen::generatePseudoLegalMoves(const Color color){
    vector<Move> legalMoves;
	legalMoves.reserve(218);

	appendPawnPseudoLegalMoves(color, legalMoves);
	appendKingPseudoLegalMoves(color, legalMoves);
	appendQueenPseudoLegalMoves(color, legalMoves);
	appendRookPseudoLegalMoves(color, legalMoves);
	appendBishopPseudoLegalMoves(color, legalMoves);
	appendKnightPseudoLegalMoves(color, legalMoves);

	return legalMoves;
}

vector<Move> MoveGen::generateEvasionMoves(const Color color){
	// cout << "Inside generateEvasionMoves(const Color color)" << endl;
	vector<Move> evasionMoves;

	Color ennemyColor = Utils::getOppositeColor(color);
	U64 sliderAttacks = 0;
	U64 kbb = myBoard->getKing(color);
	Square ksq = msb(kbb);

	U64 kingAttackers = myBoard->getKingAttackers();

	// cout << "king Attackers : " << endl; BitBoardsUtils::printBitBoard(kingAttackers);

	U64 sliderAttackers = kingAttackers & ~myBoard->getPawns(ennemyColor) & ~myBoard->getKnights(ennemyColor);

	while (sliderAttackers){
		Square checksq = pop_lsb(&sliderAttackers);
		auto line = LookUpTables::lineBB[checksq][ksq];
		// cout << "Line between the king and the checker piece : " << endl; BitBoardsUtils::printBitBoard(line);
		auto removeKing = line ^ checksq;
		// cout << "Remove King BitBoard : " << endl; BitBoardsUtils::printBitBoard(removeKing);
		sliderAttacks |= removeKing;
	}

	// cout << "Slider Attacks : " << endl; BitBoardsUtils::printBitBoard(sliderAttacks);

	U64 kingAttacks = myBoard->getKingAttacks(ksq, color);

	// cout << "King Pseudo Legal Moves : " << endl; BitBoardsUtils::printBitBoard(kingAttacks);

	U64 kingEvasions = kingAttacks & ~sliderAttacks;

	// cout << "King Evasions : " << endl; BitBoardsUtils::printBitBoard(kingEvasions);

	U64 kingCaptureDestinations = kingEvasions & myBoard->getPieces(ennemyColor);

	// cout << "King Capture Destinations : " << endl; BitBoardsUtils::printBitBoard(kingCaptureDestinations);

	U64 kingQuietDestinations = kingEvasions ^ kingCaptureDestinations;

	// cout << "King Quiet Destinations : " << endl; BitBoardsUtils::printBitBoard(kingQuietDestinations);

	addQuietMoves(kingQuietDestinations, ksq, evasionMoves, Piece::KING);
	addCaptureMoves(kingCaptureDestinations, ksq, evasionMoves, Piece::KING);

	if(moreThanOne(kingAttackers)){
		return evasionMoves;
	}

	Square checkerSq = msb(kingAttackers);
	U64 target = LookUpTables::inBetween[checkerSq][ksq] ^ kingAttackers;

	appendPawnPseudoLegalMoves(color, evasionMoves, target);
	appendKnightPseudoLegalMoves(color, evasionMoves, target);
	appendBishopPseudoLegalMoves(color, evasionMoves, target);
	appendRookPseudoLegalMoves(color, evasionMoves, target);
	appendQueenPseudoLegalMoves(color, evasionMoves, target);

	// cout << "Evasion Moves List : " << endl;
	// for(Move &move : evasionMoves) cout << move.toShortString() << endl;

	return evasionMoves;
}

vector<Move> MoveGen::generateLegalMoves(){
	return generateLegalMoves(myBoard->getColorToPlay());
}

std::vector<Move> MoveGen::generateLegalMoves(const Color color){
	myBoard->updatePinnedPieces();
	myBoard->updateKingAttackers(myBoard->getColorToPlay());

	// cout << "King Attackers : " << endl; BitBoardsUtils::printBitBoard(myBoard->getKingAttackers());

	bool isCheck = myBoard->isCheck();
	// cout << "isCheck : " << isCheck << endl;

	std::vector<Move> moves;

	if(isCheck){
		moves = generateEvasionMoves(color);
	} else{
		moves = generatePseudoLegalMoves(color);
	}

	moves.erase(std::remove_if(moves.begin(), moves.end(),
			[&](Move move) mutable { return !myBoard->isMoveLegal(move, isCheck); }), moves.end());

	return moves;
}

void MoveGen::appendWhiteEnPassantMoves(std::vector<Move>& moves, U64 target) const{
	U64 validPawns = (myBoard->getWhitePawns() & LookUpTables::maskRank[4]);

	if (validPawns == 0) {return;}

	const Move* enemyLastMove(myBoard->getEnemyLastMove());

	if (!enemyLastMove) return;
	else if (enemyLastMove->getFlags() == Move::DOUBLE_PAWN_PUSH_FLAG){
		while (validPawns){
			Square enemyDestination = enemyLastMove->getDestination();
			Square validPawnIndex = pop_lsb(&validPawns);

			if (abs(validPawnIndex - enemyDestination) == 1){
				Square destination = static_cast<Square>(enemyDestination+8);
				U64 destinationbb = 0 | 1LL << destination;
				if(destinationbb & target){
					Move epMove(validPawnIndex,destination,Move::EP_CAPTURE_FLAG,Piece::PAWN);
					epMove.setCapturedPieceType(Piece::PAWN);
					moves.push_back(epMove);
				}
			}
		}
	}
	return;
}

void MoveGen::appendBlackEnPassantMoves(std::vector<Move>& moves, U64 target) const{
	U64 validPawns = (myBoard->getBlackPawns() & LookUpTables::maskRank[3]);

	if (validPawns == 0) {return;}

	const Move* enemyLastMove(myBoard->getEnemyLastMove());

	if (!enemyLastMove) return;
	else if (enemyLastMove->getFlags() == Move::DOUBLE_PAWN_PUSH_FLAG){
		while (validPawns){
			Square enemyDestination = enemyLastMove->getDestination();
			Square validPawnIndex = pop_lsb(&validPawns);

			if (abs(validPawnIndex - enemyDestination) == 1){
				Square destination = static_cast<Square>(enemyDestination-8);

				U64 destinationbb = 0 | 1LL << destination;
				if(destinationbb & target){
					Move epMove(validPawnIndex, destination, Move::EP_CAPTURE_FLAG, Piece::PAWN);
					epMove.setCapturedPieceType(Piece::PAWN);
					moves.push_back(epMove);
				}
			}
		}
		return;
	}
	return;
}

bool MoveGen::isQueenSideCastlingPossible(const Color color) const{
	bool iQSCP = myBoard->isQueenSideCastlingAllowed(color);

	if(!iQSCP) return false;

	const U64 bitBoardToBeFree = color == WHITE ? 14 : 1008806316530991104LL;
	iQSCP &= !(bitBoardToBeFree & myBoard->getAllPieces());

	Square squareNotTobeAttacked = color == WHITE ? SQ_E1 : SQ_E8;
	iQSCP &= !myBoard->isSquareAttacked(squareNotTobeAttacked, color);

	squareNotTobeAttacked = color == WHITE ? SQ_D1 : SQ_D8;
	iQSCP &= !myBoard->isSquareAttacked(squareNotTobeAttacked, color);

	squareNotTobeAttacked = color == WHITE ? SQ_C1 : SQ_C8;
	iQSCP &= !myBoard->isSquareAttacked(squareNotTobeAttacked, color);

	return iQSCP;
}

bool MoveGen::isKingSideCastlingPossible(const Color color) const{
	bool iKSCP = myBoard->isKingSideCastlingAllowed(color);

	if(!iKSCP) return false;

	const U64 bitBoardToBeFree = color == WHITE ? 96 : 6917529027641081856LL;
	iKSCP &= !(bitBoardToBeFree & myBoard->getAllPieces());

	Square squareNotTobeAttacked = color == WHITE ? SQ_E1 : SQ_E8;
	iKSCP &= !myBoard->isSquareAttacked(squareNotTobeAttacked, color);

	squareNotTobeAttacked = color == WHITE ? SQ_F1 : SQ_F8;
	iKSCP &= !myBoard->isSquareAttacked(squareNotTobeAttacked, color);

	squareNotTobeAttacked = color == WHITE ? SQ_G1 : SQ_G8;
	iKSCP &= !myBoard->isSquareAttacked(squareNotTobeAttacked, color);

	return iKSCP;
}