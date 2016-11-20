#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <sstream>

using namespace std;


//the following are UBUNTU/LINUX ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"     /* Red */
#define GREEN   "\033[32m"     /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

//Usage : cout << RED << "hello world" << RESET << endl;

struct Section {
	string Name;
	int StartAddr; 
	int EndAddr;
	int size;
	Section * Next;
};


struct Cave {
	int StartAddr = 0;
	int EndAddr = 0;
	int CaveSize = 0;
	string Section = "";
	Cave * Next = NULL;
};


struct PE {
	string MagicNumber;
	string ImageBase;
	string StartAddr;
	int SectionNum = 0;
	Section * Sections = NULL;
	int FileSize;

};


class Miner{
private:
	int CaveCount = 0;
	Cave * cave = NULL;
	PE pe;
public:
	void GetFileData(const char*); // Gathers PE file data via "objdump -x" & "grep" command
	void ParseFileSections(const char*); // Parse the start & end addresses of PE image sections
	void ValidateFile();  // This function checks the validity of PE file
	fstream * LoadPE(const char*); // Load the given named file
	void StartMiner(fstream*); // Start tracing the file byte stream for NULL bytes
	void EnumCaveLoc(Cave*); // Find out cave belongs witch section 
};







void Miner::StartMiner(fstream * File){
	int CurrentOfset = 0;
	int NullCount = 0;

	char Cursor;
	cout << RESET << BOLDYELLOW << "[*] Starting miner...\n";
	while(File->eof()){
		File->read(&Cursor,1);
		CurrentOfset++;
		if(Cursor == 0x00){
			NullCount++;
		}
		else{
			if(NullCount > 100){
				Cave * NewCave = new Cave;
				CaveCount++;
				NewCave->CaveSize = NullCount;
				NewCave->StartAddr = (CurrentOfset-NullCount);
				NewCave->EndAddr = CurrentOfset;
				EnumCaveLoc(NewCave);
				if(cave == NULL){
					cave = NewCave;
				}
				else{
					NewCave->Next = cave;
					cave = NewCave;
				}
			}
			NullCount = 0;
		}
	}
	cout << RESET << BOLDYELLOW << "[*] Mining finished.";
}




fstream * Miner::LoadPE(const char * FileName){
	fstream File;
	cout << RESET << BOLDYELLOW << "[*] Loading PE file...";
	File.open (FileName, ios::out | ios::binary);
	if(!File.is_open()){
		cout << RESET << BOLDRED << "ERROR : " << RED << "Cannot open file !" << endl;
		exit(1);		
	}

	return &File;
}

void Miner::EnumCaveLoc(Cave * _Cave){
	Section * Sec = pe.Sections;
	for(int i = 0; i < pe.SectionNum; i++) {
		if(Sec->StartAddr > _Cave->StartAddr < Sec->EndAddr) {
			_Cave->Section = Sec->Name;
		}
		Sec = Sec->Next;
	}
}


void Miner::ValidateFile(){
	if(pe.MagicNumber.find("(PE32)") == string::npos) {
		cout << RESET << BOLDRED << "ERROR : " << RED << "Given file is not a valid PE !" << endl;
		exit(1);		
	}
}

void Miner::GetFileData(const char * FileName){

	cout << RESET << BOLDYELLOW << "[*] Extracting file header data..."	;
	char * MagicNum = "objdump -x ";
	strcat(MagicNum,FileName);
	strcat(MagicNum," | grep 'Magic'");

	char * ImageBase = "objdump -x ";
	strcat(ImageBase,FileName);
	strcat(ImageBase," | grep 'ImageBase'");

	char * StartAddr = "objdump -x ";
	strcat(StartAddr,FileName);
	strcat(StartAddr," | grep 'StartAddr'");




	char * Ls = "ls ";
	strcat(Ls, FileName);

	char Data[1000];
	FILE * Check = popen(Ls, "r");
	if(Check == NULL){
		cout << RESET << BOLDRED << "ERROR : " << RED << "File enumeration failed !" << endl;
		exit(1);
	}
	if(fgets(Data,(sizeof(Data)-1),Check) != NULL) {
		for(int i = 0; i < sizeof(FileName); i++){
			if(FileName[i] != Data[i]) {
				cout << RESET << BOLDRED << "ERROR : " << RED << "No such file !" << endl;
			}
		}
	}
	delete Data;


	FILE * GetMagicNum = popen(MagicNum, "r");
	if(GetMagicNum == NULL){
		cout << RESET << BOLDRED << "ERROR : " << RED << "File enumeration failed !" << endl;
		exit(1);
	}
	if(fgets(Data,(sizeof(Data)-1),GetMagicNum) != NULL) {
		pe.MagicNumber = Data;
	}
	delete Data;

	FILE * GetImageBase = popen(ImageBase, "r");
	if(GetImageBase == NULL){
		cout << RESET << BOLDRED << "ERROR : " << RED << "File enumeration failed !" << endl;
		exit(1);
	}
	if(fgets(Data,(sizeof(Data)-1),GetImageBase) != NULL) {
		pe.ImageBase = Data;
	}
	delete Data;

	FILE * GetStartAddr = popen(StartAddr, "r");
	if(GetStartAddr == NULL){
		cout << RESET << BOLDRED << "ERROR : " << RED << "File enumeration failed !" << endl;
		exit(1);
	}
	if(fgets(Data,(sizeof(Data)-1),GetStartAddr) != NULL) {
		pe.StartAddr = Data;
	}
	delete Data;


}

void Miner::ParseFileSections(const char * FileName){

	cout << RESET << BOLDYELLOW << "[*] Parsing file sections...";
	char * _Sections = "objdump -h ";
	strcat(_Sections,FileName);
	strcat(_Sections," | grep '2**2' > sec.dat");

	system(_Sections); // objdump -h | grep '2**2' > sec.dat

	fstream SectionData;
	string Line;

	SectionData.open("sec.dat");
	if(SectionData.is_open()){
		while(getline(SectionData, Line)){
			string SecName = "";
			string SecSize = "";
			string Start = "";
			for(int i = 4; i < 11; i++){
				SecName += Line[i]; 
			}
			Section * NewSection = new Section;
			NewSection->Name = SecName;
			if(pe.Sections == NULL){
				pe.Sections = NewSection;
			}
			else{
				NewSection->Next = pe.Sections;
				pe.Sections = NewSection;
			}
			for(int i = 18; i < 27; i++){
				SecSize += Line[i];
			}
			stringstream SS;
			SS << hex << SecSize;
			SS >> NewSection->size;
			for(int i = 48; i < 56; i++){
				SecSize += Line[i];
			}
			SS << hex << Start;
			SS >> NewSection->StartAddr;
			NewSection->EndAddr = (NewSection->StartAddr + NewSection->size);			
			pe.SectionNum++; 
		}
	}
	cout << RESET << BOLDYELLOW << "[*] Section parsing complete.";



}