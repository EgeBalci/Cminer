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

struct Section { // Section data structure with linked list...
	string Name; // Sections name
	int FileOfset; // File ofset of the section
	int StartAddr; // Start address of the section 
	int EndAddr;// End address of the section 
	int size; // Size of the section
	Section * Next = NULL; // Next section data link
};


struct Cave { // Cave data structure with linked list...
	int StartAddr = 0; // Start address of the code cave (file ofset)
	int EndAddr = 0; // End address of the code cave (file ofset)
	int CaveSize = 0; // Size of the code cave
	int FileOfset = 0; // File ofset of the cave
	string Section = ""; // The section whitch contains this code cave
	Cave * Next = NULL; // Next code cave data link
};


struct PE { // This is a PE structure for parsing and storing given file data
	string ImageBase; // Base address of the PE image
	string StartAddr; // Start address of the PE image
	int SectionNum = 0; // Number of sections inside PE image
	Section * Sections = NULL; // Pointer to first section structure
	int FileSize; // Size of the PE image

};


class Miner{
private:
	int CaveCount = 0; // Number of code caves discoverd
	Cave * cave = NULL; // Pointer to the first code cave structure in linked list
	PE pe; // Define a PE structure for storing data
public:
	void GetFileData(string FileName); // Gathers PE file data via "objdump -x" & "grep" command
	void ParseFileSections(string); // Parse the start & end addresses of PE image sections
	char * LoadPE(string); // Load the given named file
	void StartMiner(char*,int); // Start tracing the file byte stream for NULL bytes
	void EnumCaveLoc(Cave*); // Find out cave belongs witch section
	void Result(); // Prints the result...
};



void Miner::Result(){
	cout << endl << BOLDYELLOW << "[+] " << BOLDGREEN << CaveCount << BOLDYELLOW << " Caves found.\n";
	int i = 1;
	Cave * temp = cave;
	while(temp != NULL){

		stringstream SS1;
		stringstream SS2;
		stringstream SS3;

		cout << BOLDGREEN << "\n[#] Cave " << i << endl;
		
		cout << BOLDYELLOW << "[*] Section: " << BOLDBLUE << temp->Section << endl;		
		cout << BOLDYELLOW << "[*] Cave Size: " << BOLDBLUE << temp->CaveSize << BOLDYELLOW << " byte." << endl;

		cout << BOLDYELLOW << "[*] Start Address: ";
		string HexStartAddr;
		SS1 << hex << temp->StartAddr;
		SS1 >> HexStartAddr;
		cout << BOLDBLUE << "0x" << HexStartAddr << endl;

		cout << BOLDYELLOW << "[*] End Address: ";
		string HexEndAddr;
		SS2 << hex << temp->EndAddr;
		SS2 >> HexEndAddr;
		cout << BOLDBLUE << "0x" << HexEndAddr << endl;

		cout << BOLDYELLOW << "[*] File Ofset: ";
		string FileOfset;
		SS3 << hex << temp->FileOfset;
		SS3 >> FileOfset;
		cout << BOLDBLUE << "0x" << FileOfset << endl;
		i++;
		temp = temp->Next; 
	}
 

	
}


