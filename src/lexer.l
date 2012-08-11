/* lexer.l */

%{
  #include "y.tab.h"
  #include <stdlib.h>
  void lyyerror(char *, char *);
  char * extract_string(char *);
%}

%%

    /* KEYWORDS */
"while"     return WHILE;
"do"        return DO;
"for"       return FOR;
"if"        return IF;
"else"      return ELSE;
"break"     return BREAK;
"continue"  return CONTINUE;
"return"    return RETURN;
"function"  return FUNCTION;
"new"       return NEW;
"var"       return VAR;
"null"      return NULLT;
"true"      return TRUE; 
"false"     return FALSE;

    /* LITERALS */
L?\"(\\.|[^\\"])*\"          { yylval.val = extract_string(yytext); return STRING; } 
L?'(\\.|[^\\'])*'            { yylval.val = extract_string(yytext); return STRING; } 
[_\$A-Za-z][_\$0-9A-Za-z]*   { yylval.val = yytext; return IDENT; }
[0-9]+                       { yylval.intval = atoi(yytext); return INTEGER; }
[0-9]+\.[0-9]+               { yylval.floatval = atof(yytext); return FLOAT; }

    /* COMMENTS */
[/][/].*       ; /* ignore comments */

    /* OPERATORS */
"||"        return OR; 
"&&"        return AND; 
"++"        return PLUSPLUS; 
"--"        return MINUSMINUS; 
"=="        return EQEQ; 
"!="        return NE; 
"<="        return LTE; 
">="        return GTE; 
"+="        return PLUSEQ;
"-="        return MINUSEQ; 
"*="        return MULTEQ; 
"/="        return DIVEQ; 
"%="        return MODEQ;
"<<"        return LSHIFT;
">>"        return RSHIFT;
"==="       return STEQ;
"!=="       return STNE;

[-+()\[\]=*/%<>,.:;!{}]  return *yytext;
[ \t\n]     ; /* skip whitespace, newlines (until we do ASI) */
<<EOF>>     return EOF;
.           lyyerror("Parse error", yytext);

%%
  
void
lyyerror(char * error, char * val)
{
  fprintf(stderr, "%s: %s\n", error, val);
}

int 
yywrap(void) 
{
  return 1;
}

char *
extract_string(char * src)
{
  char *dest = malloc(sizeof(char));
  int len = strlen(src);
  snprintf(dest, len - 1, "%.*s\n", len - 2, src + 1);
  return dest;
}
