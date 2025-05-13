#include "../maxssau/maxssau.h"
#include <stdio.h>

using namespace maxssau;

int main(int arg_count, char* arg_value[])
{
    CurrentTime ct;
    printf("Current Time: %i:%i.%i\n",ct.GetHour(),ct.GetMinute(),ct.GetSeconds());
    printf("Current Date: %i.%i.%i\n",ct.GetYear(),ct.GetMonth(),ct.GetDay());
}