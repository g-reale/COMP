#include "Parser.hpp"

#include <set>
#include <sstream>

using namespace std;


const pair<Parser::first_t,Parser::grammar_t> Parser::CONSTANTS = []{
    
    string rules = R"(
        PROGRAM ->               DECLARATION REPETITION_START DECLARATION REPETITION_END;
        DECLARATION ->           VOID IDENTIFIER VOID_FUN_DECL | INT IDENTIFIER DECLARATION_DECISION;
        DECLARATION_DECISION ->  VEC_DECL | SCALAR_DECL | INT_FUN_DECL;
           
        VEC_DECL ->              OPEN_SQUARE NUM CLOSE_SQUARE SEMI;
        SCALAR_DECL ->           SEMI;
        VOID_FUN_DECL ->         OPEN_ROUND PARAMS VOID_COMPOSED_DECL;
        INT_FUN_DECL ->          OPEN_ROUND PARAMS INT_COMPOSED_DECL;
    
        PARAMS ->                VOID CLOSE_ROUND | PARAM REPETITION_START PARAM REPETITION_END;
        PARAM ->                 INT IDENTIFIER PARAM_DECISION;
        PARAM_DECISION ->        SCALAR_PARAM | VEC_PARAM;
        SCALAR_PARAM ->          COMMA | CLOSE_ROUND;
        VEC_PARAM ->             OPEN_SQUARE CLOSE_SQUARE SCALAR_PARAM;
           
        INT_COMPOSED_DECL ->     OPEN_CURLY OPTIONAL_START LOCAL_DECL OPTIONAL_END OPTIONAL_START INT_STATEMENT_LIST OPTIONAL_END CLOSE_CURLY;
        VOID_COMPOSED_DECL ->    OPEN_CURLY OPTIONAL_START LOCAL_DECL OPTIONAL_END OPTIONAL_START VOID_STATEMENT_LIST OPTIONAL_END CLOSE_CURLY;
        LOCAL_DECL ->            VAR_DECL REPETITION_START VAR_DECL REPETITION_END;
        VAR_DECL ->              INT IDENTIFIER VAR_DECISION;
        VAR_DECISION ->          VEC_DECL | SCALAR_DECL;
           
        INT_STATEMENT_LIST ->    INT_STATEMENT REPETITION_START INT_STATEMENT REPETITION_END;
        INT_STATEMENT ->         EXPRESSION_DECL | INT_COMPOSED_DECL | INT_SELECTION_DECL | INT_ITERATION_DECL | INT_RETURN_DECL;
        INT_SELECTION_DECL ->    IF OPEN_ROUND CONDITION CLOSE_ROUND INT_IF_BODY OPTIONAL_START ELSE INT_ELSE_BODY OPTIONAL_END;
        INT_IF_BODY ->           INT_STATEMENT;
        INT_ELSE_BODY ->         INT_STATEMENT;
        INT_ITERATION_DECL ->    WHILE OPEN_ROUND CONDITION CLOSE_ROUND INT_STATEMENT;
        INT_RETURN_DECL ->       RETURN EXPRESSION SEMI;
           
        VOID_STATEMENT_LIST ->   VOID_STATEMENT REPETITION_START VOID_STATEMENT REPETITION_END;
        VOID_STATEMENT ->        EXPRESSION_DECL | VOID_COMPOSED_DECL | VOID_SELECTION_DECL | VOID_ITERATION_DECL | VOID_RETURN_DECL;
        VOID_SELECTION_DECL ->   IF OPEN_ROUND CONDITION CLOSE_ROUND VOID_IF_BODY OPTIONAL_START ELSE VOID_ELSE_BODY OPTIONAL_END;
        VOID_IF_BODY ->          VOID_STATEMENT;
        VOID_ELSE_BODY ->        VOID_STATEMENT;
        VOID_ITERATION_DECL ->   WHILE OPEN_ROUND CONDITION CLOSE_ROUND VOID_STATEMENT;
        VOID_RETURN_DECL ->      RETURN SEMI;
        EXPRESSION_DECL ->       EXPRESSION SEMI | SEMI;
    
        CONDITION ->             EXPRESSION;
        EXPRESSION ->            SIMPLE_EXP REPETITION_START EQUAL SIMPLE_EXP REPETITION_END;
        VAR ->                   IDENTIFIER OPTIONAL_START OPEN_SQUARE EXPRESSION CLOSE_SQUARE OPTIONAL_END;
        SIMPLE_EXP ->            SUM_EXP OPTIONAL_START RELATIONAL SUM_EXP OPTIONAL_END;
        RELATIONAL ->            LESS_EQ | LOGICAL_EQ | MORE_EQ | LOGICAL_EQ | NOT_EQUAL | LESS | MORE;
        SUM_EXP ->               TERM REPETITION_START PLUS_MINUS TERM REPETITION_END;
        PLUS_MINUS ->            SUM | SUB;
        TERM ->                  FACTOR REPETITION_START MUL_DIV FACTOR REPETITION_END;
        MUL_DIV ->               MUL | DIV;
        FACTOR ->                OPEN_ROUND EXPRESSION CLOSE_ROUND | IDENTIFIER OPTIONAL_START ACTIVATION_DECISION OPTIONAL_END | NUM;
        ACTIVATION_DECISION ->   FUNCTION_ACTIVATION | VECTOR_ACTIVATION;
        VECTOR_ACTIVATION ->     OPEN_SQUARE EXPRESSION CLOSE_SQUARE;
        FUNCTION_ACTIVATION ->   OPEN_ROUND ARGUMENT_DECISION;
        ARGUMENT_DECISION ->     CLOSE_ROUND | ARGUMENT REPETITION_START COMMA ARGUMENT REPETITION_END CLOSE_ROUND;
        ARGUMENT ->              EXPRESSION;
        )";
    
    istringstream slicer(rules);
    
    enum class parser_state_t{
        TARGET,
        PRODUCTION_START,
        PRODUCTION_FILL,
    };

    array<vector<rule_t>,(size_t) nonterminal_t::_COUNT> base_grammar;
    parser_state_t state = parser_state_t::TARGET;
    nonterminal_t target;
    string word;
    rule_t rule;

    //parsing the grammar configuration. Simple FSM parser

    while(!slicer.eof()){
        slicer >> word;
        switch (state){

            case parser_state_t::TARGET:   
                target = NONTERMINAL_FROM_STRING[word];
                state = parser_state_t::PRODUCTION_START;
                break;

            case parser_state_t::PRODUCTION_START:
                if(word != "->")
                    throw runtime_error("while loading the grammar, got " + word + " expected ->");
                state = parser_state_t::PRODUCTION_FILL;
                break;

            case parser_state_t::PRODUCTION_FILL:
                
                if(word == "|"){
                    base_grammar[(size_t)target].push_back(rule);
                    rule.clear();
                    continue;
                }

                if(word.back() == ';'){
                    word.pop_back();
                    rule.push_back(symbolFromString(word));
                    base_grammar[(size_t)target].push_back(rule);
                    rule.clear();
                    state = parser_state_t::TARGET;
                    continue;
                }

                rule.push_back(symbolFromString(word));
                break;
            
            default:
                break;
        }
    }

    //verifying some easy to make mistakes on the base grammar

    enum class analyzer_state_t{
        START,
        OPTIONAL_START,
        REPETITION_START,
    };

    for (const vector<rule_t>& rule_set : base_grammar) {
    for (rule_t rule : rule_set) {
            
        analyzer_state_t state = analyzer_state_t::START;

        for(symbol_t symbol : rule){

            if (holds_alternative<token_t>(symbol))
                continue;
    
            nonterminal_t nonterminal = get<nonterminal_t>(symbol);
    
            switch (state) {
                case analyzer_state_t::START:
                    switch (nonterminal) {
                        case nonterminal_t::OPTIONAL_START:
                            state = analyzer_state_t::OPTIONAL_START;
                            break;
                        case nonterminal_t::REPETITION_START:
                            state = analyzer_state_t::REPETITION_START;
                            break;
                        case nonterminal_t::OPTIONAL_END:
                            throw runtime_error("while loading grammar, found unopened optional block");
                        case nonterminal_t::REPETITION_END:
                            throw runtime_error("while loading grammar, found unopened repetition block");
                        default:
                            break;
                    }
                    break;
    
                case analyzer_state_t::OPTIONAL_START:
                    switch (nonterminal) {
                        case nonterminal_t::REPETITION_START:
                            throw runtime_error("while loading grammar, found nested optional-repetition block");
                        case nonterminal_t::OPTIONAL_START:
                            throw runtime_error("while loading grammar, found nested optional-optional block");
                        case nonterminal_t::REPETITION_END:
                            throw runtime_error("while loading grammar, found unopened repetition block");
                        case nonterminal_t::OPTIONAL_END:
                            state = analyzer_state_t::START;
                            break;
                        default:
                            break;
                    }
                    break;
    
                case analyzer_state_t::REPETITION_START:
                    switch (nonterminal) {
                        case nonterminal_t::REPETITION_START:
                            throw runtime_error("while loading grammar, found nested repetition-repetition block");
                        case nonterminal_t::OPTIONAL_START:
                            throw runtime_error("while loading grammar, found nested repetition-optional block");
                        case nonterminal_t::OPTIONAL_END:
                            throw runtime_error("while loading grammar, found unopened optional block");
                        case nonterminal_t::REPETITION_END:
                            state = analyzer_state_t::START;
                            break;
                        default:
                            break;
                    }
                    break;
            }
        }

        if (state != analyzer_state_t::START)
            throw runtime_error("while loading grammar, found unbalanced ebnf block");
        }
    }
    
    
    //calculating the first for each nonterminal
    //i'm allowing EBNF grammars with special "repetition" and "optional" nonterminals
    //rules must can't start with "repetition" or "optional" and can't be empty
    
    array<set<token_t>,(size_t)nonterminal_t::_COUNT> first;
    array<size_t,(size_t)nonterminal_t::_COUNT> sizes;
    sizes.fill(0);
    bool changed = true;
    
    while(changed){
        changed = false;
        for(size_t nonterminal = 0; nonterminal < base_grammar.size(); nonterminal++){
            const vector<rule_t>& rules = base_grammar[nonterminal];
            
            for(const rule_t& rule : rules){
                symbol_t symbol = rule[0];
                
                if(holds_alternative<token_t>(symbol))
                    first[nonterminal].insert(get<token_t>(symbol));
                else{
                    size_t recursive = (size_t)get<nonterminal_t>(symbol);
                    first[nonterminal].merge(first[recursive]);
                }
                
                changed |= sizes[nonterminal] != first[nonterminal].size();
                sizes[nonterminal] = first[nonterminal].size();
            }
        }
    }
    
    // filling the ll1 lookup table 
    
    grammar_t grammar;
    auto fill_ll1 = [&first](const auto& self, symbol_t symbol, rule_t rule, rule_set_t& rule_set){
        
        if(holds_alternative<token_t>(symbol)){
            size_t token = (size_t)get<token_t>(symbol);
            rule_set[token] = rule;
            return;
        }

        size_t recursive = (size_t)get<nonterminal_t>(symbol);
        for(symbol_t symbol : first[recursive])
            self(self,symbol,rule,rule_set);
        return;
    };

    for(size_t nonterminal = 0; nonterminal < grammar.size(); nonterminal++){
        rule_set_t& rule_set = grammar[nonterminal];
        
        for(const rule_t& rule : base_grammar[nonterminal])
            fill_ll1(fill_ll1,rule[0],rule,rule_set);
    }

    return pair{first,grammar};
}();

