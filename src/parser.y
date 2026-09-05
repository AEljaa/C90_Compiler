// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

%code requires{
    #include "ast.hpp"

    extern Node *g_root;
    extern FILE *yyin;
	extern Program *program;
	extern FunctionDefinition *currentFunction;
    int yylex(void);
    void yyerror(const char *);

	Program *GetProgram();
	FunctionDefinition *GetCurrentFunction();

}

// Represents the value associated with any kind of AST node.
%union{
  Node         *node;
  NodeList     *nodes;
  int          number_int;
  double       number_float;
  std::string  *string;
  yytokentype  token;
}

%token IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token TYPE_NAME TYPEDEF EXTERN STATIC AUTO REGISTER SIZEOF
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%type <node> translation_unit external_declaration function_definition primary_expression postfix_expression argument_expression_list
%type <node> unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression
%type <node> equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression assignment_operator
%type <node> conditional_expression assignment_expression expression constant_expression declaration declaration_specifiers init_declarator_list
%type <node> init_declarator type_specifier struct_specifier struct_declaration_list struct_declaration specifier_qualifier_list struct_declarator_list storage_class_specifier
%type <node> struct_declarator enum_specifier enumerator_list enumerator declarator direct_declarator pointer parameter_list parameter_declaration
%type <node> identifier_list type_name abstract_declarator direct_abstract_declarator initializer initializer_list statement labeled_statement
%type <node> compound_statement declaration_list expression_statement selection_statement iteration_statement jump_statement

%type <nodes> statement_list

%type <node> unary_operator

%type <number_int> INT_CONSTANT
%type <number_float> FLOAT_CONSTANT
%type <string> IDENTIFIER STRING_LITERAL


%start ROOT

%nonassoc IFX 
%nonassoc ELSE


%%

ROOT
  : translation_unit { g_root = GetProgram(); }

translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition { GetProgram()->AddFunctionDefinition($1); }
	| declaration { GetProgram()->AddGlobalDeclarations($1, true); }
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement {
		
		FunctionDefinition *functionDefinition = NULL;
		functionDefinition = GetCurrentFunction();
		functionDefinition->SetFunctionDefinition($1, $2, $3);
		$$ = functionDefinition;

		currentFunction = NULL;
	}
	| declarator declaration_list compound_statement
	| declarator compound_statement
	;


primary_expression
	: IDENTIFIER {
		$$ = new IdentifierExpression(*$1);
		delete $1;
	}
	| INT_CONSTANT {
		$$ = new IntConstant($1);
	}
    | FLOAT_CONSTANT { $$ = nullptr; }
	| STRING_LITERAL {
		$$ = new StringExpression(*$1);
		delete $1;
	}
	| '(' expression ')' { $$ = $2; }
	;

postfix_expression
	: primary_expression { $$ = $1; }
	| postfix_expression '[' expression ']' { $$ = new ArrayExpression($1, $3); }
	| postfix_expression '(' ')' { $$ = new FunctionCallExpression($1); }
	| postfix_expression '(' argument_expression_list ')' { $$ = new FunctionCallExpression($1, $3); }
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP { $$ = new PostfixOpExpression($1, INC_OP_TYPE); }
	| postfix_expression DEC_OP { $$ = new PostfixOpExpression($1, DEC_OP_TYPE); }
	;

argument_expression_list
	: assignment_expression { $$ = new NodeList($1); }
	| argument_expression_list ',' assignment_expression { ((NodeList *)$1)->PushBack($3); $$=$1; }
	;

unary_expression
	: postfix_expression { $$ = $1; }
	| INC_OP unary_expression { $$ = nullptr; }
	| DEC_OP unary_expression { $$ = nullptr; }
	| unary_operator cast_expression { $$ = new UnaryExpression($1, $2); }
	| SIZEOF unary_expression { $$ = new SizeofExpression($2, false); }
	| SIZEOF '(' type_name ')' { $$ = new SizeofExpression($3, true); }
	;

