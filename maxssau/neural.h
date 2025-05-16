#ifndef __neural__network__
#define __neural__network__

#include <cstdlib>
#include "enums.h"

namespace maxssau
{

    template <typename DataType>
    class Layer
    {
        public:
            Layer(unsigned int node_count)
            {
                Node=(DataType*)malloc(node_count*sizeof(DataType));
            }
            ~Layer()
            {
                free(Node);
            }

            DataType* Node;

            unsigned int GetNodesCount()
            {
                return node_count;
            }

        private:
            unsigned int node_count;
    };

    template <typename DataType>
    class NeuralNetwork
    {
        public:

            Layer<DataType>* Layers;

            unsigned char CreateNN(unsigned int count)
            {
                Layers=(Layer<DataType>*)malloc(count*sizeof(Layer<DataType>));
                layers_count=count;
                return STATUS_OK;
            }

            NeuralNetwork()
            {

            }

            NeuralNetwork(unsigned int count)
            {
                Layers=(Layer<DataType>*)malloc(count*sizeof(Layer<DataType>));
                layers_count=count;
            }

            ~NeuralNetwork()
            {
                free(Layers);
            }

            unsigned int GetLayersCount()
            {
                return layers_count;
            }

            virtual DataType ActivationFunction(Layer<DataType>* layers, unsigned int count)=0;

        private:

            unsigned int layers_count;
    };

    template <typename DataType>
    class NN_ActivationLinear: public NeuralNetwork<DataType>
    {
        public:
            DataType ActivationFunction(Layer<DataType>* layers, unsigned int count)
            {
                DataType result=0;

                for(int i=0;i<GetLayersCount();i++)
                {
                    
                }

                return result;
            }
    };
}


#endif