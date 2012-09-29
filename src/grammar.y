/*
 * grammar.y -- Bison/YACC parser definition for an EcmaScript
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
  #include <unistd.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <getopt.h>
  #include <readline/readline.h>
  #include <readline/history.h>
  #include "src/nodes.h"
  #include "src/eval.h"
  #include "src/runtime.h"
  #include "src/cli.h"

  #define YYDEBUG 0

  #define NEW_NODE(t,e1,e2,e3,d,s)   new_node(t,e1,e2,e3,d,s,yylloc.first_line,yylloc.first_column) 
  #define NEW_IDENT(name)            NEW_NODE(NODE_IDENT,0,0,0,0,name)
  #define NEW_WHILE(cnd,blck)        NEW_NODE(NODE_WHILE,cnd,blck,0,0,0)
  #define NEW_DOWHILE(cnd,blck)      NEW_NODE(NODE_DOWHILE,cnd,blck,0,0,0)
  #define NEW_BLOCK(stmtlst)         NEW_NODE(NODE_BLOCK,stmtlst,0,0,0,0)
  #define NEW_STMTLST(head,tail)     NEW_NODE(NODE_STMT_LST,head,tail,0,0,0)
  #define NEW_IF(p,q,r)              NEW_NODE(NODE_IF,p,q,r,0,0)
  #define NEW_TERN(p,q,r)            NEW_NODE(NODE_TERN,p,q,r,0,0)
  #define NEW_FOR(exps,stmt)         NEW_NODE(NODE_FOR,exps,stmt,0,0,0)
  #define NEW_EXPGRP(e1,e2,e3)       NEW_NODE(NODE_EXPGRP,e1,e2,e3,0,0)
  #define NEW_FORIN(lhs,in,stmt)     NEW_NODE(NODE_FORIN,lhs,in,stmt,0,0)
  #define NEW_NUM(x)                 NEW_NODE(NODE_NUM,0,0,0,x,0)
  #define NEW_BOOL(x)                NEW_NODE(NODE_BOOL,0,0,0,x,0)
  #define NEW_STR(x)                 NEW_NODE(NODE_STR,0,0,0,0,x)
  #define NEW_NULL()                 NEW_NODE(NODE_NULL,0,0,0,0,0)
  #define NEW_RETURN(exp)            NEW_NODE(NODE_RETURN,exp,0,0,0,0)
  #define NEW_CONT()                 NEW_NODE(NODE_CONT,0,0,0,0,0)
  #define NEW_BREAK()                NEW_NODE(NODE_BREAK,0,0,0,0,0)
  #define NEW_THIS()                 NEW_NODE(NODE_THIS,0,0,0,0,0)
  #define NEW_EMPTSTMT()             NEW_NODE(NODE_EMPT_STMT,0,0,0,0,0)
  #define NEW_EXP(a,b,op)            NEW_NODE(NODE_EXP,a,b,0,0,op)
  #define NEW_UNPOST(a,op)           NEW_NODE(NODE_UNARY_POST,a,0,0,0,op)
  #define NEW_UNPRE(a,op)            NEW_NODE(NODE_UNARY_PRE,a,0,0,0,op)
  #define NEW_EXPSTMT(exp)           NEW_NODE(NODE_EXP_STMT,exp,0,0,0,0)
  #define NEW_ASGN(a,b,op)           NEW_NODE(NODE_ASGN,a,b,0,0,op)
  #define NEW_ARR(ellst)             NEW_NODE(NODE_ARR,ellst,0,0,0,0)
  #define NEW_ELLST(head,tail)       NEW_NODE(NODE_EL_LST,head,tail,0,0,0)
  #define NEW_OBJ(proplst)           NEW_NODE(NODE_OBJ,proplst,0,0,0,0)
  #define NEW_PROP(name,exp)         NEW_NODE(NODE_PROP,name,exp,0,0,0)
  #define NEW_PROPLST(head,tail)     NEW_NODE(NODE_PROP_LST,head,tail,0,0,0)
  #define NEW_ARGLST(head,tail)      NEW_NODE(NODE_ARG_LST,head,tail,0,0,0)
  #define NEW_CALL(call,args)        NEW_NODE(NODE_CALL,call,args,0,0,0);
  #define NEW_NEW(exp)               NEW_NODE(NODE_NEW,exp,0,0,0,0);
  #define NEW_MEMBER(head,tail,exp)  NEW_NODE(NODE_MEMBER,head,tail,0,exp,0)
  #define NEW_FUNC(params,body,id)   NEW_NODE(NODE_FUNC,params,body,id,0,0)
  #define NEW_PARAMLST(head,tail)    NEW_NODE(NODE_PARAM_LST,head,tail,0,0,0)
  #define NEW_ELISION()              NEW_NODE(NODE_ELISION,0,0,0,0,0)
  #define NEW_SRCLST(head,tail)      NEW_NODE(NODE_SRC_LST,head,tail,0,0,0)
  #define NEW_VARSTMT(declst)        NEW_NODE(NODE_VAR_STMT,declst,0,0,0,0)
  #define NEW_VARDEC(id,exp)         NEW_NODE(NODE_VAR_DEC,id,exp,0,0,0)
  #define NEW_VARDECLST(head,tail)   NEW_NODE(NODE_VAR_DEC_LST,head,tail,0,0,0)

  void yyerror(const char *);
  int yylex(void);
  int yydebug;
  bool interactive = false;
  FILE *yyin;
  int fh_get_input(char *, int);
  Node *root;
  State *state;
%}

%error-verbose
%locations

/* LITERALS */

