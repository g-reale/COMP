#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <list>
#include <string>
#include <memory>
#include <utility>
#include <variant>
#include <iomanip>
#include <iostream>
#include <functional>
#include <unordered_map>

#define OPERATOR_TYPES\
    OPERATOR(SUM)\
    OPERATOR(SUB)\
    OPERATOR(MUL)\
    OPERATOR(DIV)\
    OPERATOR(FJMP)\
    OPERATOR(LT)\
    OPERATOR(GT)\
    OPERATOR(LEQ)\
    OPERATOR(GEQ)\
    OPERATOR(EQ)\
    OPERATOR(NEQ)\
    OPERATOR(SET)\
    OPERATOR(SETDS)\
    OPERATOR(SETDD)\
    OPERATOR(SETDDI)\
    OPERATOR(SETI)\
    OPERATOR(_COUNT)\

enum class operator_t{
    #define OPERATOR(name) name,
    OPERATOR_TYPES
    #undef OPERATOR
};

inline std::ostream& operator<<(std::ostream& os, operator_t op) {
    std::string text;
     switch (op) {
        #define OPERATOR(name) case operator_t::name: text = #name; break;
        OPERATOR_TYPES
        #undef OPERATOR
        default:
            text = "undefined operator!";
    }
    return os << text;
}

// struct assembly_t{
//     operator_t operation = operator_t::_COUNT;
//     size_t destination = 0;
//     size_t argument_a = 0;
//     size_t argument_b = 0;
// };

// inline std::ostream& operator<<(std::ostream& os, assembly_t assembly){
//     return os << assembly.operation << "(" << assembly.destination << "," << assembly.argument_a << "," << assembly.argument_b << ")";
// }

// using program_t = std::list<assembly_t>;

// inline std::ostream& operator<<(std::ostream& os, const program_t& program){
//     size_t i = 0;
//     for(auto& assembly : program){
//         os << i << ":\t" << assembly << std::endl;
//         i++;
//     }
//     return os;
// }


//order must be compatible with operator_t for easy conversion during assembly
#define TOKEN_TYPES\
    TOKEN(SUM)\
    TOKEN(SUB)\
    TOKEN(MUL)\
    TOKEN(DIV)\
    TOKEN(COMMA)\
    TOKEN(LESS)\
    TOKEN(MORE)\
    TOKEN(LESS_EQ)\
    TOKEN(MORE_EQ)\
    TOKEN(LOGICAL_EQ)\
    TOKEN(NOT_EQUAL)\
    TOKEN(EQUAL)\
    TOKEN(LINE_END)\
    TOKEN(SEMI)\
    TOKEN(COMMENT)\
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
    TOKEN(ASCII)\
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
    NONTERMINAL(VEC_PARAM_END)\
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
    NONTERMINAL(INT_RETURN_DECL)\
    NONTERMINAL(INT_WHILE_BODY)\
    NONTERMINAL(VOID_STATEMENT_LIST)\
    NONTERMINAL(VOID_STATEMENT)\
    NONTERMINAL(VOID_SELECTION_DECL)\
    NONTERMINAL(VOID_IF_BODY)\
    NONTERMINAL(VOID_ELSE_BODY)\
    NONTERMINAL(VOID_ITERATION_DECL)\
    NONTERMINAL(VOID_RETURN_DECL)\
    NONTERMINAL(VOID_WHILE_BODY)\
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
    NONTERMINAL(ARGUMENTS)\
    NONTERMINAL(EQUALITY)\
    NONTERMINAL(ARGUMENT)\
    NONTERMINAL(INDEX)\
    NONTERMINAL(PARAM_END)\
    NONTERMINAL(VOID_FUN_SHAPPING)\
    NONTERMINAL(KING)\
    NONTERMINAL(QUEEN)\
    NONTERMINAL(DUKE)\
    NONTERMINAL(JOKER)\
    NONTERMINAL(SERF)\
    NONTERMINAL(NUMERAL)\
    NONTERMINAL(CHARACTER)\
    NONTERMINAL(NAME)\
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
        using node_list_t = std::list<node_t*>;
        using iter_t = typename node_list_t::iterator;
        
        data_t data;
        node_t* parent = nullptr;
        node_list_t children = {};
        iter_t self;
    };

    node_t root;
    node_t * focused;
    size_t depth;
    typename node_t::node_list_t arena;
    typename node_t::iter_t cursor;
    
    tree_t(data_t data){
        depth = 0;
        root.data = data;
        focused = &root;
        cursor = root.children.begin();
    }

    ~tree_t(){
        for(node_t * child : arena)
            delete child;
    }

    void insert(data_t data, typename node_t::iter_t & iterator){

        node_t * child = new node_t{
            .data = data,
            .parent = focused
        };
        arena.push_back(child);

        if(!focused->children.size()){
            typename node_t::iter_t self = focused->children.insert(focused->children.begin(),child);
            child->self = self;
            iterator = focused->children.begin();
            return;
        }

        typename node_t::iter_t self = focused->children.insert(iterator,child);
        child->self = self;
    }

    void insert(data_t data){
        insert(data,cursor);
    }

    void insert(const std::vector<data_t>& data, typename node_t::iter_t & iterator){
        typename node_t::iter_t checkpoint = iterator;

        for(const data_t & element : data){
            insert(element,checkpoint);
            left(checkpoint);
        }
    }

    void insert(const std::vector<data_t>& data){
        insert(data,cursor);
    }

    bool left(typename node_t::iter_t & iterator){
        if(iterator != focused->children.begin()){
            iterator--;
            return true;
        }
        return false;
    }

    bool left(){
        return left(cursor);
    }

    bool right(typename node_t::iter_t & iterator){
        if(iterator != focused->children.end()){
            iterator++;
            return true;
        }
        return false;
    }

    bool right(){
        return right(cursor);
    }

    bool down(){

        if(!focused->children.empty()){
            focused = *cursor;
            cursor = focused->children.end();
            depth++;
            return true;
        }
        return false;
    }

    bool up(){
        if(focused->parent){
            cursor = focused->self;
            focused = focused->parent;
            depth--;
            return true;
        }
        return false;
    }

    void end(typename node_t::iter_t & iterator){
        iterator = std::prev(focused->children.end());
    }

    void end(){
        return end(cursor);
    }

    data_t& current(typename node_t::iter_t & iterator){
        return ((*iterator)->data);
    }

    data_t& current(){
        return current(cursor);
    }

    bool remove(node_t * node, typename node_t::iter_t & iterator){
        if(iterator != node->children.end()){
            iterator = focused->children.erase(iterator);
            return true;
        }
        return false;
    }

    bool remove(typename node_t::iter_t & iterator){
        return remove(focused,iterator);
    }

    bool remove(){
        return remove(cursor);
    }

    void donate(node_t * doner, node_t * receiver, typename node_t::iter_t destination){
        
        for(auto child : doner->children){
            child->parent = receiver;
            child->self = receiver->children.insert(destination,child);
        }
    }

    bool next(){
        bool success;
        while(!(success = left())){
            if(!(success = up()))
                break;
        }
        return success;
    }

    bool prev(){
        bool success;
        while(!(success = right())){
            if(!(success = up()))
                break;
        }
        return success;
    }

    template<typename Decending, typename Ascending>
    void traverse(node_t * node, const Decending& decending, const Ascending& ascending){
        decending(node);
        for(auto child : node->children)
            traverse(child,decending,ascending);
        ascending(node);
    }

    template<typename Decending, typename Ascending>
    void traverse(const Decending& decending, const Ascending& ascending){
        traverse(&root,decending,ascending);
    }

    template<typename Decending, typename Ascending>
    void traverseRev(node_t * node, const Decending& decending, const Ascending& ascending){
        decending(node);
        for(auto child = node->children.rbegin(); child != node->children.rend(); child++)
            traverse(*child,decending,ascending);
        ascending(node);
    }

    template<typename Decending, typename Ascending>
    void traverseRev(const Decending& decending, const Ascending& ascending){
        traverseRev(&root,decending,ascending);
    }

    bool empty(){
        return focused->children.empty();
    }
};

