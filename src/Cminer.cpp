#include "Miner.h"
#include <iostream>
#include <string>

using namespace std;

void PrintHelp(int);

int main(int argc, char const *argv[]) // Compile Command: g++ -std=c++11 Cminer.cpp -o Cminer
{
	int MinCaveSize;
	string MinCaveSizeStr;

	if(argc != 2 && argc != 3){
		PrintHelp(1);
		return 0;	
	}
	if(argc == 3){
		PrintHelp(0);
		MinCaveSizeStr = string(argv[2]);
		MinCaveSize = stoi(MinCaveSizeStr); // Convert the minimum cave size to integer
		cout << RESET << BOLDYELLOW << "[*] Minimum cave size set to " << argv[2] << endl;	
	}
	else{
		PrintHelp(0);
		MinCaveSize = 300;
		cout << RESET << BOLDYELLOW << "[*] Minimum cave size set to 300" << endl;
	}
	if(MinCaveSize < 0 || MinCaveSize < 100) {
		cout << RESET << BOLDRED << "[-] ERROR : " << RED << "minimumm cave size is too small !" << endl;
		return 0;
	}
	

	Miner tom; // Our humble miner tom ;)

	tom.GetFileData(argv[1]);
	tom.ParseFileSections(argv[1]);
	char * File = tom.LoadPE(argv[1]);
	tom.StartMiner(File,MinCaveSize);
	tom.Result();

	string Clean = "rm ";
	Clean += argv[1];
	Clean += ".bak";
	system(&Clean[0]);
	return 0;
}




void PrintHelp(int mode){
	cout << BOLDGREEN << "                    ___       \n"
					 "                 .-' \\\\\".     \n"
					 "                /`    ;--:    \n"
					 "               |     (  (_)== \n"
					 "               |_ ._ '.__.;   \n"
					 "               \\_/`--_---_|   \n"
					 "                (`--(./-\\.)   \n"
					 "                `|     _\\ |   \n"
					 "                 | \\  __ /    \n"
					 "                /|  '.__/     \n"
					 "             .'` \\     |_     \n"
					 "                   '-__ / `-    \n" << RESET;


	cout << BOLDRED << "   _____           _           \n"
					   "  / ____|         (_)                \n"
					   " | |     _ __ ___  _ _ __   ___ _ __ \n"
					   " | |    | '_ ` _ \\| | '_ \\ / _ \\ '__|\n"
					   " | |____| | | | | | | | | |  __/ |   \n"
					   "  \\_____|_| |_| |_|_|_| |_|\\___|_|   \n\n" << RESET;

	cout << BOLDBLUE << "https://github.com/EgeBalci/Cminer\n" << RESET;
	if(mode == 1) {
		cout << BOLDGREEN << "\nUsage: \n\tCminer <file> <MinCaveSize>\n\n" << RESET;
		cout << GREEN << "Cminer is a tool for enumerating code caves inside PE files.\n" << RESET;
	}


}