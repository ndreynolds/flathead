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
  #include <math.h>
  #include <stdlib.h>
  #include <getopt.h>
#ifndef fh_no_repl
  #include <readline/readline.h>
  #include <readline/history.h>
#endif

  #include "src/flathead.h"
  #include "src/nodes.h"
  #include "src/eval.h"
  #include "src/runtime/runtime.h"
  #include "src/debug.h"
  #include "src/cli.h"

  #define YYDEBUG 0

  #define NEW_NODE(t,e1,e2,e3,d,s)   new_node(t,e1,e2,e3,d,s,yylloc.first_line,yylloc.first_column) 

  // The Node is a one-size-fits-all struct. Nodes are differentiable by their
  // `type` members.  These macros serve as convenience functions for creating
  // new nodes of certain types, and as reference to where child nodes and
  // values are stored.

  #define NEW_ARGLST(head,tail)      NEW_NODE(NODE_ARG_LST,head,tail,0,0,0)
  #define NEW_ARR(ellst)             NEW_NODE(NODE_ARR,ellst,0,0,0,0)
  #define NEW_ASGN(a,b,op)           NEW_NODE(NODE_ASGN,a,b,0,0,op)
  #define NEW_BLOCK(stmtlst)         NEW_NODE(NODE_BLOCK,stmtlst,0,0,0,0)
  #define NEW_BOOL(x)                NEW_NODE(NODE_BOOL,0,0,0,x,0)
  #define NEW_BREAK()                NEW_NODE(NODE_BREAK,0,0,0,0,0)
  #define NEW_CALL(call,args)        NEW_NODE(NODE_CALL,call,args,0,0,0);
  #define NEW_CASEBLOCK(c1,def,c2)   NEW_NODE(NODE_CASE_BLOCK,c1,def,c2,0,0)
  #define NEW_CATCH(id,block)        NEW_NODE(NODE_CATCH,id,block,0,0,0)
  #define NEW_CLAUSE(exp,stmtlst)    NEW_NODE(NODE_CLAUSE,exp,stmtlst,0,0,0)
  #define NEW_CLAUSELST(head,tail)   NEW_NODE(NODE_CLAUSE_LST,head,tail,0,0,0)
  #define NEW_CONT()                 NEW_NODE(NODE_CONT,0,0,0,0,0)
  #define NEW_DEBUGGER()             NEW_NODE(NODE_DEBUG_STMT,0,0,0,0,0)
  #define NEW_DOWHILE(cnd,blck)      NEW_NODE(NODE_DOWHILE,cnd,blck,0,0,0)
  #define NEW_ELISION()              NEW_NODE(NODE_ELISION,0,0,0,0,0)
  #define NEW_ELLST(head,tail)       NEW_NODE(NODE_EL_LST,head,tail,0,0,0)
  #define NEW_EMPTSTMT()             NEW_NODE(NODE_EMPT_STMT,0,0,0,0,0)
  #define NEW_EXP(a,b,op)            NEW_NODE(NODE_EXP,a,b,0,0,op)
  #define NEW_EXPGRP(e1,e2,e3)       NEW_NODE(NODE_EXPGRP,e1,e2,e3,0,0)
  #define NEW_EXPSTMT(exp)           NEW_NODE(NODE_EXP_STMT,exp,0,0,0,0)
  #define NEW_FINALLY(block)         NEW_NODE(NODE_FINALLY,block,0,0,0,0)
  #define NEW_FOR(exps,stmt)         NEW_NODE(NODE_FOR,exps,stmt,0,0,0)
  #define NEW_FORIN(lhs,in,stmt)     NEW_NODE(NODE_FORIN,lhs,in,stmt,0,0)
  #define NEW_FUNC(params,body,id)   NEW_NODE(NODE_FUNC,params,body,id,0,0)
  #define NEW_IDENT(name)            NEW_NODE(NODE_IDENT,0,0,0,0,name)
  #define NEW_IF(p,q,r)              NEW_NODE(NODE_IF,p,q,r,0,0)
  #define NEW_MEMBER(head,tail,exp)  NEW_NODE(NODE_MEMBER,head,tail,0,exp,0)
  #define NEW_NEW(exp)               NEW_NODE(NODE_NEW,exp,0,0,0,0);
  #define NEW_NULL()                 NEW_NODE(NODE_NULL,0,0,0,0,0)
  #define NEW_NUM(x)                 NEW_NODE(NODE_NUM,0,0,0,x,0)
  #define NEW_OBJ(proplst)           NEW_NODE(NODE_OBJ,proplst,0,0,0,0)
  #define NEW_PARAMLST(head,tail)    NEW_NODE(NODE_PARAM_LST,head,tail,0,0,0)
  #define NEW_PROP(name,exp)         NEW_NODE(NODE_PROP,name,exp,0,0,0)
  #define NEW_PROPLST(head,tail)     NEW_NODE(NODE_PROP_LST,head,tail,0,0,0)
  #define NEW_REGEXP(x)              NEW_NODE(NODE_REGEXP,0,0,0,0,x)
  #define NEW_RETURN(exp)            NEW_NODE(NODE_RETURN,exp,0,0,0,0)
  #define NEW_SRCLST(head,tail)      NEW_NODE(NODE_SRC_LST,head,tail,0,0,0)
  #define NEW_STMTLST(head,tail)     NEW_NODE(NODE_STMT_LST,head,tail,0,0,0)
  #define NEW_STR(x)                 NEW_NODE(NODE_STR,0,0,0,0,x)
  #define NEW_SWITCH(exp,cases)      NEW_NODE(NODE_SWITCH_STMT,exp,cases,0,0,0)
  #define NEW_TERN(p,q,r)            NEW_NODE(NODE_TERN,p,q,r,0,0)
  #define NEW_THIS()                 NEW_NODE(NODE_THIS,0,0,0,0,0)
  #define NEW_THROW(exp)             NEW_NODE(NODE_THROW,exp,0,0,0,0)
  #define NEW_TRY(block,catch,final) NEW_NODE(NODE_TRY_STMT,block,catch,final,0,0)
  #define NEW_UNPOST(a,op)           NEW_NODE(NODE_UNARY_POST,a,0,0,0,op)
  #define NEW_UNPRE(a,op)            NEW_NODE(NODE_UNARY_PRE,a,0,0,0,op)
  #define NEW_VARDEC(id,exp)         NEW_NODE(NODE_VAR_DEC,id,exp,0,0,0)
  #define NEW_VARDECLST(head,tail)   NEW_NODE(NODE_VAR_DEC_LST,head,tail,0,0,0)
  #define NEW_VARSTMT(declst)        NEW_NODE(NODE_VAR_STMT,declst,0,0,0,0)
  #define NEW_WHILE(cnd,blck)        NEW_NODE(NODE_WHILE,cnd,blck,0,0,0)
  #define NEW_WITH(exp,stmt)         NEW_NODE(NODE_WITH_STMT,exp,stmt,0,0,0)

  void yyerror(const char *);
  void yyrestart(FILE *);
  int yylex(void);
  int yydebug;
  FILE *yyin;
  int fh_get_input(char *, int);
  Node *root;
  State *state;

  int fh_opt_interactive  = 0;
  int fh_opt_print_tokens = 0;
  int fh_opt_print_ast    = 0;