template<typename data_t>
std::ostream& operator<<(std::ostream& os, const tree_t<data_t>& tree){
    
    std::string indent = "";
    std::function<void(const typename tree_t<data_t>::node_t *)> traverse = [&](const typename tree_t<data_t>::node_t * node){
        if(node->self == tree.cursor)
            os << indent << "[" << node->data << "]" << std::endl;
        else if(node == tree.focused)
            os << indent << "-" << node->data << "-" << std::endl;
        else
        os << indent << node->data << std::endl;
        
        indent += "\t";
        for (auto child = node->children.rbegin(); child != node->children.rend(); child++)
            traverse(*child);
        indent.pop_back();
    };
    traverse(&tree.root);
    return os;
}

using syntax_tree_t = tree_t<symbol_t>;
using syntax_node_t = tree_t<symbol_t>::node_t *;
using lexeme_map_t = std::unordered_map<tree_t<symbol_t>::node_t *,std::string>;

struct assemblable_t{
    syntax_tree_t& tree;
    lexeme_map_t& lexemes;
};

// using binary_t = std::vector<size_t>;

// inline std::ostream& operator<<(std::ostream& os, const binary_t& binary){
//     size_t main = binary[0];
//     size_t assembly = binary[1];

//     os << "0: " << main << " (entrypoint)" << std::endl;
//     os << "1: " << assembly << " (code start)" << std::endl;

//     for(size_t i = 2; i < assembly; i++)
//         os << i << ": " << binary[i] << std::endl;

//     for(size_t i = assembly; i < binary.size(); i++){
//         size_t word = binary[i];
//         assembly_t assembly = {
//             .operation = (operator_t)((word >> 30) & 0x3FF),
//             .destination = (word >> 20) & 0x3FF,
//             .argument_a = (word >> 10) & 0x3FF,
//             .argument_b = word & 0x3FF
//         };
//         os << i << ": " << std::setw(11) << std::setfill('0') << binary[i] << "\t" << assembly << std::endl;
//     }
//     return os;
// }
#endif
