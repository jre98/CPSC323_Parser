#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <queue>

using namespace std;

//Lexer Prototypes
bool is_keyword(string lexeme);
bool is_operator(string lexeme);
bool is_seperator(string lexeme);
bool is_identifier(string lexeme);
vector<vector<string>> Lexer();

vector<vector<string>> Lexer() {

    /*Takes input_file.txt as the input source code,
    And writes to output_file.txt the output of the LA
     
     Contains all of the FSM's for the lexical analyzer
     There are 5 possible states;
        Seperator
        Operator
        Numeric
        Alphabetic
        Comment

    And from those four states there are several closing states.

    Seperator -> Closes. Token must be seperator

    Operator -> if next char is not operator; closes. 
        -> If the token = "/*"; then it enters the is comment state
            -> Comment state repeats until a closing comment operator is found.
                -> It exits the comment state, without adding anything to the output file.

        -> If the token is not "/*", then it must be an operator, and is tested against the accepted list.
        -> If the token is invalid, then it is marked as an error, otherwise it is added to output.

    Numeric -> if next char is not a number or decimal point; closes
        -> If contains a decimal point; token is real
        -> If the token does not contain decimal point; token is integer

    Alphabetic -> If next char is not a space, seperator or operator; Closes.
        -> Test if token is keyword, if so mark it.
        -> Otherwise token must be an identifier

    If no closing states are triggered, adds another character to the token and repeats. 
     */

    fstream input_file,output_file;
    input_file.open ("input_file.txt");
    output_file.open("output_file.txt");
    string word;
    char ch;

    //Saves the input file as a queue of chars to be used by the FSM
    queue<char> input_queue;
    vector<vector<string>> token_list;
    vector<string> token_pairs;
    char space = ' ';

    while (input_file >> noskipws >> ch) {
        input_queue.push(ch);
    }
    input_queue.push(space);


    //Variables needed in the FSM
    string current_token;
    string current_ch;

    //These are the states to the FSM
        bool accepted = 0;
        bool alpha_state = 0;
        bool num_state = 0;
        bool op_state = 0;
        bool comment_state = 0;
        

    //Used to split tokens into two
        bool skip_one = 0;

    //Return vector
    
  

    //token_list.push_back(token_pairs);
 

    /*This is the main body of the code.
    It takes in one character at a time from the queue
    And then using several states computes the beginning and
    end of every token. Once it has a token, it is added to the output
    file with the correct notation
    */

    while (input_queue.size() >= 1)
    { 

        //Removes extraneous spaces
        if (current_token == " " || current_token == "\n") {
            current_token = "";
        }
        //Fixes bug where a space at end of file counts as a seperate token
        if (current_ch == " " && input_queue.size() == 1) {
            break;
       }

       //Reset accepted state
        accepted = 0;

        //This is to handle tokens that are combined with no space
        //Removes the top of the queue, and adds it to the current token
        if (skip_one != 1) {     
        current_ch = input_queue.front();
        current_token.push_back(input_queue.front());
        input_queue.pop(); 
        }
        skip_one = 0;
        


        //First accepting state is just a seperator as the token
        if (is_seperator(current_ch) && current_token.length() == 1 && alpha_state == 0 && num_state == 0 && op_state == 0 && comment_state == 0) {
            //Accepting state for seperators
            token_pairs.clear();
            token_pairs.push_back(current_token);
            token_pairs.push_back("seperator");
            token_list.push_back(token_pairs);

            current_token = "";
            accepted = 1;
        }
        
        //Second state is if current_ch is a letter
        //Accepting state for the second state is next_char being a non character
        if (isalpha(current_ch[0]) && current_ch != " " && accepted == 0 && num_state == 0 && op_state == 0 && comment_state == 0) {
            //Enters the alpha state FSM
            //Closes at a seperator, operator, or space
            alpha_state = 1;
        }

        if (alpha_state == 1){      
            if (is_seperator(current_ch) || is_operator(current_ch) || current_ch == " " || current_ch == "\n") {
                //Accepting state for Alpha tokens
                //A seperator, operator, or space triggers this closure
                //If this token is not a keyword, then it must be an identifier
                if (is_keyword(current_token.substr(0, current_token.size()-1))) {
                    //Accepting keyword state
                token_pairs.clear();
                token_pairs.push_back(current_token.substr(0, current_token.size()-1));
                token_pairs.push_back("keyword");
                token_list.push_back(token_pairs);
                }
                else {
                    //Accepting identifier state
                    token_pairs.clear();
                token_pairs.push_back(current_token.substr(0, current_token.size()-1));
                token_pairs.push_back("identifier");
                token_list.push_back(token_pairs);
                }
                current_token = current_ch;
                accepted = 1;
                alpha_state = 0;
                skip_one = 1;               
            }          
        }

      if ((isdigit(current_ch[0]) || current_ch == "." )&& accepted == 0 && alpha_state == 0 && op_state == 0 && comment_state == 0) {
          //Enters the "is number" FSM
          //Breaks when the next character is a space or a non number (except a decimal)
            num_state = 1; 
        }

        if (num_state == 1){
            //Accepts when the next digit is no longer a number; excluding decimal points
            if (!isdigit(current_ch[0]) && current_ch[0] != '.') {
                //Accepting state for Numbers; Real and Integer
                int decimal_count = 0;

                for (int i = 0; i < current_token.length();i++) {
                    if (current_token[i] == '.') {decimal_count++;}
                }
                if(decimal_count == 1 && current_token[0] != '.') {
                    //If token contains "." then it is real
                    token_pairs.clear();
                token_pairs.push_back(current_token.substr(0, current_token.size()-1));
                token_pairs.push_back("real");
                token_list.push_back(token_pairs);
                }

                else if (decimal_count == 0){
                    //If token does not contain ".", it is integer
                    token_pairs.clear();
                token_pairs.push_back(current_token.substr(0, current_token.size()-1));
                token_pairs.push_back("integer");
                token_list.push_back(token_pairs);
                }
                else {
                    //If token has more than one decimal or begins with a decimal it is an error
                    token_pairs.clear();
                token_pairs.push_back(current_token.substr(0, current_token.size()-1));
                token_pairs.push_back("error");
                token_list.push_back(token_pairs);
                }
                current_token = current_ch;
                accepted = 1;
                num_state = 0;
                skip_one = 1;               
                }      
        
            }       

          if (is_operator(current_ch) && accepted == 0 && alpha_state == 0 && num_state == 0) {
          //Enters the "is operator" FSM
          //Operators must be seperated by a space based on the grammar;
          //Therefore "+++" is not valid, and will return an error.
          op_state = 1;
            
            
        }
         
        if ((op_state == 1 && !is_operator(current_ch) && comment_state == 0) || (current_token.length() == 3 && op_state == 1)){
            //Accepting state for operators.
            //Accepts whenever the next character is not also an operator, or when the token length is two
            //As all operators are one or two characters
                if (current_token.substr(0, current_token.size()-1) == "/*") {
                    //This catches the comments!
                    //Swaps from operator state to comment state
                    comment_state = 1;
                    op_state = 0;
                }
                else if (is_operator(current_token.substr(0, current_token.size()-1))) {
                token_pairs.clear();
                token_pairs.push_back(current_token.substr(0, current_token.size()-1));
                token_pairs.push_back("operator");
                token_list.push_back(token_pairs);
                }
                /*
                else if (current_token.length() == 2) {
                    if(is_operator(current_token.substr(0,1)) && is_operator(current_token.substr(1,2))) {}
                    // Cathes operators together cases (+- , -- , etc.)
                    token_pairs.clear();
                    token_pairs.push_back(current_token.substr(0,1));
                    token_pairs.push_back("operator");
                    token_list.push_back(token_pairs);
                    token_pairs.clear();
                    token_pairs.push_back(current_token.substr(1,2));
                    token_pairs.push_back("operator");
                    token_list.push_back(token_pairs);
                            
                }*/
                else {
                    //This is if the operator is invalid
                token_pairs.clear();
                token_pairs.push_back(current_token.substr(0, current_token.size()-1));
                token_pairs.push_back("Error");
                token_list.push_back(token_pairs);
    
                }

                current_token = current_ch;
                accepted = 1;
                op_state = 0;
                skip_one = 1;   
            
        }
         if (comment_state == 1 && current_token.find("*/") != string::npos) {
             //when the closing operator for a comment is found, the comment state is accepted
             //Nothing from this state is written to the output file
             op_state = 0;
             comment_state = 0;
             accepted = 1;
             current_token = "";
         }
      
    }
    return(token_list);
}

bool is_keyword(string lexeme) {
    /*e.g., integer, if, else, endif, while, return, get, put etc.
    */
   string valid_keywords[] = {"boolean", "real", "true", "false", "integer", "if", "else", "endif", "while", "return", "get",
   "put", "for","function"};
   for (int i = 0; i  < sizeof(valid_keywords)/sizeof(valid_keywords[0]); i++) {
        if (lexeme == valid_keywords[i]) {
            return true;
        }
   }
    return false;
}

bool is_operator(string lexeme){
    //Returns true if valid operator, false otherwise
   string valid_operators[] = {"=", "!=", ">",
   "<","<=",">=","+","-","*","/", "=="};
   for (int i = 0; i  < sizeof(valid_operators)/sizeof(valid_operators[0]); i++) {
        if (lexeme == valid_operators[i]) {
            return true;
        }
   }
    return false;
}

bool is_seperator(string lexeme) {
    /*E.g ( ) { } , ;
    */
    string valid_seperators[] = {"(", ")", "{", "}", ";",
    ",","#","$"};
    for (int i = 0; i  < sizeof(valid_seperators)/sizeof(valid_seperators[0]); i++) {
            if (lexeme == valid_seperators[i]) {
                return true;
            }
    }
    return 0;
}
