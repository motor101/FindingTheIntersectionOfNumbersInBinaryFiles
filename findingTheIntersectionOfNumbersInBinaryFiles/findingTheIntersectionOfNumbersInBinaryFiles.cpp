/**
*
* Solution to homework task
* Data Structures Course
* Faculty of Mathematics and Informatics of Sofia University
* Winter semester 2016/2017
*
* @author Kamen Ivanov
* @idnumber 61894
* @task 5
* @compiler VC
*
*/

/*
This source file defines some functions, which are used to find the common
elements in a set of binary files of integers. These common elements are then
written to a binary file in the current directory.

The algorithm that we'll use to accomplish this goal is as follows:

1)Check every binary file in the set and find the one that has the minimal size.
(i.e.) the binary file that has the least number of elements (integers)

2)Scan the elements of the minimal file one by one and put them in a list.
This list will be used as a dictionary for the search operations.

3)For every other binary file in the set of files:
- scan the elements of the file one by one
- for every read element search if it is in the list from step 2
- if the element is in the list, mark that using a bool array
- scan the elements in the list and remove the elements which weren't
found in the file

4)Create a binary file in the current directory and write the remaining
elements in the list onto that file

*/


#include<iostream>
#include<fstream>
#include<new>
#include<cstdint>	//uint64_t
#include<exception>

using namespace std;

//str must be equal to NULL
//The function reads characters from the input and
//appends them to the string while newline or EOF
//is encountered. The newline is extracted from the stream,
//but isn't appended to the string. 
//The new string is null terminated
void enterStringFromStandardInput(char *&str);

//sorts the elements of arr in the interval [left, right) in ascending order
void mergeSort(uint64_t *arr, int left, int right, uint64_t *tempArr);

bool findingTheIntersectionOfNumbersInBinaryFiles();

static void deleteDinamicallyAllocatedMemory
(char **listOfFileNames, int numberOfBinaryFiles
, uint64_t *listOfCommonIntegers, uint64_t *tempList, bool *tableOfFoundIntegersInTheListOfCommonIntegers);

//opens all files in the list and finds the one with the minimal size
//the values of minSizeOfFile and indexOfFileWithMinNumberOfElements
//are updated
static bool findInfoAboutFileWithMinNumberOfElements
(char **listOfFileNames, int numberOfBinaryFiles
, int &minSizeOfFile, int &indexOfFileWithMinNumberOfElements);

//every integer that the file contains is added to the listOfCommonIntegers
static bool readElementsFromFileAndPutThemInList
(uint64_t *listOfCommonIntegers, const char* binaryFileName, int numberOfElements);

static void removeDuplicatingElementsFromList(uint64_t *listOfCommonIntegers, int &numberOfElements);

//searches for the integer in the interal [left, right]
//if found, the value of the parallel element in the bool array
//if set to true
static bool binarySearchForIntegerInTheListOfCommonIntegers
(uint64_t *listOfCommonIntegers, int left, int right
, bool *tableOfFoundIntegersInTheListOfCommonIntegers, uint64_t integerThatWeAreLookingFor);

static void removeElementsWhichWereNotFoundFromTheListOfCommonIntegers
(uint64_t *listOfCommonIntegers, int &numberOfElements
, bool *tableOfFoundIntegersInTheListOfCommonIntegers);

static bool readBinaryFileAndUpdateListOfCommonIntegers
(const char *binaryFileName, uint64_t *listOfCommonIntegers
, int &numberOfElements, bool *tableOfFoundIntegersInTheListOfCommonIntegers);

//creates a binary file called binaryFileName, and writes the values of all integers
//from the list in the file
static bool writeTheIntegersFromTheListOntoTheBinaryFile
(const char *binaryFileName, const uint64_t *listOfCommonIntegers, int numberOfElements);

//////////////////////////////////////////////////////////////////////////////////////////////
//definitions

static void deleteDinamicallyAllocatedMemory
(char **listOfFileNames, int numberOfBinaryFiles
, uint64_t *listOfCommonIntegers, uint64_t *tempList, bool *tableOfFoundIntegersInTheListOfCommonIntegers)
{
	for (int i = 0; i < numberOfBinaryFiles; ++i)
		delete[] listOfFileNames[i];

	delete[] listOfFileNames;
	delete[] listOfCommonIntegers;
	delete[] tempList;
	delete[] tableOfFoundIntegersInTheListOfCommonIntegers;
}

