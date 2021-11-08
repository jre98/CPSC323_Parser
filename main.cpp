#include <iostream>
#include "parser.cpp"
#include <string>
#include <fstream>
#include <iomanip>
#include <queue>
// #include "Lexer.cpp"

using namespace std;

/*
This is the driver function for the parser
*/
int main() {
  // This main function calls the RAT21F() function that is included from
  // parser.cpp. Once this function is called, the parser will parse the lexed
  // code until one of the following happens: the parser reaches the end of the
  // file and finds the $ symbol (indicating a successful parsing of the file),
  // or, the parser encounters an error while parsing, in which case, the
  // program will exit and display an error message

  // Idea is to call the RAT21F function (with $ appended at end) with the input
  // parameter being a string containing the entire source codes
  RAT21F();
  return 0;
}