const Parser::first_t & Parser::FIRST = []{return CONSTANTS.first;}();
const Parser::grammar_t & Parser::GRAMMAR = []{return CONSTANTS.second;}();

Parser::Parser()
: syntax_tree((rule_ctx_t){.rule = nullptr, .at = 0, .repetition_start = 0}), 
  backup_tree((rule_ctx_t){.rule = nullptr, .at = 0, .repetition_start = 0})
{
    ctxPush(nonterminal_t::PROGRAM,token_t::INT);
}

symbol_t Parser::ctxNext(){
    context->at++;
    return ctxCurrent();
}

symbol_t Parser::ctxCurrent(){
    while(true){

        // if(contexts.empty()) 
        //     throw runtime_error("derivation stack empty");
        
        if(!syntax_tree.depth) 
            throw runtime_error("derivation stack empty");
        
        if(context->at < context->rule->size()){
            cout << syntax_tree << endl;
            break;
        }

        // contexts.pop_back();
        // context = &contexts.back();
        
        syntax_tree.pop();
        context = syntax_tree.top;
    }
    return (*context->rule)[context->at];
}

symbol_t Parser::ctxPush(nonterminal_t nonterminal, token_t token){

    const rule_t * rule = &GRAMMAR[(size_t)nonterminal][(size_t)token];

    if(rule->empty())
        throw runtime_error("unable to find rule for token");

    // contexts.push_back(
    //     (rule_ctx_t){
    //         .rule = rule,
    //         .at = 0,
    //         .repetition_start = 0,
    //     }
    // );
    // context = &contexts.back();
    // return (*context->rule)[0];

    syntax_tree.push(
        (rule_ctx_t){
            .rule = rule,
            .at = 0,
            .repetition_start = 0,
        }
    );
    context = syntax_tree.top;
    return (*context->rule)[0];
}

