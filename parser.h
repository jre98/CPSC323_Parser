#ifndef PARSER_H
#define PARSER_H

// This .h file holds all of the function prototypes so that the program will
// not give a undeclared function error
void RAT21F();
void Opt_Function_Defs();
void Function_Defs();
void Function_Defs_P();
void Function();
void Opt_Parameter_List();
void Parameter_List();
void Parameter_List_P();
void Parameter();
void Qualifier();
void Body();
void Opt_Declaration_List();
void Declaration_List();
void Declaration_List_P();
void Declaration();
void IDs();
void IDs_P();
void Statement_List();
void Statement_List_P();
void Statement();
void Compound();
void Assign();
void If();
void If_P();
void If_P_P();
void Return();
void Return_P();
void Print();
void Scan();
void While();
void Condition();
void Relop();
void Expression();
void Expression_P();
void Term();
void Term_P();
void Factor();
void Primary();
void Empty();
void Identifier();
void Integer();
void Real();

void match(std::string& lexeme, std::string corrrect_lexeme, std::string& token, std::string curr_prod);

#endif
