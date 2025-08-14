#include "Lexer.hpp"
#include "Parser.hpp"
#include "Globals.hpp"

using namespace std;

int main(){
    Lexer lexer("example.cm");
    
    bool done = false;
    while(!done)
        lexer.tokenize(done);
}