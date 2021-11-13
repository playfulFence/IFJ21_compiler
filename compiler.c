#include "parser.h"
#include "scanner.h"
#include "dynamic_string.h"

int main(int argc, char** argv)
{
    char *tokens[] = {"ID", ";", ",", "INT", "NUM", "STR", "WHILE", "DO", "IF", "THEN", "ELSE", "END", "FUNC", "GLOBAL", "LOCAL", "NIL", "REQUAIRE", "RETURN", "#", "+","-","*","/","//","..","<",">","<=",">=","==","=","LB","RB","EOF"};
    FILE* prog = openFile(argc, argv);
    token_t token;
    while (1)
    {
        token = getToken(prog);
        if(token.type == TOKEN_ID || token.type == TOKEN_STR)
        {
            printf("token :\t%s\ndata  :\t%s\n\n", tokens[token.type], token.data.tokenStringVal);    
        }
        else if(token.type == TOKEN_INT)
        {
            printf("token :\t%s\ndata  :\t%d\n\n", tokens[token.type], token.data.tokenIntVal);
        }
        else if(token.type == TOKEN_NUM)
        {
            printf("token :\t%s\ndata  :\t%f\n\n", tokens[token.type], token.data.tokenNumVal);
        }
        else
        {
            printf("token :\t%s\ndata  :\t*_*\n\n", tokens[token.type]);
        }
        if (token.type == TOKEN_EOF)
        {
            break;
        }        
    }
    
    
    fclose(prog);
    return 0;
}