%token<floatval> FLOAT
%token<intval> INTEGER
%token<val> IDENT STRING TRUE FALSE


/* OPERATORS */

/* Boolean */
%token<val> AND OR
/* Unary */
%token<val> PLUSPLUS MINUSMINUS VOID DELETE TYPEOF
/* Relational */
%token<val> EQEQ GTE LTE NE STEQ STNE
/* Bitwise */
%token<val> LSHIFT RSHIFT
/* Assignment */
%token<val> PLUSEQ MINUSEQ MULTEQ DIVEQ MODEQ


/* KEYWORDS */

/* Iteration */
%token<val> WHILE DO FOR
/* Branch */
%token<val> IF ELSE
/* Control */
%token<val> BREAK CONTINUE RETURN
/* Misc */
%token<val> VAR IN THIS NULLT FUNCTION NEW 


/* ASSOCIATIVITY */

%left '+' '-' '*' '%' '/' AND OR

%union {
  char *val;
  int intval;
  double floatval;
  struct Node *node;
}

%type<val> AssignmentOperator 

%type<node> Program SourceElements SourceElement Statement Block 
%type<node> VariableStatement EmptyStatement ExpressionStatement IfStatement 
%type<node> IterationStatement ContinueStatement BreakStatement ReturnStatement 
%type<node> Literal NumericLiteral BooleanLiteral StringLiteral NullLiteral 
%type<node> ArrayLiteral ObjectLiteral Identifier Expression PrimaryExpression 
%type<node> LeftHandSideExpression UnaryExpression PostfixExpression 
%type<node> ShiftExpression RelationalExpression AdditiveExpression 
%type<node> EqualityExpression BitwiseANDExpression BitwiseORExpression 
%type<node> BitwiseXORExpression LogicalORExpression LogicalANDExpression 
%type<node> MultiplicativeExpression ConditionalExpression StatementList 
%type<node> AssignmentExpression ElementList PropertyName PropertyAssignment 
%type<node> PropertyNameAndValueList Function FunctionBody FunctionExpression 
%type<node> FormalParameterList CallExpression MemberExpression NewExpression 
%type<node> Arguments ArgumentList Elision VariableDeclarationList 
%type<node> VariableDeclaration Initializer

%%

Program                  : SourceElements                                    
                             { root = $1; }
                         ;

