#include <iostream>
#include <fstream>
#include <string>
#include "Miner.h"

using namespace std;

void PrintHelp(int);

int main(int argc, char const *argv[])
{
	
	if(argc == 1){
		PrintHelp(1);	
	}
	else {
		PrintHelp(0);
	}
	cout << "1";
	Miner tom;
	cout << "2";
	tom.GetFileData(argv[1]);
	cout << "3";

	





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
					 "                   '-__ / `-    \n";


	cout << BOLDRED << "   _____           _           \n"
					   "  / ____|         (_)                \n"
					   " | |     _ __ ___  _ _ __   ___ _ __ \n"
					   " | |    | '_ ` _ \\| | '_ \\ / _ \\ '__|\n"
					   " | |____| | | | | | | | | |  __/ |   \n"
					   "  \\_____|_| |_| |_|_|_| |_|\\___|_|   \n\n";

	cout << BOLDBLUE << "Github: github.com/EgeBalci/Cminer\n";
	if(mode == 1) {
		cout << BOLDGREEN << "\nUsage: Cminer <file>\n\n";
		cout << RESET << GREEN << "Cminer is a tool for enumerating code caves inside PE files.\n"
	  				 		  	  "(All code caves under the size of 100 bytes will be ignored)\n";
	}


}