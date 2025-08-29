#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <list>
#include <string>
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

template<typename T>
class tree_t {
public:
    struct node_t {
        node_t* parent;
        std::vector<node_t*> children;
        T data;
    };
    
    T * top;
    size_t depth;
    node_t * root;
    node_t * head;
    std::list<node_t*> arena;

    friend std::ostream& operator<<(std::ostream& os, const tree_t<T>& tree) {
        if (!tree.root) return os;

        std::function<void(node_t*, std::string)> traverse;
        traverse = [&](node_t* node, std::string indent) {
            
            if(node == tree.head)
                os << tree.depth << "->" << indent << node->data << "\n";
            else
                os << indent << node->data << "\n";

            for (auto* child : node->children)
                traverse(child, indent + "\t");
        };

        traverse(tree.root, "");
        return os;
    }

    node_t * insert(node_t* parent, const T& data) {
        node_t* child = new node_t{
            parent,
            {},
            data
        };
        parent->children.push_back(child);
        arena.push_back(child);
        return child;
    }

    void push(T data){
        depth++;
        head = insert(head,data);
        top = &head->data;
        std::cout << (*this) << std::endl;
    }

    void pop(){
        if(depth){
            depth--;
            head = head->parent;
            top = &head->data;
            std::cout << (*this) << std::endl;
        }
    }

    void update(tree_t &other) {
        while (depth < other.depth)
            other.pop();
        while (other.depth < depth)
            pop();
        while (head->data != other.head->data) {
            pop();
            other.pop();
        }
        std::function<void(node_t*)> replay = [this, &replay](node_t* from) {
            for (node_t* child : from->children) {
                push(child->data);
                replay(child);
                pop();
            }
        };
        replay(other.head);
    }
    
    tree_t(T data){
        node_t* child = new node_t{
            nullptr,
            {},
            data
        };
        arena.push_back(child);
        root = child;
        head = child;
        depth = 0;
    }

    ~tree_t() {
        for (node_t* node : arena)
            delete node;
        arena.clear();
    }
};

#endif
