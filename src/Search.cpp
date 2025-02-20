#include "Search.h"
#include "TT.h"
#include "Utils.h"
#include "MoveGen.h"

#include <bits/stdc++.h>

Search::Search(shared_ptr<Board> boardPtr) : myBestMove(), myEval(boardPtr), myMoveOrder(), nodesSearched(0), TTTableHits(0), myPly(0), myPvTable{0}, myPvLength{0}, myMovesSearched(0){
    myBoard = boardPtr;
	memset(historyTable, 0, sizeof(historyTable));
}

void Search::clearSearchData(){
	myMoveOrder.clearKillers();
	memset(historyTable, 0, sizeof(historyTable)); 
	mySearchDurationMS = 0;
}

bool Search::isInsufficentMatingMaterial() const{
	bool isPawns = myBoard->getAllPawns();
	bool isQueensOrRooks = myBoard->getAllQueens() || myBoard->getAllRooks();
	auto numberOfWhiteMinorPieces = popcount(myBoard->getWhiteKnights() | myBoard->getWhiteBishops());
	auto numberOfBlackMinorPieces = popcount(myBoard->getBlackKnights() | myBoard->getBlackBishops());

	return !isPawns && !isQueensOrRooks && numberOfWhiteMinorPieces <= 1 && numberOfBlackMinorPieces <=1;
}

/* TODO : This definition of endgfame can be further tuned in future */
bool Search::isEndGame() const {
	return popcount(myBoard->getAllPieces() & ~myBoard->getAllPawns()) <= 6;
}

void Search::updateHistoryTable(const Move &move, int depth){
	historyTable[myBoard->getColorToPlay()][move.getOrigin()][move.getDestination()] += depth*depth;
}

int Search::evaluate(){
	myMovesSearched++;
    return (-2*myBoard->getColorToPlay() + 1)*myEval.evaluate();
}

void Search::sendInfoToUCI(int alpha){
	cout << "info";
	cout << " depth " << myDepth;
	cout << " score cp " << alpha;
	cout << " nodes " << myMovesSearched;

	unsigned int nps = 1000 * myMovesSearched / mySearchDurationMS;
	cout << " nps " << nps;
	cout << " time " << mySearchDurationMS;
	cout << endl;

	std::cout << "PV : " ;

	cout << myPvTable[0][0].toShortString() << "   ";
	unsigned int j = 1;
    while (!myPvTable[1][j].isNullMove()){
		std::cout << myPvTable[1][j].toShortString() << "   ";
        j++;
	}
	std::cout << std::endl;
}

void Search::generateRandomMove(){
    random_device rd;
    mt19937 gen(rd());

    MoveGen mg(myBoard);
    vector<Move> moveList = mg.generateMoves();

    uniform_int_distribution<> dis(0, moveList.size()-1);
    int randomIndex = dis(gen);

    Move bestMove = moveList[randomIndex];
    myBestMove = bestMove.getMove16();
}

int Search::qSearch(int alpha, const int beta){
	int stand_pat = evaluate();

	if(stand_pat >= beta){
		// nodesSearched++;
		return beta;
	}

	/* DELTA PRUNNING */
	// if(stand_pat + DELTA_MARGIN < alpha) return alpha;
	
	if(alpha < stand_pat) alpha = stand_pat;

	MoveGen moveGen(myBoard);
	vector<Move> moveList = moveGen.generateMoves();

	myMoveOrder.rateMoves(moveList, myBoard, myPly, false);
	myMoveOrder.sortMoves(moveList);

	for(auto currentMove : moveList){
		if(currentMove.isCapture() && myBoard->seeCapture(currentMove, Utils::getOppositeColor(myBoard->getColorToPlay())) > 0){
			int score = 0;
			myBoard->executerMove(currentMove);
			myEval.updateEvalAttributes(currentMove);
			myPly++;

			score = -qSearch(-beta, -alpha);

			myPly--;
			myBoard->undoMove(currentMove);
			myEval.rewindEvalAttributes(currentMove);

			if(score >= beta) return beta;
			else if(score > alpha) alpha = score;
		}
	}
	return alpha;
}