%}

%error-verbose
%locations


/* LITERALS */

%token<floatval> NUMBER
%token<val> IDENT STRING REGEXP TRUE FALSE


/* OPERATORS */

%token<val> AND OR
%token<val> PLUSPLUS MINUSMINUS VOID DELETE TYPEOF 
%token<val> EQEQ GTE LTE NE STEQ STNE INSTANCEOF
%token<val> LSHIFT RSHIFT ULSHIFT URSHIFT
%token<val> PLUSEQ MINUSEQ MULTEQ DIVEQ MODEQ
%token<val> LSHIFTEQ RSHIFTEQ URSHIFTEQ 
%token<val> ANDEQ XOREQ OREQ


/* KEYWORDS */

%token<val> WHILE DO FOR
%token<val> IF ELSE
%token<val> BREAK CONTINUE RETURNT THROW
%token<val> VAR IN THIS NULLT FUNCTION NEW WITH DEBUGGER
%token<val> SWITCH CASE DEFAULT
%token<val> TRY CATCH FINALLY


/* ASSOCIATIVITY */

%left '+' '-' '*' '%' '/' AND OR

%union {
  char *val;
  int intval;
  double floatval;
  struct Node *node;
}

%type<val> AssignmentOperator 

%type<node> AdditiveExpression
%type<node> ArgumentList
%type<node> Arguments
%type<node> ArrayLiteral
%type<node> AssignmentExpression
%type<node> AssignmentExpressionNoIn
%type<node> BitwiseANDExpression
%type<node> BitwiseANDExpressionNoIn
%type<node> BitwiseORExpression
%type<node> BitwiseORExpressionNoIn
%type<node> BitwiseXORExpression
%type<node> BitwiseXORExpressionNoIn
%type<node> Block
%type<node> BooleanLiteral
%type<node> BreakStatement
%type<node> CallExpression
%type<node> CaseBlock
%type<node> CaseClause
%type<node> CaseClauses
%type<node> Catch
%type<node> ConditionalExpression
%type<node> ConditionalExpressionNoIn
%type<node> ContinueStatement
%type<node> DebuggerStatement
%type<node> DefaultClause
%type<node> ElementList
%type<node> Elision
%type<node> EmptyStatement
%type<node> EqualityExpression
%type<node> EqualityExpressionNoIn
%type<node> Expression
%type<node> ExpressionNoIn
%type<node> ExpressionStatement
%type<node> Finally
%type<node> FormalParameterList
%type<node> Function
%type<node> FunctionBody
%type<node> FunctionExpression
%type<node> Identifier
%type<node> IfStatement
%type<node> Initializer
%type<node> InitializerNoIn
%type<node> IterationStatement
%type<node> LeftHandSideExpression
%type<node> Literal
%type<node> LogicalANDExpression
%type<node> LogicalANDExpressionNoIn
%type<node> LogicalORExpression
%type<node> LogicalORExpressionNoIn
%type<node> MemberExpression
%type<node> MultiplicativeExpression
%type<node> NewExpression
%type<node> NullLiteral
%type<node> NumericLiteral
%type<node> ObjectLiteral
%type<node> PostfixExpression
%type<node> PrimaryExpression
%type<node> Program
%type<node> PropertyAssignment
%type<node> PropertyName
%type<node> PropertyNameAndValueList
%type<node> RegularExpressionLiteral
%type<node> RelationalExpression
%type<node> RelationalExpressionNoIn
%type<node> ReturnStatement
%type<node> ShiftExpression
%type<node> SourceElement
%type<node> SourceElements
%type<node> Statement
%type<node> StatementList
%type<node> StringLiteral
%type<node> SwitchStatement
%type<node> ThrowStatement
%type<node> TryStatement
%type<node> UnaryExpression
%type<node> VariableDecList
%type<node> VariableDecListNoIn
%type<node> VariableDeclaration
%type<node> VariableDeclarationNoIn
%type<node> VariableStatement
%type<node> WithStatement

