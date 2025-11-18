#ifndef PARSER_HPP
#define PARSER_HPP

#include <set>
#include <array>
#include <stack>
#include <vector>
#include <utility>
#include <exception>
#include <unordered_map>

#include "Globals.hpp"

class Parser{
    private:
        using rule_t = std::vector<symbol_t>;
        using rule_set_t = std::array<rule_t,(size_t)token_t::_COUNT>;
        using grammar_t = std::array<rule_set_t,(size_t)nonterminal_t::_COUNT>;
        static const grammar_t & GRAMMAR;
        
        bool inFirst(symbol_t symbol, token_t token);
        using token_space_t = std::set<token_t>;
        using first_t = std::array<token_space_t,(size_t)nonterminal_t::_COUNT>;
        static const first_t & FIRST;
        
        static const std::pair<first_t,grammar_t> CONSTANTS;
        
        lexeme_map_t lexemes;
        syntax_tree_t syntax_tree;
        void remove(symbol_t end);
        void copy(symbol_t end);
        symbol_t push(nonterminal_t nonterminal, token_t token);
        symbol_t next();

    public:
        Parser();
        void parse(const parseble_t& parseble);
        void backup();
        assemblable_t getTree();
};
#endif