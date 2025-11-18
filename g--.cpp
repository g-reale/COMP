// #include <getopt.h>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Globals.hpp"
#include "Assembler.hpp"

using namespace std;

int main(int argc, char* argv[]){

    if(argc < 3)
        throw runtime_error("too few arguments");

    string program(argv[1]);
    string binary(argv[2]);

    Lexer lexer(program);
    Parser parser;
    Assembler assembler;

    bool done = false;
    bool failed = false;
    
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

        try{
            parser.parse(parseble);
        }
        catch(runtime_error& ex){
            auto [token,lexeme] = parseble;
            cout << "Parser on " << token << "(" << lexeme << "):" << lexer.getLine() << ": " << ex.what() << endl;
            parser.backup();
            failed = true;
        }
    }

    if(failed){
        cout << "compilation failed" << endl;
        exit(-1);
    }

    assemblable_t assemblable = parser.getTree();
    assembler.assemble(assemblable);
    assembler.writeBinary(binary);
}