#include "parser.h"
#include "scanner.h"


int main(int argc, char** argv)
{
    FILE* prog = openFile(argc, argv);

    fclose(prog);
    return 0;
}
