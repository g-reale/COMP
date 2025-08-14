#include "Parser.hpp"
#include <regex>

using namespace std;

const Parser::grammar_t Parser::GRAMMAR = []{
    grammar_t grammar;
    
    string rules = R"(
        PROGRAM -> INT IDENTIFIER INT_DECL
        INT_DECL -> SEMI SCALAR_DECL | OPEN_SQUARE VEC_DECL
        SCALAR_DECL -> EPS
        VEC_DECL -> NUM CLOSE_SQUARE
    )";
    
    
    // R"([A-Z_ ]+)"
    // R"(([A-Z_]+))"

    regex RULE_FINDER(R"(^\s*([A-Z_]+)\s*->\s*(.*)$)");
    
    auto begin = sregex_iterator(rules.begin(), rules.end(), RULE_FINDER);
    auto end = sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        smatch match = *it;
        string target = match[1].str();
        string productions = match[2].str();
        
        
    }

    return grammar;
}();

Parser::Parser(){
    matches.push(nonterminal_t::PROGRAM);
}

bool Parser::parse(token_t token){
    
    // while(holds_alternative<nonterminal_t>(matches.top())){
    //     nonterminal_t nonterminal = get<nonterminal_t>(matches.top());
    //     rule_t rule = GRAMMAR[(size_t)nonterminal][(size_t)token];
    //     rule_t alternative = GRAMMAR[(size_t)nonterminal][(size_t)EPS];
    //     rule_t target;

    //     if(rule.empty() && alternative.empty())
    //         return false;
        
    //     target = !rule.empty() ? rule : alternative;
    //     for(int i = target.size(); 0 <= i; i--)
    //         matches.push(target[i]);
    // }

    // token_t expected = get<token_t>(matches.top());
    // cout << expected << " " << token << endl;
    
    // if(expected != token && expected != EPS)
    //     return false;
    
    // matches.pop();
    return true;
}