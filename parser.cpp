#include <iostream>
#include <string>
#include "parser.h"
#include "Lexer.cpp"


// Example string being used to parse through tree
std::string test = "function convert1x (fahr integer) { return fahr; } # integer a; a=5; # $";
// A  bool that can be changed depending on if we want to print out the current
// production or not
bool print_switch = false;

// The only way I could figure out how to get Opt_Function_Defs to pick the
// correct path
bool is_past_func_defs = false;

vector<vector<string>> lexemes = Lexer();

// String to hold the current lexeme that we are dealing with
std::string curr_lexeme = lexemes[0][0];
// String to hold the current token we are dealing with
std::string curr_token = lexemes[0][1];

// Starting function to kick off recursive parser
void RAT21F() {

  // Initialize curr_lexeme and curr_token to whatever first token and lexeme
  // are in the source code (initialize here)

  Opt_Function_Defs();

  // After the function definitions comes the "main" function, which is required
  // to be enclosed by # CODE #
  // Match the # symbol and update vars
  match(curr_lexeme, "#", curr_token, "RAT21F");
  is_past_func_defs = true;
  // Call next productions
  Opt_Declaration_List();
  Statement_List();
  // Match end of "main" function marker
  match(curr_lexeme, "#", curr_token, "RAT21F");
  // If we get to the final token and it is the end of string marker, we know
  // that the parsing was successful, given there were no other errors along the
  // way
  if (curr_lexeme == "$") {
    std::cout << "This parser has reached the end of the file, parsing was "
                 " successful." << std::endl;
  }
  // Otherwise, there was some issue with parsing the file
  else {
    std::cout << "There was an error while parsing the file" << std::endl;
  }

}

void Opt_Function_Defs() {
  // if the current lexeme is the # seperator, this means that there were no
  // function definitions in the source file. If there are no function
  // definitions, we call Empty(). If there are, we call Function_Defs()
  if(curr_lexeme != "#") {
    Function_Defs();
  }
  else {
    Empty();
  }
}

void Function_Defs() {
    Function();
    Function_Defs_P();
}

void Function_Defs_P() {
  // If the current lexeme is #, we know that there are no more function
  // definitions, as the # indicates the start of the main function. In the case
  // where we have reached the main function, we call Epsilon, so we just return
  if (curr_lexeme == "#") {
    return;
  }
  // Otherwise, there are still more functions being declared, so we will loop
  // back to Function_Defs()
  else {
    Function_Defs();
  }
}

void Function() {
  // Function definitions come at the beginning of the RAT21F file and are all
  // declared using the keyword function. If curr_lexeme is 'function', a
  // function is being declared and we will match it and advance to next lexeme
  match(curr_lexeme, "function", curr_token, "Function1");

  // After matching the lexeme, the match function updates the global variables,
  // so we continue on and call the next function

  Identifier();
  // Match '('
  match(curr_lexeme, "(", curr_token, "Function2");
  Opt_Parameter_List();
  //  Match ')'
  match(curr_lexeme, ")", curr_token, "Function3");
  Opt_Declaration_List();
  Body();
  // Call Body (Body -> { <Statement List> })

}

void Opt_Parameter_List() {
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
  Parameter();
  Parameter_List_P();
}

void Parameter_List_P() {
  // If we have more than one parameter, the next correct lexeme would be a
  // comma to separate the parameters
  if(curr_lexeme == ",") {
    // Update variables
    match(curr_lexeme, ",", curr_token, "Parameter_List_P");
    Parameter_List();
  }
  // If the next lexeme is not a comma, this means we have reached the end of
  // the parameter list, so call Empty()
  else {
    Empty();
  }
}

void Parameter() {
  IDs();
  Qualifier();
}

