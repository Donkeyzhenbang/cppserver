#include <cstdio>
#include <cstdlib> 
#include "utils.h"

void errif(bool condition, const char* errmsg)
{
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}