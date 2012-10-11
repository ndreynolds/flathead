/* lexer.l */

%{
  #include "y.tab.h"
  #include <stdlib.h>
  void parse_error(char *);
  int yycolumn = 1;
  void yyuseraction(void);
  int interactive;
  int print_tokens;
  char * fh_extract_string(char *);
  int fh_get_input(char *, int);
  int fh_token(char *, int, int);
  #define TOKEN(name,tok) fh_token((name),(tok),0)
  #define TOKEN2(name,tok) fh_token((name),(tok),1)
  #define YY_USER_ACTION yyuseraction();
  #undef YY_INPUT
  #define YY_INPUT(buf,result,max_size) result = fh_get_input(buf, max_size);
%}

%option yylineno
%option nounput
%option noinput

%%

    /* KEYWORDS */
"while"     return TOKEN("WHILE", WHILE);
"do"        return TOKEN("DO", DO);
"for"       return TOKEN("FOR", FOR);
"if"        return TOKEN("IF", IF);
"in"        return TOKEN("IN", IN);
"else"      return TOKEN("ELSE", ELSE);
"break"     return TOKEN("BREAK", BREAK);
"continue"  return TOKEN("CONTINUE", CONTINUE);
"return"    return TOKEN("RETURN", RETURN);
"function"  return TOKEN("FUNCTION", FUNCTION);
"delete"    return TOKEN("DELETE", DELETE);
"void"      return TOKEN("VOID", VOID);
"typeof"    return TOKEN("TYPEOF", TYPEOF);
"new"       return TOKEN("NEW", NEW);
"var"       return TOKEN("VAR", VAR);
"null"      return TOKEN("NULL", NULLT);
"true"      return TOKEN("TRUE", TRUE); 
"false"     return TOKEN("FALSE", FALSE);

    /* LITERALS */
L?\"(\\.|[^\\"])*\"          { yylval.val = fh_extract_string(yytext); 
                               return TOKEN2("STR", STRING); } 
L?'(\\.|[^\\'])*'            { yylval.val = fh_extract_string(yytext); 
                               return TOKEN2("STR", STRING); } 
[_\$A-Za-z][_\$0-9A-Za-z]*   { yylval.val = yytext; 
                               return TOKEN2("IDENT", IDENT); }
[0-9]+                       { yylval.intval = atoi(yytext); 
                               return TOKEN2("INT", INTEGER); }
[0-9]+\.[0-9]+               { yylval.floatval = atof(yytext); 
                               return TOKEN2("FLOAT", FLOAT); }

    /* COMMENTS */
[/][/].*       ; /* ignore comments */

    /* OPERATORS */
"||"        return TOKEN2("OP", OR); 
"&&"        return TOKEN2("OP", AND); 
"++"        return TOKEN2("OP", PLUSPLUS); 
"--"        return TOKEN2("OP", MINUSMINUS); 
"=="        return TOKEN2("OP", EQEQ); 
"!="        return TOKEN2("OP", NE); 
"<="        return TOKEN2("OP", LTE); 
">="        return TOKEN2("OP", GTE); 
"+="        return TOKEN2("OP", PLUSEQ);
"-="        return TOKEN2("OP", MINUSEQ); 
"*="        return TOKEN2("OP", MULTEQ); 
"/="        return TOKEN2("OP", DIVEQ); 
"%="        return TOKEN2("OP", MODEQ);
"<<"        return TOKEN2("OP", LSHIFT);
">>"        return TOKEN2("OP", RSHIFT);
"==="       return TOKEN2("OP", STEQ);
"!=="       return TOKEN2("OP", STNE);

[-+()\[\]=*/%<>,.:;?!{}]  return *yytext;
[ \t\v\f]   ;
\n          { yycolumn = 0; 
              TOKEN("NEWLINE", 0);
              if (interactive) return EOF; }
<<EOF>>     { yycolumn = 0; 
              if (interactive) exit(0);
              return EOF; }
.           parse_error(yytext);

%%
  
void
parse_error(char * val)
{
  fprintf(
    stderr, "ParseError: unexpected '%s'\n  at Line %d:%d\n", 
    val, yylloc.first_line, yylloc.first_column
  );
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

// Wrap token returns for debugging.
int
fh_token(char *name, int token, int show_text)
{
  if (print_tokens) {
    if (show_text)
      printf("(%s %s)\n", name, yytext);
    else
      printf("(%s)\n", name);
  }
  return token;
}

// Chop off the first and last characters. Used for "x" => x
char *
fh_extract_string(char *src)
{
  char *dest = malloc((strlen(src) - 1) * sizeof(char));
  int len = strlen(src);
  snprintf(dest, len - 1, "%.*s\n", len - 2, src + 1);
  return dest;
}
