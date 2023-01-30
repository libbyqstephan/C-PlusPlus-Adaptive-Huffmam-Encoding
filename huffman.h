#include <iostream>
#include <string.h>
#include <math.h>
#include <vector>
#include <iterator>
#include <sstream>
#include <fstream>
using namespace std;

class huffmanTree
{
private:
    class huffmanNode
    {
    public:
        huffmanNode* parent;
        huffmanNode* left;
        huffmanNode* right;
        huffmanNode* prev;
        huffmanNode* next;
        int count;
        char character;
        huffmanNode()
        {
            count = 0;
            parent = nullptr;
            left = nullptr;
            right = nullptr;
            prev = nullptr;
            next = nullptr;
        }
        ~huffmanNode(){}
    };
    vector<char> charArray;
    vector<huffmanNode*> alphaVector;
    huffmanNode* root;
    huffmanNode* zero;
    string alpha;
    bool isAlphaProcessed;

    //helper functions
    huffmanNode* createNodes(char newChar);
    void initializeArray();
    bool isCharInAlpha(char curChar);
    huffmanNode* findCharPtr(char curChar);
    string findPath(huffmanNode* curNode);
    void checkThread(huffmanNode* curNode);
    void cleanUp(huffmanNode* curNode);

    //fucntion used to check on pointers
    void printNodeInfo(huffmanNode* node);
    
public:
    huffmanTree(string alphaString){
        alpha = alphaString;
        isAlphaProcessed = false;
    }
    ~huffmanTree(){
        cleanUp(root);
    }
 
    string encode(string input);
    string decode(string input);
};

string huffmanTree::encode(string input)
{
    string output;

    //create zero node
    zero = new huffmanNode;
    root = zero;

    //determine if alpha has already been processed
    if(isAlphaProcessed == false)
        initializeArray();

    //tokenize input into inputVector
    vector<char> inputVector;
    inputVector.resize(input.length());                       //resize the vector to the length of input
    copy(input.begin(), input.end(), inputVector.begin());    //copying the contents of the input to vector

    //beginning encoding process
    char curChar;
    vector<char>::iterator iter;
    for(iter = inputVector.begin(); iter!= inputVector.end(); iter++)
    {
        curChar = *iter;
        if(isCharInAlpha(curChar) == false)     //if the character is not in alphabet,
            continue;                           //move on to next character
        
        //this is a new character we have encountered
        if(findCharPtr(curChar) == zero)
        {            
            //output path to zero node (if there)
            if(root != zero)              
                output.append(findPath(findCharPtr(curChar)));

            //output ASCII
            //1. translate char to ASCII int value
            int asciiVal = int(curChar);
            
            //2. translate int to binary using /2 method
            int quotient = 1;
            int remainder = 1;

            vector<string> asciiBinary;
            while(quotient != 0)
            {
                quotient = asciiVal/2;
                remainder = asciiVal%2;
                
                if(remainder == 1)
                    asciiBinary.push_back("1");
                else
                    asciiBinary.push_back("0");
                asciiVal = quotient;
            }
            quotient = 1;

            //3. fill out any remaining zeros ahead
            while(asciiBinary.size() != 8)
            {
                asciiBinary.push_back("0");
            }

            //4. append the binary string backwards, now clear the binary vector
            vector<string>::reverse_iterator it = asciiBinary.rbegin();
            while (it != asciiBinary.rend())
            {
                output.append(*it);
                it++;
            }
            asciiBinary.clear();

            //create new parent and sibling nodes to zero, now check the thread
            checkThread(createNodes(curChar));
        }

        //we have already encountered this character
        else
        {    
            //output path to the char, increment its count and checkThread
            huffmanNode* charNode = findCharPtr(curChar);
            output.append(findPath(charNode));
            charNode->count++;
            checkThread(charNode);
        }
    }
    return output;
}