SourceElements           : SourceElement
                             { $$ = NEW_SRCLST($1, NULL); }
                         | SourceElements SourceElement
                             { $$ = NEW_SRCLST($2, $1); } 
                         ;

SourceElement            : Statement
                             { $$ = $1; }
                         | Function
                             { $$ = $1; }
                         ;

Statement                : Block                                             
                             { $$ = $1; }
                         | VariableStatement                               
                             { $$ = $1; }
                         | EmptyStatement                                  
                             { $$ = $1; }
                         | ExpressionStatement                             
                             { $$ = $1; }
                         | IfStatement                                     
                             { $$ = $1; }
                         | IterationStatement                              
                             { $$ = $1; }
                         | ContinueStatement                               
                             { $$ = $1; }
                         | BreakStatement                                  
                             { $$ = $1; }
                         | ReturnStatement                                 
                             { $$ = $1; }
                         ; 

Block                    : '{' StatementList '}'                             
                             { $$ = NEW_BLOCK($2); }
                         ;

StatementList            : Statement                                         
                             { $$ = NEW_STMTLST($1, NULL); } 
                         | StatementList Statement                         
                             { $$ = NEW_STMTLST($2, $1); }
                         ;

VariableStatement        : VAR VariableDeclarationList ';'
                             { $$ = NEW_VARSTMT($2); }
                         ;

VariableDeclarationList  : VariableDeclaration
                             { $$ = NEW_VARDECLST($1, NULL); }
                         | VariableDeclarationList ',' VariableDeclaration
                             { $$ = NEW_VARDECLST($3, $1); }
                         ;

VariableDeclaration      : Identifier Initializer
                             { $$ = NEW_VARDEC($1, $2); }
                         | Identifier
                             { $$ = NEW_VARDEC($1, NULL); }
                         ;

Initializer              : '=' AssignmentExpression
                             { $$ = $2; }
                         ;

EmptyStatement           : ';'                                               
                             { $$ = NEW_EMPTSTMT(); }
                         ;

ExpressionStatement      : Expression ';'                                    
                             { $$ = NEW_EXPSTMT($1); }
                         ;

IfStatement              : IF '(' Expression ')' Statement ELSE Statement    
                             { $$ = NEW_IF($3, $5, $7); }
                         | IF '(' Expression ')' Statement                 
                             { $$ = NEW_IF($3, $5, NULL); }
                         ;

IterationStatement       : DO Statement WHILE '(' Expression ')' ';'                              
                             { $$ = NEW_DOWHILE($2, $5); }
                         | WHILE '(' Expression ')' Statement                                
                             { $$ = NEW_WHILE($3, $5); }

                         /* for ( LHS/VarDecl in Expression ) Statement */

                         | FOR '(' LeftHandSideExpression IN Expression ')' Statement
                             { $$ = NEW_FORIN($3, $5, $7); }
                         | FOR '(' VAR VariableDeclaration IN Expression ')' Statement
                             { $$ = NEW_FORIN($4, $6, $8); }

                         /* for ( Expression/VarDecl ; Expresion ; Expression ) Statement */

                         | FOR '(' Expression ';' Expression ';' Expression ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($3, $5, $7), $9); }
                         | FOR '(' VAR VariableDeclarationList ';' Expression ';' Expression ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($4, $6, $8), $10); }
                         | FOR '(' Expression ';' Expression ';' ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($3, $5, NULL), $8); }
                         | FOR '(' VAR VariableDeclarationList ';' Expression ';' ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($4, $6, NULL), $9); }
                         | FOR '(' Expression ';' ';' Expression ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($3, NULL, $6), $8); }
                         | FOR '(' VAR VariableDeclarationList ';' ';' Expression ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($4, NULL, $7), $9); }
                         | FOR '(' Expression ';' ';' ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($3, NULL, NULL), $7); }
                         | FOR '(' VAR VariableDeclarationList ';' ';' ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($4, NULL, NULL), $8); }
                         | FOR '(' ';' Expression ';' Expression ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP(NULL, $4, $6), $8); }
                         | FOR '(' ';' Expression ';' ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP(NULL, $4, NULL), $7); }
                         | FOR '(' ';' ';' Expression ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP(NULL, NULL, $5), $7); }
                         | FOR '(' ';' ';' ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP(NULL, NULL, NULL), $6); }
                         ;