//opens all files in the list and finds the one with the minimal size
//the values of minSizeOfFile and indexOfFileWithMinNumberOfElements
//are updated
static bool findInfoAboutFileWithMinNumberOfElements
(char **listOfFileNames, int numberOfBinaryFiles
, int &minSizeOfFile, int &indexOfFileWithMinNumberOfElements)
{
	//this variable will store the size of the current file
	int sizeOfFile;

	ifstream binaryFile;

	for (int i = 0; i < numberOfBinaryFiles; ++i)
	{
		binaryFile.open(listOfFileNames[i], ios::in | ios::binary);
		
		if (! binaryFile.is_open())
		{
			cerr << "could not open " << listOfFileNames[i] << " for reading\n";
			return false;
		}

		binaryFile.seekg(0, ios::end);

		sizeOfFile = binaryFile.tellg();

		//the file contains only the binary values ot elements of type uint64_t
		//so the size of the file should be equal to the size of uint64_t
		if (sizeOfFile % sizeof(uint64_t) != 0)
		{
			cerr << "the content of " << listOfFileNames[i] << " is incorrect\n";
			binaryFile.close();
			return false;
		}


		if (sizeOfFile < minSizeOfFile || i == 0)
		{
			minSizeOfFile = sizeOfFile;
			indexOfFileWithMinNumberOfElements = i;
		}

		binaryFile.close();
	}

	return true;
}


//every integer that the file contains is added to the listOfCommonIntegers
static bool readElementsFromFileAndPutThemInList
(uint64_t *listOfCommonIntegers, const char* binaryFileName, int numberOfElements)
{
	ifstream binaryFile(binaryFileName, ios::in|ios::binary);

	if (!binaryFile)
	{
		cerr << "could not open " << binaryFileName << " for reading\n";
		return false;
	}

	for (int i = 0; i < numberOfElements; ++i)
	{
		if (!binaryFile)
		{
			cerr << "the content of " << binaryFileName 
				 << " has been changed during the execution of the program\n";
			return false;
		}
		binaryFile.read((char*)&listOfCommonIntegers[i], sizeof(uint64_t));
	}

	return true;
}

static void removeDuplicatingElementsFromList(uint64_t *listOfCommonIntegers, int &numberOfElements)
{
	uint64_t lastIntegerRead;
	
	if (numberOfElements < 1)
		return;

	lastIntegerRead = listOfCommonIntegers[0];

	//the element on position 0 will surely remain in the list
	//we won't overwrite it
	int currentPositionForWritingInTheList = 1;
	
	//the element on position 0 will surely remain in the list
	//we won't overwrite it
	//That's why we start the scanning from the element with index 1
	for (int i = 1; i < numberOfElements; ++i)
	{
		if (lastIntegerRead != listOfCommonIntegers[i])
		{
			lastIntegerRead = listOfCommonIntegers[i];

			listOfCommonIntegers[currentPositionForWritingInTheList] = listOfCommonIntegers[i];

			++currentPositionForWritingInTheList;
		}
	}

	numberOfElements = currentPositionForWritingInTheList;
}

//searches for the integer in the interal [left, right]
//if found, the value of the parallel element in the bool array
//if set to true
static bool binarySearchForIntegerInTheListOfCommonIntegers
(uint64_t *listOfCommonIntegers, int left, int right
, bool *tableOfFoundIntegersInTheListOfCommonIntegers, uint64_t integerThatWeAreLookingFor)
{
	if (right < left)
		return false;

	int middle = (left + right) / 2;

	if (listOfCommonIntegers[middle] < integerThatWeAreLookingFor)
	{
		return binarySearchForIntegerInTheListOfCommonIntegers
			(listOfCommonIntegers, middle + 1, right
			, tableOfFoundIntegersInTheListOfCommonIntegers, integerThatWeAreLookingFor);
	}
	else
	{
		if (integerThatWeAreLookingFor < listOfCommonIntegers[middle])
		{
			return binarySearchForIntegerInTheListOfCommonIntegers
				(listOfCommonIntegers, left, middle - 1
				, tableOfFoundIntegersInTheListOfCommonIntegers, integerThatWeAreLookingFor);
		}
		else //the integer is found
		{
			tableOfFoundIntegersInTheListOfCommonIntegers[middle] = true;
			return true;
		}
	}
}

