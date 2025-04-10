#ifndef STATES_DFA_H
#define STATES_DFA_H
 typedef enum{
	START=(unsigned char)32,
	END=(unsigned char)33,
	FAILURE=(unsigned char)34,
	SUM=(unsigned char)35,
	SUB=(unsigned char)36,
	MUL=(unsigned char)37,
	DIV=(unsigned char)38,
	LESS=(unsigned char)39,
	MORE=(unsigned char)40,
	SEMI=(unsigned char)41,
	COMMA=(unsigned char)42,
	EQUAL=(unsigned char)43,
	OPEN_ROUND=(unsigned char)44,
	CLOSE_ROUND=(unsigned char)45,
	OPEN_SQUARE=(unsigned char)46,
	CLOSE_SQUARE=(unsigned char)47,
	OPEN_CURLY=(unsigned char)48,
	CLOSE_CURLY=(unsigned char)49,
	LESS_EQ=(unsigned char)50,
	MORE_EQ=(unsigned char)51,
	LOGICAL_EQ=(unsigned char)52,
	NOT=(unsigned char)53,
	NOT_EQUAL=(unsigned char)54,
	NUM=(unsigned char)55,
	IDENTIFIER=(unsigned char)56,
	AMBIGUOUS=(unsigned char)57,
	COMMENT=(unsigned char)58,
	COMMENT_OUT=(unsigned char)59,
	IF=(unsigned char)60,
	INT=(unsigned char)61,
	VOID=(unsigned char)62,
	ELSE=(unsigned char)63,
	WHILE=(unsigned char)64,
	RETURN=(unsigned char)65,
	PROGRAM=(unsigned char)66,
	DECLARATION=(unsigned char)67,
	DECLARATION_DECISION=(unsigned char)68,
	VEC_DECL=(unsigned char)69,
	SCALAR_DECL=(unsigned char)70,
	VOID_FUN_DECL=(unsigned char)71,
	INT_FUN_DECL=(unsigned char)72,
	PARAMS=(unsigned char)73,
	PARAM=(unsigned char)74,
	INT_COMPOSED_DECL=(unsigned char)75,
	VOID_COMPOSED_DECL=(unsigned char)76,
	LOCAL_DECL=(unsigned char)77,
	VAR_DECL=(unsigned char)78,
	VAR_DECISION=(unsigned char)79,
	INT_STATEMENT_LIST=(unsigned char)80,
	INT_STATEMENT=(unsigned char)81,
	INT_SELECTION_DECL=(unsigned char)82,
	INT_ITERATION_DECL=(unsigned char)83,
	INT_RETURN_DECL=(unsigned char)84,
	VOID_STATEMENT_LIST=(unsigned char)85,
	VOID_STATEMENT=(unsigned char)86,
	VOID_SELECTION_DECL=(unsigned char)87,
	VOID_ITERATION_DECL=(unsigned char)88,
	VOID_RETURN_DECL=(unsigned char)89,
	EXPRESSION_DECL=(unsigned char)90,
	EXPRESSION=(unsigned char)91,
	VAR=(unsigned char)92,
	SIMPLE_EXP=(unsigned char)93,
	RELATIONAL=(unsigned char)94,
	SUM_EXP=(unsigned char)95,
	PLUS_MINUS=(unsigned char)96,
	TERM=(unsigned char)97,
	MUL_DIV=(unsigned char)98,
	FACTOR=(unsigned char)99,
	ACTIVATION_DECISION=(unsigned char)100,
	ARGS=(unsigned char)101,
	OPTIONAL=(unsigned char)102,
	REPETITION=(unsigned char)103,

}dfa_states;
static const char STATE_NAMES[73][21] = {"START", "END", "FAILURE", "SUM", "SUB", "MUL", "DIV", "LESS", "MORE", "SEMI", "COMMA", "EQUAL", "OPEN_ROUND", "CLOSE_ROUND", "OPEN_SQUARE", "CLOSE_SQUARE", "OPEN_CURLY", "CLOSE_CURLY", "LESS_EQ", "MORE_EQ", "LOGICAL_EQ", "NOT", "NOT_EQUAL", "NUM", "IDENTIFIER", "AMBIGUOUS", "COMMENT", "COMMENT_OUT", "IF", "INT", "VOID", "ELSE", "WHILE", "RETURN", "PROGRAM", "DECLARATION", "DECLARATION_DECISION", "VEC_DECL", "SCALAR_DECL", "VOID_FUN_DECL", "INT_FUN_DECL", "PARAMS", "PARAM", "INT_COMPOSED_DECL", "VOID_COMPOSED_DECL", "LOCAL_DECL", "VAR_DECL", "VAR_DECISION", "INT_STATEMENT_LIST", "INT_STATEMENT", "INT_SELECTION_DECL", "INT_ITERATION_DECL", "INT_RETURN_DECL", "VOID_STATEMENT_LIST", "VOID_STATEMENT", "VOID_SELECTION_DECL", "VOID_ITERATION_DECL", "VOID_RETURN_DECL", "EXPRESSION_DECL", "EXPRESSION", "VAR", "SIMPLE_EXP", "RELATIONAL", "SUM_EXP", "PLUS_MINUS", "TERM", "MUL_DIV", "FACTOR", "ACTIVATION_DECISION", "ARGS", "OPTIONAL", "REPETITION"};
#endif