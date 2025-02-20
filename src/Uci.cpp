#include "Uci.h"
#include "MoveGen.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <thread>  

#include "TT.h"

void Uci::updatePosition(std::istringstream& is){
	Move m;
	std::string token, fen;

	is >> token;

	if (token == "startpos"){
		myBoardPtr = std::shared_ptr<Board>(new Board());
	} else if (token == "fen"){
		while (is >> token && token != "moves") fen += token + " ";
		myBoardPtr = std::shared_ptr<Board>(new Board(fen));
	} else {
		return;
	}

	while (is >> token){
		if (token != "moves"){
			m = strToMove(token);
			myBoardPtr->executerMove(m);
		}
	}
	initSearch();
}

std::string Uci::getOption(const std::string str) const{
	UciOption uciOption = (*(myOptionsMap.find(str))).second;
	return uciOption.getCurrentValue();
}

void Uci::setoption(std::istringstream& is) {

	std::string token, name, value;
	is >> token;

	while (is >> token && token != "value") name += std::string(" ", name.empty() ? 0 : 1) + token;

	while (is >> token) value += std::string(" ", value.empty() ? 0 : 1) + token;

	if (myOptionsMap.count(name)){
		myOptionsMap[name] = value;
		std::cout << "Option: " << name << " updated to "<< value << std::endl;
	} else {
		std::cout << "No such option: " << name << std::endl;
	}
}

void Uci::printOptions() const{
	std::cout << "Current options" << std::endl;
	for(auto elem : myOptionsMap){
		UciOption uciOption = elem.second;

		std::cout << "option name " << elem.first << " type spin default "  <<  uciOption.getDefaultValue()
				<< " min " << uciOption.getMin() << " max " << uciOption.getMax() << std::endl;
	}
}

Move Uci::strToMove(std::string str){
	MoveGen mg(myBoardPtr);
	std::vector<Move> moves = mg.generateLegalMoves();

	for (auto move : moves){
		if(str == move.toShortString()) return move;
	}

	std::cout << " PROBLEM [" << str << "]" <<std::endl;

	Move m = Move();
	return m;
}

void Uci::initSearch(){
	mySearch = Search(myBoardPtr); 
}

void Uci::init(){
	globalTT.init_TT_size(512);
    Pawn::initPawnTable();
}

void Uci::newGame(){
}

void Uci::loop()
{
	std::string line;
	std::string token;

	// Make sure that the outputs are sent straight away to the GUI
	std::cout.setf (std::ios::unitbuf);

	printOptions();

	while (std::getline(std::cin, line))
	{
		std::istringstream is(line);

		token.clear(); // getline() could return empty or blank line
		is >> std::skipws >> token;
		//	std::cout << token << std::endl;

		if (token == "uci")
		{
			std::cout << "id name Deepov 0.4.1" << std::endl;
			std::cout << "id author Romain Goussault - Navid Hedjazian" << std::endl;
			printOptions();
			std::cout << "uciok" << std::endl;
		}
		else if (token == "isready")
		{
			std::cout << "readyok" << std::endl;
		}

		else if (token == "setoption") 
            setoption(is);

		else if (token == "color")

			std::cout << "colorToPlay: " << myBoardPtr->getColorToPlay() << std::endl;

		else if (token == "ucinewgame")
		{
		    newGame();
        }
        
		else if (token == "position")
			updatePosition(is);

		else if (token == "print")
			std::cout << *myBoardPtr << std::endl;

		else if (token == "printOptions")
			printOptions();

		else if (token == "go")
		{
			while (is >> token)
			{
				if (token == "wtime")          is >> wtime;
				else if (token == "btime")     is >> btime;
				else if (token == "winc")      is >> winc;
				else if (token == "binc")      is >> binc;
				else if (token == "movestogo") is >> movestogo;
			}

			//http://stackoverflow.com/questions/12624271/c11-stdthread-giving-error-no-matching-function-to-call-stdthreadthread
			thrd::thread thr(&Uci::search, this);
			thrd::swap(thr, myThread);
			myThread.join();
		}
		else if (token == "quit")
		{
			std::cout << "Terminating.." << std::endl;
			break;
		}
		else
			// Command not handled
			std::cout << "what?" << std::endl;
	}
}

void Uci::search(){
	// LazySMP lazySmp = LazySMP(1, 5000, myBoardPtr);
	// Move16 bestMove = lazySmp.search();
	mySearch.negaMaxRoot(6);
	Move16 bestMove = mySearch.myBestMove;
	std::cout << "bestmove " << Utils::Move16ToShortString(bestMove) << std::endl;
}