ContinueStatement        : CONTINUE ';'                       
                             { $$ = NEW_CONT(); }
                         ;

BreakStatement           : BREAK ';'                          
                             { $$ = NEW_BREAK(); }
                         ;

ReturnStatement          : RETURN ';'                         
                             { $$ = NEW_RETURN(NULL); }
                         | RETURN Expression ';'            
                             { $$ = NEW_RETURN($2); }
                         ;

Literal                  : NullLiteral                        
                             { $$ = $1; }
                         | BooleanLiteral                   
                             { $$ = $1; }
                         | NumericLiteral                   
                             { $$ = $1; }
                         | StringLiteral                    
                             { $$ = $1; }
                         | ObjectLiteral                    
                             { $$ = $1; }
                         ;

ArrayLiteral             : '[' ']'                            
                             { $$ = NEW_ARR(NULL); }
                         | '[' Elision ']'                  
                             { $$ = NEW_ARR(NULL); }
                         | '[' ElementList ']'              
                             { $$ = NEW_ARR($2); }
                         | '[' ElementList ',' ']'          
                             { $$ = NEW_ARR($2); }
                         | '[' ElementList ',' Elision ']'  
                             { $$ = NEW_ARR($2); }
                         ;

Elision                  : ','                                
                             { $$ = NEW_ELISION(); }
                         | Elision ','                      
                             { $$ = NEW_ELISION(); }
                         ;

ElementList              : AssignmentExpression                            
                             { $$ = NEW_ELLST($1, NULL); }
                         | Elision AssignmentExpression                  
                             { $$ = NEW_ELLST($2, NULL); }
                         | ElementList ',' AssignmentExpression          
                             { $$ = NEW_ELLST($3, $1); }
                         | ElementList ',' Elision AssignmentExpression  
                             { $$ = NEW_ELLST($4, $1); }
                         ;

BooleanLiteral           : TRUE                  
                             { $$ = NEW_BOOL(1); }
                         | FALSE               
                             { $$ = NEW_BOOL(0); }
                         ;

StringLiteral            : STRING                
                             { $$ = NEW_STR($1); }
                         ;

NullLiteral              : NULLT                 
                             { $$ = NEW_NULL(); }
                         ;

NumericLiteral           : INTEGER               
                             { $$ = NEW_NUM((double)$1); }
                         | FLOAT               
                             { $$ = NEW_NUM($1); }
                         ;

ObjectLiteral            : '{' '}'                                
                             { $$ = NEW_OBJ(NULL); }
                         | '{' PropertyNameAndValueList '}'     
                             { $$ = NEW_OBJ($2); }
                         | '{' PropertyNameAndValueList ',' '}' 
                             { $$ = NEW_OBJ($2); }
                         ;
                           
PropertyNameAndValueList : PropertyAssignment                                 
                             { $$ = NEW_PROPLST($1, NULL); }
                         | PropertyNameAndValueList ',' PropertyAssignment  
                             { $$ = NEW_PROPLST($3, $1); }
                         ;

PropertyAssignment       : PropertyName ':' AssignmentExpression  
                             { $$ = NEW_PROP($1, $3); }
                         ;

PropertyName             : Identifier                    
                             { $$ = $1; }
                         | StringLiteral               
                             { $$ = $1; }
                         | NumericLiteral              
                             { $$ = $1; }
                         ;
                         
                         /* Whether a named function is a declaration or expression is ambiguous
                            here. We'll decide that on evaluation. */