void Miner::StartMiner(char * Cursor, int MinCaveSize){
	int CurrentOfset = 0;
	int NullCount = 0;

	cout << RESET << BOLDYELLOW << "[*] Starting cave mining process...\n";

	for(int i = 0; i < pe.FileSize; i++){

		
		if(Cursor[i] == (char)0x00){ // Current ofset = i
			NullCount++;
		}
		else{
			if(NullCount > MinCaveSize){
				Cave * NewCave = new Cave;
				CaveCount++;
				cout << RESET << BOLDGREEN << "[+] New cave detected !\n";
				NewCave->CaveSize = NullCount;
				NewCave->StartAddr = (i-NullCount);
				NewCave->EndAddr = i;
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




char * Miner::LoadPE(string FileName){
	fstream File;
	cout << RESET << BOLDYELLOW << "[*] Loading PE file...\n";
	File.open (&FileName[0], ios::out | ios::in | ios::binary);
	if(!(File.is_open())){
		cout << RESET << BOLDRED << "[-] ERROR : " << RED << "Cannot open file !" << endl;
		exit(1);		
	}

	string Backup = "cp " + FileName + " " + FileName + ".bak";
	system(&Backup[0]);

	File.seekg(0, File.end);
	int FileSize = File.tellg();
	File.seekg(0, File.beg);


	
	char * Cursor = new char[FileSize];

	for(int i = 0; i < FileSize; i++){
		File.get(Cursor[i]);
	}
	pe.FileSize = FileSize;
	

	cout << RESET << BOLDYELLOW << "[*] File Size: " << BOLDBLUE << FileSize << "\n";

	return Cursor;
}

void Miner::EnumCaveLoc(Cave * _Cave){
	Section * Sec = pe.Sections;
	for(int i = 0; i < pe.SectionNum; i++) {
		if((Sec->FileOfset < _Cave->StartAddr) && (_Cave->StartAddr < (Sec->FileOfset + Sec->size))) {
			_Cave->Section = Sec->Name;
			_Cave->FileOfset = _Cave->StartAddr;
			_Cave->StartAddr = (Sec->StartAddr + (_Cave->StartAddr - Sec->FileOfset)); // Calculating the VMA(Virtual Memory Address) of the code cave
			_Cave->EndAddr = (_Cave->StartAddr + _Cave->CaveSize);
			break;
		}
		
		Sec = Sec->Next;
	}
	if(_Cave->Section == ""){
		_Cave->Section = "No Section.";
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
		for(int i = 11; i < (sizeof(Line)-1); i++){
			pe.ImageBase += Line[i]; 
		}
		
		cout << BOLDYELLOW << "[*] Image Base: " << BOLDBLUE << pe.ImageBase << endl;
		
		getline(DataFile, Line);
		for(int i = 14; i < (sizeof(Line)-1); i++){
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
	_Sections += " | grep '*' > sec.dat";

	system(&_Sections[0]); // objdump -h | grep '*' > sec.dat

	fstream SectionData;
	string Line;

	SectionData.open("sec.dat");
	if(SectionData.is_open()){
		while(getline(SectionData, Line)){

			string _Line = Line;

			string SecName = "";
			string SecSize = "";
			string Start = "";
			string Ofset = "";

			Section * NewSection = new Section;
			if(pe.Sections == NULL){
				pe.Sections = NewSection;
			}
			else{
				NewSection->Next = pe.Sections;
				pe.Sections = NewSection;
			}


			SecName = Line.substr(4,11);
			
			NewSection->Name = SecName;

			Line = Line.substr(18,(sizeof(Line)-6));
			
			SecSize = Line.substr(0,Line.find("  "));

			stringstream SS1;
			SS1 << hex << SecSize;
			SS1 >> NewSection->size;

			Line = Line.substr((Line.find("  ")+2),sizeof(Line));

			Start = Line.substr(0,Line.find("  "));

			stringstream SS2;
			SS2 << hex << Start;
			SS2 >> NewSection->StartAddr;
			NewSection->EndAddr = (NewSection->StartAddr + NewSection->size);


			Ofset = _Line.substr((_Line.length()-14),(_Line.length()-6));

			stringstream SS3;
			SS3 << hex << Ofset;
			SS3 >> NewSection->FileOfset;
						
			pe.SectionNum++; 
			cout << NewSection->Name << " Size: " << NewSection->size << " Start: " << NewSection->StartAddr << " End: " << NewSection->EndAddr << " File Ofset: " << NewSection->FileOfset <<  " Ofset Line:" << Ofset << ":" << endl;
		}
	}
	Section * temp = pe.Sections;
	while(temp != NULL){
		string Start;
		string End;
		stringstream SS1;
		stringstream SS2;
		cout << BOLDYELLOW << "[>] " << BOLDGREEN << temp->Name << BOLDBLUE << "(";
		SS1 << hex << temp->StartAddr;
		SS1 >> Start;
		cout << "0x" << Start << "/";
		SS2 << hex << temp->EndAddr;
		SS2 >> End;
		cout << "0x" << End << ")"  << endl;
		temp = temp->Next;
	}
	cout << RESET << BOLDYELLOW << "[*] Section parsing complete.\n";

	system("rm sec.dat");

}