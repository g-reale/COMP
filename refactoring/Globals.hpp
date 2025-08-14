#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <string>
#include <utility>
#include <variant>
#include <iostream>
#include <unordered_map>

#define TOKEN_TYPES\
    TOKEN(COMMENT)\
    TOKEN(LESS_EQ)\
    TOKEN(MORE_EQ)\
    TOKEN(LOGICAL_EQ)\
    TOKEN(NOT_EQUAL)\
    TOKEN(SUM)\
    TOKEN(SUB)\
    TOKEN(DIV)\
    TOKEN(MUL)\
    TOKEN(LESS)\
    TOKEN(MORE)\
    TOKEN(SEMI)\
    TOKEN(COMMA)\
    TOKEN(EQUAL)\
    TOKEN(OPEN_ROUND)\
    TOKEN(CLOSE_ROUND)\
    TOKEN(OPEN_SQUARE)\
    TOKEN(CLOSE_SQUARE)\
    TOKEN(OPEN_CURLY)\
    TOKEN(CLOSE_CURLY)\
    TOKEN(NUM)\
    TOKEN(IF)\
    TOKEN(INT)\
    TOKEN(VOID)\
    TOKEN(ELSE)\
    TOKEN(WHILE)\
    TOKEN(RETURN)\
    TOKEN(IDENTIFIER)\
    TOKEN(EPS)\
    TOKEN(_COUNT)
    
enum class token_t{
    #define TOKEN(name) name,
    TOKEN_TYPES
    #undef TOKEN
};

inline std::ostream& operator<<(std::ostream& os, token_t token) {
    std::string text;
     switch (token) {
        #define TOKEN(name) case token_t::name: text = #name; break;
        TOKEN_TYPES
        #undef TOKEN
        default:
            text = "undefined token!";
    }
    return os << text;
}

inline std::unordered_map<std::string,token_t> TOKEN_FROM_STRING = {
    #define TOKEN(name) {#name,token_t::name},
    TOKEN_TYPES
    #undef TOKEN
};

using parseble_t = std::pair<token_t,std::string>;

#define NONTERMINAL_TYPES\
    NONTERMINAL(PROGRAM)\
    NONTERMINAL(INT_DECL)\
    NONTERMINAL(_COUNT)

enum class nonterminal_t{
    #define NONTERMINAL(name) name,
    NONTERMINAL_TYPES
    #undef NONTERMINAL
};

inline std::ostream& operator<<(std::ostream& os, nonterminal_t nonterminal) {
    std::string text;
     switch (nonterminal) {
        #define NONTERMINAL(name) case nonterminal_t::name: text = #name; break;
        NONTERMINAL_TYPES
        #undef NONTERMINAL
        default:
            text = "undefined nonterminal!";
    }
    return os << text;
}

inline std::unordered_map<std::string,nonterminal_t> NONTERMINAL_FROM_STRING = {
    #define NONTERMINAL(name) {#name,nonterminal_t::name},
    NONTERMINAL_TYPES
    #undef NONTERMINAL
};

using symbol_t = std::variant<nonterminal_t,token_t>;

#endif
