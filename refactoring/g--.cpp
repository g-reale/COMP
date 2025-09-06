#include "Lexer.hpp"
// #include "Parser.hpp"
#include "Globals.hpp"

using namespace std;

int main(){
    Lexer lexer("example.cm");
    // Parser parser;

    bool done = false;
    bool failed = false;
    
    tree_t<int> test(-1);
    test.insert(0);
    test.left();
    test.insert(1);
    test.left();
    test.insert(2);
    test.left();
    test.insert(3);
    test.left();
    test.insert(4);
    test.left();
    test.insert(5);
    test.left();
    test.insert(6);
    test.left();
    test.insert(7);

    test.end();

    test.left();
    test.remove();
    test.left();
    test.remove();
    cout << (*test.cursor)->data << endl << endl;

    test.end();

    cout << (*test.cursor)->data << endl;
    test.left();
    cout << (*test.cursor)->data << endl;
    test.left();
    cout << (*test.cursor)->data << endl;
    test.left();
    cout << (*test.cursor)->data << endl;
    test.left();
    cout << (*test.cursor)->data << endl;
    test.left();
    cout << (*test.cursor)->data << endl;
    test.left();
    cout << (*test.cursor)->data << endl;
    test.left();
    cout << (*test.cursor)->data << endl;
    

    while(true){
        
        parseble_t parseble;
        try{
            parseble = lexer.tokenize(done);
        }
        catch(runtime_error& ex){
            cout << "Lexer: " << ex.what();
            failed = true;
        }
        
        if(done)
            break;

        if(parseble.first == token_t::LINE_END || parseble.first == token_t::COMMENT)
            continue;

        // try{
        //     parser.parse(parseble);
        // }
        // catch(runtime_error& ex){
        //     auto [token,lexeme] = parseble;
        //     cout << "Parser on " << token << "(" << lexeme << "):" << lexer.getLine() << ": " << ex.what() << endl;
        //     parser.backup();
        //     failed = true;
        // }
    }

    if(failed)
        exit(-1);

    // cout << parser.getTree();

}