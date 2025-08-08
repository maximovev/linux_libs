#ifndef __typeconvert__
#define __typeconvert__

/*
Maximov Evgeny
Russia, Samara, 13 may 2025

upd: 2025/08/06 - add align and no align

Data-to-bytes converter
*/

namespace maxssau
{

#pragma pack(push, 1)

template<typename StructType> union byte_converter_no_align
{
    public:
        StructType value;
        unsigned char bytes[sizeof(StructType)];
};

#pragma pack(pop)

template<typename StructType> union byte_converter_align
{
    public:
        StructType value;
        unsigned char bytes[sizeof(StructType)];
};

}

#endif
