#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "scanner.h"

token_t getToken(FILE *f)
{
    token_t newToken;

    char symbol;
    while (1)
    {
        symbol = getc(f);

        switch (symbol) {
            case ' ': // white space
                continue;
            case '\t': // tab
                continue;
            case '\n': // newline
                continue;
            case '\r': // other newline
                continue;
            case EOF:
                newToken.type = TOKEN_EOF;
                break;
            case '_':
            case 'a'...'z':
            case 'A'...'Z':



        }
    }
    return token;
}


