/*
    Handling compatibility for CG.
    Copyright (C) 2020-2021 Renjian Wang

    This file is part of SCC.

    SCC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SCC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SCC.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "cg.h"

namespace scc
{
const char* lang = "\
NONE,\n\
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
COMMENT,////\n\
INTERR,//[0-9][_a-zA-Z0-9]*\n\
CHARERR,//'[ -~]'\n\
FEOF,\n\
END,\n\
#define TYPE_NAME_MAX 10\n\
";
}
