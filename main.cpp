#include <iostream>
#include "parser.cpp"
#include <string>
#include <fstream>
#include <iomanip>
#include <queue>
// #include "Lexer.cpp"

using namespace std;


int main() {

//Token_list will be all the lexed tokens. {Token, Lexeme}
  vector<vector<string>> token_list = Lexer();

  std::cout << "This is the list of all lexemes and the corresponding token types"
            << std::endl;
  for (int i = 0; i < token_list.size(); i++) {
    cout << i << " " << token_list[i][0] << " " << token_list[i][1] << "\n";
  }




  // Idea is to call the RAT21F function (with $ appended at end) with the input
  // parameter being a string containing the entire source codes
  RAT21F();
  return 0;
}
