#include "tokenizer.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    char *tok;

    ifstream input;
    input.open(argv[1]);
    
    while(1) {
        tok = getToken(input);
        if(tok == NULL)
            break;
        cout << "Token: " << linenum << ':' << lineofs << " : " << tok << '\n';
    }
    cout << "Final Spot in File : " << "line=" << linenum << ' ' << "offset=" << line_end << '\n';
    input.close();

    return EXIT_SUCCESS;
}