void Qualifier() {
  // A qualifier is allowed to be an int, bool or real. If the current lexeme is
  // any of these three, we will enter the if statement
  // if (curr_lexeme == "integer" || curr_lexeme == "boolean" || curr_lexeme == "real") {
  //   // Use the match function to update variables and output information
  //   match(curr_lexeme, curr_lexeme, curr_token);
  // }
  if (curr_lexeme == "integer" || curr_lexeme == "real" || curr_lexeme == "boolean") {
    match(curr_lexeme, curr_lexeme, curr_token, "Qualifier");
  }
  else {
    std::cout << "Invalid Qualifier" << std::endl;
  }
}

void Body() {
  // Beginning of function body is {
  match(curr_lexeme, "{", curr_token, "Body");
  // Statements inside the function
  Statement_List();
  // End of function
  match(curr_lexeme, "}", curr_token, "Body");
}

void Opt_Declaration_List() {
  // If the current token is an identifier, we know that there will be a list of
  // declarations to handle, so we will call Declaration_List()
  // if(curr_lexeme == "integer" || curr_lexeme == "real" || curr_lexeme == "boolean") {
  //   Declaration_List();
  // }
  // // Otherwise, if there are no declarations, we will continue on
  // else {
  //   return;
  // }
  if(curr_lexeme == "{" || curr_lexeme == "#") {
    Empty();
  }
  else {
    Declaration_List();
  }

}

void Declaration_List() {

  Declaration();
  match(curr_lexeme, ";", curr_token, "Declaration_List");
  Declaration_List_P();
}

void Declaration_List_P() {
  //<Declaration List>’ ::= <Declaration List>  |  Ep
  // If the next token is a keyword, then we know that we have another
  // declaration coming, so we call Declaration_List()
  if (curr_lexeme == "integer" || curr_lexeme == "real" || curr_lexeme == "boolean") {
    Declaration_List();
  }
  // Otherwise, we return and continue on
  else {
    return;
  }
}

void Declaration() {
  //<Declaration> :: = integer    |    boolean    |  real         <IDs>
  // Match the current lexeme to the correct keyword, then have variables be
  // updated by match
  // match(curr_lexeme, curr_lexeme, curr_token, "Declaration");
  if(curr_lexeme == "integer") {
    match(curr_lexeme, "integer", curr_token, "Declaration1");
  }
  else if(curr_lexeme == "boolean") {
    match(curr_lexeme, "boolean", curr_token, "Declaration2");
  }
  else if(curr_lexeme == "real") {
    match(curr_lexeme, "real", curr_token, "Declaration3");
  }
  else {
    std::cout << "Invalid Declaration" << std::endl;
  }

  // After matching the keyword, call IDs
  IDs();
}

void IDs() {
  Identifier();
  IDs_P();
}

void IDs_P() {
  // If the next token is an identifier, call IDs again to make sure the next
  // identifier on the list is valid
  if(curr_lexeme == ",") {
    match(curr_lexeme, ",", curr_token, "IDs_P");
    IDs();
  }
  // Otherwise, call empty and move on
  else {
    Empty();
  }
}

void Statement_List() {
  Statement();
  Statement_List_P();
}

void Statement_List_P() {
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
  // Compound production begins with terminal symbol {
  if (curr_lexeme == "{") {
    // Compound();
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
    // Print();
  }
  // Scan statements start with a terminal get
  else if (curr_lexeme == "get") {
    // Scan();
  }
  // While statements begin with a terminal while
  else if (curr_lexeme == "while") {
    // While();
  }
  else {
    // std::cout << curr_lexeme << std::endl;
    // std::cout << curr_token << std::endl;
    // std::cout << curr_lexeme[8] << std::endl;
    // std::cout << curr_token[8] << std::endl;
    // std::cout << "Invalid Statement" << std::endl;
  }
}

// TODO
void Compound() {
  match(curr_lexeme, "{", curr_token, "Compound");
  Statement_List();
}

void Assign() {
  //<Assign> ::=     <Identifier> = <Expression> ;
  Identifier();
  // Match the = for the correct assignment
  match(curr_lexeme, "=", curr_token, "Assign1");
  Expression();
  // Next, match the ";" that ends the assignment
  match(curr_lexeme, ";", curr_token, "Assign2");

}

