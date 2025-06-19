#define __use__minmax__

#include <stdio.h>
#include "../maxssau/maxssau.h"

using namespace maxssau;

int main(int arg_count,char* arg_values[])
{
    MinMax<int> calc;

    printf("Init min=%i\n",calc.GetMinValue());
    printf("Init max=%i\n",calc.GetMaxValue());

    calc.Calculate(10);
    calc.Calculate(-20);

    printf("Calc min=%i\n",calc.GetMinValue());
    printf("Calc max=%i\n",calc.GetMaxValue());

    return 0;
}