static void removeElementsWhichWereNotFoundFromTheListOfCommonIntegers
(uint64_t *listOfCommonIntegers, int &numberOfElements
, bool *tableOfFoundIntegersInTheListOfCommonIntegers)
{
	int lastFreePositionInTheList = 0;

	for (int i = 0; i < numberOfElements; ++i)
	{
		//if the element with index i was found in the file,
		//than save it (don't remove it from the list)
		if (tableOfFoundIntegersInTheListOfCommonIntegers[i] == true)
		{
			listOfCommonIntegers[lastFreePositionInTheList++] = listOfCommonIntegers[i];
		}
	}

	numberOfElements = lastFreePositionInTheList;

	//make the boolean table ready for the next search operation
	for (int i = 0; i < lastFreePositionInTheList; ++i)
		tableOfFoundIntegersInTheListOfCommonIntegers[i] = false;
}

static bool readBinaryFileAndUpdateListOfCommonIntegers
(const char *binaryFileName, uint64_t *listOfCommonIntegers
, int &numberOfElements, bool *tableOfFoundIntegersInTheListOfCommonIntegers)
{
	ifstream binaryFile(binaryFileName, ios::in | ios::binary);

	if (!binaryFile)
	{
		cerr << "could not open " << binaryFileName << " for reading\n";
		return false;
	}

	//we'll store the elements of the binary file in this buffer,
	//as we read them one by one
	uint64_t integerBuffer;

	while (true)
	{
		binaryFile.read((char*) &integerBuffer, sizeof(uint64_t));
		
		if (!binaryFile)
			break;
		
		binarySearchForIntegerInTheListOfCommonIntegers
			(listOfCommonIntegers, 0, numberOfElements-1
			, tableOfFoundIntegersInTheListOfCommonIntegers, integerBuffer);
	}

	removeElementsWhichWereNotFoundFromTheListOfCommonIntegers
		(listOfCommonIntegers, numberOfElements
		, tableOfFoundIntegersInTheListOfCommonIntegers);

	return true;
}

//creates a binary file called binaryFileName, and writes the values of all integers
//from the list in the file
static bool writeTheIntegersFromTheListOntoTheBinaryFile
(const char *binaryFileName, const uint64_t *listOfCommonIntegers, int numberOfElements)
{
	ofstream binaryFile (binaryFileName, ios::out | ios::binary);

	if (!binaryFile)
	{
		cerr << "could not open " << binaryFileName << " for writing\n";
		return false;
	}

	for (int i = 0; i < numberOfElements; ++i)
	{
		binaryFile.write((const char*)&listOfCommonIntegers[i], sizeof(uint64_t));
	}

	binaryFile.close();

	return true;
}