Function                 : FUNCTION Identifier '(' FormalParameterList ')' '{' FunctionBody '}'
                             { $$ = NEW_FUNC($4, $7, $2); }
                         | FUNCTION Identifier '(' ')' '{' FunctionBody '}'
                             { $$ = NEW_FUNC(NULL, $6, $2); }
                         ;

                         /* Anonymous functions, OTOH, can readily be labeled expressions. */
FunctionExpression       : FUNCTION '(' FormalParameterList ')' '{' FunctionBody '}'
                             { $$ = NEW_FUNC($3, $6, NULL); }
                         | FUNCTION '(' ')' '{' FunctionBody '}'
                             { $$ = NEW_FUNC(NULL, $5, NULL); }
                         ;

FormalParameterList      : Identifier                             
                             { $$ = NEW_PARAMLST($1, NULL); }
                         | FormalParameterList ',' Identifier   
                             { $$ = NEW_PARAMLST($3, $1); }
                         ;

FunctionBody             : SourceElements
                             { $$ = $1; }
                         |
                             { $$ = NULL; }
                         ;

Identifier               : IDENT                 
                             { $$ = NEW_IDENT($1); }
                         ;

PrimaryExpression        : THIS                  
                             { $$ = NEW_THIS(); }
                         | Identifier          
                             { $$ = $1; }
                         | Literal             
                             { $$ = $1; }
                         | ArrayLiteral        
                             { $$ = $1; }
                         | '(' Expression ')'  
                             { $$ = $2; }
                         ;

ConditionalExpression    : LogicalORExpression                                  
                             { $$ = $1; }
                         | LogicalORExpression '?' AssignmentExpression ':' AssignmentExpression
                             { $$ = NEW_TERN($1, $3, $5); }
                         ;

LogicalORExpression      : LogicalANDExpression                                 
                             { $$ = $1; }
                         | LogicalORExpression OR LogicalANDExpression        
                             { $$ = NEW_EXP($1, $3, "||"); }
                         ;

LogicalANDExpression     : BitwiseORExpression                                  
                             { $$ = $1; }
                         | LogicalANDExpression AND BitwiseORExpression       
                             { $$ = NEW_EXP($1, $3, "&&"); }
                         ;

BitwiseORExpression      : BitwiseXORExpression                                 
                             { $$ = $1; }
                         | BitwiseORExpression '|' BitwiseXORExpression       
                             { $$ = NEW_EXP($1, $3, "|"); }
                         ;

BitwiseXORExpression     : BitwiseANDExpression                                 
                             { $$ = $1; }
                         | BitwiseXORExpression '^' BitwiseANDExpression      
                             { $$ = NEW_EXP($1, $3, "^"); }
                         ;

BitwiseANDExpression     : EqualityExpression                                   
                             { $$ = $1; }
                         | BitwiseANDExpression '&' EqualityExpression        
                             { $$ = NEW_EXP($1, $3, "&"); }
                         ;

EqualityExpression       : RelationalExpression                                 
                             { $$ = $1; }
                         | EqualityExpression EQEQ RelationalExpression       
                             { $$ = NEW_EXP($1, $3, "=="); }
                         | EqualityExpression NE RelationalExpression         
                             { $$ = NEW_EXP($1, $3, "!="); }
                         | EqualityExpression STEQ RelationalExpression       
                             { $$ = NEW_EXP($1, $3, "==="); }
                         | EqualityExpression STNE RelationalExpression       
                             { $$ = NEW_EXP($1, $3, "!=="); }
                         ;

RelationalExpression     : ShiftExpression
                             { $$ = $1; }
                         | RelationalExpression '<' ShiftExpression           
                             { $$ = NEW_EXP($1, $3, "<"); }
                         | RelationalExpression '>' ShiftExpression           
                             { $$ = NEW_EXP($1, $3, ">"); }
                         | RelationalExpression LTE ShiftExpression           
                             { $$ = NEW_EXP($1, $3, "<="); }
                         | RelationalExpression GTE ShiftExpression           
                             { $$ = NEW_EXP($1, $3, ">="); }
                         ;

