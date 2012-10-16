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
  int fh_token(char *, int);
  #define TOKEN(name,tok) fh_token((name),(tok))
  #define YY_USER_ACTION yyuseraction();
  #undef YY_INPUT
  #define YY_INPUT(buf,result,max_size) result = fh_get_input(buf, max_size);
%}

%option yylineno
%option nounput
%option noinput

%%

    /* KEYWORDS */
"while"      return TOKEN("LITERAL", WHILE);
"do"         return TOKEN("LITERAL", DO);
"for"        return TOKEN("LITERAL", FOR);
"if"         return TOKEN("LITERAL", IF);
"in"         return TOKEN("LITERAL", IN);
"else"       return TOKEN("LITERAL", ELSE);
"break"      return TOKEN("LITERAL", BREAK);
"continue"   return TOKEN("LITERAL", CONTINUE);
"return"     return TOKEN("LITERAL", RETURN);
"throw"      return TOKEN("LITERAL", THROW);
"function"   return TOKEN("LITERAL", FUNCTION);
"delete"     return TOKEN("LITERAL", DELETE);
"void"       return TOKEN("LITERAL", VOID);
"typeof"     return TOKEN("LITERAL", TYPEOF);
"instanceof" return TOKEN("LITERAL", INSTANCEOF);
"switch"     return TOKEN("LITERAL", SWITCH);
"case"       return TOKEN("LITERAL", CASE);
"default"    return TOKEN("LITERAL", DEFAULT);
"new"        return TOKEN("LITERAL", NEW);
"var"        return TOKEN("LITERAL", VAR);
"null"       return TOKEN("LITERAL", NULLT);
"true"       return TOKEN("LITERAL", TRUE); 
"false"      return TOKEN("LITERAL", FALSE);

    /* LITERALS */
L?\"(\\.|[^\\"])*\"          { yylval.val = fh_extract_string(yytext); 
                               return TOKEN("STR", STRING); } 
L?'(\\.|[^\\'])*'            { yylval.val = fh_extract_string(yytext); 
                               return TOKEN("STR", STRING); } 
[_\$A-Za-z][_\$0-9A-Za-z]*   { yylval.val = yytext; 
                               return TOKEN("IDENT", IDENT); }
[0-9]+                       { yylval.intval = atoi(yytext); 
                               return TOKEN("INT", INTEGER); }
[0-9]+\.[0-9]+               { yylval.floatval = atof(yytext); 
                               return TOKEN("FLOAT", FLOAT); }
    /* TODO: This is very incomplete */
\/[^\ ]+\/([imgy]{0,4})      { yylval.val = yytext;
                               return TOKEN("REGEXP", REGEXP); }

    /* COMMENTS */
[/][/].*       ; /* ignore comments */

    /* OPERATORS */
"||"        return TOKEN("OP", OR); 
"&&"        return TOKEN("OP", AND); 
"++"        return TOKEN("OP", PLUSPLUS); 
"--"        return TOKEN("OP", MINUSMINUS); 
"=="        return TOKEN("OP", EQEQ); 
"!="        return TOKEN("OP", NE); 
"<="        return TOKEN("OP", LTE); 
">="        return TOKEN("OP", GTE); 
"+="        return TOKEN("OP", PLUSEQ);
"-="        return TOKEN("OP", MINUSEQ); 
"*="        return TOKEN("OP", MULTEQ); 
"/="        return TOKEN("OP", DIVEQ); 
"%="        return TOKEN("OP", MODEQ);
"<<"        return TOKEN("OP", LSHIFT);
">>"        return TOKEN("OP", RSHIFT);
"==="       return TOKEN("OP", STEQ);
"!=="       return TOKEN("OP", STNE);

[-+()\[\]=*/%<>,.:;?!{}]  return *yytext;
[ \t\v\f]   ;
\n          { yycolumn = 0; 
              yytext = NULL;
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
fh_token(char *name, int token)
{
  if (print_tokens) {
    if (yytext)
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
