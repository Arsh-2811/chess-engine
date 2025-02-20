#ifndef UCI_H
#define UCI_H

#include<bits/stdc++.h>

#include <memory>
#include <map>
#include <thread>

namespace thrd = std;

#include "Board.h"
#include "Move.h"
#include "Search.h"
#include "UciOption.h"
#include "LazySMP.h"

typedef map<string, UciOption> OptionsMap;

class Uci{
public :
	Uci() : wtime(10000), btime(10000), winc(0), binc(0), movestogo(0),
	myBoardPtr(std::shared_ptr<Board>(new Board())), mySearch(Search(myBoardPtr)), myOptionsMap(){
		myOptionsMap["hash"] = UciOption("256", "spin",1,2048); //hash size in MB, default 256 MB
	}

	void loop();
	void init();
    void setoption(std::istringstream& is);
    void printOptions() const;
    void initSearch();
    void newGame();
    std::string getOption(const std::string str) const;

    // get methods    
    inline Search getUciSearch() const {return mySearch;}

private:

	unsigned int wtime;
	unsigned int btime;
	unsigned int winc;
	unsigned int binc;
	unsigned int movestogo;
	std::shared_ptr<Board> myBoardPtr;
	Search mySearch;
	thrd::thread myThread;


	void updatePosition(std::istringstream& is);
	Move strToMove(std::string str);
	void search();

	OptionsMap myOptionsMap;

};

#endif