string huffmanTree::decode(string input)
{
    string output;
    vector<string> asciiBinary;

    //create zero node
    zero = new huffmanNode;
    root = zero;

    //determine if alpha has already been processed
    if(isAlphaProcessed == false)
        initializeArray();

    try
    {
        //do first 8 bits exist for ASCII?
        if(input.size() < 8)
            throw "Not enough information in input.";

        //tokenize input into inputVector
        vector<string> inputVector;
        inputVector.resize(input.length());                       //resize the vector to the length of input
        copy(input.begin(), input.end(), inputVector.begin());    //copying the contents of the input to vector
        
        //beginning decoding process
        char curChar;
        vector<string>::iterator iter;
        iter = inputVector.begin();
        while(iter != inputVector.end())
        {
            //serperate 8 bits to determine ASCII vector
            for(int i = 0; i < 8; i++)
            {
                asciiBinary.push_back(*iter);
                iter++;
            }
            
            //calculate ASCII value into asciiVal
            int asciiVal = 0;
            int binVal;
            vector<string>::reverse_iterator it = asciiBinary.rbegin();
            for(int i = 0; i < 8; i++)
            {
                binVal = stoi(*it);
                if(binVal == 1)
                        binVal = pow(2, i);
                asciiVal += binVal; 
                it++;
            }

            //convert ASCII value to char, then append to output
            curChar = char(asciiVal);
            if(isCharInAlpha(curChar) == false)     //if the character is not in alphabet,
                continue;                           //move on to next character
            output += curChar;

            //create new nodes and check thread for newChar
            checkThread(createNodes(curChar));

            //traverse tree from input directions until you hit a leaf node
            while(iter != inputVector.end())
            {
                huffmanNode* traverseNode = root;
                while(traverseNode->left != nullptr)
                {
                    int bit = stoi(*iter);
                    if(bit == 1)
                        traverseNode = traverseNode->right;
                    else
                        traverseNode = traverseNode->left;
                    iter++;
                }
    
                //now at a leaf
                //if at 0 node, break to outer while loop to look at next 8 bits
                if(traverseNode == zero)
                    break;
                //if not, appened the new character, increment its node, and checkThread
                else
                {
                    output += traverseNode->character;
                    traverseNode->count++;
                    checkThread(traverseNode);
                }
            }
        }

    }catch(const char* msg){
            cerr << msg << endl;
    }
    return output;
}

huffmanTree::huffmanNode* huffmanTree::createNodes(char newChar)
{
    //this function creates new parents and sibling nodes to the zero node
    //the sibling node will contain the newChar
    //the old parent if there, will also be incremented
    //will also update the newChar's pointer in the alphaVector
    //returns pointer to the last incremented node

    huffmanNode* newParent = new huffmanNode;
    huffmanNode* newSibling = new huffmanNode;
    huffmanNode* oldParent;
    huffmanNode* oldSibling;

    newParent->count = 1;
    newSibling->count = 1;
    newSibling->character = newChar;

    if (root != zero)                   //non-empty tree
    {
        oldParent = zero->parent;
        oldSibling = oldParent->right;

        newParent->parent = oldParent;
        newParent->prev = oldSibling;
        oldParent->left = newParent;
        oldParent->count++;
        oldSibling->next = newParent;

        /*
        cout << "oldParent's..." << endl;
        printNodeInfo(oldParent);
        cout << "oldSibling's..." << endl;
        printNodeInfo(oldSibling);
        */
    }

    zero->parent = newParent;
    zero->prev = newSibling;
    newParent->left = zero;
    newParent->right = newSibling;
    newParent->next = newSibling;
    newSibling->parent = newParent;
    newSibling->prev = newParent;
    newSibling->next = zero;
    /*
    cout << "newParent's..." << endl;
    printNodeInfo(newParent);
    cout << "newSibling's..." << endl;
    printNodeInfo(newSibling);
    */
   
    //setting the alphaVector pointer to the new character node
    //below is a duplicate of findCharPtr, but it's here to change the actual ptr
    vector<char>::iterator it;
    vector<huffmanNode*>::iterator it2;
    int index = 0;

    //finding char's index in the charArray
    for(it = charArray.begin(); it != charArray.end(); it++)
    {
        if(*it == newChar)
            break;
        else
            index++;
    }

    //matching it to corresponding pointer in alphaVector
    it2 = next(alphaVector.begin(), index);
    *it2 = newSibling;

    if(root != zero)
        return oldParent;
    else
    {
        root = newParent;
        return newParent;
    }
}

void huffmanTree::initializeArray()
{
    //this function goes through all alpha string to initialize corresponding pointers to zero node

    //breaking alphabet's string into a character vector
    charArray.resize(alpha.length());                       //resize the vector to the length of alpha
    copy(alpha.begin(), alpha.end(), charArray.begin());    // copying the contents of the string to char array
    
    //initializing vector of pointers
    alphaVector.resize(alpha.length());                     //resize the vector to the length of alpha
    vector<huffmanNode*>::iterator it;
    for(it = alphaVector.begin(); it != alphaVector.end(); it++)
    {
        *it = zero;
    }
    return;
}

