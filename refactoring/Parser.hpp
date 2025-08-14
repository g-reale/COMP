#ifndef PARSER_HPP
#define PARSER_HPP

#include <stack>
#include <array>
#include <vector>

#include "Globals.hpp"

class Parser{
    private:
        using rule_t = std::vector<symbol_t>;
        using rule_set_t = std::array<rule_t,(size_t)token_t::_token_COUNT>;
        using grammar_t = std::array<rule_set_t,(size_t)nonterminal_t::_nonterminal_COUNT>;
        static const grammar_t GRAMMAR;
        std::stack<symbol_t> matches;
    public:
        Parser();
        bool parse(token_t token);
};
#endif