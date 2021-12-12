#include <iostream>
#include <string>
#include <stack>
#include "parser.h"
#include "Lexer.cpp"

// This file is only called once from the main function, in the form of calling
// the initial RAT21F Function. Once it has been called, the recursive nature of
// the parser will carry the parsing through to the end, or until an error is
// encountered




// The only way I could figure out how to get Opt_Function_Defs to pick the
// correct path
bool is_past_func_defs = false;

fstream output_file;

// Global int for the "memory addresses" that are added to the symbol table
long int mem_addr = 7000;

// Symbol table which is needed for object code generation
std::vector<std::vector<std::string>> symbol_table;

// vector containing lexed source code with the lexeme, token type and line num
vector<vector<string>> lexemes = Lexer();

// String to hold the current lexeme that we are dealing with
std::string curr_lexeme = lexemes[0][0];
// String to hold the current token we are dealing with
std::string curr_token = lexemes[0][1];
// The line number of each lexeme is stored in lexemes[0][2]

//String to hold current token into a temp variable
std::string save = curr_token;

//Global Variable instru_address, increments in function gen_instr so that Address when printing instruct_table increments
int instru_address = 1;

//Creating instruction table array
string Instruct_Table[1000][3] = { {"Address", "Op", "Oprnd"}};


//Generates the assembly code, grabbing the memory address, op, and oprnd
void gen_instr(std::string op, string oprnd){
  int instr_address;
  Instruct_Table[instru_address][0] = instr_address;
  Instruct_Table[instru_address][1] = op;
  Instruct_Table[instru_address][2] = oprnd;
  instru_address++;
  mem_addr++;
}

//Grabs the address of the current token when token == identifier
// void get_address(std::string save){
//     if()
// }
// Starting function to kick off recursive parser
void RAT21F() {
  // Open the output file, which is where the info about parsing the code will be
  // placed as it is parsed
  output_file.open("output_file.txt");
  // Print out production rule
  output_file << setw(10) << left << "<Rat21F>  ::=   <Opt Function Definitions>"
  "   #  <Opt Declaration List>  <Statement List>  #\n";
  // Call to determine if there are function definitions to be parsed
  // Comment out for simplified grammar
  // Opt_Function_Defs();

  // After the function definitions comes the "main" function, which is required
  // to be enclosed by # CODE #
  // Match the # symbol and update vars
  match(curr_lexeme, "#", curr_token);
  // Update bool to tell code that we have moved past the fuction_defs
  is_past_func_defs = true;
  // Call next productions for the declarations and code statements
  Opt_Declaration_List();
  Statement_List();
  // Match end of "main" function marker
  match(curr_lexeme, "#", curr_token);

  // If we get to the final token and it is the end of string marker ($), we
  // know that the parsing was successful, given there were no other errors
  // along the way (which would cause the program to quit)
  if (curr_lexeme == "$") {
    output_file << setw(10) << left << "This parser has reached the end of the "
    "file, parsing was successful." << std::endl;
    print_symbol_table();
  }
  // If there is no "$" at the end of the file, display a custom error message
  else {
    output_file << setw(10) << left << "There was an error while parsing the "
    "file: the end of file marker '$' was not found" << std::endl;
  }
  // Close the output file once the parser comes to a close
  output_file.close();

}

void Opt_Function_Defs() {
    output_file << setw(10) << left << "<Opt Function Definitions> ::= <Function"
    " Definitions>  | <Empty>\n";

  // if the current lexeme is the # seperator, this means that there were no
  // function definitions in the source file. If there are no function
  // definitions, we call Empty(). If there are, we call Function_Defs()
  if (curr_lexeme == "#") {
    Empty();
  }
  else if (curr_lexeme == "function") {
    Function_Defs();
  }
  // If neither case is met, call the error function
  else {
    error_info();
  }

}

void Function_Defs() {
  // Simply call the productions from the grammar
  output_file << setw(10) << left << "<Function Definitions> ::= <Function> "
  "<Function Definitions>’\n";
  Function();
  Function_Defs_P();
}