/**
 * 'NoIn' variants are used to differentiate between the 'in' operatior in a
 * relational expression and the 'in' operator in a for-statement. 
 * (see ECMA-252 11.8)
 */

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
                         | ThrowStatement                                 
                             { $$ = $1; }
                         | TryStatement
                             { $$ = $1; }
                         | WithStatement
                             { $$ = $1; }
                         | SwitchStatement                                 
                             { $$ = $1; }
                         | DebuggerStatement
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

VariableStatement        : VAR VariableDecList ';'
                             { $$ = NEW_VARSTMT($2); }
                         ;

VariableDecList          : VariableDeclaration
                             { $$ = NEW_VARDECLST($1, NULL); }
                         | VariableDecList ',' VariableDeclaration
                             { $$ = NEW_VARDECLST($3, $1); }
                         ;

VariableDecListNoIn      : VariableDeclarationNoIn
                             { $$ = NEW_VARDECLST($1, NULL); }
                         | VariableDecListNoIn ',' VariableDeclarationNoIn
                             { $$ = NEW_VARDECLST($3, $1); }
                         ;

VariableDeclaration      : Identifier Initializer
                             { $$ = NEW_VARDEC($1, $2); }
                         | Identifier
                             { $$ = NEW_VARDEC($1, NULL); }
                         ;

VariableDeclarationNoIn  : Identifier InitializerNoIn
                             { $$ = NEW_VARDEC($1, $2); }
                         | Identifier
                             { $$ = NEW_VARDEC($1, NULL); }
                         ;

Initializer              : '=' AssignmentExpression
                             { $$ = $2; }
                         ;

