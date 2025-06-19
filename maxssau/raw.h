#ifndef __raw__
#define __raw__

#include "enums.h"

namespace maxssau
{
    
    template <typename Type>struct MinMaxValues
    {
        Type Min;
        Type Max;
    };

    template <typename Type>struct RGBMinMaxValues
    {
        MinMaxValues<Type> R;
        MinMaxValues<Type> G;
        MinMaxValues<Type> B;
    };
    

    template <typename TypeInputData, typename TypeOutputData> class raw_converter
    {
        public:
            raw_converter()
            {

            };

            ~raw_converter()
            {
                
            }



            int Process(TypeInputData *input, TypeOutputData *output,MinMaxValues<TypeInputData> MinMax, unsigned int height, unsigned int width)
            {
                return STATUS_OK;
            }

        private:
    };


}

#endif