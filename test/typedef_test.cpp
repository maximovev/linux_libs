#include "../maxssau/maxssau.h"
#include <stdio.h>

using namespace maxssau;

int main(int arg_count, char* arg_value[])
{
    byte_converter<int> test_data;

    test_data.value=0xAAFF55CC;

    printf("Data: 0x%X\n",test_data.value);

    for(int i=0;i<sizeof(test_data);i++)
    {    
        printf("bytes of data[%i]=0x%x\n",i,test_data.bytes[i]);
    }

    return 0;
}