bool findingTheIntersectionOfNumbersInBinaryFiles()
{
	int numberOfBinaryFiles;

	cin >> numberOfBinaryFiles;

	if (!cin)
	{
		cerr << "Some error occured, while reading the number of files\n"
				"You have probably typed in a letter!\n";
		return false;
	}

	if (numberOfBinaryFiles <= 0)
	{
		cerr << "the number of files must be a positive number\n";
		return false;
	}

	//extract the white character from the stream
	cin.ignore();

	char **listOfFileNames = NULL;
	
	//this list will hold all common integers in the files that we'll read
	uint64_t *listOfCommonIntegers = NULL;

	//we'll use this pointer for the merge sort
	uint64_t *tempList = NULL;

	//we'll use this array when we make
	//the search process for common integers in the files
	bool *tableOfFoundIntegersInTheListOfCommonIntegers = NULL;
	
	try
	{
		//all char* in the array are initialized with NULL
		listOfFileNames = new char*[numberOfBinaryFiles] {};
	}
	catch (bad_alloc &)
	{
		cerr << "not enough memory for the list of file names\n";
		return false;
	}

	//enter the name of every binary file
	for (int i = 0; i < numberOfBinaryFiles; ++i)
	{
		enterStringFromStandardInput(listOfFileNames[i]);
		
		//if the input operation was unsuccessfull
		if (listOfFileNames[i] == NULL)
		{
			cerr << "could not read the filename number " << i << '\n';
			deleteDinamicallyAllocatedMemory
				(listOfFileNames, numberOfBinaryFiles, listOfCommonIntegers
				, tempList, tableOfFoundIntegersInTheListOfCommonIntegers);
			return false;
		}
	}

	//the size of the file in the list with the minimum size
	int minSizeOfFile = -1;

	int indexOfFileWithMinNumberOfElements= -1;

	/*
	We'll find the file with the least number of elements. Then we'll sort
	this elements ascendingly and scan all other files to find the common elements,
	using binarySearch.
	*/
	bool result = findInfoAboutFileWithMinNumberOfElements
	(listOfFileNames, numberOfBinaryFiles, minSizeOfFile, indexOfFileWithMinNumberOfElements);

	if (!result)
	{
		deleteDinamicallyAllocatedMemory
			(listOfFileNames, numberOfBinaryFiles
			, listOfCommonIntegers, tempList, tableOfFoundIntegersInTheListOfCommonIntegers);
		return false;
	}

	//we'll swap the file with min number of elements with the first file in the list
	char *temp = listOfFileNames[0];
	listOfFileNames[0] = listOfFileNames[indexOfFileWithMinNumberOfElements];
	listOfFileNames[indexOfFileWithMinNumberOfElements] = temp;

	//Now the first file in the list has the smallest size

	//the number of elements in the list of integers
	int numberOfElements = minSizeOfFile / sizeof(uint64_t);

	
	if (numberOfElements > 0)
	{
		try
		{
			listOfCommonIntegers = new uint64_t[numberOfElements];
			tempList = new uint64_t[numberOfElements];;
		}
		catch (bad_alloc&)
		{
			cerr << "not enough dynamic memory for the elements in the file\n";
			deleteDinamicallyAllocatedMemory
				(listOfFileNames, numberOfBinaryFiles
				, listOfCommonIntegers, tempList, tableOfFoundIntegersInTheListOfCommonIntegers);
			return false;
		}
	}

	result = readElementsFromFileAndPutThemInList
		(listOfCommonIntegers, listOfFileNames[0], numberOfElements);

	if (!result)
	{
		deleteDinamicallyAllocatedMemory
			(listOfFileNames, numberOfBinaryFiles
			, listOfCommonIntegers, tempList, tableOfFoundIntegersInTheListOfCommonIntegers);
		return false;
	}

	mergeSort(listOfCommonIntegers, 0, numberOfElements, tempList);

	//we don't need the tempList anymore, so we can free the dynamic memory
	delete[] tempList;
	tempList = NULL;

	//the list is now ordered, but still may contain some duplicating integers
	//we'll remove them
	removeDuplicatingElementsFromList(listOfCommonIntegers, numberOfElements);
	
	/*
		We'll use the following algorithm for searching elements and finding their intersection:
		We' ll make the tableOfFoundIntegersInTheListOfCommonIntegers a parallel array to the 
		listOfCommonIntegers. When we search for an integer in the list, if it is found, its 
		parallel element in the boolean array(the table of found integers) wil become true.
		After the reading of the file with integers stops, we'll remove the integers from the
		list which weren't found in the file (i.e. their parallel elements have value false)
	*/
	
	if (numberOfElements > 0)
	{
		try
		{
			//all elements have initial values false
			tableOfFoundIntegersInTheListOfCommonIntegers = new bool[numberOfElements] {};
		}
		catch (bad_alloc&)
		{
			cerr << "not enough dynamic memory for the elements in the file\n";
			deleteDinamicallyAllocatedMemory
				(listOfFileNames, numberOfBinaryFiles
				, listOfCommonIntegers, tempList, tableOfFoundIntegersInTheListOfCommonIntegers);
			return false;
		}
	}

	//we have already read the file with index 0, so now
	//we'll start the scanning from the binary file with index 1
	for (int i = 1; i < numberOfBinaryFiles; ++i)
	{
		//if the list of common integers is empty
		if (numberOfElements == 0)
			break;

		result = readBinaryFileAndUpdateListOfCommonIntegers
			(listOfFileNames[i], listOfCommonIntegers
			, numberOfElements, tableOfFoundIntegersInTheListOfCommonIntegers);
		
		if (!result)
		{
			deleteDinamicallyAllocatedMemory
				(listOfFileNames, numberOfBinaryFiles
				, listOfCommonIntegers, tempList, tableOfFoundIntegersInTheListOfCommonIntegers);
			return false;
		}
	}

	const char *nameOfOutputFileContainingTheElementsOfTheIntersection = "result.bin";
	
	result = writeTheIntegersFromTheListOntoTheBinaryFile
		(nameOfOutputFileContainingTheElementsOfTheIntersection, listOfCommonIntegers, numberOfElements);

	deleteDinamicallyAllocatedMemory
		(listOfFileNames, numberOfBinaryFiles
		, listOfCommonIntegers, tempList, tableOfFoundIntegersInTheListOfCommonIntegers);

	return (result == true) ? true : false;
}