//TODO
void If() {
  // std::cout << "This is the If production" << std::endl;
  match(curr_lexeme, "if", curr_token, "If1");
  match(curr_lexeme, "(", curr_token, "If2");
  Condition();
  match(curr_lexeme, ")", curr_token, "If3");
  Statement();
  If_P();
}

void If_P() {
  if (curr_lexeme == "endif") {
    match(curr_lexeme, curr_lexeme, curr_token, "If_P1");
  }
  else if (curr_lexeme == "else") {
    match(curr_lexeme, curr_lexeme, curr_token, "If_P2");
    Statement();
    match(curr_lexeme, "endif", curr_token, "If_P3");
  }
}

void Return() {
  // This production begins with a terminal "return"
  match(curr_lexeme, "return", curr_token, "Return");
  Return_P();
}

void Return_P() {
  // There are two cases: something like: return; OR return fahr;. The top if
  // statement deals with cases like return;, the bottom adds the ability to
  // parse the Expression and then match the ; separator
  if (curr_lexeme == ";") {
    return;
  }
  else {
    Expression();
    match(curr_lexeme, ";", curr_token, "Return_P");
  }
}

//TODO
void Print() {
  match(curr_token, "put", curr_token, "Print1");
  match(curr_token, "(", curr_token, "Print2");
  Expression();
  match(curr_token, ")", curr_token, "Print3");
  match(curr_token, ";", curr_token, "Print4");
}

//TODO
void Scan() {
  match(curr_token, "get", curr_token, "Scan1");
  match(curr_token, "(", curr_token, "Scan2");
  IDs();
  match(curr_token, ")", curr_token, "Scan3");
  match(curr_token, ";", curr_token, "Scan4");
}

//TODO
void While() {
  match(curr_token, "while", curr_token, "While1");
  match(curr_token, "(", curr_token, "While2");
  Condition();
  match(curr_token, ")", curr_token, "While3");
  Statement();
}

//TODO
void Condition() {
  Expression();
  Relop();
  Expression();
}

//TODO
void Relop() {
  if(curr_lexeme ==  "==") {
    match(curr_lexeme, curr_lexeme, curr_token, "Relop1");
  }
  else if(curr_lexeme == "!=") {
    match(curr_lexeme, curr_lexeme, curr_token, "Relop2");
  }
  else if(curr_lexeme == ">") {
    match(curr_lexeme, curr_lexeme, curr_token, "Relop3");
  }
  else if(curr_lexeme == "<") {
    match(curr_lexeme, curr_lexeme, curr_token, "Relop4");
  }
  else if(curr_lexeme == "<=") {
    match(curr_lexeme, curr_lexeme, curr_token, "Relop5");
  }
  else if(curr_lexeme == "=>") {
    match(curr_lexeme, curr_lexeme, curr_token, "Relop6");
  }
  else {
    std::cout << "Invalid real operator" << std::endl;
  }
}


void Expression() {
  Term();
  Expression_P();
}

void Expression_P() {
  // If statements based on revised grammar
  if (curr_lexeme == "+") {
    match(curr_lexeme, "+", curr_token, "Expression_P1");
    Term();
    Expression_P();
  }
  else if (curr_lexeme == "-") {
    match(curr_lexeme, "-", curr_token, "Expression_P");
    Term();
    Expression_P();
  }
  // If we have neither + nor - as the lexeme, we just return
  else {
    return;
  }
}

void Term() {
  Factor();
  Term_P();
}

void Term_P() {
  // If statements based on revised grammar
  if (curr_lexeme == "*") {
    match(curr_lexeme, "*", curr_token, "Term_P");
    Term_P();
  }
  else if (curr_lexeme == "/") {
    match(curr_lexeme, "/", curr_token, "Term_P");
    Term_P();
  }
  // If we have neither * nor / as the lexeme, just return and keep going
  else {
    return;
  }
}

void Factor() {
  // <Factor> ::=      -  <Primary>    |    <Primary>
  // Case where we have a terminal -
  if (curr_lexeme == "-") {
    match(curr_lexeme, "-", curr_token, "Factor");
    Primary();
  }
  // Case where we do not have terminal -
  else {
    Primary();
  }
}