ShiftExpression          : AdditiveExpression  
                             { $$ = $1; }
                         | ShiftExpression LSHIFT AdditiveExpression          
                             { $$ = NEW_EXP($1, $3, $2); }
                         | ShiftExpression RSHIFT AdditiveExpression          
                             { $$ = NEW_EXP($1, $3, $2); }
                         ;

AdditiveExpression       : MultiplicativeExpression                             
                             { $$ = $1; }
                         | AdditiveExpression '+' MultiplicativeExpression    
                             { $$ = NEW_EXP($1, $3, "+"); }
                         | AdditiveExpression '-' MultiplicativeExpression    
                             { $$ = NEW_EXP($1, $3, "-"); }
                         ;

MultiplicativeExpression : UnaryExpression
                             { $$ = $1; }
                         | MultiplicativeExpression '*' UnaryExpression       
                             { $$ = NEW_EXP($1, $3, "*"); }
                         | MultiplicativeExpression '/' UnaryExpression       
                             { $$ = NEW_EXP($1, $3, "/"); }
                         | MultiplicativeExpression '%' UnaryExpression       
                             { $$ = NEW_EXP($1, $3, "%"); }
                         ;

UnaryExpression          : PostfixExpression                                    
                             { $$ = $1; }
                         | DELETE UnaryExpression
                             { $$ = NEW_UNPRE($2, "delete"); }
                         | VOID UnaryExpression
                             { $$ = NEW_UNPRE($2, "void"); }
                         | TYPEOF UnaryExpression
                             { $$ = NEW_UNPRE($2, "typeof"); }
                         | PLUSPLUS UnaryExpression                           
                             { $$ = NEW_UNPRE($2, "++"); }
                         | MINUSMINUS UnaryExpression                         
                             { $$ = NEW_UNPRE($2, "--"); }
                         | '+' UnaryExpression                                
                             { $$ = NEW_UNPRE($2, "+"); }
                         | '-' UnaryExpression                                
                             { $$ = NEW_UNPRE($2, "-"); }
                         | '!' UnaryExpression                                
                             { $$ = NEW_UNPRE($2, "!"); }
                         ;

PostfixExpression        : LeftHandSideExpression                               
                             { $$ = $1; }
                         | LeftHandSideExpression PLUSPLUS                    
                             { $$ = NEW_UNPOST($1, "++"); }
                         | LeftHandSideExpression MINUSMINUS                  
                             { $$ = NEW_UNPOST($1, "--"); }
                         ;

Expression               : AssignmentExpression                                 
                             { $$ = $1; }
                         | Expression ',' AssignmentExpression                
                             { $$ = NEW_ASGN($1, $3, 0); }
                         ;

AssignmentExpression     : ConditionalExpression                                                 
                             { $$ = $1; }
                         | LeftHandSideExpression '=' AssignmentExpression                     
                             { $$ = NEW_ASGN($1, $3, "="); }
                         | LeftHandSideExpression AssignmentOperator AssignmentExpression      
                             { $$ = NEW_ASGN($1, $3, $2); }
                         ;

AssignmentOperator       : PLUSEQ                                    
                             { $$ = "+="; }
                         | MINUSEQ                                 
                             { $$ = "-="; }
                         | MULTEQ                                  
                             { $$ = "*="; }
                         | DIVEQ                                   
                             { $$ = "/="; }
                         | MODEQ                                   
                             { $$ = "%="; }
                         ;

LeftHandSideExpression   : NewExpression                             
                             { $$ = $1; }
                         | CallExpression                          
                             { $$ = $1; }
                         ;

CallExpression           : MemberExpression Arguments                
                             { $$ = NEW_CALL($1, $2); }
                         | CallExpression Arguments                
                             { $$ = NEW_CALL($1, $2); }
                         | CallExpression '[' Expression ']'       
                             { $$ = NEW_CALL($1, $3); }
                         | CallExpression '.' Identifier           
                             { $$ = NEW_CALL($1, $3); }
                         ;

