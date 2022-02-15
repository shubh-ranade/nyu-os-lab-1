#include "linker.hpp"
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    ifstream input;
    input.open(argv[1]);

    pass1(input);

    input.close();

    input.open(argv[1]);

    pass2(input);

    input.close();

    return EXIT_SUCCESS;
}