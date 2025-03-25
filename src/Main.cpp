#include<bits/stdc++.h>

#include"Board.h"
#include"LookUpTables.h"
#include"Move.h"
#include"MoveGen.h"
#include "Pawn.h"
#include"Uci.h"
#include"Search.h"
#include"Utils.h"
#include"Evaluation.h"
#include"MagicMoves.h"
#include"chrono"
#include "TT.h"
#include "LazySMP.h"

using namespace std;

#define U64 std::uint64_t

int main(){

    LookUpTables::init();
    MagicMoves::initmagicmoves();
    ZK::initZobristKeys();

    // Uci uci;
    // uci.init();
    // uci.loop();
    
    // CLI based Full Gameplay mode
    int ttSize = 512;
    globalTT.init_TT_size(ttSize);
    Pawn::initPawnTable();

    shared_ptr<Board> board = make_shared<Board>("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    // // shared_ptr<Board> board = make_shared<Board>("8/2p5/6k1/3pP3/3P4/6K1/8/8 w - - 0 53");

    MoveGen mg(board);
    Search search = Search(board);
    Evaluation eval(board);
    
    while(true){
        cout << *board << endl;
        vector<Move> legalmoves = mg.generateMoves();

        if(legalmoves.size() == 0){
            if(board->isCheck()) cout << "You loose !!!" << endl;
            else cout << "Stalemate !!!" << endl;

            cout << "Final Board Position : " << endl;
            cout << *board << endl;

            break;
        }

        cout << "Enter you're move : " << endl;
        string usermove; cin >> usermove;
        // string usermove = "e2e4";

        Move userMove = Move();
        for(const Move &move : legalmoves){
            if(move.toShortString() == usermove){
                userMove = move;
                break;
            }
        }
        cout << "You're move : " << userMove.toShortString() << endl;
        board->executerMove(userMove);

        // Computer's move
        vector<Move> computerMoves = mg.generateLegalMoves();
        if(computerMoves.size() == 0){
            if(board->isCheck()) cout << "You win !!!" << endl;
            else cout << "Stalemate !!!" << endl;

            cout << "Final Board Position : " << endl;
            cout << *board << endl;

            break;
        }

        // LazySMP lazySmp = LazySMP(8, 2000, board);
        search.negaMaxRootIterativeDeepening(15000);
        // search.negaMaxRoot(6);
        string computermove = Utils::Move16ToShortString(search.myBestMove);

        Move computerMove;
        for(const Move &move : computerMoves){
            if(move.toShortString() == computermove){
                computerMove = move;
            }
        }
        board->executerMove(computerMove);
        cout << "Computer's Move : " << computermove << endl;

        vector<Zkey> keys = board->getKeysHistory();
	    Zkey currentKey = board->key;
        int repetitionCount = count(keys.begin(), keys.end(), currentKey);
        if (repetitionCount >= 3) {
            cout << "Draw by threefold repetition!" << endl;
            break;
        }
    }
    
    return 0;
}
