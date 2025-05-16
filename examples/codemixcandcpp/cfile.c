#include <stdio.h>
#include "cfile.h"

int main (int arg_count, char* arg_value[])
{
    c_func(10);
    cpp_func(20);

    return 0;
}

void c_func(int param)
{
    printf("c function, param value %i\n",param);
}