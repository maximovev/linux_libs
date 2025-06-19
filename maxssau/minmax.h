#ifndef __minmax__
#define __minmax__

#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>

namespace maxssau
{
    template <typename ClassType>
    class MinMax
    {
    public:
        MinMax()
        {
            Reset();
        }

        ClassType GetMaxValue()
        {
            return MaxValue;
        }

        ClassType GetMinValue()
        {
            return MinValue;
        }

        void Calculate(ClassType value)
        {
            if (MinValue > value)
            {
                MinValue = value;
            }

            if (MaxValue < value)
            {
                MaxValue = value;
            }
        }

        void SetMaxValue(ClassType value)
        {
            MaxValue = value;
        }

        void SetMinValue(ClassType value)
        {
            MinValue = value;
        }

        void Reset()
        {
            constexpr ClassType min = std::numeric_limits<ClassType>::lowest();
            constexpr ClassType max = std::numeric_limits<ClassType>::max();

            // thats all right!
            MinValue = max;
            MaxValue = min;
        }

    private:
        ClassType MinValue;
        ClassType MaxValue;
    };

}

#endif