int Search::negaMax(const int depth, int alpha, const int beta){
    int alpha_old = alpha;
	int extensions = 0;
	myPvLength[myPly] = myPly;

	myBoard->updateKingAttackers(myBoard->getColorToPlay());
	if(myBoard->isCheck() && myPly <= MAX_DEPTH) extensions++;

	if (depth + extensions <= 0){
		if(myBoard->getEnemyLastMove()->isCapture()) return qSearch(alpha, beta);
		else {
			// nodesSearched++;
			return evaluate();
		}
    }

	// Null Move Pruning
	if(depth >= 3 && !myBoard->isCheck() && !isEndGame()){
		myBoard->executeNullMove();
		int nullScore = -negaMax(depth -1 - 2, -beta, -beta + 1);
		myBoard->undoNullMove();
		if(nullScore >= beta) return beta;
	}

	vector<Zkey> keys = myBoard->getKeysHistory();
	Zkey currentKey = myBoard->key;
	keys.pop_back();

	if(find(keys.begin(), keys.end(), currentKey) != keys.end()) return Evaluation::DRAW_SCORE;
	if(isInsufficentMatingMaterial()) return Evaluation::DRAW_SCORE;

	auto ttEntry = globalTT.probeTT(currentKey, depth);
	if(ttEntry){
		if(ttEntry->getNodeType() == NodeType::EXACT){
			TTTableHits++;
			return ttEntry->getScore();
		}
		else if (ttEntry->getNodeType() == NodeType::UPPER && ttEntry->getScore() <= alpha){
			TTTableHits++;
			return alpha;
		}
		else if(ttEntry->getNodeType() == NodeType::LOWER && ttEntry->getScore() >= beta){
			TTTableHits++;
			return beta;
		}
	}

	MoveGen moveGen(myBoard);
	vector<Move> moveList = moveGen.generateLegalMoves();

	if(moveList.empty()){
		if(myBoard->isCheck()){
			// nodesSearched++;
			return -(Evaluation::CHECKMATE_SCORE + depth);
		} else {
			// nodesSearched++;
			return Evaluation::DRAW_SCORE;
		}
	}


	myMoveOrder.rateMoves(moveList, myBoard, myPly, false);
	myMoveOrder.sortMoves(moveList);

	int score = 0;
	Move bestMove = Move();
    int bestScore = -999999;
	auto moveNumber = 0;

	for (auto currentMove : moveList){
		/* DELTA PRUNNING */
		// if(depth <= 3 && !currentMove.isCapture() && !currentMove.isPromotion() &&
		// 	evaluate() + DELTA_MARGIN < alpha){
		// 	continue;
		// }

		moveNumber++;

		bool isEscapingCheck = myBoard->isCheck();
		myBoard->executerMove(currentMove);
		myEval.updateEvalAttributes(currentMove);
		myPly++;

		myBoard->updateKingAttackers();

		// Aggressive Late Move Reductions
		// if (depth >= 3 && moveNumber >= 4 && !currentMove.isCapture() && !currentMove.isPromotion() && !myBoard->isCheck()) {
        //     int reduction = 1 + (moveNumber > 6) + (historyTable[myBoard->getColorToPlay()][currentMove.getOrigin()][currentMove.getDestination()] < 0);
        //     reduction = min(reduction, depth - 1);
        //     score = -negaMax(depth - 1 - reduction, -beta, -alpha);
        //     if (score >= alpha) {
        //         score = -negaMax(depth - 1, -beta, -alpha);
        //     }
        // } else {
        //     score = -negaMax(depth + extensions - 1, -beta, -alpha);
        // }
		score = -negaMax(depth + extensions - 1, -beta, -alpha);

		myBoard->undoMove(currentMove);
		myEval.rewindEvalAttributes(currentMove);
		myPly--;

		if(score >= beta){
			myMoveOrder.setNewKiller(currentMove, myPly);
			updateHistoryTable(currentMove, depth);
			globalTT.setTTEntry(currentKey, depth, score, NodeType::LOWER, currentMove.getMove16(), myBoard->getply());
			return beta;
		}

        if (score > bestScore) {
            bestScore = score ;
			bestMove = currentMove;
		    if( score > alpha ){
			    alpha = score;

				myPvTable[myPly][myPly] = currentMove;
				for (unsigned int i=myPly+1; i<myPvLength[myPly+1]; i++){
                    myPvTable[myPly][i] = myPvTable[myPly+1][i];
                }
				myPvLength[myPly] = myPvLength[myPly+1];
		    }
        }
	}

	if(alpha > alpha_old) globalTT.setTTEntry(currentKey, depth, bestScore, NodeType::EXACT, bestMove.getMove16(), myBoard->getply());
	else globalTT.setTTEntry(currentKey, depth, alpha, NodeType::UPPER, bestMove.getMove16(), myBoard->getply());

	return alpha;
}

