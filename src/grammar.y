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
  #include "src/nodes.h"
  #include "src/runtime.h"

  #define YYDEBUG 0

  #define NEW_NODE(t,e1,e2,e3,d,s)   new_node(t,e1,e2,e3,d,s,yylloc.first_line,yylloc.first_column) 
  #define NEW_IDENT(name)            NEW_NODE(NODE_IDENT,0,0,0,0,name)
  #define NEW_VARSTMT(ident,exp)     NEW_NODE(NODE_VAR_STMT,ident,exp,0,0,0)
  #define NEW_WHILE(cnd,blck)        NEW_NODE(NODE_WHILE,cnd,blck,0,0,0)
  #define NEW_DOWHILE(cnd,blck)      NEW_NODE(NODE_DOWHILE,cnd,blck,0,0,0)
  #define NEW_BLOCK(stmtlst)         NEW_NODE(NODE_BLOCK,stmtlst,0,0,0,0)
  #define NEW_STMTLST(head,tail)     NEW_NODE(NODE_STMT_LST,head,tail,0,0,0)
  #define NEW_IF(cnd,ifb,elseb)      NEW_NODE(NODE_IF,cnd,ifb,elseb,0,0)
  #define NEW_FOR(init,cnd,post)     NEW_NODE(NODE_FOR,init,cnd,post,0,0)
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
  #define NEW_MEMBER(head,tail)      NEW_NODE(NODE_MEMBER,head,tail,0,0,0)
  #define NEW_FUNC(params,body,id)   NEW_NODE(NODE_FUNC,params,body,id,0,0)
  #define NEW_FUNCDL(params,body,id) NEW_NODE(NODE_FUNC_DECL,params,body,id,0,0)
  #define NEW_PARAMLST(head,tail)    NEW_NODE(NODE_PARAM_LST,head,tail,0,0,0)
  #define NEW_ELISION()              NEW_NODE(NODE_ELISION,0,0,0,0,0)

  void yyerror(const char *);
  int yylex(void);
  int yydebug;
  FILE *yyin;
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
%token<val> PLUSPLUS MINUSMINUS
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
%token<val> VAR THIS NULLT FUNCTION NEW DELETE


/* ASSOCIATIVITY */

%left '+' '-' '*' '%' '/' AND OR

%union {
  char *val;
  int intval;
  double floatval;
  struct Node *node;
}

%type<val> AssignmentOperator 

%type<node> Program Statement Block VariableStatement EmptyStatement 
%type<node> ExpressionStatement IfStatement IterationStatement ContinueStatement 
%type<node> BreakStatement ReturnStatement Literal NumericLiteral BooleanLiteral 
%type<node> StringLiteral NullLiteral ArrayLiteral ObjectLiteral Identifier 
%type<node> Expression PrimaryExpression LeftHandSideExpression UnaryExpression 
%type<node> PostfixExpression ShiftExpression RelationalExpression 
%type<node> AdditiveExpression EqualityExpression BitwiseANDExpression 
%type<node> BitwiseORExpression BitwiseXORExpression LogicalORExpression
%type<node> LogicalANDExpression MultiplicativeExpression ConditionalExpression 
%type<node> StatementList AssignmentExpression ElementList SourceElements 
%type<node> PropertyName PropertyAssignment PropertyNameAndValueList 
%type<node> FunctionDeclaration FunctionBody FunctionExpression 
%type<node> FormalParameterList CallExpression MemberExpression NewExpression 
%type<node> Arguments ArgumentList Elision

%%

Program                  : SourceElements                                    
                             { root = $1; }
                         ;

SourceElements           : StatementList                                     
                             { $$ = $1; }
                         | Block                                           
                             { $$ = $1; }
                         | FunctionDeclaration                             
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

VariableStatement        : VAR Identifier ';'                                
                             { $$ = NEW_VARSTMT($2, NULL); }
                         | VAR Identifier '=' AssignmentExpression ';'     
                             { $$ = NEW_VARSTMT($2, $4); }
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

IterationStatement       : DO Statement WHILE '(' Expression ')'                               
                             { $$ = NEW_DOWHILE($2, $5); }
                         | WHILE '(' Expression ')' Statement                                
                             { $$ = NEW_WHILE($3, $5); }
                         | FOR '(' Expression ';' Expression ';' Expression ')' Statement    
                             { $$ = NEW_FOR($3, $5, $7); }
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

FunctionDeclaration      : FUNCTION Identifier '(' FormalParameterList ')' '{' FunctionBody '}'   
                             { $$ = NEW_FUNCDL($4, $7, $2); }
                         ;

FunctionExpression       : FUNCTION '(' FormalParameterList ')' '{' FunctionBody '}'              
                             { $$ = NEW_FUNC($3, $6, NULL); }
                         | FUNCTION Identifier '(' FormalParameterList ')' '{' FunctionBody '}' 
                             { $$ = NEW_FUNC($4, $7, $2); }
                         | FUNCTION Identifier '(' ')' '{' FunctionBody '}'                     
                             { $$ = NEW_FUNC(NULL, $6, $2); }
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
                             { $$ = NEW_MEMBER($3, $1); }
                         | MemberExpression '.' Identifier         
                             { $$ = NEW_MEMBER($3, $1); }
                         | NEW MemberExpression Arguments          
                             { $$ = NEW_NEW(NEW_MEMBER($3, $2)); }
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
  char *trimmed = strndup(s + 14, strlen(s) - 14);
  State *state = fh_new_state(yylloc.first_line, yylloc.first_column);
  fh_error(state, E_SYNTAX, trimmed);
}

int 
main(int argc, char *argv[]) 
{
  int c, print_parse_tree = 0;

  while((c = getopt(argc, argv, "vp")) != -1) {
    switch(c) {
      case 'v':
        yydebug = 1;
        break;
      case 'p':
        print_parse_tree = 1;
        break;
    }
  }

  yyin = optind < argc ? fopen(argv[optind], "r") : stdin;
  if (!yyin) {
    fh_error(NULL, E_ERROR, "Invalid input file.");
  }

  // Parse it.
  yyparse();

  if (print_parse_tree) 
    print_node(root, true, 0);

  JSValue *global = fh_bootstrap();

  // Evaluate.
  fh_eval(global, root);

  // Debug.
  if (yydebug) 
    JSDEBUG(global);

  return 0;
}