void Function_Defs_P() {
  output_file << setw(10) << left << "<Function Definitions>’ ::= <Function "
  "Definitions> | Ep\n";
  // If the current lexeme is #, we know that there are no more function
  // definitions, as the # indicates the start of the main function. In the case
  // where we have reached the main function, we call Epsilon, so we just return
  if (curr_lexeme == "#") {
    return;
  }
  // Otherwise, there are still more functions being declared, so we will loop
  // back to Function_Defs()
  else if (curr_lexeme == "function") {
    Function_Defs();
  }
  // Otherwise, call error function
  else {
    error_info();
  }
}

void Function() {
  output_file << setw(10) << left << "<Function> ::= function  <Identifier>   ("
  " <Opt Parameter List> )  <Opt Declaration List>  <Body>\n";
  // Function definitions come at the beginning of the RAT21F file and are all
  // declared using the keyword function. If curr_lexeme is 'function', a
  // function is being declared and we will match it and advance to next lexeme
  if (curr_lexeme == "function") {
    match(curr_lexeme, "function", curr_token);
  }
  else {
    error_info();
  }

  // After matching the lexeme, the match function updates the global variables,
  // so we continue on and call the next functions
  Identifier();
  // Match '('
  match(curr_lexeme, "(", curr_token);
  // Parameters for the function
  Opt_Parameter_List();
  //  Match ')'
  match(curr_lexeme, ")", curr_token);
  // Variable declarations in the function
  Opt_Declaration_List();
  // Function body
  Body();
}

void Opt_Parameter_List() {
  output_file << setw(10) << left << "<Opt Parameter List> ::=  <Parameter List>"
  "    |     <Empty>\n";
  // If there are no parameters in the parameter list, next token will be ")",
  // as a func with no parameters looks like: foo() and we call Empty()
  if(curr_lexeme == ")") {
    Empty();
  }
  // Otherwise, there are still more parameters, so we call Paramemter_List()
  else {
    Parameter_List();
  }
}

void Parameter_List() {
  // Simply call functions from grammar
  output_file << setw(10) << left << "<Parameter List> ::= <Parameter> "
  "<Parameter List>’\n";
  Parameter();
  Parameter_List_P();
}

void Parameter_List_P() {
  output_file << setw(10) << left << " <Parameter List>’ ::= , <Parameter List>"
  "  |  Ep\n";
  // If we have more than one parameter, the next correct lexeme would be a
  // comma to separate the parameters
  if(curr_lexeme == ",") {
    // Update variables
    match(curr_lexeme, ",", curr_token);
    Parameter_List();
  }
  // If the next lexeme is not a comma and is a ), we know we have reached the
  // end of the parameter list, so we just call Empty()
  else if (curr_lexeme == ")"){
    Empty();
  }
  // If neither case is met, then we will have an error
  else {
    error_info();
  }
}

void Parameter() {
  // Functions called from grammar
  output_file << setw(10) << left << "<Parameter> ::=  <IDs >  <Qualifier>\n";
  IDs();
  Qualifier();
}

void Qualifier() {
  output_file << setw(10) << left << "<Qualifier> ::=  integer  |  boolean  |  real\n";
  // A qualifier is allowed to be an int, bool or real. If the current lexeme is
  // any of these three, we will enter the if statement
  // if (curr_lexeme == "integer" || curr_lexeme == "boolean" || curr_lexeme == "real") {
  //   // Use the match function to update variables and output information
  //   match(curr_lexeme, curr_lexeme, curr_token);
  // }
  if (curr_lexeme == "integer" || curr_lexeme == "boolean") {
    match(curr_lexeme, curr_lexeme, curr_token);
  }
  // If we do not have any of these lexemes, then we have an error
  else {
    error_info();
  }
}

void Body() {
  output_file << setw(10) << left << "<Body>  ::=  {  < Statement List>  }\n";
  // Beginning of function body is {
  match(curr_lexeme, "{", curr_token);
  // Statements inside the function
  Statement_List();
  // End of function
  match(curr_lexeme, "}", curr_token);
}

void Opt_Declaration_List() {
  output_file << setw(10) << left << "<Opt Declaration List> ::= <Declaration "
  "List>   |    <Empty>\n";
  // If the current token is an identifier, we know that there will be a list of
  // declarations to handle, so we will call Declaration_List().
  if(curr_lexeme == "integer" || curr_lexeme == "boolean") {
    Declaration_List();
  }
  // Otherwise, we know that we need to call empty, as there will be no list
  else {
    Empty();
  }

}