unary_operator
	: '&' { $$ = new ExpressionOperator('&'); }
	| '*' { $$ = new ExpressionOperator('*'); }
	| '+' { $$ = new ExpressionOperator('+'); }
	| '-' { $$ = new ExpressionOperator('-'); }
	| '~' { $$ = new ExpressionOperator('~'); }
	| '!' { $$ = new ExpressionOperator('!'); }
	;

cast_expression
	: unary_expression { $$ = $1; }
	| '(' type_name ')' cast_expression { $$ = nullptr; }
	;

multiplicative_expression
	: cast_expression { $$ = $1; }
	| multiplicative_expression '*' cast_expression { $$ = new GenericExpression($1, '*', $3); }
	| multiplicative_expression '/' cast_expression { $$ = new GenericExpression($1, '/', $3); }
	| multiplicative_expression '%' cast_expression { $$ = new GenericExpression($1, '%', $3); }
	;

additive_expression
	: multiplicative_expression { $$ = $1; }
	| additive_expression '+' multiplicative_expression { $$ = new GenericExpression($1, '+', $3); }
	| additive_expression '-' multiplicative_expression { $$ = new GenericExpression($1, '-', $3); }
	;

shift_expression
	: additive_expression { $$ = $1; }
	| shift_expression LEFT_OP additive_expression { $$ = new GenericExpression($1, LEFT_OP_TYPE, $3); }
	| shift_expression RIGHT_OP additive_expression { $$ = new GenericExpression($1, RIGHT_OP_TYPE, $3); }
	;

relational_expression
	: shift_expression { $$ = $1; }
	| relational_expression '<' shift_expression { $$ = new GenericExpression($1, '<', $3); }
	| relational_expression '>' shift_expression { $$ = new GenericExpression($1, '>', $3); }
	| relational_expression LE_OP shift_expression { $$ = new GenericExpression($1, LE_OP_TYPE, $3); }
	| relational_expression GE_OP shift_expression { $$ = new GenericExpression($1, GE_OP_TYPE, $3); }
	;

equality_expression
	: relational_expression { $$ = $1; }
	| equality_expression EQ_OP relational_expression { $$ = new GenericExpression($1, EQ_OP_TYPE, $3); }
	| equality_expression NE_OP relational_expression { $$ = new GenericExpression($1, NE_OP_TYPE, $3); }
	;

and_expression
	: equality_expression { $$ = $1; }
	| and_expression '&' equality_expression { $$ = new GenericExpression($1, '&', $3); }
	;

exclusive_or_expression
	: and_expression { $$ = $1; }
	| exclusive_or_expression '^' and_expression { $$ = new GenericExpression($1, '^', $3); }
	;

