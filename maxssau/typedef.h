
#ifndef __typeconvert__
#define __typeconvert__

/*
Maximov Evgeny
Russia, Samara, 13 may 2025

Data-to-bytes converter
*/

namespace maxssau
{

#pragma pack(push, 1)

template<typename StructType> union byte_converter
{
    public:
        StructType value;
        unsigned char bytes[sizeof(StructType)];    
};

#pragma pack(pop)

}

#endif
