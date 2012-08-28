/* lexer.l */

%{
  #include "y.tab.h"
  #include <stdlib.h>
  void yyerror_alt(char *, char *);
  int yycolumn = 1;
  void yyuseraction(void);
  char * extract_string(char *);
  #define YY_USER_ACTION yyuseraction();
%}

%option yylineno

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
"delete"    return DELETE;
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
[ \t\v\f]   ;
\n          yycolumn = 0;
<<EOF>>     { yycolumn = 0; return EOF; }
.           yyerror_alt("Parse error", yytext);

%%
  
void
yyerror_alt(char * error, char * val)
{
  fprintf(stderr, "Line %d, %s: %s\n", yylloc.first_line, error, val);
}

int 
yywrap(void) 
{
  return 1;
}

void
yyuseraction(void)
{
  yylloc.first_line = yylloc.last_line = yylineno;
  yylloc.first_column = yycolumn; 
  yylloc.last_column = yycolumn + yyleng - 1;
  yycolumn += yyleng; 
}

char *
extract_string(char * src)
{
  char *dest = malloc((strlen(src) - 1) * sizeof(char));
  int len = strlen(src);
  snprintf(dest, len - 1, "%.*s\n", len - 2, src + 1);
  return dest;
}
