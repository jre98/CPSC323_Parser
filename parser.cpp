#include <iostream>
#include <string>
#include "parser.h"
//#include "LexicalAnalyzer.cpp"

// Example string being used to parse through tree
std::string test = "function convert1x (fahr integer) { return fahr; } # integer a; a=5; # $";
// A  bool that can be changed depending on if we want to print out the current
// production or not
bool print_switch = false;

// String to hold the current lexeme that we are dealing with
std::string curr_lexeme = "function";
// String to hold the current token we are dealing with
std::string curr_token = "identifier";

// Starting function to kick off recursive parser
void RAT21F() {

  // Initialize curr_lexeme and curr_token to whatever first token and lexeme
  // are in the source code (initialize here)

  // Infinite Recursion happening down this route
  Opt_Function_Defs();

  // After the function definitions comes the "main" function, which is required
  // to be enclosed by # CODE #
  // Match the # symbol and update vars
  match(curr_lexeme, "#", curr_token);
  // Call next productions
  Opt_Declaration_List();
  Statement_List();
  // Match end of "main" function marker
  match(curr_lexeme, "#", curr_token);
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
  match(curr_lexeme, "function", curr_token);

  // After matching the lexeme, the match function updates the global variables,
  // so we continue on and call the next function

  Identifier();
  // Match '('
  match(curr_lexeme, "(", curr_token);
  Opt_Parameter_List();
  //  Match ')'
  match(curr_lexeme, ")", curr_token);
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
    match(curr_lexeme, ",", curr_token);
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
  if (curr_lexeme == "integer" || curr_lexeme == "boolean" || curr_lexeme == "real") {
    // Use the match function to update variables and output information
    match(curr_lexeme, curr_lexeme, curr_token);
  }
}

void Body() {
  // Beginning of function body is {
  match(curr_lexeme, "{", curr_token);
  // Statements inside the function
  Statement_List();
  // End of function
  match(curr_lexeme, "}", curr_token);
}

void Opt_Declaration_List() {
  // If the current token is an identifier, we know that there will be a list of
  // declarations to handle, so we will call Declaration_List()
  if(curr_token == "integer" || curr_token == "real" || curr_token == "boolean") {
    Declaration_List();
  }
  // Otherwise, if there are no declarations, we will continue on
  else {
    return;
  }
}

void Declaration_List() {
  Declaration();
  match(curr_lexeme, ";", curr_token);
  Declaration_List_P();
}

void Declaration_List_P() {
  //<Declaration List>’ ::= <Declaration List>  |  Ep
  // If the next token is a keyword, then we know that we have another
  // declaration coming, so we call Declaration_List()
  if (curr_token == "integer" || curr_token == "real" || curr_token == "boolean") {
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
  if(curr_lexeme == "integer") {
    match(curr_lexeme, "integer", curr_token);
  }
  else if(curr_lexeme == "boolean") {
    match(curr_lexeme, "boolean", curr_token);
  }
  else if(curr_lexeme == "real") {
    match(curr_lexeme, "real", curr_token);
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
  if(curr_token == "identifier") {
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
    // If();
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
    std::cout << "Invalid Statement" << std::endl;
  }
}

// TODO
void Compound() {

}

void Assign() {
  //<Assign> ::=     <Identifier> = <Expression> ;
  Identifier();
  // Match the = for the correct assignment
  match(curr_lexeme, "=", curr_token);
  Expression();
  // Next, match the ";" that ends the assignment
  match(curr_lexeme, ";", curr_token);

}

//TODO
void If() {

}

//TODO
void If_P() {

}

void Return() {
  // This production begins with a terminal "return"
  match(curr_lexeme, "return", curr_token);
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
    match(curr_lexeme, ";", curr_token);
  }
}

//TODO
void Print() {

}

//TODO
void Scan() {

}

//TODO
void While() {

}

//TODO
void Condition() {

}

//TODO
void Relop() {

}


void Expression() {
  Term();
  Expression_P();
}

void Expression_P() {
  // If statements based on revised grammar
  if (curr_lexeme == "+") {
    match(curr_lexeme, "+", curr_token);
    Expression_P();
  }
  else if (curr_lexeme == "-") {
    match(curr_lexeme, "-", curr_token);
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
    match(curr_lexeme, "*", curr_token);
    Term_P();
  }
  else if (curr_lexeme == "/") {
    match(curr_lexeme, "/", curr_token);
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
    match(curr_lexeme, "-", curr_token);
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
  if (curr_token == "identifier") {
    Identifier();
  }
  else if (curr_token == "integer") {
    // Not defined yet
    // Integer();
  }
  else if (curr_token == "real") {
    // Real is not yet defined
    // Real();
  }
  else if (curr_lexeme == "(") {
    // match(curr_lexeme, "(", curr_token);
    // Expression();
    // match(curr_lexeme, ")", curr_token);
  }
  else if (curr_lexeme == "true" || curr_lexeme == "false") {
    return;
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
  }
  else {
    std::cout << "Invalid integer value" << std::endl;
  }
}

// Match function is for matching terminal symbols and making sure they are correct
// std::string token is a parameter because if we match, we want to print out
// the info about the lexeme, including its token type
void match(std::string curr_lexeme, std::string corrrect_lexeme, std::string token) {
  // If the lexeme is correct, display its information and token type
  if(curr_lexeme == corrrect_lexeme) {
    // std::cout << "Token: " << curr_token << "          Lexeme: " << curr_lexeme << std::endl;
    // After printing info about the lexeme, update the global variables that
    // store the current token and the current lexeme
  }
  // Otherwise, display an error message of some sort
  else {
    std::cout << "Error Message" << std::endl;
  }
}
