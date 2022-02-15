#include "tokenizer.hpp"

using namespace std;

char buffer[BUFFER_SIZE] = {'\0'};
char* tok = NULL;

int linenum = 0;
int lineofs;
int line_end;
int marker = 0;
bool eof = false;
char delimiters[] = " \t\n\r";
bool parse_error = false;

//TODO: what do we do in case the buffer is not big enough to read
// assume buffer is always big enough to read acc to 
// https://discord.com/channels/924479444163366972/924479444477964343/941126978227015690
void getStream(ifstream& f) {
    f.getline(buffer, BUFFER_SIZE);
    
    // check gcount before increasing linenum
    if(f.gcount() > 0)
        linenum++;

    if(f.eof())
        eof = true;
}

// tokens returned by this function are null-terminated
char* getToken(ifstream& f) {
    if(buffer[0] == '\0') {
        getStream(f);

        int t = f.gcount();
        while(!t && !eof) {
            getStream(f);
            t = f.gcount();
        }
        line_end = t > 0 ? t : line_end;
        marker = t;
        if(marker < BUFFER_SIZE) buffer[marker] = '\0';
        
        tok = strtok(buffer, delimiters);
    }

    else
        tok = strtok(NULL, delimiters);

    if(tok == NULL && !eof) {
        buffer[0] = '\0';
        tok = getToken(f);
    }

    if(eof) {
        // cout << "reached eof\n";
        lineofs = line_end;
    }

    if(tok != NULL) {
        lineofs = tok - buffer + 1;
        // cout << "Token: " << linenum << ':' << lineofs << " : " << tok << " eof " << eof << '\n';
    }

    return tok;
}

//TODO: where are we checking eof?
int readInt(ifstream& f) {
    char* tok = getToken(f);
    int t = 0;

    // return -1 when eof reached.
    // tok == false and eof == true happen at the same time
    if(!tok && eof) {
        return -1;
    }

    //this is redundant
    else if(!tok) {
        __parseerror(0);
        exit(EXIT_FAILURE);
    }

    while(tok[t]) {
        char c = tok[t++];
        if(!isdigit(c)) {
            __parseerror(0);
            exit(EXIT_FAILURE);
        }
    }

    int n = atoi(tok);
    if(n >= MAX_NUM) {
        __parseerror(0);
        exit(EXIT_FAILURE);
    }

    // cout << "int read " << n << endl;
    return n;
}

char* readSymbol(ifstream& f) {
    char* tok = getToken(f);
    int t = 0;

    if(!tok) {
        __parseerror(1);
        exit(EXIT_FAILURE);
    }
    // symbol regex is : [a-Z][a-Z0-9]*
    if(tok[t] && !isalpha(tok[t++])) {
        __parseerror(1);
        exit(EXIT_FAILURE);
    }
    while(tok[t]) {
        // check symbol length first
        if(t >= 16) {
            __parseerror(3);
            exit(EXIT_FAILURE);
        }
        char c = tok[t++];
        if(!isalnum(c)) {
            __parseerror(1);
            exit(EXIT_FAILURE);
        }
    }

    // cout << "symbol read " << tok << endl;

    return tok;
}

char readIAER(ifstream& f) {
    // cout << "addrmode in\n";
    char* tok = getToken(f);
    
    if(!tok) {
        __parseerror(2);
        exit(EXIT_FAILURE);
    }
    char c = tok[0];
    if(tok[1] != '\0' || (c != 'I' && c != 'A' && c != 'E' && c != 'R')) {
        __parseerror(2);
        exit(EXIT_FAILURE);
    }
    // cout << "addrmode read " << c << endl;
    
    return c;
}