NewExpression            : MemberExpression                          
                             { $$ = $1; }
                         | NEW NewExpression                       
                             { $$ = NEW_NEW($2); }
                         ;

MemberExpression         : PrimaryExpression
                             { $$ = $1; }
                         | FunctionExpression
                             { $$ = $1; }
                         | MemberExpression '[' Expression ']'
                             { $$ = NEW_MEMBER($3, $1, 42); }
                         | MemberExpression '.' Identifier
                             { $$ = NEW_MEMBER($3, $1, false); }
                         | NEW MemberExpression Arguments
                             { $$ = NEW_NEW(NEW_MEMBER($3, $2, false)); }
                         ;

Arguments                : '(' ')'                                   
                             { $$ = NEW_ARGLST(NULL, NULL); }
                         | '(' ArgumentList ')'                    
                             { $$ = $2; }
                         ;

ArgumentList             : AssignmentExpression                      
                             { $$ = NEW_ARGLST($1, NULL); }
                         | ArgumentList ',' AssignmentExpression   
                             { $$ = NEW_ARGLST($3, $1); }
                         ;

%%

void 
yyerror(const char *s) 
{
  // Trim the "syntax error: " prefix so we can use fh_error.
  char *trimmed = strndup(s + 14, strlen(s) - 14);
  State *state = fh_new_state(yylloc.first_line, yylloc.first_column);
  fh_error(state, E_SYNTAX, trimmed);
}

// This is our replacement function when we redefine YY_INPUT
int 
fh_get_input(char *buf, int size)
{
  // For the REPL:
  if (interactive) {
    char *line;
    line = readline("> ");
    if (!line)
      return 0;
    if (strlen(line) > size - 3) {
      fh_error(state, E_ERROR, "input line too long");
      return 0;
    }
    strcpy(buf, line);
    // Ghetto ASI
    if (!strchr(buf, ';')) strcat(buf, ";");
    strcat(buf, "\n");
    free(line);
    add_history(buf);
  }
  // For file or stdin:
  else {
    if (!yyin)
      fh_error(NULL, E_ERROR, "invalid input file");
    if (feof(yyin)) 
      return 0;
    size_t read;
    read = fread(buf, size, 1, yyin);
  }
  return strlen(buf);
}

int
main(int argc, char **argv)
{
  static int print_ast = 0;
  static int print_tokens = 0;

  int c = 0, fakeind = 0, optind = 1;
  static struct option long_options[] = {
    {"version", no_argument, NULL, 'v'},
    {"help", no_argument, NULL, 'h'},
    {"interactive", no_argument, NULL, 'i'},
    {"nodes", no_argument, &print_ast, 'n'},
    {"tokens", no_argument, &print_tokens, 't'},
    {NULL, 0, NULL, 0}
  };

  while((c = getopt_long(argc, argv, "vhint", long_options, &fakeind)) != -1) {
    switch (c) {
      case 0: break;
      case 'v': print_version(); return 0;
      case 'h': print_help(); return 0;
      case 'i': interactive = true; break;
      case 'n': print_ast = true; break;
      case 't': print_tokens = true; break;
      default: break;
    }
    optind++;
  }

  if (optind < argc)
    yyin = fopen(argv[optind], "r");
  else if (!isatty(fileno(stdin)) && !interactive)
    yyin = stdin;
  else 
    interactive = true;

  // Bootstrap our runtime
  JSValue *global = fh_bootstrap();

  // We can operate as a REPL or file/stdin mode.
  if (interactive) {
    printf("Flathead %s\n", VERSION);
    while(true) {
      yyparse();
      if (print_ast) print_node(root, true, 0);
      JSDEBUG(fh_eval(global, root));
    }
  } else {
    yyparse();
    if (print_ast) print_node(root, true, 0);
    fh_eval(global, root);
  }

  return 0;
}
