#ifndef STATES_DFA_H
#define STATES_DFA_H
 typedef enum{
	START=(unsigned char)'0',
	END=(unsigned char)'?',
	FAILURE=(unsigned char)':',
	SUM=(unsigned char)'1',
	SUB=(unsigned char)'2',
	MUL=(unsigned char)'3',
	DIV=(unsigned char)'4',
	LESS=(unsigned char)'5',
	MORE=(unsigned char)'6',
	SEMI=(unsigned char)'7',
	COMMA=(unsigned char)'8',
	EQUAL=(unsigned char)'a',
	OPEN_ROUND=(unsigned char)'b',
	CLOSE_ROUND=(unsigned char)'c',
	OPEN_SQUARE=(unsigned char)'d',
	CLOSE_SQUARE=(unsigned char)'e',
	OPEN_CURLY=(unsigned char)'f',
	CLOSE_CURLY=(unsigned char)'g',
	LESS_EQ=(unsigned char)'h',
	MORE_EQ=(unsigned char)'i',
	LOGICAL_EQ=(unsigned char)'j',
	NOT=(unsigned char)'k',
	NOT_EQUAL=(unsigned char)'l',
	NUM=(unsigned char)'m',
	IDENTIFIER=(unsigned char)'n',
	AMBIGUOUS=(unsigned char)'o',
	COMMENT=(unsigned char)'p',
	COMMENT_OUT=(unsigned char)'q',

}dfa_states;
#endif
