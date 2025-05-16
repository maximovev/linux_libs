#define __use__neural__

#include <stdio.h>
#include "../maxssau/maxssau.h"

using namespace maxssau;

#define LAYERS_COUNT    5
#define NODE_COUNT      10

int main(int arg_count, char* arg_value[])
{

    NN_ActivationLinear<double>* linear_network=new NN_ActivationLinear<double>;

    linear_network->CreateNN(LAYERS_COUNT);

    linear_network->Layers[0].Node=new double;

    for(int i=1;i<linear_network->GetLayersCount()-1;i++)
    {
        linear_network->Layers[i].Node=(double*)malloc(sizeof(double)*NODE_COUNT);

        for(int j=0;j<NODE_COUNT;j++)
        {
            linear_network->Layers[i].Node[j]=(double)(i*j);
        }
    }

    

    printf("Neural Network simple test\n");

    return 0;
}