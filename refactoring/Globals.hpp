#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <list>
#include <string>
#include <memory>
#include <utility>
#include <variant>
#include <iostream>
#include <functional>
#include <unordered_map>

#define TOKEN_TYPES\
    TOKEN(COMMENT)\
    TOKEN(LINE_END)\
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
    TOKEN(_COUNT)\
    TOKEN(ERROR)
    
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
    NONTERMINAL(DECLARATION)\
    NONTERMINAL(DECLARATION_DECISION)\
    NONTERMINAL(VEC_DECL)\
    NONTERMINAL(SCALAR_DECL)\
    NONTERMINAL(VOID_FUN_DECL)\
    NONTERMINAL(INT_FUN_DECL)\
    NONTERMINAL(PARAMS)\
    NONTERMINAL(PARAM)\
    NONTERMINAL(PARAM_DECISION)\
    NONTERMINAL(SCALAR_PARAM)\
    NONTERMINAL(VEC_PARAM)\
    NONTERMINAL(INT_COMPOSED_DECL)\
    NONTERMINAL(VOID_COMPOSED_DECL)\
    NONTERMINAL(LOCAL_DECL)\
    NONTERMINAL(VAR_DECL)\
    NONTERMINAL(VAR_DECISION)\
    NONTERMINAL(INT_STATEMENT_LIST)\
    NONTERMINAL(INT_STATEMENT)\
    NONTERMINAL(INT_SELECTION_DECL)\
    NONTERMINAL(INT_IF_BODY)\
    NONTERMINAL(INT_ELSE_BODY)\
    NONTERMINAL(INT_ITERATION_DECL)\
    NONTERMINAL(INT_RETURN_DECL    )\
    NONTERMINAL(VOID_STATEMENT_LIST)\
    NONTERMINAL(VOID_STATEMENT)\
    NONTERMINAL(VOID_SELECTION_DECL)\
    NONTERMINAL(VOID_IF_BODY)\
    NONTERMINAL(VOID_ELSE_BODY)\
    NONTERMINAL(VOID_ITERATION_DECL)\
    NONTERMINAL(VOID_RETURN_DECL)\
    NONTERMINAL(EXPRESSION_DECL)\
    NONTERMINAL(CONDITION)\
    NONTERMINAL(EXPRESSION)\
    NONTERMINAL(VAR)\
    NONTERMINAL(SIMPLE_EXP)\
    NONTERMINAL(RELATIONAL)\
    NONTERMINAL(SUM_EXP)\
    NONTERMINAL(PLUS_MINUS)\
    NONTERMINAL(TERM)\
    NONTERMINAL(MUL_DIV)\
    NONTERMINAL(FACTOR)\
    NONTERMINAL(ACTIVATION_DECISION)\
    NONTERMINAL(VECTOR_ACTIVATION)\
    NONTERMINAL(FUNCTION_ACTIVATION)\
    NONTERMINAL(ARGUMENT_DECISION)\
    NONTERMINAL(ARGUMENT)\
    NONTERMINAL(PARAM_END)\
    NONTERMINAL(_COUNT)\
    NONTERMINAL(OPTIONAL_START)\
    NONTERMINAL(OPTIONAL_END)\
    NONTERMINAL(REPETITION_START)\
    NONTERMINAL(REPETITION_END)

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

inline std::ostream& operator<<(std::ostream& os, symbol_t symbol) {
    if(std::holds_alternative<token_t>(symbol))
        return operator<<(os,std::get<token_t>(symbol));
    return operator<<(os,std::get<nonterminal_t>(symbol));
}

inline symbol_t symbolFromString(const std::string& string){
    if(NONTERMINAL_FROM_STRING.count(string))
        return symbol_t(NONTERMINAL_FROM_STRING[string]);
    else if (TOKEN_FROM_STRING.count(string))
        return symbol_t(TOKEN_FROM_STRING[string]);
    
    std::cerr << "string: " << string << " is neither token nor nonterminal" << std::endl;
    exit(-1);
    return symbol_t();
}

template<typename data_t>
class tree_t{
    public:

    struct node_t {
        using node_list_t = std::list<std::unique_ptr<node_t>>;
        using iter_t = typename node_list_t::iterator;
        
        data_t data;
        node_t* parent = nullptr;
        node_list_t children = {};
        iter_t self;
    };

    node_t root;
    node_t * focused;
    typename node_t::iter_t cursor;
    
    tree_t(data_t data){
        root.data = data;
        focused = &root;
        cursor = root.children.begin();
    }

    void insert(data_t data, typedef node_t::iter_t & iterator = cursor){

        if(!focused->children.size()){
            typename node_t::iter_t child = focused->children.insert(focused->children.begin(),std::make_unique<node_t>());
            *(*child) = (node_t){
                .data = data,
                .parent = focused,
                .self = child
            };
            iterator = focused->children.begin();
            return;
        }

        typename node_t::iter_t child = focused->children.insert(iterator,std::make_unique<node_t>());
        *(*child) = (node_t){
            .data = data,
            .parent = focused,
            .self = child
        };
    }

    bool left(typedef node_t::iter_t & iterator = cursor){
        if(iterator != focused->children.begin()){
            iterator--;
            return true;
        }
        return false;
    }

    bool right(typedef node_t::iter_t & iterator = cursor){
        if(std::next(iterator) != focused->children.end()){
            iterator++;
            return true;
        }
        return false;
    }

    bool down(typedef node_t::iter_t & iterator = cursor){
        if(!focused->children.empty()){
            focused = (*iterator).get();
            iterator = focused->children.begin();
            return true;
        }
        return false;
    }

    bool up(typedef node_t::iter_t & iterator = cursor){
        if(focused->parent){
            iterator = focused->self;
            focused = focused->parent;
            return true;
        }
        return false;
    }

    void end(typedef node_t::iter_t & iterator = cursor){
        iterator = std::prev(focused->children.end());
    }

    data_t * current(typedef node_t::iter_t & iterator = cursor){
        return &((*iterator)->data);
    }

    typename node_t::iter_t checkpoint(){
        return cursor;
    }

    void checkpoint(typename node_t::iter_t save){
        cursor = save;
    }

    bool remove(typedef node_t::iter_t & iterator = cursor){
        if(iterator != focused->children.end()){
            iterator = focused->children.erase(iterator);
            return true;
        }
        return false;
    }

};

#endif
