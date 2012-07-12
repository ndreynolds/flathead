%{
  #include <stdio.h>
  #include "src/nodes.h"

  #define YYDEBUG 1
  #define P(x)  print_node(x, false, 0)
  #define PR(x) print_node(x, true, 0)
  #define O(s)  puts(s)

  void yyerror(char *);
  int yylex(void);
  int yydebug;
  FILE *yyin;
  struct JLNode *root;
%}

%error-verbose

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
%token<val> EQEQ GTE LTE NE
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
%token<val> VAR THIS NULLT


/* ASSOCIATIVITY */

%left '+' '-' '*' '%' AND OR

%union {
  char * val;
  int intval;
  double floatval;
  struct JLNode * node;
}

%type<val> AssignmentOperator 
%type<node> Program Statement Block VariableStatement EmptyStatement ExpressionStatement 
IfStatement IterationStatement ContinueStatement BreakStatement ReturnStatement
Literal NumericLiteral BooleanLiteral StringLiteral NullLiteral ArrayLiteral Identifier
Expression PrimaryExpression LeftHandSideExpression UnaryExpression 
PostfixExpression ShiftExpression RelationalExpression AdditiveExpression EqualityExpression
BitwiseANDExpression BitwiseORExpression BitwiseXORExpression LogicalORExpression
LogicalANDExpression MultiplicativeExpression ConditionalExpression StatementList
AssignmentExpression ElementList

%%

Program:
    Statement                                         { root = $1; }
    ;

Statement:
    Block                                             { $$ = $1; }
    | VariableStatement                               { $$ = $1; }
    | EmptyStatement                                  { $$ = $1; }
    | ExpressionStatement                             { $$ = $1; }
    | IfStatement                                     { $$ = $1; }
    | IterationStatement                              { $$ = $1; }
    | ContinueStatement                               { $$ = $1; }
    | BreakStatement                                  { $$ = $1; }
    | ReturnStatement                                 { $$ = $1; }
    ;

Block:
    '{' StatementList '}'                             { $$ = block_node($2); }
    ;

StatementList:
    Statement                                         { $$ = stmtlst_node($1, NULL); } 
    | StatementList Statement                         { $$ = stmtlst_node($2, $1); }
    ;

VariableStatement:
    VAR Identifier ';'                                { $$ = varstmt_node($2); }
    ;

EmptyStatement:
    ';'                                               { $$ = emptstmt_node(); }
    ;

ExpressionStatement:
    Expression ';'                                    { $$ = expstmt_node($1); }
    ;

IfStatement:
    IF '(' Expression ')' Statement ELSE Statement    { $$ = if_node($3, $5, $7); }
    | IF '(' Expression ')' Statement                 { $$ = if_node($3, $5, NULL); }
    ;

IterationStatement:
    DO Statement WHILE '(' Expression ')'                               { $$ = dowhile_node($2, $5); }
    | WHILE '(' Expression ')' Statement                                { $$ = while_node($3, $5); }
    | FOR '(' Expression ';' Expression ';' Expression ')' Statement    { $$ = for_node($3, $5, $7, $9); }
    ;

ContinueStatement:
    CONTINUE ';'                       { $$ = ctl_node(NULL); }
    ;

BreakStatement:
    BREAK ';'                          { $$ = ctl_node(NULL); }
    ;

ReturnStatement:
    RETURN ';'                         { $$ = ctl_node(NULL); }
    | RETURN Expression ';'            { $$ = ctl_node($2); }
    ;

Literal:
    NullLiteral                        { $$ = $1; }
    | BooleanLiteral                   { $$ = $1; }
    | NumericLiteral                   { $$ = $1; }
    | StringLiteral                    { $$ = $1; }
    ;

ArrayLiteral:
    '[' Elision ']'                    { $$ = arr_node(); }
    | '[' ElementList ']'              { $$ = arr_node(); }
    | '[' ElementList ',' Elision ']'  { $$ = arr_node(); }
    ;

ElementList:
    | Elision AssignmentExpression                  { $$ = ellst_node(); }
    | ElementList ',' Elision AssignmentExpression  { $$ = ellst_node(); }
    ;

Elision:
    ','
    | Elision ','
    ;

BooleanLiteral:
    TRUE                  { $$ = bool_node(1); }
    | FALSE               { $$ = bool_node(0); }
    ;

StringLiteral:
    STRING                { $$ = str_node($1); }
    ;

NullLiteral:
    NULLT                 { $$ = null_node(); }
    ;

NumericLiteral:
    INTEGER               { $$ = num_node((double)$1); }
    | FLOAT               { $$ = num_node($1); }
    ;

Identifier:
    IDENT                 { $$ = ident_node($1); }
    ;

PrimaryExpression:
    THIS                  { $$ = this_node($1); }
    | Identifier          { $$ = $1; }
    | Literal             { $$ = $1; }
    | ArrayLiteral        { $$ = $1; }
    | '(' Expression ')'  { $$ = $2; }
    ;

ConditionalExpression:
    LogicalORExpression                                  { $$ = $1; }
    ;