void Declaration_List() {
  output_file << setw(10) << left << "<Declaration List> ::= <Declaration> ; "
  "<Declaration List>’\n";
  // Call declaration, then match the ";", then call Declaration_List_P
  Declaration();
  match(curr_lexeme, ";", curr_token);
  Declaration_List_P();
}

void Declaration_List_P() {
  output_file << setw(10) << left << "<Declaration List>’ ::= <Declaration "
  "List>  |  Ep\n";
  // If the next token is a keyword, then we know that we have another
  // declaration coming, so we call Declaration_List()
  if (curr_lexeme == "integer" || curr_lexeme == "boolean") {
    Declaration_List();
  }
  // Otherwise, we return and continue on
  else {
    return;
  }
}

void Declaration() {
  output_file << setw(10) << left << "<Declaration> :: = integer  |  boolean  |"
  "  real      <IDs>\n";
  // Match the correct lexeme to the beginning of the definition
  if(curr_lexeme == "integer") {
    match(curr_lexeme, "integer", curr_token);
  }
  else if(curr_lexeme == "boolean") {
    match(curr_lexeme, "boolean", curr_token);
  }
  // else if(curr_lexeme == "real") {
  //   match(curr_lexeme, "real", curr_token);
  // }
  // If the lexeme is not integer, boolean or real, call error_info()
  else {
    error_info();
  }
  // After matching the keyword, call IDs
  IDs();
}

void IDs() {
  // Call productions based on the grammar
  output_file << setw(10) << left << "<IDs> ::=  <Identifier> <IDs>’\n";
  Identifier();
  IDs_P();
}

void IDs_P() {
  output_file << setw(10) << left << "IDs>’ ::= ,  <IDs>    |      Ep\n";
  // If the next token is a "," we can match it and loop back to IDs
  if(curr_lexeme == ",") {
    match(curr_lexeme, ",", curr_token);
    IDs();
  }
  // Otherwise, call empty and move on
  else {
    Empty();
  }
}

void Statement_List() {
  // Call productions based off of grammar
  output_file << setw(10) << left << "<Statement List> ::= <Statement> "
  "<Statement List>’\n";
  Statement();
  Statement_List_P();
}

void Statement_List_P() {
  output_file << setw(10) << left <<"<Statement List>’ ::= <Statement List>   |"
  "    Ep\n";
  // This lexeme would indicate the end of the function definition or the
  // beginning of the main function, and in these cases, we just want to
  // continue on with the parsing
  if (curr_lexeme == "}" || curr_lexeme == "#") {
    return;
  }
  // Otherwise, there are more statements to be parsed, so call Statement_List()
  else {
    Statement_List();
  }
}

void Statement() {
  output_file << setw(10) << left << "<Statement> ::=   <Compound>  |  <Assign>"
  "  |   <If>  |  <Return>  |  <Print>   |   <Scan>   |  <While>\n";
  // Compound production begins with terminal symbol {
  if (curr_lexeme == "{") {
    Compound();
  }
  // Assign production begins with an identifier
  else if (curr_token == "identifier") {
    Assign();
  }
  // If statements start with a terminal if
  else if (curr_lexeme == "if") {
    If();
  }
  else if (curr_lexeme == "return") {
    Return();
  }
  // Print statements begin with a terminal put
  else if (curr_lexeme == "put") {
     Print();
  }
  // Scan statements start with a terminal get
  else if (curr_lexeme == "get") {
     Scan();
  }
  // While statements begin with a terminal while
  else if (curr_lexeme == "while") {
     While();
  }
  else {
    error_info();
  }
}

void Compound() {
  output_file << setw(10) << left << "<Compound> ::= { <Statement List> }\n";
  // Match the preceding "{", then call Statement_List to get statements, then
  // match the ending "}"
  match(curr_lexeme, "{", curr_token);
  Statement_List();
  match(curr_lexeme, "}", curr_token);
}

