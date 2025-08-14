#ifndef LEXER_HPP
#define LEXER_HPP

#include <regex>
#include <array>
#include <string>
#include <fstream>
#include <iterator>
#include <iostream>

#include "Globals.hpp"

class Lexer{
    private:
        static const std::array<std::string, (size_t)token_t::_token_COUNT> TOKEN_RULES;
        static const std::regex TOKEN_FINDER;
        std::string code;
        std::string::iterator at;
        std::string::iterator end;
    public:
        Lexer(const std::string& path);
        void open(const std::string& path);
        parseble_t tokenize(bool& finished);
};

#endif