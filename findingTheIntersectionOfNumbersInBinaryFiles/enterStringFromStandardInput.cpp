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

#include<iostream>
#include<cstring>
#include<cassert>

using namespace std;


//appends the content of buffer to str, including a null terminating character
static void  addContentOfBufferToStr
(char *&str, const char *buffer)
{
	int lengthOfBuffer = strlen(buffer);

	int lengthOfStr = ( (str == NULL) ? 0 : strlen(str) );

	char* newBuffer = new char[lengthOfBuffer + lengthOfStr + 1];

	//str could be NULL, so we can't use strcpy()
	strncpy(newBuffer, str, lengthOfStr);

	delete[] str;
	str = newBuffer;

	//this function automanically apppends '\0' at the end of str
	strcpy(&str[lengthOfStr], buffer);
}

//str must be equal to NULL
//The function reads characters from the input and
//appends them to the string while newline or EOF
//is encountered. The newline is extracted from the stream,
//but isn't appended to the string. 
//The new string is null terminated
void enterStringFromStandardInput (char *&str)
{
	assert(str == NULL);

	static const int sizeOfBuffer = 100;

	static char buffer[sizeOfBuffer];

	while (cin.good())
	{
		cin.getline(buffer, sizeOfBuffer);

		//append the newly read content to the string
		addContentOfBufferToStr(str, buffer);

		//if the input operation was successfull, either because
		//'\n' or EOF was encountered
		if (cin.good() || cin.eof())
			break;

		//the failbit of cin is on because the content to read is too big.
		//So we must read the remaining part of the content.
		cin.clear();
	}
}