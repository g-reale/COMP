#ifndef PARSER_HPP
#define PARSER_HPP

#include <set>
#include <array>
#include <stack>
#include <vector>
#include <utility>
#include <exception>

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
    
    public:
        struct rule_ctx_t{
            const rule_t * rule;
            size_t at;
            size_t repetition_start;
            bool operator==(const rule_ctx_t& other) const;
            bool operator!=(const rule_ctx_t& other) const;
        };
    private:
        rule_ctx_t * context;
        // std::vector<rule_ctx_t> contexts;
        symbol_t ctxPush(nonterminal_t nonterminal, token_t token);
        symbol_t ctxCurrent();
        symbol_t ctxNext();
        std::vector<rule_ctx_t> backup_contexts;
        void updateVersion();
        
    public:
        tree_t<rule_ctx_t> syntax_tree;
        tree_t<rule_ctx_t> backup_tree;
        
        Parser();
        void parse(const parseble_t& parseble);
        void backItUp();
};

std::ostream& operator<<(std::ostream& os, const Parser::rule_ctx_t& context);

#endif