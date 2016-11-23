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
	Section * Next = NULL;
};


struct Cave {
	int StartAddr = 0;
	int EndAddr = 0;
	int CaveSize = 0;
	string Section = "";
	Cave * Next = NULL;
};


struct PE {
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
	void GetFileData(string FileName); // Gathers PE file data via "objdump -x" & "grep" command
	void ParseFileSections(string); // Parse the start & end addresses of PE image sections
	fstream * LoadPE(string); // Load the given named file
	void StartMiner(fstream*); // Start tracing the file byte stream for NULL bytes
	void EnumCaveLoc(Cave*); // Find out cave belongs witch section
	void Result();
};



void Miner::Result(){
	cout << endl << BOLDYELLOW << "[+] " << BOLDGREEN << CaveCount << BOLDYELLOW << " Caves found.\n";
	int i = 0;
	while(cave != NULL){
		cout << BOLDYELLOW << "\n[#] Cave " << i << endl;
		cout << BOLDYELLOW << "[*] Section: " << BOLDBLUE << cave->Section << endl;
		cout << BOLDYELLOW << "[*] Start Address: " << BOLDBLUE << cave->StartAddr << endl;
		cout << BOLDYELLOW << "[*] End Address: " << BOLDBLUE << cave->EndAddr << endl;
		
		cave = cave->Next; 
	}


	
}


void Miner::StartMiner(fstream * File){
	int CurrentOfset = 0;
	int NullCount = 0;

	File->seekg(0, File->end);
	int FileSize = File->tellg();
	File->seekg(0, File->beg);

	char * Cursor = new char[FileSize];
	File->read(Cursor,FileSize);
	cout << RESET << BOLDYELLOW << "[*] Starting miner...\n";
	for(int i = 0; i < FileSize; i++){
		
		CurrentOfset++;
		if(Cursor[i] == 0x00){
			NullCount++;
		}
		else{
			if(NullCount > 100){
				Cave * NewCave = new Cave;
				CaveCount++;
				cout << RESET << BOLDGREEN << "[+] New cave detected !\n";
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
	cout << RESET << BOLDYELLOW << "[*] Mining finished.\n";
}




fstream * Miner::LoadPE(string FileName){
	fstream File;
	cout << RESET << BOLDYELLOW << "[*] Loading PE file...\n";
	File.open (&FileName[0], ios::out | ios::in | ios::binary);
	if(!(File.is_open())){
		cout << RESET << BOLDRED << "[-] ERROR : " << RED << "Cannot open file !" << endl;
		exit(1);		
	}

	string Backup = "cp " + FileName + " " + FileName + ".bak";
	system(&Backup[0]);

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



void Miner::GetFileData(string FileName){

	cout << RESET << BOLDYELLOW << "[*] Extracting file header data...\n"	;
	string MagicNum = "objdump -x ";
	MagicNum += FileName;
	MagicNum += " | grep 'Magic'";

	string ImageBase = "objdump -x ";
	ImageBase += FileName;
	ImageBase += " | grep 'ImageBase' >> file.dat";

	string StartAddr = "objdump -x ";
	StartAddr += FileName;
	StartAddr += " | grep 'start address' >> file.dat";




	string Ls = "ls ";
	Ls += FileName;

	int FileFound = system(&Ls[0]);
	if(FileFound != 0) {
		cout << RESET << BOLDRED << "[-] ERROR : " << RED << "No such file !" << endl;
		exit(1);
	}

	int IsPE = system(&MagicNum[0]);
	if(IsPE != 0){
		cout << RESET << BOLDRED << "[-] ERROR : " << RED << "File is not a valid PE32 !" << endl;
		exit(1);
	}

	int ImageBaseFound = system(&ImageBase[0]);
	if(ImageBaseFound != 0){
		cout << RESET << BOLDRED << "[-] ERROR : " << RED << "Unable to find image base !" << endl;
		exit(1);
	}

	int StartAddrFound = system(&StartAddr[0]);
	if(StartAddrFound != 0){
		cout << RESET << BOLDRED << "[-] ERROR : " << RED << "Unable to find start address !" << endl;
		exit(1);
	}	

	fstream DataFile;
	string Line;

	DataFile.open("file.dat");
	if(DataFile.is_open()) {
		getline(DataFile, Line);
		for(int i = 11; i < 25; i++){
			pe.ImageBase += Line[i]; 
		}
		
		cout << BOLDYELLOW << "[*] Image Base: " << BOLDBLUE << pe.ImageBase << endl;
		
		getline(DataFile, Line);
		for(int i = 14; i < 25; i++){
			pe.StartAddr += Line[i]; 
		}

		cout << BOLDYELLOW << "[*] Start Address: " << BOLDBLUE << pe.StartAddr << endl;
	}
	system("rm file.dat");


}

void Miner::ParseFileSections(string FileName){

	cout << RESET << BOLDYELLOW << "[*] Parsing file sections...\n";
	string _Sections = "objdump -h ";
	_Sections += FileName;
	_Sections += " | grep '2**2' > sec.dat";

	system(&_Sections[0]); // objdump -h | grep '2**2' > sec.dat

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
	Section * temp = pe.Sections;
	while(temp != NULL){
		cout << BOLDYELLOW << "[>] " << BOLDGREEN << temp->Name << endl;
		temp = temp->Next;
	}
	cout << RESET << BOLDYELLOW << "[*] Section parsing complete.\n";

	system("rm sec.dat");

}