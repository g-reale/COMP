#include "Lexer.hpp"

using namespace std;

const array<string,(size_t)token_t::_COUNT> Lexer::TOKEN_RULES = []{
    array<string,(size_t)token_t::_COUNT> rules;
    rules[(size_t)token_t::SUM]          = R"(\+)";
    rules[(size_t)token_t::SUB]          = R"(\-)";
    rules[(size_t)token_t::DIV]          = R"(\/)";
    rules[(size_t)token_t::MUL]          = R"(\*)";
    rules[(size_t)token_t::LESS]         = R"(\<)";
    rules[(size_t)token_t::MORE]         = R"(\>)";
    rules[(size_t)token_t::SEMI]         = R"(\;)";
    rules[(size_t)token_t::COMMA]        = R"(\,)";
    rules[(size_t)token_t::EQUAL]        = R"(\=)";
    rules[(size_t)token_t::OPEN_ROUND]   = R"(\()";
    rules[(size_t)token_t::CLOSE_ROUND]  = R"(\))";
    rules[(size_t)token_t::OPEN_SQUARE]  = R"(\[)";
    rules[(size_t)token_t::CLOSE_SQUARE] = R"(\])";
    rules[(size_t)token_t::OPEN_CURLY]   = R"(\{)";
    rules[(size_t)token_t::CLOSE_CURLY]  = R"(\})";
    rules[(size_t)token_t::LESS_EQ]      = R"(\<\=)";
    rules[(size_t)token_t::MORE_EQ]      = R"(\>\=)";
    rules[(size_t)token_t::LOGICAL_EQ]   = R"(\=\=)";
    rules[(size_t)token_t::NOT_EQUAL]    = R"(\!\=)";
    rules[(size_t)token_t::NUM]          = R"(\d+)";
    rules[(size_t)token_t::COMMENT]      = R"(\/\*(?:\n|.)*?\*\/)";
    rules[(size_t)token_t::IF]           = R"(if\W)";
    rules[(size_t)token_t::INT]          = R"(int\W)";
    rules[(size_t)token_t::VOID]         = R"(void\W)";
    rules[(size_t)token_t::ELSE]         = R"(else\W)";
    rules[(size_t)token_t::WHILE]        = R"(while\W)";
    rules[(size_t)token_t::RETURN]       = R"(return\W)";
    rules[(size_t)token_t::IDENTIFIER]   = R"([a-zA-Z]+)";
    
    return rules;
}();

const regex Lexer::TOKEN_FINDER = []{
    std::string rules = "";
    for(size_t i = 0; i < (size_t)token_t::EPS; i++)
        rules += "(" + TOKEN_RULES[i] + ")|";
    rules.pop_back();
    cout << rules << endl;
    return regex(rules);
}();

void Lexer::open(const string& path){
    ifstream source = ifstream(path,std::ios::in);
    if(!source.is_open()){
        cerr << "Unable to open input file: " << path << endl; 
        exit(-1);
    }

    //load the whole file in memory (ugly but makes life easier for the regex)
    code = string(istreambuf_iterator<char>(source), //sometimes i hate the std lib...
                  istreambuf_iterator<char>());
    at = code.begin();
    end = code.end();
    source.close();
}

Lexer::Lexer(const string& path){
    at = end;
    open(path);
}

parseble_t Lexer::tokenize(bool& finished){
    finished = false;
    
    if(at == end){
        finished = true;
        return parseble_t(token_t::_COUNT,"");
    }

    match_results<std::string::iterator> matches;
    if(!regex_search(at, end, matches, TOKEN_FINDER)){
        finished = true;
        return parseble_t(token_t::_COUNT,"");
    }

    token_t token = token_t::_COUNT;
    string lexeme = "";

    for(size_t i = 1; i < matches.size(); i++){
        if(!matches[i].matched)
            continue;
        
        token = (token_t)(i-1);
        lexeme = matches[i].str();
        at = matches[0].second;
    }
    
    return parseble_t(token,lexeme);
}