void Primary() {
  // <Primary> ::=  <Identifier>  |  <Integer>  |   <Identifier>  ( <IDs> )   |   ( <Expression> )   |  <Real>  |   true   |  false
  // This is not exactly correct, do we need left factorization here? Otherwise
  // the parser will not know whether to go down the <Identifier> or <Identifier> ( <IDs> ) path
  // Could be done by calling the parser and using it as a sort of lookahead?
  // For now, just deal with <Identifier>
  // if (curr_token == "identifier" && lookahead == "(" )

  // Lots of if statements to determind where the current lexeme falls in terms
  // of which production should be called next
  // std::cout << "The lexeme is: "<< lexemes[1][0] << std::endl;
  if (curr_token == "identifier" && lexemes[1][0] == "(") {
    std::cout << "Statement has been reached" << std::endl;
    Identifier();
    match(curr_lexeme, "(", curr_token, "Primary1");
    IDs();
    match(curr_lexeme, ")", curr_token, "Primary2");
  }
  else if (curr_token == "identifier") {

    Identifier();
  }
  else if (curr_token == "integer") {
    // Not defined yet
    Integer();
  }
  else if (curr_token == "real") {
    // Real is not yet defined
    Real();
  }
  else if (curr_lexeme == "(") {
    match(curr_lexeme, "(", curr_token, "Primary3");
    Expression();
    match(curr_lexeme, ")", curr_token, "Primary4");
  }
  else if (curr_lexeme == "true" || curr_lexeme == "false") {
    match(curr_lexeme, curr_lexeme, curr_token, "Primary5");
  }
}

// Empty function (goes to epsilon)
void Empty() {
  return;
}

void Identifier() {
  // Since we already have the lexeme, we just need to make sure the token is
  // not error, as this would indicate that we have an incorrect lexeme
  if(curr_token == "identifier") {
    // std::cout << "Token: " << curr_token << "          Lexeme: " << curr_lexeme << std::endl;
    match(curr_lexeme, curr_lexeme, curr_token, "Identifier");
    // Update curr_token and curr_lexeme
  }
  else {
    std::cout << "Error: invalid identifier" << std::endl;
  }
}

void Integer() {
  // Since we already have the lexeme, we just need to make sure the token is
  // not error, as this would indicate that we have an incorrect lexeme
  if(curr_token == "integer") {
    // std::cout << "Token: " << curr_token << "          Lexeme: " << curr_lexeme << std::endl;
    // Update lexeme and token here
    match(curr_lexeme, curr_lexeme, curr_token, "Integer");
  }
  else {
    std::cout << "Invalid integer value" << std::endl;
  }
}

void Real() {
  if (curr_token == "real") {
    match(curr_lexeme, curr_lexeme, curr_token, "Real");
  }
  else {
    std::cout << "Invalid integer value" << std::endl;
  }
}


// Match function is for matching terminal symbols and making sure they are correct
// std::string token is a parameter because if we match, we want to print out
// the info about the lexeme, including its token type
void match(std::string& curr_lexeme, std::string corrrect_lexeme, std::string& token, std::string curr_prod) {
  // If the lexeme is correct, display its information and token type
  if(curr_lexeme == corrrect_lexeme) {
    std::cout << "Token: " << curr_token << "          Lexeme: " << curr_lexeme << std::endl;
    // After printing info about the lexeme, update the global variables that
    // store the current token and the current lexeme

    // Erase the first row of entries from the lexemes vector
    lexemes.erase(lexemes.begin());
    // Since the next lexeme and token are now at the beginning of the list, we
    // simply reassign curr_token and curr_lexeme to the beginning of the vector
    curr_lexeme = lexemes[0][0];
    curr_token = lexemes[0][1];
  }
  // Otherwise, display an error message of some sort
  else {
    std::cout << "Error Message generated by production " << curr_prod << std::endl;
  }
}

// Function to determine if there is a statement upcoming (for use in Opt_Declaration_List)
