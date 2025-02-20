#include "LazySMP.h"

void searchThread(int threadId, shared_ptr<Board> board, unsigned int allocatedTimeMS, SearchResult &result){
    Search search = Search(board);
    search.setInitialDepth(threadId+1);

    result.score = search.negaMaxRootIterativeDeepeningLazySMP(allocatedTimeMS);
    result.bestMove = search.myBestMove;
    result.searchDepth = search.myDepth;
}

Move16 selectBestMove(const vector<SearchResult> &results){
    int maxDepth = 0;
    for(const auto &result : results) maxDepth = max(maxDepth, result.searchDepth);

    vector<SearchResult> maxDepthResults;
    for(const auto &result : results){
        if(result.searchDepth == maxDepth) maxDepthResults.push_back(result);
    }

    unordered_map<Move16, int> moveCounts;
    for(const auto &result : maxDepthResults){
        moveCounts[result.bestMove]++;
    }

    int maxCount = 0;
    vector<Move16> bestMoves;
    for(const auto& [move, count] : moveCounts){
        if(count > maxCount){
            maxCount = count;
            bestMoves.clear();
            bestMoves.push_back(move);
        } else if(count == maxCount){
            bestMoves.push_back(move);
        }
    }

    if(bestMoves.size() > 1){
        int bestScore = -999999;
        Move16 finalMove = bestMoves[0];

        for(const auto &result : maxDepthResults){
            if(find(bestMoves.begin(), bestMoves.end(), result.bestMove) != bestMoves.end()){
                if(result.score > bestScore){
                    bestScore = result.score;
                    finalMove = result.bestMove;
                }
            }
        }
        return finalMove;
    }

    return bestMoves[0];
}

Move16 LazySMP::search(){
    vector<thread> threads(this->num_threads);
    vector<SearchResult> results(this->num_threads);

    for(int i = 0; i < this->num_threads; i++){
        Board boardCopy(*this->myBoard);
        threads[i] = thread(searchThread, i, make_shared<Board>(boardCopy), this->allocatedTimeMS, ref(results[i]));
    }

    for(auto &t : threads) t.join();

    for(int i = 0; i < this->num_threads; i++){
        cout << "Thread " << i << " : " << results[i].score 
             << " " << Utils::Move16ToShortString(results[i].bestMove)
             << " " << results[i].searchDepth << endl;
    }

    return selectBestMove(results);
}