/*
 * lexer.l - Flex scanner spec
 *
 * Copyright (c) 2012 Nick Reynolds
 *  
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

%{
  #include "src/flathead.h"
  #include "y.tab.h"
  #include <stdlib.h>
  void parse_error(char *);
  int yycolumn = 1;
  void yyuseraction(void);
  char * fh_extract_string(char *);
  int fh_get_input(char *, int);
  int fh_token(char *, int);

  #define TOKEN(name,tok)   fh_token((name),(tok))
  #define OP(tok)           fh_token("OP",(tok))
  #define KEYWORD(tok)      fh_token("KEYWORD",(tok))

  #define YY_USER_ACTION yyuseraction();
  #undef YY_INPUT
  #define YY_INPUT(buf,result,max_size) result = fh_get_input(buf, max_size);
%}

%option yylineno
%option nounput
%option noinput

D            [0-9]
H            [a-fA-F0-9]
E            [Ee][+-]?{D}+


%%

    /* KEYWORDS (reserved words) */
"while"      return KEYWORD(WHILE);
"do"         return KEYWORD(DO);
"for"        return KEYWORD(FOR);
"if"         return KEYWORD(IF);
"in"         return KEYWORD(IN);
"else"       return KEYWORD(ELSE);
"break"      return KEYWORD(BREAK);
"continue"   return KEYWORD(CONTINUE);
"return"     return KEYWORD(RETURNT);
"throw"      return KEYWORD(THROW);
"try"        return KEYWORD(TRY);
"catch"      return KEYWORD(CATCH);
"finally"    return KEYWORD(FINALLY);
"with"       return KEYWORD(WITH);
"function"   return KEYWORD(FUNCTION);
"delete"     return KEYWORD(DELETE);
"void"       return KEYWORD(VOID);
"typeof"     return KEYWORD(TYPEOF);
"instanceof" return KEYWORD(INSTANCEOF);
"switch"     return KEYWORD(SWITCH);
"case"       return KEYWORD(CASE);
"default"    return KEYWORD(DEFAULT);
"debugger"   return KEYWORD(DEBUGGER);
"new"        return KEYWORD(NEW);
"var"        return KEYWORD(VAR);
"null"       return KEYWORD(NULLT);
"true"       return KEYWORD(TRUE); 
"false"      return KEYWORD(FALSE);

    /* LITERALS */
L?\"(\\.|[^\\"])*\"          { yylval.val = fh_extract_string(yytext); 
                               return TOKEN("STR", STRING); } 
L?'(\\.|[^\\'])*'            { yylval.val = fh_extract_string(yytext); 
                               return TOKEN("STR", STRING); } 
[_\$A-Za-z][_\$0-9A-Za-z]*   { yylval.val = yytext; 
                               return TOKEN("IDENT", IDENT); }
{D}+({E})?                   { yylval.floatval = atof(yytext); 
                               return TOKEN("INT", NUMBER); }
0[xX]{H}+                    { yylval.floatval = atof(yytext);
                               return TOKEN("HEX", NUMBER); }
{D}+\.{D}+({E})?             { yylval.floatval = atof(yytext); 
                               return TOKEN("FLOAT", NUMBER); }

    /* FIXME: This is very incomplete */
L?\/[^\ \/]+\/([imgy]{0,4})  { yylval.val = yytext;
                               return TOKEN("REGEXP", REGEXP); }

    /* COMMENTS */
[/][/].*       ; /* ignore comments */

    /* MULTI-CHARACTER PUNCTUATORS & OPERATORS */
"||"        return OP(OR); 
"&&"        return OP(AND); 
"++"        return OP(PLUSPLUS); 
"--"        return OP(MINUSMINUS); 
"=="        return OP(EQEQ); 
"!="        return OP(NE); 
"<="        return OP(LTE); 
">="        return OP(GTE); 
"+="        return OP(PLUSEQ);
"-="        return OP(MINUSEQ); 
"*="        return OP(MULTEQ); 
"/="        return OP(DIVEQ); 
"%="        return OP(MODEQ);
"&="        return OP(ANDEQ);
"^="        return OP(XOREQ);
"|="        return OP(OREQ);
"<<"        return OP(LSHIFT);
">>"        return OP(RSHIFT);
"<<="       return OP(LSHIFTEQ);
">>="       return OP(RSHIFTEQ);
"<<<"       return OP(ULSHIFT);
">>>"       return OP(URSHIFT);
"==="       return OP(STEQ);
"!=="       return OP(STNE);
">>>="      return OP(URSHIFTEQ);

    /* SINGLE-CHARACTER PUNCTUATORS & OPERATORS */
[-+()\[\]=*/%<>,.:`;?!{}]  { TOKEN("LITERAL", 0); 
                            return *yytext; }

    /* WHITESPACE & TABS */
[ \t\v\f]   ;

    /* NEWLINES */
\n          { yycolumn = 0; 
              yytext = NULL;
              TOKEN("NEWLINE", 0);
              if (fh_opt_interactive) return EOF; }

    /* END OF FILE */
<<EOF>>     { yycolumn = 0; 
              if (fh_opt_interactive) exit(0);
              return EOF; }

    /* UNKNOWN CHARACTER */
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
  if (fh_opt_print_tokens) {
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