int Search::negaMaxRoot(const int depth){
	clearSearchData();

	// nodesSearched = 0;
	TTTableHits = 0;

    int alpha = -999999;
	int beta = -alpha;
	int score = 0;

	myPly=0;
    myPvLength[myPly] = myPly;

	auto currentKey = myBoard->key;
	auto ttEntry = globalTT.probeTT(currentKey, depth);
	if(ttEntry){
		if(ttEntry->getNodeType() == NodeType::EXACT){
			TTTableHits++;
			myBestMove = ttEntry->getBestmove();
			return ttEntry->getScore();
		}
	}

	MoveGen moveGen(myBoard);
	vector<Move> moveList = moveGen.generateMoves();

	myMoveOrder.rateMoves(moveList, myBoard, myPly, true);
	myMoveOrder.sortMoves(moveList);

	for (auto currentMove : moveList){
		myBoard->executerMove(currentMove);
		myEval.updateEvalAttributes(currentMove);
		myPly++;

	    score = -negaMax(depth - 1, -beta, -alpha);

		myBoard->undoMove(currentMove);
		myEval.rewindEvalAttributes(currentMove);
		myPly--;

		if( score > alpha ){
			alpha = score;
			myBestMove = currentMove.getMove16();

			myPvTable[myPly][myPly] = currentMove ; 
            for (unsigned int i=myPly+1; i<myPvLength[myPly+1]; i++){
                myPvTable[myPly][i] = myPvTable[myPly+1][i];
            }
            myPvLength[myPly] = myPvLength[myPly+1];
		}
	}

	globalTT.setTTEntry(myBoard->key, depth, alpha, NodeType::EXACT, myBestMove, myBoard->getply());

	sendInfoToUCI(alpha);

	return alpha;
}

int Search::negaMaxRootIterativeDeepening(const unsigned int allocatedTimeMS){
    clearSearchData();

	int alpha = -999999;
	int beta = -alpha;
	int score = 0;
	unsigned int minDepth = 2;

	myMovesSearched = 0;
	myPly = 0;
	myPvLength[myPly] = myPly;

	chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();
	chrono::high_resolution_clock::time_point time;
	auto dur = time - startTime;

	myDepth = 1;

	while(true){
		alpha = -999999;
		beta = -alpha;
		int score = 0;
		bool isPvs = false;

		if(mySearchDurationMS > 0.5 * allocatedTimeMS && myDepth >= minDepth){
			break;
		}
		
		auto currentKey = myBoard->key;
		auto ttEntry = globalTT.probeTT(currentKey, myDepth);
		if(ttEntry){
			if(ttEntry->getNodeType() == NodeType::EXACT){
				myBestMove = ttEntry->getBestmove();
				alpha = ttEntry->getScore();
			}
		} else {
			MoveGen moveGen(myBoard);

			vector<Move> moveList = moveGen.generateMoves();
			myMoveOrder.rateMoves(moveList, myBoard, myPly, true);
			myMoveOrder.sortMoves(moveList);

			unsigned int j = 0;
			for(auto currentmove : moveList){
				j++;
				bool isMovelistHalfDone = 2*j > moveList.size();

				time = chrono::high_resolution_clock::now();
				dur = time - startTime;
				mySearchDurationMS = chrono::duration_cast<chrono::milliseconds>(dur).count();

				if(mySearchDurationMS > allocatedTimeMS && myDepth >= minDepth && isMovelistHalfDone){
					break;
				}

				myBoard->executerMove(currentmove);
				myEval.updateEvalAttributes(currentmove);
				myPly++;

				if(isPvs){
					score = -negaMax(myDepth - 1, alpha - 1, -alpha);
					if((score > alpha) && (score < beta)){
						score = -negaMax(myDepth - 1, -beta, -alpha);
					}
				} else {
					score = -negaMax(myDepth - 1, -beta, -alpha);
				}

				if(score > alpha){
					alpha = score;
					isPvs = true;
					myBestMove = currentmove.getMove16();

					myPvLength[0] = 1;
                	myPvTable[0][0] = currentmove;
	
                	if(myPvLength[1] > 0) {
                	    for(unsigned i=0; i<myPvLength[1]; i++) {
                	        if((i+1) < MAX_DEPTH) {  // Prevent overflow
                	            myPvTable[0][i+1] = myPvTable[1][i];
                	            myPvLength[0]++;
                	        }
                	    }
                	}

					// myPvTable[myPly][myPly] = currentmove;
					
					// for(unsigned int i=myPly+1; i<myPvLength[myPly+1]; i++){
					// 	myPvTable[myPly][i] = myPvTable[myPly+1][i];
					// }

					// myPvLength[myPly] = myPvLength[myPly+1];
				}

				myBoard->undoMove(currentmove);
				myEval.rewindEvalAttributes(currentmove);
				myPly--;
			}
			globalTT.setTTEntry(myBoard->key, myDepth, alpha, NodeType::EXACT, myBestMove, myBoard->getply());
		}
		time = chrono::high_resolution_clock::now();
		dur = time - startTime;

		mySearchDurationMS = chrono::duration_cast<chrono::milliseconds>(dur).count();
		mySearchDurationMS = max(mySearchDurationMS, (unsigned int)1);
		sendInfoToUCI(alpha);

		myDepth ++;
	}

	// printPvTable(myDepth);
	return alpha;
}