InitializerNoIn          : '=' AssignmentExpressionNoIn
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
                         | FOR '(' VAR VariableDeclarationNoIn IN Expression ')' Statement
                             { $$ = NEW_FORIN($4, $6, $8); }

                         /* for ( Expression/VarDecl ; Expresion ; Expression ) Statement */

                         | FOR '(' ExpressionNoIn ';' Expression ';' Expression ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($3, $5, $7), $9); }
                         | FOR '(' VAR VariableDecListNoIn ';' Expression ';' Expression ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($4, $6, $8), $10); }
                         | FOR '(' ExpressionNoIn ';' Expression ';' ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($3, $5, NULL), $8); }
                         | FOR '(' VAR VariableDecListNoIn ';' Expression ';' ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($4, $6, NULL), $9); }
                         | FOR '(' ExpressionNoIn ';' ';' Expression ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($3, NULL, $6), $8); }
                         | FOR '(' VAR VariableDecListNoIn ';' ';' Expression ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($4, NULL, $7), $9); }
                         | FOR '(' ExpressionNoIn ';' ';' ')' Statement    
                             { $$ = NEW_FOR(NEW_EXPGRP($3, NULL, NULL), $7); }
                         | FOR '(' VAR VariableDecListNoIn ';' ';' ')' Statement    
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

ReturnStatement          : RETURNT ';'                         
                             { $$ = NEW_RETURN(NULL); }
                         | RETURNT Expression ';'            
                             { $$ = NEW_RETURN($2); }
                         ;

WithStatement            : WITH '(' Expression ')' Statement
                             { $$ = NEW_WITH($3, $5); }
                         ;

TryStatement             : TRY Block Catch
                             { $$ = NEW_TRY($2, $3, NULL); }
                         | TRY Block Finally
                             { $$ = NEW_TRY($2, NULL, $3); }
                         | TRY Block Catch Finally
                             { $$ = NEW_TRY($2, $3, $4); }
                         ;

Catch                    : CATCH '(' Identifier ')' Block
                             { $$ = NEW_CATCH($3, $5); }
                         ;

Finally                  : FINALLY Block
                             { $$ = NEW_FINALLY($2); }
                         ;

DebuggerStatement        : DEBUGGER ';'
                             { $$ = NEW_DEBUGGER(); }
                         ;

ThrowStatement           : THROW Expression ';'
                             { $$ = NEW_THROW($2); }
                         ;

SwitchStatement          : SWITCH '(' Expression ')' CaseBlock
                             { $$ = NEW_SWITCH($3, $5); }
                         ;

CaseBlock                : '{' CaseClauses '}'
                             { $$ = NEW_CASEBLOCK($2, NULL, NULL); }
                         | '{' DefaultClause '}'
                             { $$ = NEW_CASEBLOCK(NULL, $2, NULL); }
                         | '{' CaseClauses DefaultClause '}'
                             { $$ = NEW_CASEBLOCK($2, $3, NULL); }
                         | '{' CaseClauses DefaultClause CaseClauses '}'
                             { $$ = NEW_CASEBLOCK($2, $3, $4); }
                         ;

CaseClauses              : CaseClause
                             { $$ = NEW_CLAUSELST($1, NULL); }
                         | CaseClauses CaseClause
                             { $$ = NEW_CLAUSELST($2, $1); }

CaseClause               : CASE Expression ':' StatementList
                             { $$ = NEW_CLAUSE($2, $4); }
                         | CASE Expression ':'
                             { $$ = NEW_CLAUSE($2, NULL); }
                         ;

DefaultClause            : DEFAULT ':' StatementList
                             { $$ = NEW_CLAUSE(NULL, $3); }
                         | DEFAULT ':'
                             { $$ = NEW_CLAUSE(NULL, NULL); }
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
                         | RegularExpressionLiteral
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

NumericLiteral           : NUMBER
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

RegularExpressionLiteral : REGEXP
                             { $$ = NEW_REGEXP($1); }
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

ConditionalExpressionNoIn: LogicalORExpressionNoIn                                  
                             { $$ = $1; }
                         | LogicalORExpressionNoIn '?' AssignmentExpression ':' AssignmentExpressionNoIn
                             { $$ = NEW_TERN($1, $3, $5); }
                         ;

LogicalORExpression      : LogicalANDExpression                                 
                             { $$ = $1; }
                         | LogicalORExpression OR LogicalANDExpression        
                             { $$ = NEW_EXP($1, $3, "||"); }
                         ;

