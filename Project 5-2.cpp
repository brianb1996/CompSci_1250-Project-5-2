//Brian Bredahl
//October 22, 2019
//Project 5-2
//File Stream Random.txt


/*
This program aims to surpass the following task


The attached file contains a long list of random numbers (Random.txt).

1. Copy the file to your hard drive.

2. Then write a program that asks user to enter the file name to open and validates that the file is successfully open.

3. Once the user opens the file (Random.txt) successfully, your program should calculate the following:(The output should be in the following format)

		  A) The  number of  even numbers in the file:

		  B) The  number of  odd numbers in the file:

		  C) The number of numbers in the file:

		  D) The sum of all the numbers in the file:

		  E) The average of all the numbers in the file:
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stdlib.h>



using namespace std;

typedef vector<string> strivec; // vector made of strings
typedef vector<int> intvec;

struct FileData {
	strivec DrivesFound; // used to store the drives used for storage
	strivec FilesDirectoriesAvailable; // used to store availabe file directories to choose from
	strivec FilesAvailable; // used to store available files to choose from
	strivec FILEPATH; // stores the current file path in a vector
	string filepath; // the vector above is used to create a string so that the current filepath can be searched or used for file stream operations
}FileInfo;

/*
decided not to create a class to search file directories for a file may do in free time

class FileName {
private:
	string FilePath;
	string nextFolder;
public:
	string FinalPath;
	void addFolder() {
		FilePath = FilePath + nextFolder;
	}
	void addPath(string adder) {
		nextFolder = adder;
		addFolder;
	}
	void FinalFilePath() {
		FinalPath = FilePath;
	}
}UserFilePath;
*/
/*
ToDo list
	
	- Add more notes

*/

