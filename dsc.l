%{
/**************************************************************************
 * Source Id :
 *
 * $Id: dsc.l,v 1.13 1993/11/03 10:04:46 kevinl Exp $
 *-------------------------------------------------------------------------
 * Project Notes :
 *
 *  Diamond Base
 *  ============
 *      A solid database implementation, spurred on by the continuing
 *  Metal (Lead) Base saga.
 *
 *  Project Team :
 *        A. Davison
 *        K. Lentin
 *        D. Platt
 *
 *	Project Commenced : 05-02-1993
 *
 *-------------------------------------------------------------------------
 *  Module Notes :
 *
 *
 *	Lexical Analyser for Relation Compiler.
 *
 *
 *  Original Author : Andy
 *
 *-------------------------------------------------------------------------
 * Revision History:
 *
 * $Log: dsc.l,v $
 * Revision 1.13  1993/11/03  10:04:46  kevinl
 * Added ichar and utils.{h,cc}
 *
 * Revision 1.12  1993/10/19  14:27:37  kevinl
 * Fixed constructors/retrievers
 *
 * Revision 1.11  1993/10/19  11:50:27  kevinl
 * Assignment/default values/constructors
 *
 * Revision 1.10  1993/10/19  08:27:13  davison
 * Added assign token
 *
 * Revision 1.9  1993/10/18  08:01:41  kevinl
 * Added constructors and fixed some probs
 *
 * Revision 1.8  1993/10/10  03:05:23  davison
 * Added grammar prototypes for relational links
 *
 * Revision 1.7  1993/09/26  06:40:32  kevinl
 * Added dbData support
 *
 * Revision 1.6  1993/07/11  09:42:05  kevinl
 * Changed String to dbString
 *
 * Revision 1.5  1993/06/20  13:43:40  kevinl
 * Added STRING
 *
 * Revision 1.4  1993/04/06  06:57:42  davison
 * Fixed - underscored allowed in ident names.
 *
 * Revision 1.3  1993/04/02  02:17:00  davison
 * Removed a few keywords.
 *
 * Revision 1.2  1993/03/15  18:59:15  davison
 * Implemented a few nice things, like more than 2 index fields, line number
 * reporting on syntax errors, and usage reporting.
 *
 * Just for you, Daz :-)
 *
 * Revision 1.1  1993/03/15  18:55:09  davison
 * Initial revision
 *
 **************************************************************************/
#ifdef __BORLANDC__
#include <io.h>
#endif
%}


newline [\n]
delim   [ \t\r] 
ws      {delim}+                      
alpha   [A-Za-z_]                      
digit   [0-9]                         
alnum   [a-zA-Z0-9_]
id		{alpha}{alnum}*                      
num		[0-9]+                                      
dot		[.]
tcomment "//"[^\n]*"\n"
hcomment "#"[^\n]*"\n"
comma	","

%%                                    
{ws}	    {/*No action, no return*/}    
{newline}	{ linenum++; }
relation	{return(RELATION);}
";"		{return(SEMI);}
"{"		{return(LCBRACE);}
"}"		{return(RCBRACE);}
"["		{return(LSBRACE);}
"]"		{return(RSBRACE);}
"="		{return(ASSIGN);}

short	{return(SHORT);}
ushort	{return(USHORT);}
long	{return(LONG);}
ulong	{return(ULONG);}
double	{return(DOUBLE);}
float	{return(FLOAT);}
money	{return(MONEY);}
date	{return(DATE);}
char	{return(CHAR);}
ichar	{return(ICHAR);}
dbString	{return(STRING);}
dbData	{return(DATA);}
index	{return(INDEX);}
on		{return(ON);}
unique	{return(UNIQUE);}
is		{return(IS);}
called  {return(CALLED);}
ordered {return(ORDER);}
using	{return(USING);}
construct { return(CONSTRUCT);}

{tcomment} {linenum++;}
{hcomment} {linenum++;}
{id}	{return(IDENT);} 
{num}	{return(NUM);}
{comma} {return(COMMA);}
{dot}   {return(DOT);}
%%

void yyGetToSemi(void)
{
	long len = 0;
	char c;

	while ((c=yyinput()) != ';')
		yytext[len++] = c;
	
	yytext[len] = 0;
	unput(c);
}