LogicalORExpressionNoIn  : LogicalANDExpressionNoIn
                             { $$ = $1; }
                         | LogicalORExpressionNoIn OR LogicalANDExpressionNoIn
                             { $$ = NEW_EXP($1, $3, "||"); }
                         ;

LogicalANDExpression     : BitwiseORExpression                                  
                             { $$ = $1; }
                         | LogicalANDExpression AND BitwiseORExpression       
                             { $$ = NEW_EXP($1, $3, "&&"); }
                         ;

LogicalANDExpressionNoIn : BitwiseORExpressionNoIn
                             { $$ = $1; }
                         | LogicalANDExpressionNoIn AND BitwiseORExpressionNoIn
                             { $$ = NEW_EXP($1, $3, "&&"); }
                         ;

BitwiseORExpression      : BitwiseXORExpression                                 
                             { $$ = $1; }
                         | BitwiseORExpression '|' BitwiseXORExpression       
                             { $$ = NEW_EXP($1, $3, "|"); }
                         ;

BitwiseORExpressionNoIn  : BitwiseXORExpressionNoIn
                             { $$ = $1; }
                         | BitwiseORExpressionNoIn '|' BitwiseXORExpressionNoIn
                             { $$ = NEW_EXP($1, $3, "|"); }
                         ;

BitwiseXORExpression     : BitwiseANDExpression                                 
                             { $$ = $1; }
                         | BitwiseXORExpression '^' BitwiseANDExpression      
                             { $$ = NEW_EXP($1, $3, "^"); }
                         ;

BitwiseXORExpressionNoIn : BitwiseANDExpressionNoIn
                             { $$ = $1; }
                         | BitwiseXORExpressionNoIn '^' BitwiseANDExpressionNoIn
                             { $$ = NEW_EXP($1, $3, "^"); }
                         ;

BitwiseANDExpression     : EqualityExpression                                   
                             { $$ = $1; }
                         | BitwiseANDExpression '&' EqualityExpression        
                             { $$ = NEW_EXP($1, $3, "&"); }
                         ;

BitwiseANDExpressionNoIn : EqualityExpressionNoIn
                             { $$ = $1; }
                         | BitwiseANDExpressionNoIn '&' EqualityExpressionNoIn
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

EqualityExpressionNoIn   : RelationalExpressionNoIn
                             { $$ = $1; }
                         | EqualityExpressionNoIn EQEQ RelationalExpressionNoIn
                             { $$ = NEW_EXP($1, $3, "=="); }
                         | EqualityExpressionNoIn NE RelationalExpressionNoIn         
                             { $$ = NEW_EXP($1, $3, "!="); }
                         | EqualityExpressionNoIn STEQ RelationalExpressionNoIn       
                             { $$ = NEW_EXP($1, $3, "==="); }
                         | EqualityExpressionNoIn STNE RelationalExpressionNoIn
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
                         | RelationalExpression INSTANCEOF ShiftExpression           
                             { $$ = NEW_EXP($1, $3, "instanceof"); }
                         | RelationalExpression IN ShiftExpression           
                             { $$ = NEW_EXP($1, $3, "in"); }
                         ;

RelationalExpressionNoIn : ShiftExpression
                             { $$ = $1; }
                         | RelationalExpressionNoIn '<' ShiftExpression           
                             { $$ = NEW_EXP($1, $3, "<"); }
                         | RelationalExpressionNoIn '>' ShiftExpression           
                             { $$ = NEW_EXP($1, $3, ">"); }
                         | RelationalExpressionNoIn LTE ShiftExpression           
                             { $$ = NEW_EXP($1, $3, "<="); }
                         | RelationalExpressionNoIn GTE ShiftExpression           
                             { $$ = NEW_EXP($1, $3, ">="); }
                         | RelationalExpressionNoIn INSTANCEOF ShiftExpression           
                             { $$ = NEW_EXP($1, $3, "instanceof"); }
                         ;

ShiftExpression          : AdditiveExpression  
                             { $$ = $1; }
                         | ShiftExpression LSHIFT AdditiveExpression          
                             { $$ = NEW_EXP($1, $3, "<<"); }
                         | ShiftExpression RSHIFT AdditiveExpression          
                             { $$ = NEW_EXP($1, $3, ">>"); }
                         | ShiftExpression ULSHIFT AdditiveExpression
                             { $$ = NEW_EXP($1, $3, "<<<"); }
                         | ShiftExpression URSHIFT AdditiveExpression
                             { $$ = NEW_EXP($1, $3, ">>>"); }
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