bool Parser::inFirst(symbol_t symbol, token_t token){
    if(holds_alternative<nonterminal_t>(symbol))
        return FIRST[(size_t)get<nonterminal_t>(symbol)].count(token);
    return symbol == symbol_t(token);
}


void Parser::parse(const parseble_t& parseble){

    auto [token,lexeme] = parseble;
    cout << "-/-" << token << "-/-" << endl << endl;
    symbol_t expected = ctxCurrent();

    while(holds_alternative<nonterminal_t>(expected)){
        
        nonterminal_t nonterminal = get<nonterminal_t>(expected);
        switch (nonterminal){
            
            case nonterminal_t::REPETITION_START:
                context->repetition_start = context->at;
            
            case nonterminal_t::OPTIONAL_START:
                expected = ctxNext();
                if(!inFirst(expected,token)){ // skip ebnf block
                    symbol_t end = nonterminal == nonterminal_t::OPTIONAL_START ? nonterminal_t::OPTIONAL_END : nonterminal_t::REPETITION_END;
                    while(end != ctxNext());
                    expected = ctxNext();
                }
                break;
            
            case nonterminal_t::REPETITION_END:
                context->at = context->repetition_start;
                expected = ctxCurrent();
                break;
            
            case nonterminal_t::OPTIONAL_END:
                expected = ctxNext();
                break;
                
            default:
                context->at++;
                expected = ctxPush(nonterminal,token);
                break;
        }
            
    }

    token_t correct = get<token_t>(expected);
    if(correct == token){
        ctxNext();

        // if(token == token_t::SEMI ||
        //    token == token_t::OPEN_CURLY ||
        //    token == token_t::CLOSE_CURLY 
        // )
        //     backup_tree.update(syntax_tree);
        return;
    }

    throw runtime_error("token did not match expected");
}

bool Parser::rule_ctx_t::operator==(const rule_ctx_t& other) const{
    return rule == other.rule
        && at == other.at
        && repetition_start == other.repetition_start;
}

bool Parser::rule_ctx_t::operator!=(const rule_ctx_t& other) const{
    return rule != other.rule;
}

ostream& operator<<(ostream& os, const Parser::rule_ctx_t& context){
    
    if(!context.rule){
        os << "EMPTY";
        return os;
    }

    for(size_t i = 0; i < context.rule->size(); i++){
        if(context.at == i)
            os << "[" << (*context.rule)[i] << "] ";
        else
            os << (*context.rule)[i] << " ";
    }
    return os;
}

void Parser::updateVersion(){
    // size_t common = std::min(backup_contexts.size(), contexts.size());
    // while (0 < common) {
    //     if (backup_contexts[common - 1] == contexts[common - 1])
    //         break;
    //     common--;
    // }
    // backup_contexts.resize(contexts.size());
    // copy(contexts.begin() + common, contexts.end(), backup_contexts.begin() + common);
}

void Parser::backItUp(){
    // contexts = backup_contexts;
    // context = &contexts.back();
    // syntax_tree.update(backup_tree);
    // context = syntax_tree.top;
}