int Search::negaMaxRootIterativeDeepeningLazySMP(const unsigned int allocatedTimeMS){
    clearSearchData();

	int alpha = -999999;
	int beta = -alpha;
	int score = 0;
	unsigned int minDepth = 1;

	myMovesSearched = 0;
	myPly = 0;
	myPvLength[myPly] = myPly;

	chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();
	chrono::high_resolution_clock::time_point time;
	auto dur = time - startTime;

	myDepth = initialDepth;

	while(true){
		alpha = -999999;
		beta = -alpha;

		int score = 0;
		bool isPvs = false;

		if(mySearchDurationMS > 0.5 * allocatedTimeMS && myDepth >= minDepth) return alpha;
		
		auto currentKey = myBoard->key;
		auto ttEntry = globalTT.probeTT(currentKey, myDepth);
		if(ttEntry){
			if(ttEntry->getNodeType() == NodeType::EXACT){
				myBestMove = ttEntry->getBestmove();
				alpha = ttEntry->getScore();
			}
		} else {
			MoveGen moveGen(myBoard);

			vector<Move> moveList = moveGen.generateMoves();
			myMoveOrder.rateMoves(moveList, myBoard, myPly, true);
			myMoveOrder.sortMoves(moveList);

			unsigned int j = 0;
			for(auto currentmove : moveList){
				j++;
				bool isMovelistHalfDone = 2*j > moveList.size();

				time = chrono::high_resolution_clock::now();
				dur = time - startTime;
				mySearchDurationMS = chrono::duration_cast<chrono::milliseconds>(dur).count();

				if(mySearchDurationMS > allocatedTimeMS && myDepth >= minDepth && isMovelistHalfDone) return alpha;

				myBoard->executerMove(currentmove);
				myEval.updateEvalAttributes(currentmove);
				myPly++;

				if(isPvs){
					score = -negaMax(myDepth - 1, alpha - 1, -alpha);
					if((score > alpha) && (score < beta)){
						score = -negaMax(myDepth - 1, -beta, -alpha);
					}
				} else {
					score = -negaMax(myDepth - 1, -beta, -alpha);
				}

				if(score > alpha){
					alpha = score;
					isPvs = true;
					myBestMove = currentmove.getMove16();
					myPvTable[myPly][myPly] = currentmove;
					
					for(unsigned int i=myPly+1; i<myPvLength[myPly+1]; i++){
						myPvTable[myPly][i] = myPvTable[myPly+1][i];
					}

					myPvLength[myPly] = myPvLength[myPly+1];
				}

				myBoard->undoMove(currentmove);
				myEval.rewindEvalAttributes(currentmove);
				myPly--;
			}
			globalTT.setTTEntry(myBoard->key, myDepth, alpha, NodeType::EXACT, myBestMove, myBoard->getply());
		}
		time = chrono::high_resolution_clock::now();
		dur = time - startTime;

		mySearchDurationMS = chrono::duration_cast<chrono::milliseconds>(dur).count();
		mySearchDurationMS = max(mySearchDurationMS, (unsigned int)1);
		// sendInfoToUCI(alpha);

		myDepth++;
	}
	return alpha;
}

void Search::printPvTable(const unsigned int numLines){
	cout << "Printing pv Table..." << endl;
    unsigned int j;
	for(unsigned int i = 0; i<=numLines; i++) // For each ply starting at 0 (root) to numLines
	{
        // for (j=0; j<i; j++)
        // {
        //     std::cout << "       ";
        // }
        j=i;
        while (!myPvTable[i][j].isNullMove()) // Print successives moves until first null move
		{
			std::cout << myPvTable[i][j].toShortString() << "   ";
            j++;
		}
		std::cout << std::endl;
	}
}