void Assign() {
  output_file << setw(10) << left << "<Assign> ::=    <Identifier> = "
  " <Expression> ;\n";
  // Find if there is an identifier
  Identifier();
  // Match the = for the correct assignment
  if(curr_token == "="){
  match(curr_lexeme, "=", curr_token);
  // Find if there is a valid expression
  Expression();
  //gen_instr("POPM", get_address(save));
  // Next, match the ";" that ends the assignment
  match(curr_lexeme, ";", curr_token);
  }

}

void If() {
  output_file << setw(10) << left << "<If> ::= if ( <Condition> )  <Statement> "
  " <If>’ \n";
  // If statements start with "if", so match that, then match the "("
  match(curr_lexeme, "if", curr_token);
  match(curr_lexeme, "(", curr_token);
  // Get the condition
  Condition();
  // Match closing parentheses
  match(curr_lexeme, ")", curr_token);
  // Get the statement, then call If'
  Statement();
  If_P();
}

void If_P() {
  output_file << setw(10) << left << "<If>’ ::= endif | else <Statement> endif\n";
  // If the lexeme is "endif", just match that
  if (curr_lexeme == "endif") {
    match(curr_lexeme, curr_lexeme, curr_token);
  }
  // If it is "else", match that
  else if (curr_lexeme == "else") {
    match(curr_lexeme, curr_lexeme, curr_token);
    Statement();
    match(curr_lexeme, "endif", curr_token);
  }
  // Otherwise, call the error function
  else {
    error_info();
  }
}

void Return() {
  output_file << setw(10) << left << "<Return> ::= return  <Return>’";
  // Determine if the lexeme is "return" or not, then call Return'
  match(curr_lexeme, "return", curr_token);
  Return_P();
}

void Return_P() {
  output_file << setw(10) << left << "<Return>’ ::=   ;  |  <Expression>;\n";
  // There are two cases: something like: return; OR return fahr;. The top if
  // statement deals with cases like return;, the bottom adds the ability to
  // parse the Expression and then match the ; separator
  if (curr_lexeme == ";") {
    return;
  }
  else {
    Expression();
    match(curr_lexeme, ";", curr_token);
  }
}

void Print() {
  output_file << setw(10) << left << "<Print> ::=    put ( <Expression>);\n";
  // Match the put, then the "("
  match(curr_lexeme, "put", curr_token);
  match(curr_lexeme, "(", curr_token);
  // Then, get the expression
  Expression();
  // Then, match terminal symbols at the end of the print statement
  match(curr_lexeme, ")", curr_token);
  match(curr_lexeme, ";", curr_token);
}

void Scan() {
  output_file << setw(10) << left << "<Scan> ::=    get ( <IDs> );\n";
  // Match the initial terminal symbols
  match(curr_lexeme, "get", curr_token);
  match(curr_lexeme, "(", curr_token);
  // Then, get the IDs that will go in the scan function
  IDs();
  // Finally, match the ending terminal symbols
  match(curr_lexeme, ")", curr_token);
  match(curr_lexeme, ";", curr_token);
}

void While() {
  output_file << setw(10) << left << "<While> ::=  while ( <Condition> ) "
  "<Statement>\n";
  // Match initial terminal symbols
  match(curr_lexeme, "while", curr_token);
  match(curr_lexeme, "(", curr_token);
  // Get the condition
  Condition();
  // Match the ending terminal symbols
  match(curr_lexeme, ")", curr_token);
  Statement();
}

void Condition() {
  // Call productions based off of grammar
  output_file << setw(10) << left << "<Condition> ::=   <Expression>  <Relop>  "
  " <Expression>\n;";
  Expression();
  Relop();
  Expression();
}

void Relop() {
  output_file << setw(10) << left << "<Relop> ::=    ==   |   !=    |   >   |  "
  " <   |  <=  |   =>\n";
  // If statements to determine if the real operator is valid
  if(curr_lexeme ==  "==") {
    match(curr_lexeme, curr_lexeme, curr_token);
  }
  else if(curr_lexeme == "!=") {
    match(curr_lexeme, curr_lexeme, curr_token);
  }
  else if(curr_lexeme == ">") {
    match(curr_lexeme, curr_lexeme, curr_token);
  }
  else if(curr_lexeme == "<") {
    match(curr_lexeme, curr_lexeme, curr_token);
  }
  else if(curr_lexeme == "<=") {
    match(curr_lexeme, curr_lexeme, curr_token);
  }
  else if(curr_lexeme == "=>") {
    match(curr_lexeme, curr_lexeme, curr_token);
  }
  // If the real operator is not valid, call the error function
  else {
    error_info();
  }
}