inclusive_or_expression
	: exclusive_or_expression { $$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression { $$ = new GenericExpression($1, '|', $3); }
	;

logical_and_expression
	: inclusive_or_expression { $$ = $1; }
	| logical_and_expression AND_OP inclusive_or_expression { $$ = new GenericExpression($1, AND_OP_TYPE, $3); }
	;

logical_or_expression
	: logical_and_expression { $$ = $1; }
	| logical_or_expression OR_OP logical_and_expression { $$ = new GenericExpression($1, OR_OP_TYPE, $3); }
	;

conditional_expression
	: logical_or_expression { $$ = $1; }
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression
	: conditional_expression { $$ = $1; }
	| unary_expression assignment_operator assignment_expression { $$ = new GenericExpression($1, $2, $3); }
	;

assignment_operator
	: '=' { $$ = new ExpressionOperator('='); }
	| MUL_ASSIGN { $$ = new ExpressionOperator(MUL_ASSIGN_OP_TYPE); }
	| DIV_ASSIGN { $$ = nullptr; }
	| MOD_ASSIGN { $$ = nullptr; }
	| ADD_ASSIGN { $$ = new ExpressionOperator(ADD_ASSIGN_OP_TYPE);; }
	| SUB_ASSIGN { $$ = nullptr; }
	| LEFT_ASSIGN { $$ = nullptr; }
	| RIGHT_ASSIGN { $$ = nullptr; }
	| AND_ASSIGN { $$ = nullptr; }
	| XOR_ASSIGN { $$ = nullptr; }
	| OR_ASSIGN { $$ = nullptr; }
	;

expression
	: assignment_expression { $$ = $1; }
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';' { $$ = new Declaration($1, $2); };
	;

declaration_specifiers
	: storage_class_specifier { $$ = new NodeList($1); }
	| storage_class_specifier declaration_specifiers { ((NodeList *)$2)->PushBack($1); $$=$2; }
	| type_specifier { $$ = new NodeList($1); }
	| type_specifier declaration_specifiers { ((NodeList *)$2)->PushBack($1); $$=$2; }
	;

init_declarator_list
	: init_declarator { $$ = new NodeList($1); }
	| init_declarator_list ',' init_declarator { ((NodeList *)$1)->PushBack($3); $$=$1; }
	;

init_declarator
	: declarator { $$ = $1;}
	| declarator '=' initializer { ((Declarator *)$1)->SetInitializer($3); $$ = $1; }
	;

storage_class_specifier
	: TYPEDEF {
		$$ = new StorageClassSpecifier("typedef");
	}
	| EXTERN {
		$$ = new StorageClassSpecifier("extern");
	}
	| STATIC {
		$$ = new StorageClassSpecifier("static");
	}
	| AUTO {
		$$ = new StorageClassSpecifier("auto");
	}
	| REGISTER {
		$$ = new StorageClassSpecifier("register");
	}
	;

type_specifier
	: VOID {
		$$ = new TypeSpecifier("void");
	}
	| CHAR {
		$$ = new TypeSpecifier("char");
	}
	| SHORT {
		$$ = new TypeSpecifier("short");
	}
	| INT {
		$$ = new TypeSpecifier("int");
	}
	| LONG {
		$$ = new TypeSpecifier("long");
	}
	| FLOAT {
		$$ = new TypeSpecifier("float");
	}
	| DOUBLE {
		$$ = new TypeSpecifier("double");
	}
	| SIGNED {
		$$ = new TypeSpecifier("signed");
	}
	| UNSIGNED {
		$$ = new TypeSpecifier("unsigned");
	}
    | struct_specifier {
		$$ = new TypeSpecifier("struct");
	}
	| enum_specifier {
		$$ = new TypeSpecifier("enum");
	}
	| TYPE_NAME { $$ = nullptr; }
	;

struct_specifier
	: STRUCT IDENTIFIER '{' struct_declaration_list '}' { $$ = nullptr; }
	| STRUCT '{' struct_declaration_list '}' { $$ = nullptr; }
	| STRUCT IDENTIFIER { $$ = nullptr; }
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list { ((NodeList *)$2)->PushBack($1); $$=$2; }
	| type_specifier { $$ = new NodeList($1); }
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	| ':' constant_expression { $$ = nullptr; }
	| declarator ':' constant_expression
	;

enum_specifier
	: ENUM '{' enumerator_list '}' { $$ = nullptr; }
	| ENUM IDENTIFIER '{' enumerator_list '}' { $$ = nullptr; }
	| ENUM IDENTIFIER { $$ = nullptr; }
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: IDENTIFIER { $$ = nullptr; }
	| IDENTIFIER '=' constant_expression { $$ = nullptr; }
	;

declarator
	: pointer direct_declarator { ((Declarator *)$2)->SetPointer($1); $$ = $2; }
	| direct_declarator { $$ = $1; }
	;

direct_declarator
	: IDENTIFIER {
		$$ = new Identifier(*$1);
		delete $1;
	}
	| '(' declarator ')' { $$ = nullptr; }
	| direct_declarator '[' constant_expression ']' {
		$$ = new Array_Declaration($1, $3);
	}
	| direct_declarator '[' ']'
	| direct_declarator '(' parameter_list ')' {
		$$ = new DirectDeclarator($1, $3);
	}
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')' {
		$$ = new DirectDeclarator($1);
	}
	;

pointer
	: '*' { $$ = new NodeList(new StorageClassSpecifier("*")); }
	| '*' pointer { ((NodeList *)$2)->PushBack(new StorageClassSpecifier("*")); $$=$2; }
	;

parameter_list
	: parameter_declaration { $$ = new NodeList($1); }
	| parameter_list ',' parameter_declaration{ ((NodeList *)$1)->PushBack($3); $$=$1; }
	;

parameter_declaration
	: declaration_specifiers declarator { $$ = new Parameter_Declaration($1, $2); }
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER { $$ = nullptr; }
	| identifier_list ',' IDENTIFIER
	;

type_name
	: specifier_qualifier_list { $$ = $1; }
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')' { $$ = nullptr; }
	| '[' ']' { $$ = nullptr; }
	| '[' constant_expression ']' { $$ = nullptr; }
	| direct_abstract_declarator '[' ']'
	/* | INT IDENTIFIER '[' constant_expression ']' {$$ = new Array_Declaration($1, $2, $4);} */
	| '(' ')' { $$ = nullptr; }
	| '(' parameter_list ')' { $$ = nullptr; }
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_list ')'
	;

initializer
	: assignment_expression { $$ = $1; }
	| '{' initializer_list '}' { $$ = nullptr; }
	| '{' initializer_list ',' '}' { $$ = nullptr; }
	;

initializer_list
	: initializer
	| initializer_list ',' initializer
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement { $$ = $1; }
	| selection_statement
	| iteration_statement
	| jump_statement { $$ = $1; }
	;

labeled_statement
	: IDENTIFIER ':' statement { $$ = nullptr; }
	| CASE constant_expression ':' statement { $$ = nullptr; }
	| DEFAULT ':' statement { $$ = nullptr; }
	;

compound_statement
	: '{' '}' {
		// TODO: correct this
		$$ = new CompoundStatement(nullptr, nullptr);
	}
	| '{' statement_list '}' {
		$$ = new CompoundStatement(nullptr, $2);
	}
	| '{' declaration_list '}' {
		// TODO: correct this
		GetCurrentFunction()->AddLocalDeclarationsList($2);
		$$ = new CompoundStatement($2, nullptr);
	}
	| '{' declaration_list statement_list '}'  {
		// TODO: correct this
		GetCurrentFunction()->AddLocalDeclarationsList($2);
		$$ = new CompoundStatement($2, $3);
	}
	;

declaration_list
	: declaration { $$ = new NodeList($1); }
	| declaration_list declaration { ((NodeList *)$1)->PushBack($2); $$=$1; }
	;

statement_list
	: statement { $$ = new NodeList($1); }
	| statement_list statement { ((NodeList *)$1)->PushBack($2); $$=$1; }
	;

expression_statement
	: ';' { $$ = nullptr; }
	| expression ';' { $$ = new ExpressionStatement($1); }
	;

selection_statement
	: IF '(' expression ')' statement %prec IFX { $$ = new IfStatement($3, $5, nullptr); }
	| IF '(' expression ')' statement ELSE statement { $$ = new IfStatement($3, $5, $7); }
	| SWITCH '(' expression ')' statement { $$ = nullptr; }
	;

iteration_statement
	: WHILE '(' expression ')' statement { $$ = new WhileStatement($3, $5); }
	| DO statement WHILE '(' expression ')' ';' { $$ = nullptr; }
	| FOR '(' expression_statement expression_statement ')' statement { $$ = new ForStatement($3, $4, nullptr, $6); }
	| FOR '(' expression_statement expression_statement expression ')' statement { $$ = new ForStatement($3, $4, $5, $7); }
	;

jump_statement
	: GOTO IDENTIFIER ';' { $$ = nullptr; }
	| CONTINUE ';' { $$ = nullptr; }
	| BREAK ';' { $$ = nullptr; }
	| RETURN ';' {
		$$ = new ReturnStatement(nullptr);
	}
	| RETURN expression ';' {
		$$ = new ReturnStatement($2);
	}
	;



%%

Node *g_root;

Node *ParseAST(std::string file_name)
{
  yyin = fopen(file_name.c_str(), "r");
  if(yyin == NULL){
    std::cerr << "Couldn't open input file: " << file_name << std::endl;
    exit(1);
  }
  g_root = nullptr;
  yyparse();
  return g_root;
}

Program *GetProgram()
{
	if(NULL == program)
	{
		program = new Program();	
	}

	return program;
}

FunctionDefinition *GetCurrentFunction()
{
    if (NULL == currentFunction)
    {
        currentFunction = new FunctionDefinition();
    }

    return currentFunction;
}