LogicalORExpression:
    LogicalANDExpression                                 { $$ = $1; }
    | LogicalORExpression OR LogicalANDExpression        { $$ = exp_node($2, $1, $3); }
    ;

LogicalANDExpression:
    BitwiseORExpression                                  { $$ = $1; }
    | LogicalANDExpression AND BitwiseORExpression       { $$ = exp_node($2, $1, $3); }
    ;

BitwiseORExpression:
    BitwiseXORExpression                                 { $$ = $1; }
    | BitwiseORExpression '|' BitwiseXORExpression       { $$ = exp_node("|", $1, $3); }
    ;

BitwiseXORExpression:
    BitwiseANDExpression                                 { $$ = $1; }
    | BitwiseXORExpression '^' BitwiseANDExpression      { $$ = exp_node("^", $1, $3); }
    ;

BitwiseANDExpression:
    EqualityExpression                                   { $$ = $1; }
    | BitwiseANDExpression '&' EqualityExpression        { $$ = exp_node("&", $1, $3); }
    ;

EqualityExpression:
    RelationalExpression                                 { $$ = $1; }
    | EqualityExpression EQEQ RelationalExpression       { $$ = exp_node($2, $1, $3); }
    | EqualityExpression NE RelationalExpression         { $$ = exp_node($2, $1, $3); }
    ;

RelationalExpression: 
    ShiftExpression                                      { $$ = $1; }
    | RelationalExpression '<' ShiftExpression           { $$ = exp_node("<", $1, $3); }
    | RelationalExpression '>' ShiftExpression           { $$ = exp_node(">", $1, $3); }
    | RelationalExpression LTE ShiftExpression           { $$ = exp_node($2, $1, $3); }
    | RelationalExpression GTE ShiftExpression           { $$ = exp_node($2, $1, $3); }

ShiftExpression:
    AdditiveExpression                                   { $$ = $1; }
    | ShiftExpression LSHIFT AdditiveExpression          { $$ = exp_node($2, $1, $3); }
    | ShiftExpression RSHIFT AdditiveExpression          { $$ = exp_node($2, $1, $3); }
    ;

AdditiveExpression:
    MultiplicativeExpression                             { $$ = $1; }
    | AdditiveExpression '+' MultiplicativeExpression    { $$ = exp_node("+", $1, $3); }
    | AdditiveExpression '-' MultiplicativeExpression    { $$ = exp_node("-", $1, $3); }
    ;

MultiplicativeExpression:
    UnaryExpression                                      { $$ = $1; }
    | MultiplicativeExpression '*' UnaryExpression       { $$ = exp_node("*", $1, $3); }
    | MultiplicativeExpression '/' UnaryExpression       { $$ = exp_node("/", $1, $3); }
    | MultiplicativeExpression '%' UnaryExpression       { $$ = exp_node("%", $1, $3); }
    ;

UnaryExpression:
    PostfixExpression                                    { $$ = $1; }
    | PLUSPLUS UnaryExpression                           { $$ = exp_node($1, NULL, $2); }
    | MINUSMINUS UnaryExpression                         { $$ = exp_node($1, NULL, $2); }
    | '+' UnaryExpression                                { $$ = exp_node("+", NULL, $2); }
    | '-' UnaryExpression                                { $$ = exp_node("-", NULL, $2); }
    | '!' UnaryExpression                                { $$ = exp_node("!", NULL, $2); }
    ;

PostfixExpression:
    LeftHandSideExpression                               { $$ = $1; }
    | LeftHandSideExpression PLUSPLUS                    { $$ = exp_node($2, $1, NULL); }
    | LeftHandSideExpression MINUSMINUS                  { $$ = exp_node($2, $1, NULL); }
    ;

Expression:
    AssignmentExpression                                 { $$ = $1; }
    | Expression ',' AssignmentExpression                { $$ = asgnexp_node($1, $3); }
    ;

AssignmentExpression:
    ConditionalExpression                                                 { $$ = $1; }
    | LeftHandSideExpression '=' AssignmentExpression                     { $$ = asgn_node("=", $1, $3); }
    | LeftHandSideExpression AssignmentOperator AssignmentExpression      { $$ = asgn_node($2, $1, $3); }
    ;

AssignmentOperator:
    PLUSEQ               { $$ = $1; }
    | MINUSEQ            { $$ = $1; }
    | MULTEQ             { $$ = $1; }
    | DIVEQ              { $$ = $1; }
    | MODEQ              { $$ = $1; }
    ;

LeftHandSideExpression:
    PrimaryExpression /* Ridiculously incomplete */      { $$ = $1; }
    ;

%%

void 
yyerror(char *s) 
{
  fprintf(stderr, "%s haha\n", s);
}

int 
main(int argc, char *argv[]) 
{
  yydebug = 1;

  yyin = argc > 0 ? fopen(argv[1], "r") : stdin;
  if (!yyin) return 1;

  puts("Parsing...");
  yyparse();
  puts("Done parsing.\n");

  puts("Parse tree:");
  puts("-----------------------------------------------");
  PR(root);

  return 0;
}