void Expression() {
  // Call productions based off of grammar
  output_file << setw(10) << left << "<Expression> ::= <Term><Expression>’\n";
  Term();
  Expression_P();
}

void Expression_P() {
  output_file << setw(10) << left << "<Expression>’ ::=  +<Term> <Expression>’ "
  " |  -<Term> <Expression>’ | ep\n";
  // If there is a +, we will go down this path
  if (curr_lexeme == "+") {
    match(curr_lexeme, "+", curr_token);
    Term();
    gen_instr("ADD","nil");
    Expression_P();
  }
  // If there is a -, we will go down this path
  else if (curr_lexeme == "-") {
    match(curr_lexeme, "-", curr_token);
    Term();
    gen_instr("SUB","nil");
    Expression_P();
  }
  // If we have neither + nor - as the lexeme, we just return
  else {
    return;
  }
}

void Term() {
  // Call prodctions based off of gramar
  output_file << setw(10) << left << "<Term> ::= <Factor><Term>’\n";
  Factor();
  Term_P();
}

void Term_P() {
  output_file << setw(10) << left << "<Term>’ ::=  *<Factor> <Term>’ | / "
  "<Factor> <Term>’ | ep\n";
  // If there is a *, we will go down this path
  if (curr_lexeme == "*") {
    match(curr_lexeme, "*", curr_token);
    Factor();
    gen_instr("MUL","nil");
    Term_P();
  }
  // If there is a /, we will go down this path
  else if (curr_lexeme == "/") {
    match(curr_lexeme, "/", curr_token);
    Factor();
    gen_instr("DIV","nil");
    Term_P();
  }
  // If we have neither * nor / as the lexeme, just return and keep going
  else {
    return;
  }
}

void Factor() {
  output_file << setw(10) << left << "<Factor> ::=   -  <Primary>    |    "
  "<Primary>\n";
  // Case where we have a terminal '-'
  if (curr_lexeme == "-") {
    match(curr_lexeme, "-", curr_token);
    Primary();
  }
  // Case where we do not have terminal (errors handled by Primary())-
  else {
    Primary();
  }
}

void Primary() {
  // Since there are two cases that begin with the non terminal <Identifier>, we
  // need to distinguish between the two paths. Since one of the paths is
  // followed by a terminal "(", we simply look ahead to see if that character
  // is next. If it is, we choose one path; if it is not, we choose the other
  output_file << setw(10) << left << "<Primary> ::=     <Identifier>  |  "
  "<Integer>  |   <Identifier> ( <IDs> )   |   ( <Expression> )   |  <Real>  | "
  "  true   |  false\n";
  // Case of <Identifier> ( <IDs> )
  if (curr_token == "identifier" && lexemes[1][0] == "(") {
    Identifier();
    match(curr_lexeme, "(", curr_token);
    IDs();
    match(curr_lexeme, ")", curr_token);
  }
  // Case of just <Identifier>
  else if (curr_token == "identifier") {
    Identifier();
  }
  // Case of integer
  else if (curr_token == "integer") {
    Integer();
  }
  // Case of real
  // else if (curr_token == "real") {
  //   Real();
  // }
  // <Expression> here is always surrounded by ( ), so look for ( in this case
  else if (curr_lexeme == "(") {
    match(curr_lexeme, "(", curr_token);
    Expression();
    match(curr_lexeme, ")", curr_token);
  }
  // Case of a Boolean
  else if (curr_lexeme == "true" || curr_lexeme == "false") {
    match(curr_lexeme, curr_lexeme, curr_token);
  }
  // Otherwise, display some error info, as the Primary was not defined correctly
  else {
    error_info();
  }
}

// Empty function (goes to epsilon)
void Empty() {
  output_file << setw(10) << left << "<Empty>   ::=  epsilon\n";
  return;
}

