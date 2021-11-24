#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

FILE* openFile(int argc, char** argv)
{
    FILE* f = (argc > 1) ? fopen(argv[1], "r") : stdin;
    return f;
}




void processExpression()
{

}


