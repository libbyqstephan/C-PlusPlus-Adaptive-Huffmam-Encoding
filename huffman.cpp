#include "huffman.h"
#include <iostream>
#include <string.h>
#include <vector>
#include <iterator>
#include <sstream>
#include <fstream>

using namespace std;

const int VALID_ARG_COUNT = 4;
int main(int argc, char* argv[])
{
    //check for correct number of arguments
    if(argc != VALID_ARG_COUNT)
    {
        cout << "Invalid number of arguments" << endl;
        return -1;
    }

    //identify command given
    string command = argv[1];
    if(command != "encode" && command != "decode")
    {
        cout << "Invalid command" << endl;
        return -1;
    }

    //get alphabet from alpha file
    string alphaString;          //holds the actual alphabet string
    fstream alphaFile;           //the name of the alphabet file object
    alphaFile.open(argv[2]);   

    if(alphaFile.is_open())
    {
        getline(alphaFile, alphaString);
    }
    alphaFile.close();
    cout << "Alphabet: " << alphaString << endl;


    //get message from message file
    string messageString;          //holds the actual message string
    fstream messageFile;           //the name of the message file object
    messageFile.open(argv[3]);   

    if(messageFile.is_open())
    {
        getline(messageFile, messageString);
    }
    messageFile.close();
    cout << "Message:  " << messageString << endl;

    //create huffman object, pass in alphabet string, and call corresponing function
    string outputMessage;               //actual outputted message
    ofstream outputFile;                //name of output file object
    string outputString = argv[3];      //name of output file
    huffmanTree huff(alphaString);
    if(command == "encode")
    {
        outputMessage = huff.encode(messageString);
        outputString.append(".encoded");
    }
    else
    {
        outputMessage = huff.decode(messageString);
        outputString.append(".decoded");
    }
    cout << "Output Message: " << outputMessage << endl;
    cout << "Output File Name: " << outputString << endl;
    
    //create new output file with the outputted message
    outputFile.open(outputString);
    if(outputFile.is_open())
    {
        outputFile << outputMessage << endl;
    }
    outputFile.close();

    return 0;
}