void Identifier() {
  // Since we already have the lexeme, we just need to make sure the token is
  // not error, as this would indicate that we have an incorrect lexeme
  if(curr_token == "identifier") {
    std::string save = curr_token;
    lexemes;
    // std::cout << "Token: " << curr_token << "          Lexeme: " << curr_lexeme << std::endl;
    match(curr_lexeme, curr_lexeme, curr_token);
    // Update curr_token and curr_lexeme
    Expression();
    //gen_instr("POPM", get_address(save));
  }
  else {
    std::cout << " id expected";
    //error_info();
  }
}

void Integer() {
  // Since we already have the lexeme, we just need to make sure the token is
  // not error, as this would indicate that we have an incorrect lexeme
  if(curr_token == "integer") {
    // std::cout << "Token: " << curr_token << "          Lexeme: " << curr_lexeme << std::endl;
    // Update lexeme and token here
    match(curr_lexeme, curr_lexeme, curr_token);
  }
  else {
    error_info();
  }
}

void Real() {
  // Since we already have the lexeme, we just need to make sure the token is
  // not error, as this would indicate that we have an incorrect lexeme
  if (curr_token == "real") {
    match(curr_lexeme, curr_lexeme, curr_token);
  }
  else {
    error_info();
  }
}


// Match function is for matching terminal symbols and making sure they are correct
// std::string token is a parameter because if we match, we want to print out
// the info about the lexeme, including its token type
void match(std::string& curr_lexeme, std::string corrrect_lexeme,
           std::string& token) {
  // If the lexeme is correct, display its information and token type
  if(curr_lexeme == corrrect_lexeme) {
    output_file << setw(10) << left << "Token: " << curr_token << setw(20)
                << right << "Lexeme: " << curr_lexeme << "\n\n";
    update(curr_lexeme, token);
  }
  // Otherwise, display an error message of some sort
  else {
    error_info();
  }
}

// Separate update function to make things easier
void update(std::string& lexeme, std::string& token) {
  // If the token is an identifier, add it to the symbol table
  if(token == "identifier") {
    addto_symboltable(lexeme);
  }
  else if(lexeme != "identifier"){
    std::cout << "This identifier hasn't been decalred";
  }
  else{
  // Erase the first row of entries from the lexemes vector
  lexemes.erase(lexemes.begin());
  // Since the next lexeme and token are now at the beginning of the list, we
  // simply reassign curr_token and curr_lexeme to the beginning of the vector
  lexeme = lexemes[0][0];
  token = lexemes[0][1];}
}

void error_info() {
  // Print out the error line
  output_file << setw(10) << left << "\nAn error has occured on line "
              << lexemes[0][2] << "\n";
  // Say which lexeme was incorrectly used
  output_file << setw(10) << left << curr_lexeme << " is used incorrectly in "
  "this context, please try again\n";
  // If we encounter an error, we exit from the parser, as this is allowed in
  // error handling per the assignment description
  output_file << setw(10) << left << "Since an error was encountered, the parser"
  " will now be exited\n";
  output_file.close();
  std::abort();
}

void addto_symboltable(std::string& lexeme) {
  // First, check to see if the lexeme is already in the symbol table
    for(int i = 0; i < symbol_table.size(); i++){
      if(symbol_table[i][0] == curr_lexeme){              //if lexeme is already in symbol_table error message
      std::cout << " This lexeme is already declared.";}
      else{
      std::vector<string> temp;
      temp.push_back(curr_lexeme);
      temp.push_back(std::to_string(mem_addr));
      symbol_table.push_back(temp);
      mem_addr++;
    }
    }
}

void print_symbol_table() {
  std::cout << "\n\nHere is the symbol table:\n";
  for(int i = 0; i < symbol_table.size()-1; i++) {
    std::cout << "Lexeme: " << symbol_table[i][0] << "     Memory Address: "
              << symbol_table[i][1] << std::endl;
  }
}

//Printing instruction table array
void print_instruct_table() {
  std::cout << "\n\nHere is the symbol table:\n";
  for(int i = 0; i <= 1000; i++) {
    std::cout << "Address: " << Instruct_Table[i][0] << "     Op: "
              << Instruct_Table[i][1] << "     Oprnd: "<< Instruct_Table[0][1][i] << std::endl;
  }
}