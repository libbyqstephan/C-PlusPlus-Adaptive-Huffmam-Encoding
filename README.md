# C-PlusPlus-Adaptive-Huffman-Encoding
Encode and decode strings using the Adaptive Huffman algorithm.

This C++ program is an assignment from an Advanced Data Structures class. In it, object-oriented programming is used to implement an Adaptive Huffman tree. The compressed/encoded result is not true compression as it is a string of 0s and 1s. Also, the characters allowed for encoding are restricted to the alphabet.

To use:
- Upon execution, the user includes "encode" or "decode" on the command line. Following this, the user adds the name of the file that holds the "alphabet" to be used. Finally, the user includes the name of the file containing the string they would like encoded/decoded.
- The command line should appear like this: ./huffman.exe encode alpha.txt message.txt
- Once the program is finished, the results are outputed as a new text file of the same name as the message file with ".encode" or ".decode" appended to the end.

