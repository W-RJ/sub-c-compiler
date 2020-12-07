#include "cg.h"

namespace scc
{
const wchar_t* lang = 
L"NONE,\n\
INTCON,//0|[1-9][0-9]*\n\
CHARCON,//'[+\\-*/_a-zA-Z0-9]'\n\
STRCON,//\"[ !#-~]*\"\n\
CONSTTK,//const\n\
INTTK,//int\n\
CHARTK,//char\n\
VOIDTK,//void\n\
MAINTK,//main\n\
IFTK,//if\n\
ELSETK,//else\n\
DOTK,//do\n\
WHILETK,//while\n\
FORTK,//for\n\
SCANFTK,//scanf\n\
PRINTFTK,//printf\n\
RETURNTK,//return\n\
PLUS,//+\n\
MINU,//-\n\
MULT,//\\*\n\
DIV,///\n\
LSS,//<\n\
LEQ,//<=\n\
GRE,//>\n\
GEQ,//>=\n\
EQL,//==\n\
NEQ,//!=\n\
ASSIGN,//=\n\
SEMICN,//;\n\
COMMA,//,\n\
LPARENT,//(\n\
RPARENT,//)\n\
LBRACK,//\\[\n\
RBRACK,//\\]\n\
LBRACE,//{\n\
RBRACE,//}\n\
IDENFR,//[_a-zA-Z][_a-zA-Z0-9]*\n\
END,\n";
}
