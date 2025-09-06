#include "Parser.hpp"

#include <set>
#include <sstream>

using namespace std;


const pair<Parser::first_t,Parser::grammar_t> Parser::CONSTANTS = []{
    
    string rules = R"(
        PROGRAM -> DECLARATION REPETITION_START DECLARATION REPETITION_END;
        DECLARATION -> INT IDENTIFIER DECLARATION_DECISION | VOID IDENTIFIER VOID_FUN_DECL;
        DECLARATION_DECISION -> OPEN_SQUARE NUM CLOSE_SQUARE SEMI | SEMI | OPEN_ROUND PARAMS OPEN_CURLY;
        VOID_FUN_DECL -> OPEN_ROUND PARAMS OPEN_CURLY;

        PARAMS -> VOID | PARAM REPETITION_START PARAM REPETITION_END;
        PARAM -> INT IDENTIFIER PARAM_DECISION;
        PARAM_DECISION -> OPEN_SQUARE CLOSE_SQUARE PARAM_END | PARAM_END;
        PARAM_END -> CLOSE_ROUND | COMMA;
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
// : syntax_tree((rule_ctx_t){.rule = nullptr, .at = 0, .repetition_start = 0}), 
//   backup_tree((rule_ctx_t){.rule = nullptr, .at = 0, .repetition_start = 0})
{
    // ctxPush(nonterminal_t::PROGRAM,token_t::INT);
}

// symbol_t Parser::ctxNext(){
//     context->at++;
//     return ctxCurrent();
// }

// symbol_t Parser::ctxCurrent(){
//     while(true){
        
//         if(!syntax_tree.depth) 
//             throw runtime_error("derivation stack empty");
    
//         if(context->at < context->rule->size())
//             break;

//         syntax_tree.pop();
//         context = syntax_tree.top;
//     }
//     return (*context->rule)[context->at];
// }

symbol_t Parser::push(nonterminal_t nonterminal, token_t token){

    const rule_t * rule = &GRAMMAR[(size_t)nonterminal][(size_t)token];

    if(rule->empty())
        throw runtime_error("unable to find rule for token");

    syntax_tree.down();
    for(symbol_t symbol : *rule){
        syntax_tree.insert(symbol);
        syntax_tree.left();
    }
    syntax_tree.end();
    return *syntax_tree.current();
}

bool Parser::inFirst(symbol_t symbol, token_t token){
    if(holds_alternative<nonterminal_t>(symbol))
        return FIRST[(size_t)get<nonterminal_t>(symbol)].count(token);
    return symbol == symbol_t(token);
}


void Parser::parse(const parseble_t& parseble){

    auto [token,lexeme] = parseble;
    symbol_t expected = *syntax_tree.current();

    while(holds_alternative<nonterminal_t>(expected)){
        
        nonterminal_t nonterminal = get<nonterminal_t>(expected);
        switch (nonterminal){
            
            // case nonterminal_t::REPETITION_START:
            //     context->repetition_start = context->at;
            
            // case nonterminal_t::OPTIONAL_START:
            //     expected = ctxNext();
            //     if(!inFirst(expected,token)){ // skip ebnf block
            //         symbol_t end = nonterminal == nonterminal_t::OPTIONAL_START ? nonterminal_t::OPTIONAL_END : nonterminal_t::REPETITION_END;
            //         while(end != ctxNext());
            //         expected = ctxNext();
            //     }
            //     break;
            
            // case nonterminal_t::REPETITION_END:
            //     context->at = context->repetition_start;
            //     expected = ctxCurrent();
            //     break;
            
            // case nonterminal_t::OPTIONAL_END:
            //     expected = ctxNext();
            //     break;

            case nonterminal_t::REPETITION_START:{
                
                auto checkpoint = syntax_tree.checkpoint();
                syntax_tree.left(checkpoint);
                expected = *syntax_tree.current(checkpoint);

                if(!inFirst(expected,token)){//skip ebnf
                    do{
                        expected = *syntax_tree.current(checkpoint);
                        syntax_tree.remove(checkpoint);
                        syntax_tree.left(checkpoint);
                    }while(expected != (symbol_t)nonterminal_t::REPETITION_END);
                    syntax_tree.checkpoint(checkpoint);
                    expected = *syntax_tree.current();
                    continue;
                }
                
                syntax_tree

            } break;
                
            default:
                expected = push(nonterminal,token);
                break;
        }
            
    }

    token_t correct = get<token_t>(expected);
    if(correct == token){
        
        if(token == token_t::IDENTIFIER || 
           token == token_t::NUM
        )
            ctxSave(lexeme);

        ctxNext();

        if(token == token_t::SEMI ||
           token == token_t::OPEN_CURLY ||
           token == token_t::CLOSE_CURLY 
        )
            backup_tree.update(syntax_tree);

        return;
    }

    throw runtime_error("token did not match expected");
}

bool Parser::rule_ctx_t::operator==(const rule_ctx_t& other) const{
    return rule == other.rule &&
           at == other.at &&
           repetition_start == other.repetition_start;
}

bool Parser::rule_ctx_t::operator!=(const rule_ctx_t& other) const{
    return !(*this == other);
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

ostream& operator<<(ostream& os, const Parser::par_seq_t& sequence){
    
    if(sequence.empty()){
        os << "EMPTY";
        return os;
    }

    for(parseble_t parseble : sequence){
        auto [token,lexeme] = parseble;
        os << parseble.first;
        if(!lexeme.empty())
            os << "(" << lexeme << ")";
        os << " ";
    }
    return os;
}

void Parser::backup(){
    syntax_tree.update(backup_tree);
    context = syntax_tree.top;
}

tree_t<Parser::par_seq_t> Parser::getTree(){

    static const set<token_t> BLACKLIST = {
        // token_t::CLOSE_CURLY,
        // token_t::OPEN_CURLY,
        // token_t::OPEN_ROUND,
        token_t::CLOSE_ROUND,
        token_t::SEMI,
        token_t::COMMA,
        token_t::VOID,
        token_t::INT,
        token_t::CLOSE_SQUARE
    };
    
    tree_t<par_seq_t> abstract_tree{par_seq_t()};
    function<void(tree_t<rule_ctx_t>::node_t * from)> traverse = [&](const tree_t<rule_ctx_t>::node_t * from){
        par_seq_t parsebles;
    
        if(from->data.rule != nullptr){
            const rule_t & rule = (*from->data.rule);
            const lex_seq_t & lexemes = from->data.lexemes;
            for(size_t i = 0; i < rule.size(); i++){
                symbol_t symbol = rule[i];
                const string & lexeme = lexemes[i];

                if(holds_alternative<nonterminal_t>(symbol))
                    continue;
                token_t token = get<token_t>(symbol);
                if(BLACKLIST.count(token))
                    continue;
                parsebles.push_back(parseble_t(token,lexeme));
            }
        }
    
        if(!parsebles.empty())
            abstract_tree.push(parsebles);
        
        for(tree_t<rule_ctx_t>::node_t * child : from->children)
            traverse(child);
        
        if(!parsebles.empty())
            abstract_tree.pop();
    };
    
    traverse(syntax_tree.root);
    return abstract_tree;
}