ExpressionNoIn           : AssignmentExpressionNoIn
                             { $$ = $1; }
                         | ExpressionNoIn ',' AssignmentExpressionNoIn
                             { $$ = NEW_ASGN($1, $3, 0); }
                         ;

AssignmentExpression     : ConditionalExpression                                                 
                             { $$ = $1; }
                         | LeftHandSideExpression '=' AssignmentExpression                     
                             { $$ = NEW_ASGN($1, $3, "="); }
                         | LeftHandSideExpression AssignmentOperator AssignmentExpression      
                             { $$ = NEW_ASGN($1, $3, $2); }
                         ;

AssignmentExpressionNoIn : ConditionalExpressionNoIn                                                 
                             { $$ = $1; }
                         | LeftHandSideExpression '=' AssignmentExpressionNoIn
                             { $$ = NEW_ASGN($1, $3, "="); }
                         | LeftHandSideExpression AssignmentOperator AssignmentExpressionNoIn
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
                         | LSHIFTEQ                                   
                             { $$ = "<<="; }
                         | RSHIFTEQ                                   
                             { $$ = ">>="; }
                         | URSHIFTEQ                                   
                             { $$ = ">>>="; }
                         | ANDEQ
                             { $$ = "&="; }
                         | XOREQ
                             { $$ = "^="; }
                         | OREQ
                             { $$ = "|="; }
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
  if (fh_opt_interactive) {
#ifdef fh_no_repl
    fh_error(NULL, E_ERROR, "REPL not available. Build with readline.");
#else
    char *line;
    line = readline("> ");
    if (!line)
      return 0;
    if (strlen(line) > size - 3) {
      fh_error(NULL, E_ERROR, "input line too long");
      return 0;
    }
    strcpy(buf, line);
    // Ghetto ASI
    if (!strchr(buf, ';')) strcat(buf, ";");
    strcat(buf, "\n");
    free(line);
    add_history(buf);
#endif
  }
  // For file or stdin:
  else {
    if (!yyin)
      fh_error(NULL, E_ERROR, "invalid input file");
    if (feof(yyin))
      return 0;
    fread(buf, size, 1, yyin);
  }
  return strlen(buf);
}

JSValue *
fh_eval_file(FILE *file, JSValue *ctx, int is_repl)
{
  // This may be called during evaluation, so we need
  // to keep track of the globals.
  Node *root_ = root;
  int interactive_ = fh_opt_interactive;
  fh_opt_interactive = is_repl;

  yyrestart(file);
  yyparse();

  if (fh_opt_print_ast) 
    print_node(root, true, 0);

  JSValue *res = fh_eval(ctx, root);

  // Reset the originals
  root = root_;
  fh_opt_interactive = interactive_;

  return res;
}

int
main(int argc, char **argv)
{
  int c = 0, fakeind = 0, optind = 1;
  static struct option long_options[] = {
    {"version", no_argument, NULL, 'v'},
    {"help", no_argument, NULL, 'h'},
    {"interactive", no_argument, NULL, 'i'},
    {"nodes", no_argument, NULL, 'n'},
    {"tokens", no_argument, NULL, 't'},
    {NULL, 0, NULL, 0}
  };

  while((c = getopt_long(argc, argv, "vhint", long_options, &fakeind)) != -1) {
    switch (c) {
      case 0: break;
      case 'v': print_version(); return 0;
      case 'h': print_help(); return 0;
      case 'i': fh_opt_interactive = true; break;
      case 'n': fh_opt_print_ast = true; break;
      case 't': fh_opt_print_tokens = true; break;
      default: break;
    }
    optind++;
  }

  FILE *source;
  if (optind < argc)
    source = fopen(argv[optind], "r");
  else if (!isatty(fileno(stdin)) && !fh_opt_interactive)
    source = stdin;
  else 
    fh_opt_interactive = true;

  // Bootstrap our runtime
  JSValue *global = fh_bootstrap();

  // We can operate as a REPL or in file/stdin mode.
  if (fh_opt_interactive) {
    print_startup();
    while(true)
      DEBUG(fh_eval_file(source, global, true));
  } else {
    fh_eval_file(source, global, false);
  }

  return 0;
}