bool huffmanTree::isCharInAlpha(char curChar)
{
    //this function verifies if a character from input (curChar) is present in the alphabet string
    try
    {
        bool match = false;
        vector<char>::iterator iter;
        for(iter = charArray.begin(); iter != charArray.end(); iter++)
        {
            if(curChar == *iter)
                match = true;
        }
        if(match == true)
            return true;
        else
            throw "Character does not appear in alphabet. Ingnoring character.";
    }catch(const char* msg){
            cerr << msg << endl;
    }
    return false;
}

huffmanTree::huffmanNode* huffmanTree::findCharPtr(char curChar)
{
    //this function finds the given character in the alphabet and returns its corresponding pointer
    //note: isCharInAlpha should be called prior to ensure character exists in alphabet

    vector<char>::iterator it;
    vector<huffmanNode*>::iterator it2;
    int index = 0;

    //finding char's index in the charArray
    for(it = charArray.begin(); it != charArray.end(); it++)
    {
        if(*it == curChar)
            break;
        else
            index++;
    }

    //matching it to corresponding pointer in alphaVector
    it2 = next(alphaVector.begin(), index);
    return *it2;
}

string huffmanTree::findPath(huffmanNode* curNode)
{
    //this function finds the path from the root to the given node
    //it will return the string of binary directions
    //note: assumes that a path does exist (not just a one node tree)

    huffmanNode* parent;
    vector<string> reversePath;
    string path;
    while(curNode != root)
    {
        parent = curNode->parent;
        if(parent->left == curNode)
            reversePath.push_back("0");
        else 
            reversePath.push_back("1");
        curNode = parent;
    }
    vector<string>::reverse_iterator it = reversePath.rbegin();
    while (it != reversePath.rend())
    {
        path.append(*it);
        it++;
    }
    return path;
}

void huffmanTree::checkThread(huffmanNode* curNode)
{
    //this function checks the "thread" of the tree from the perspective of the curNode
    //curNode should've already been incremented (handled in newNodes function)
    //if curNode is root, return (thread is already okay)

    if(curNode == root)
        return;

    huffmanNode* violation = nullptr;
    huffmanNode* leader;
    huffmanNode* tempNode = curNode;
    
    while(curNode != root)
    {
        //determine if thread going up is non-increasing
        if(curNode->count <= curNode->prev->count)
            curNode = curNode->prev;

        else    //violation found
        {
            violation = curNode;
            leader = violation->prev;

            //find the leader
            if(leader->prev != nullptr)
            {
                while(leader->prev->count == leader->count)
                {
                    leader = leader->prev;
                }
            }

            //see if leader is the parent
            if(leader == violation->parent)
                violation->parent->count++;
            else
            {
                //swap leader with the violator and its subtree
                huffmanNode* LParent = leader->parent;
                huffmanNode* LPrev = leader->prev;
                huffmanNode* LNext = leader->next;     
                huffmanNode* VParent = violation->parent;
                huffmanNode* VPrev = violation->prev;
                huffmanNode* VNext = violation->next;

                violation->parent = LParent;         
                if(LParent->left == leader)
                    LParent->left = violation;
                else
                    LParent->right = violation;

                leader->parent = VParent;        
                if(VParent->left == violation)
                    VParent->left = leader;
                else
                    VParent->right = leader;
        
                LPrev->next = violation;
                VNext->prev = leader;
                violation->prev = LPrev;
                leader->next = VNext;

                //edge case: leader and violator are directly connected by the thread
                if(LNext == violation)
                {
                    violation->next = leader;
                    leader->prev = violation;
                }
                else
                {               
                    violation->next = LNext;
                    leader->prev = VPrev;
                    LNext->prev = violation;
                    VPrev->next = leader;   
                }

                //update violation's new parent
                violation->parent->count++;
            }
            tempNode = violation->parent;
            checkThread(tempNode);
            break;
        }
    }
    //if no violation was found, recall checkThread with the newly incremented parent
    if(violation == nullptr)
    {
        if(tempNode->parent != nullptr)
            tempNode->parent->count++;
        if(tempNode->parent != root)
            checkThread(tempNode->parent);
    }
    else
        return;
}

void huffmanTree::cleanUp(huffmanNode* curNode)
{  
    if (curNode == nullptr) 
        return;
 
    /* first delete both subtrees */
    cleanUp(curNode->left);
    cleanUp(curNode->right);
     
    /* then delete the node */
    delete curNode;   
}