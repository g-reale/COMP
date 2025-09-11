#include "Parser.hpp"

#include <set>
#include <sstream>

using namespace std;


const pair<Parser::first_t,Parser::grammar_t> Parser::CONSTANTS = []{
    
    string rules = R"(
        PROGRAM -> DECLARATION REPETITION_START DECLARATION REPETITION_END;
        DECLARATION -> VOID IDENTIFIER VOID_FUN_DECL | INT IDENTIFIER DECLARATION_DECISION;
        DECLARATION_DECISION -> VEC_DECL | SCALAR_DECL | INT_FUN_DECL;
        VEC_DECL -> OPEN_SQUARE NUM CLOSE_SQUARE SEMI;
        SCALAR_DECL -> SEMI;
        VOID_FUN_DECL -> OPEN_ROUND PARAMS VOID_COMPOSED_DECL;
        INT_FUN_DECL -> OPEN_ROUND PARAMS INT_COMPOSED_DECL;
        PARAMS -> VOID CLOSE_ROUND | PARAM REPETITION_START PARAM REPETITION_END;
        PARAM -> INT IDENTIFIER PARAM_DECISION;
        PARAM_DECISION -> SCALAR_PARAM | VEC_PARAM;
        SCALAR_PARAM -> COMMA | CLOSE_ROUND;
        VEC_PARAM -> OPEN_SQUARE CLOSE_SQUARE SCALAR_PARAM;
        INT_COMPOSED_DECL -> OPEN_CURLY OPTIONAL_START LOCAL_DECL OPTIONAL_END OPTIONAL_START INT_STATEMENT_LIST OPTIONAL_END CLOSE_CURLY;
        VOID_COMPOSED_DECL -> OPEN_CURLY OPTIONAL_START LOCAL_DECL OPTIONAL_END OPTIONAL_START VOID_STATEMENT_LIST OPTIONAL_END CLOSE_CURLY;
        LOCAL_DECL -> VAR_DECL REPETITION_START VAR_DECL REPETITION_END;
        VAR_DECL -> INT IDENTIFIER VAR_DECISION;
        VAR_DECISION -> VEC_DECL | SCALAR_DECL;
        INT_STATEMENT_LIST -> INT_STATEMENT REPETITION_START INT_STATEMENT REPETITION_END;
        INT_STATEMENT -> EXPRESSION_DECL | INT_COMPOSED_DECL | INT_SELECTION_DECL | INT_ITERATION_DECL | INT_RETURN_DECL;
        INT_SELECTION_DECL -> IF OPEN_ROUND CONDITION CLOSE_ROUND INT_IF_BODY OPTIONAL_START ELSE INT_ELSE_BODY OPTIONAL_END;
        INT_IF_BODY -> INT_STATEMENT;
        INT_ELSE_BODY -> INT_STATEMENT;
        INT_ITERATION_DECL -> WHILE OPEN_ROUND CONDITION CLOSE_ROUND INT_STATEMENT;
        INT_RETURN_DECL -> RETURN EXPRESSION SEMI;
        VOID_STATEMENT_LIST -> VOID_STATEMENT REPETITION_START VOID_STATEMENT REPETITION_END;
        VOID_STATEMENT -> EXPRESSION_DECL | VOID_COMPOSED_DECL | VOID_SELECTION_DECL | VOID_ITERATION_DECL | VOID_RETURN_DECL;
        VOID_SELECTION_DECL -> IF OPEN_ROUND CONDITION CLOSE_ROUND VOID_IF_BODY OPTIONAL_START ELSE VOID_ELSE_BODY OPTIONAL_END;
        VOID_IF_BODY -> VOID_STATEMENT;
        VOID_ELSE_BODY -> VOID_STATEMENT;
        VOID_ITERATION_DECL -> WHILE OPEN_ROUND CONDITION CLOSE_ROUND VOID_STATEMENT;
        VOID_RETURN_DECL -> RETURN SEMI;
        EXPRESSION_DECL -> EXPRESSION SEMI | SEMI;
        CONDITION -> EXPRESSION;
        EXPRESSION -> SIMPLE_EXP REPETITION_START EQUAL SIMPLE_EXP REPETITION_END;
        VAR -> IDENTIFIER OPTIONAL_START OPEN_SQUARE EXPRESSION CLOSE_SQUARE OPTIONAL_END;
        SIMPLE_EXP -> SUM_EXP OPTIONAL_START RELATIONAL SUM_EXP OPTIONAL_END;
        RELATIONAL -> LESS_EQ | LOGICAL_EQ | MORE_EQ | LOGICAL_EQ | NOT_EQUAL | LESS | MORE;
        SUM_EXP -> TERM REPETITION_START PLUS_MINUS TERM REPETITION_END;
        PLUS_MINUS -> SUM | SUB;
        TERM -> FACTOR REPETITION_START MUL_DIV FACTOR REPETITION_END;
        MUL_DIV -> MUL | DIV;
        FACTOR -> OPEN_ROUND EXPRESSION CLOSE_ROUND | IDENTIFIER OPTIONAL_START ACTIVATION_DECISION OPTIONAL_END | NUM;
        ACTIVATION_DECISION -> FUNCTION_ACTIVATION | VECTOR_ACTIVATION;
        VECTOR_ACTIVATION -> OPEN_SQUARE EXPRESSION CLOSE_SQUARE;
        FUNCTION_ACTIVATION -> OPEN_ROUND ARGUMENT_DECISION;
        ARGUMENT_DECISION -> CLOSE_ROUND | ARGUMENT REPETITION_START COMMA ARGUMENT REPETITION_END CLOSE_ROUND;
        ARGUMENT -> EXPRESSION;
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

Parser::Parser() : 
syntax_tree(token_t::ERROR)
{
    syntax_tree.insert(nonterminal_t::PROGRAM);
}

symbol_t Parser::push(nonterminal_t nonterminal, token_t token){

    
    const rule_t & rule = GRAMMAR[(size_t)nonterminal][(size_t)token];
    
    if(rule.empty())
        throw runtime_error("unable to find rule for token");

    const static set<nonterminal_t> CHECKPOINTS = {
        nonterminal_t::PROGRAM,

    };

    syntax_tree.down();
    syntax_tree.insert(rule);
    syntax_tree.left();
    return syntax_tree.current();
}

symbol_t Parser::next(){
    
    if(!syntax_tree.next())
        throw runtime_error("reached derivation tree end");
    return syntax_tree.current();
}

void Parser::copy(symbol_t end){
    
    auto read_head = syntax_tree.cursor;
    auto write_head = read_head;
    syntax_tree.right();
    syntax_tree.right(write_head);
    syntax_tree.left(read_head);
    
    symbol_t symbol = syntax_tree.current(read_head);
    while(symbol != end){
        syntax_tree.insert(symbol,write_head);
        syntax_tree.left(read_head);
        syntax_tree.left(write_head);
        symbol = syntax_tree.current(read_head);
    }
}

void Parser::remove(symbol_t end){
    auto destroy_head = syntax_tree.cursor;
    syntax_tree.right();

    symbol_t symbol; 
    do{
        symbol = syntax_tree.current(destroy_head);
        syntax_tree.remove(destroy_head);
        syntax_tree.left(destroy_head);
    }while(symbol != end);
}

bool Parser::inFirst(symbol_t symbol, token_t token){
    if(holds_alternative<nonterminal_t>(symbol))
        return FIRST[(size_t)get<nonterminal_t>(symbol)].count(token);
    return symbol == symbol_t(token);
}

void Parser::parse(const parseble_t& parseble){

    auto [token,lexeme] = parseble;
    symbol_t expected = syntax_tree.current();

    while(holds_alternative<nonterminal_t>(expected)){
        
        nonterminal_t nonterminal = get<nonterminal_t>(expected);
        switch (nonterminal){

            case nonterminal_t::REPETITION_START:{
                syntax_tree.left();
                symbol_t look_ahead = syntax_tree.current();
                syntax_tree.right();

                if(!inFirst(look_ahead,token)){
                    remove(nonterminal_t::REPETITION_END);
                    expected = next();
                    continue;
                }
                
                copy(nonterminal_t::REPETITION_END);
                expected = next();
            } break;

            case nonterminal_t::OPTIONAL_START:{
                syntax_tree.left();
                symbol_t look_ahead = syntax_tree.current();
                syntax_tree.right();

                if(!inFirst(look_ahead,token)){
                    remove(nonterminal_t::OPTIONAL_END);
                    expected = next();
                    continue;
                }
                
                auto optional_start = syntax_tree.cursor;
                copy(nonterminal_t::OPTIONAL_END);
                auto end_position = syntax_tree.cursor;
                syntax_tree.cursor = optional_start;
                remove(nonterminal_t::OPTIONAL_END);
                syntax_tree.cursor = end_position;
                expected = next();
            
            } break;
                
            default:
                expected = push(nonterminal,token);
                break;
        }
            
    }

    token_t correct = get<token_t>(expected);
    if(correct == token){

        if(token == token_t::NUM || token == token_t::IDENTIFIER)//save lexeme for latter use
            lexemes[*syntax_tree.cursor] = lexeme;
        next();
        return;
    }

    throw runtime_error("token did not match expected");
}

void Parser::backup(){
    //very dumb but very fast and easy
    syntax_tree.insert(nonterminal_t::PROGRAM);
    syntax_tree.next();
}

tree_t<assemblable_t> Parser::getTree(){
    
    tree_t<assemblable_t> abstract_tree(assemblable_t{token_t::ERROR});
    using syntax_node_t = tree_t<symbol_t>::node_t *;
    // using abstract_tree_t = tree_t<assemblable_t>::node_t *;

    // syntax_tree.focused = &syntax_tree.root;
    // syntax_tree.end();

    static const set<symbol_t> BLACKLIST = {
        token_t::COMMA,
        token_t::SEMI,
        nonterminal_t::DECLARATION_DECISION,
        nonterminal_t::PARAM_DECISION,
        nonterminal_t::SCALAR_PARAM,
        nonterminal_t::CONDITION
    };
    
    bool changed = true;
    std::function<bool(syntax_node_t)> flatten = [&](syntax_node_t node) -> bool {
        
        
        // while(changed){
        //     changed = false;
            // auto child = node->children.begin();
            // while(child != node->children.end()){

            //     symbol_t symbol = (*child)->data;
            //     if(holds_alternative<nonterminal_t>(symbol)){
            //         for(auto grandchild : (*child)->children){
            //             grandchild->parent = node;
            //             grandchild->self = node->children.insert((*child)->self,grandchild);
            //         }
            //         auto deceased = child;
            //         child++;
            //         node->children.erase(deceased);
            //         changed = true;
            //     }
            //     else
            //         child++;
            // }
        // }
        
        if(node->children.size() == 1){
            auto child = *node->children.begin();
            symbol_t symbol = child->data;
            if(holds_alternative<nonterminal_t>(symbol) || BLACKLIST.count(symbol)){
                for(auto grandchild : child->children){
                    grandchild->parent = node;
                    grandchild->self = node->children.insert(child->self,grandchild);
                }
                node->children.erase(child->self);
                changed = true;
            }
        }
            
        if(2 <= node->children.size()){
            auto brother = node->children.begin(); 
            auto sister = std::next(node->children.begin());

            while(sister != node->children.end()){
                symbol_t symbol = (*brother)->data;

                if(BLACKLIST.count(symbol)){
                    for (auto it = (*brother)->children.rbegin();
                        it != (*brother)->children.rend(); ++it) {
                        auto grandchild = *it;
                        grandchild->parent = *sister;
                        (*sister)->children.push_back(grandchild);
                        grandchild->self = std::prev((*sister)->children.end());
                    }
                    brother = node->children.erase(brother);
                    sister++;
                    changed = true;
                }
                else{
                    brother++;
                    sister++;
                }
            }
        }

        // if(2 <= node->children.size()){
        //     auto brother = node->children.begin();
        //     auto sister = std::next(node->children.begin());

        //     while(sister != node->children.end()){
        //         symbol_t symbol = (*sister)->data;
        //         if(holds_alternative<nonterminal_t>(symbol)){
        //             for (auto it = (*sister)->children.rbegin();
        //                 it != (*sister)->children.rend(); ++it) {
        //                 auto grandchild = *it;
        //                 grandchild->parent = *brother;
        //                 (*brother)->children.push_back(grandchild);
        //                 grandchild->self = std::prev((*brother)->children.end());
        //             }
        //             sister = node->children.erase(sister);
        //         }
        //         else{
        //             brother++;
        //             sister++;
        //         }
        //     }
        // }
        // else if(node->children.size()){
            
        // }

        for(auto child : node->children)
            flatten(child);
    };

    // flatten(&syntax_tree.root);
    while(changed){
        changed = false;
        flatten(&syntax_tree.root);
        cout << "executed" << endl;
    }
    // flatten(&syntax_tree.root);
    cout << syntax_tree;
    return abstract_tree;
}

// ostream& operator<<(ostream& os, const Parser::par_seq_t& sequence){
    
//     if(sequence.empty()){
//         os << "EMPTY";
//         return os;
//     }

//     for(parseble_t parseble : sequence){
//         auto [token,lexeme] = parseble;
//         os << parseble.first;
//         if(!lexeme.empty())
//             os << "(" << lexeme << ")";
//         os << " ";
//     }
//     return os;
// }

// void Parser::backup(){
//     syntax_tree.update(backup_tree);
//     context = syntax_tree.top;
// }

// tree_t<Parser::par_seq_t> Parser::getTree(){

//     static const set<token_t> BLACKLIST = {
//         // token_t::CLOSE_CURLY,
//         // token_t::OPEN_CURLY,
//         // token_t::OPEN_ROUND,
//         token_t::CLOSE_ROUND,
//         token_t::SEMI,
//         token_t::COMMA,
//         token_t::VOID,
//         token_t::INT,
//         token_t::CLOSE_SQUARE
//     };
    
//     tree_t<par_seq_t> abstract_tree{par_seq_t()};
//     function<void(tree_t<rule_ctx_t>::node_t * from)> traverse = [&](const tree_t<rule_ctx_t>::node_t * from){
//         par_seq_t parsebles;
    
//         if(from->data.rule != nullptr){
//             const rule_t & rule = (*from->data.rule);
//             const lex_seq_t & lexemes = from->data.lexemes;
//             for(size_t i = 0; i < rule.size(); i++){
//                 symbol_t symbol = rule[i];
//                 const string & lexeme = lexemes[i];

//                 if(holds_alternative<nonterminal_t>(symbol))
//                     continue;
//                 token_t token = get<token_t>(symbol);
//                 if(BLACKLIST.count(token))
//                     continue;
//                 parsebles.push_back(parseble_t(token,lexeme));
//             }
//         }
    
//         if(!parsebles.empty())
//             abstract_tree.push(parsebles);
        
//         for(tree_t<rule_ctx_t>::node_t * child : from->children)
//             traverse(child);
        
//         if(!parsebles.empty())
//             abstract_tree.pop();
//     };
    
//     traverse(syntax_tree.root);
//     return abstract_tree;
// 