int DriveFinder() {
	char drive[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	string DRIVE = "";// used to convert char to string for filefinder program
	int i = 0;
	do {
		DRIVE = drive[i]; // assigns a letter to the string drive -->> used to check each letter for the drives available
		DRIVE.append(":/*"); // adds :/* to the string DRIVE --> used to search if the drive with the letter designated exist and if it has content
		WIN32_FIND_DATA drivedata; // holds information about the drives that were searched for ie name, size, file attribute
		HANDLE DriveFind; //Handle used to point to the location of the drive being searched
		DriveFind = FindFirstFile(DRIVE.c_str(), &drivedata); // will output FFFFFFF if the Folder does not exist 
		if (DriveFind != INVALID_HANDLE_VALUE /*is FFFFFF*/) {
			DRIVE.pop_back(); // removes *
			DRIVE.pop_back(); // removes /
			FileInfo.DrivesFound.push_back(DRIVE); // stores the name of the drive to the string vector DrivesFound
		}
		FindClose(DriveFind); // closes the handle
		DRIVE.clear(); // clears the drive string
		i++; //used to count to 25
	} while (i <= 25); // exits the loop once it reaches 25
	int vectorsize = FileInfo.DrivesFound.size();// stores the sizes of the vector DrivesFound
	return vectorsize; // returns the integer vectorsize
}

void DriveSelector(int options) {
	int selection, i; // selection is used to store the input from the user, i as a counter to exit the for loop once all drives found have been displayed to the user
	bool error; // used to indicate if a invalid selection has been input. true if input is invalid. false if valid input
	string error_catcher; // used to clear buffer when an invalid input has been detected. stores the invalid input as a string 
	system("CLS"); // clears the screen for a nice looking output
	cout << "\n\n\nPlease select the drive that the file is stored in." << endl << endl;
	for (i = 0; i < options; i++) {
		cout << "\tFor Drive\t" << FileInfo.DrivesFound.at(i) << "\tpress\t" << i+1 << endl; // using i+1 because a I need to allow for error catching
	}

// Error catching loop found below. If input doesn't meet conditions the program will ask for a new input
	do {
		error = false;
		cout << "\nPlease enter your selection now: ";
		cin >> selection; // input from user --> user inputs a number that is correlated to the drive 

		if (selection == 0 || selection > options ) { // if the input is not within the stated range a the if statement with execute
			error = true; // used in do-while loop so the user may enter a valid input
			cout << "invalid input please try again" << endl; // output to user
			cin.clear(); // clears cin object
			getline(cin, error_catcher); // removes anything in the buffer
		}
	} while (error == true);

	FileInfo.FILEPATH.push_back(FileInfo.DrivesFound.at(selection-1)); // stores the string from the drive selected to the filepath vector
}

/*
	The File Finder function looks for files located within the current filepath
	When there are no more files the file exits after returning the number of elements to the main function
*/
int FileFinder() {
	FileInfo.FilesDirectoriesAvailable.clear(); // clears the vector FileDirectoriesFound so that new file directories can be stored in the vector
	FileInfo.FilesAvailable.clear(); // clears the vector FilesAvailable so that files in the current directory can be selected 
	string fileNAME = FileInfo.filepath; // string of current filepath being searched
	WIN32_FIND_DATA filedata; // used to store information about files in the current file path
	HANDLE FileFind; // points to the location of the current file path and the files associated with it
	FileFind = FindFirstFile(fileNAME.c_str(), &filedata); // will output FFFFFFF if the Folder does not exist		
	do {
		if (FileFind != INVALID_HANDLE_VALUE) {
			//cout <<filedata.cFileName << endl <<filedata.dwFileAttributes <<endl << endl;
			if (filedata.dwFileAttributes == 32) { // if the file attribute is 32 it is a file 
				FileInfo.FilesAvailable.push_back(filedata.cFileName); // stores name of the file found to the FilesAvailable vector
			}
			else if (filedata.cFileName == ".." || filedata.dwFileAttributes == 16 || filedata.dwFileAttributes == 17 || filedata.dwFileAttributes == 18){ // these file attributes are directories or sub-directories that can generally be accessed although not all can
				FileInfo.FilesDirectoriesAvailable.push_back(filedata.cFileName); // stores the name of the file directory found to the vector FileDirectoriesAvailable
			}
		}
	} while (FindNextFile(FileFind, &filedata) != 0);
	FindClose(FileFind); // closes the current handle
	if (FileInfo.FilesDirectoriesAvailable.empty()) {// if nothing is found -1  will be return to indicate that the contents cant be accessed
		return -1;
	} else {
		int vectorsize = FileInfo.FilesDirectoriesAvailable.size(); // used to store the number of file directories or sub-directories found
		return vectorsize;
	}
}

/*
FileSelector allows the user to select a file or file directory based on what was found in the current filepath
if the files cant be access the user will be notified
*/
bool FileSelector(int options) { // int options is the size of the vector FileDirectoriesAvailable
	system("CLS"); // clears screen for nice looking output
	cout << "Current File Path:\n" << FileInfo.filepath << endl << endl; // displays current filepath
	if (options == -1) { // if files cant be accessed
		string CantFile;
		CantFile = FileInfo.filepath;
		cout << "Unable to access files in " << CantFile << endl; // ouputs the path that cant be accessed
		FileInfo.FILEPATH.pop_back(); // eliminates the last selection made because the file cant be accessed allowing the user to select an new filepath
		if (FileInfo.FILEPATH.empty()) { // if no filepath currently exist the user will be sent to the drive to select the drive they would like to search
			DriveSelector(FileInfo.DrivesFound.size());
		}
		FileInfo.FilesDirectoriesAvailable.clear(); // clears files directories currently found so
		return false; // returns false to indicate that a file wasn't selected
	} else {
		int selection; // used to store the selction the user inputs
		bool error; // used to indicate if an invalid input had been made
		string error_catcher; // clears buffer so user can enter a valid input
		cout << "Please select the file directory that the file is stored in" << endl << endl;
		for (int i = 1; i < options; i++) { // used to display the file directories found
			if (FileInfo.FilesDirectoriesAvailable.at(i) == "..") {
				cout << "\t\tPress\t" << i << "\tfor\t<--Back" << endl; // allows the user to go backwards in the filepath
			}
			else {
				cout << "\t\tPress\t" << i << "\tfor\t" << FileInfo.FilesDirectoriesAvailable.at(i) << endl;
			}
		}
		int counter, helper; // counter is used to output the files found in the appropriate order, helper is used to display the number the user would enter to select the file they wish to evaluate
		helper = FileInfo.FilesAvailable.size() + options;
		if (helper > options) { // if files are found the files will be displayed
			cout << "\n\t\t\tOR\n\n Select your File from the list \n\n";

			for (int i = options, counter = 0; i < helper; i++, counter++) {
				cout << "\t\tPress\t" << i << "\tfor\t" << FileInfo.FilesAvailable.at(counter) << endl;
			}
		}
		// used for error catching invalid selections made by the user
		do {
			error = false; 
			cout << "\nPlease enter your selection now: ";
			cin >> selection;
			if (selection == 0 || selection > helper) { // if the input is out of range the program will loop allowing the user to input a valid selection
				error_catcher = true; 
				cin.clear();
				getline(cin, error_catcher);
				cout << "Error invalid selection" << endl << endl;
			}
		} while (error == true);
		if (selection < options) { // if the number input is only associated with the file directories the code below will execute
			if (FileInfo.FilesDirectoriesAvailable.at(selection) == "..") { // if the user chooses to go backwards in the file path
				FileInfo.FILEPATH.pop_back(); // eliminates the last entry made into the filepath
				FileInfo.FilesDirectoriesAvailable.clear(); // clears the filedirectories found
			}
			else {
				FileInfo.FILEPATH.push_back(FileInfo.FilesDirectoriesAvailable.at(selection)); // will append the file directory selected to the filepath vector
			}
		}
		else {
			selection = selection - options; // if the number selected is greater than the the numbers associated with file directories a file has been selected
			FileInfo.FILEPATH.push_back(FileInfo.FilesAvailable.at(selection)); //stores the file to the filepath vector
			return true; // returns true to indicate that a file has been selected
		}
	}
	return false; // returns false to indicate that a file wasnt selected
}

//Used to turn the vector FILEPATH into a string and is used to search the directories
void FilePath(bool exit) {
	int vectorsize;
	FileInfo.filepath.clear();
	vectorsize = FileInfo.FILEPATH.size();
	if (exit) { // if exit is true a, file was selected, and the string will be formated so that the user can stream the contents of the file into the program

		FileInfo.filepath.append(FileInfo.FILEPATH.at(0));
		for (int i = 1; i < vectorsize; i++) {
			FileInfo.filepath.append("\\");
			FileInfo.filepath.append(FileInfo.FILEPATH.at(i));
		}

	}
	else { // used to create a  string for the filepath that allows the user to view the contents of each file
		for (int i = 0; i < vectorsize; i++) {
			FileInfo.filepath.append(FileInfo.FILEPATH.at(i));
			FileInfo.filepath.append("/");
		}
		FileInfo.filepath.append("*");
	}
}

/*
this function allows the user to manually input the file path

*/
void user_input_path() {
	string userInput;
	cin.clear();
	cin.ignore();
	cout << "Please input the file path: " << endl;
	getline(cin, userInput);
	FileInfo.filepath = userInput;
}

/*
File_Search_Path is used to allow the user to select the file path from the options found on the computer, once a file is found the
function will exit
*/
void File_Search_Path(int option) {
	int selection; // options is used to associate a number with the vector containing file names //selection is the number the user picks based off the options 
	bool exit = false; // used to exit the loop used to explore files

	 // options is set to the value returned from the DriveFinder Function -->> drive finder is used to search the computer for the available drives to search

	DriveSelector(option); // drive selector takes input from user and returns a number based on the selction -->> number is associated to the spot the string is stored in the vector
	FilePath(exit);

	do {
		option = FileFinder(); // options is the number of file directiories found while searching
		exit = FileSelector(option); // exit indicates if a file was selected or a) a error occured or b) a file directory was selected (true if a file was selected, false otherwise) 
		FilePath(exit); // goes to create a string form of the file path

	} while (exit == false); // will exit if a file was selected
	
}

/*
FileEval is used to evaluate the contents of a file and to organize the data into the categories established from the prompt given
the outputs are displayed to the user so they can see the result of the file stream.
*/
bool FileEval() {
	system("CLS");
	int fromfile = 0;
	int total = 0, avg;
	intvec data_in_file;
	intvec evennums;	// not needed, but used to store even numbers to a vector
	intvec oddnums;		// not needed, but used to store odd numbers to a vector
	int NumOfOdds = 0, NumOfEvens = 0, TotalNums = 0;
	bool good_file_path = true; // if file open the path was good, if file doesnt open the path was bad
	fstream file_to_use; // used for filestreaming the contents of a file
	file_to_use.open(FileInfo.filepath); // opens the filepath indicated by the user

	if (file_to_use) { // if the file opens the contents will be evaluated
		while (file_to_use >> fromfile) {
			data_in_file.push_back(fromfile);
			TotalNums++; // adds 1 every time a even number is discovered( is pretty much a counter
			if (fromfile % 2 == 0) {
				evennums.push_back(fromfile);
				NumOfEvens++;// adds 1 every time a odd number is dicovered(is pretty much a counter)
			} else {
				oddnums.push_back(fromfile);
				NumOfOdds++;
			}
			total += fromfile; // sums the all the numbers of the file
		}
		avg = (total) / (data_in_file.size() + 1);
		cout << "\n\tThe number of even numbers in the file: " << NumOfEvens << endl << endl;
		cout << "\tThe number of odd numbers in the file: " << NumOfOdds << endl << endl;
		cout << "\tThe number of numbers in the file: " << TotalNums << endl << endl;
		cout << "\tThe sum of all numbers in the file: " << total << endl << endl;
		cout << "\tThe average of all the numbers in the file: " << avg << endl << endl;
	} else {
		good_file_path = false;
		cout << endl << endl << "The File Path indicated does not exist or can't be accessed" << endl << endl;
	}
	file_to_use.close();
	return good_file_path;
}

/*
Is_this_correct_file_path allows the user to view the file path and indicate if that filepath is correct
*/
bool Is_this_correct_file_path() {
	system("CLS");
	char YorN;
	string error_catcher;
	bool value_to_return, error;
	cout << endl << "The File Path Selected is:" << endl << endl;
	cout << FileInfo.filepath << endl << endl;
	cout << "Is this the correct file Path you would like to use?" << endl << endl;
	do {
		cout << "Press [ Y ] to continue\t\tPress[ N ] to input new filepath" << endl;
		cout << "Please input your selection now: ";
		cin >> YorN;
		switch (YorN) {
		case 'Y':
		case 'y':
			error = false;
			value_to_return = false;
			break;
		case'N':
		case'n':
			error = false;
			value_to_return = true;
			break;
		default:
			getline(cin, error_catcher);
			cin.clear();
			cout << endl << endl << "\t\tERROR: Invalid Input" << endl << endl;
			error = true;
		}
	} while (error == true);

	return value_to_return;
}

int main() {
	int input, option = 0 ; // input is associated with the users selection to input a filepath manually or to use the filesearcher
	bool error = false; // used to loop if a invalid input has been made or if a file doesnt open
	bool is_correct_path; // used to loop if the user indicates they entered or selected the wrong filepath
	string error_catcher; // used to store the invalid inputs from the user to a string so the buffer is clear
	cout << "This program evaluates the contents of a file" << endl << endl
		<< "Would You like to input the file path manually or use the file searcher to find the file?" << endl;
	do {
		do {
			FileInfo.FilesAvailable.clear();			//	<---|
			FileInfo.FilesDirectoriesAvailable.clear();	//	<---|----clears the strings and vectors so a new filepath can be entered
			FileInfo.filepath.clear();					//	<---|
			FileInfo.FILEPATH.clear();					//	<---|

			do { // loop used to allow the user to enter a valid input
				cout << "Press [ 1 ] to enter the file manually" << endl;
				cout << "Press [ 2 ] to use the file searcher" << endl;
				cout << "Press [ 3 ] to exit" << endl;
				cout << "please input your selection now: ";
				error = false;
				cin >> input;
				switch (input) {
				case 1: user_input_path();
					break;
				case 2:
					if (FileInfo.DrivesFound.empty() == true) {
						option = DriveFinder();
					}
					File_Search_Path(option);
					break;
				case 3:
					exit(0);
					break;
				default:
					cin.clear();
					getline(cin, error_catcher);
					system("CLS");
					cout << "ERROR: Invalid Input" << endl;
					error = true;
				}
			} while (error == true);
			error_catcher.clear();
			// returns false if the correct path is choosen
			is_correct_path = Is_this_correct_file_path();
			
		} while (is_correct_path == true);
		error = FileEval();


	} while (error == false);
	return 0;
}


/*
notes/ideas

give the user the option to open file stored in the project folder or to type in a file path for a file stored in another folder.
switch
case "user wants to open file stored in the project folder":
		link to function to open file stored in the project folder
case "user want to input the file path for a file stored in another folder"
		link to function to open a function based on what the user inputs
			- ask for name/letter of drive that the file is located on
			- ask for the the next file name
			- continue to ask for the next folder until the user enters a file name ie an extension .txt .jpg .doc etc.
			
output to user to input file path where to where the file is stored

ask user if the filepath is correct

if user selected the correct file path then the data from the file will be evaluated
*/

