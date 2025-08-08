#define __use__raw__

#include "../maxssau/maxssau.h"

#include <libraw/libraw.h>

#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace maxssau;

int main(int arg_count, char* arg_value[])
{
    LibRaw RawConverter;

		int LibRAW_Result = 0;

		int processing_state = 0;

		if (arg_count > 1)
		{
			processing_state = 1;
			LibRAW_Result = RawConverter.open_file(arg[1]);
			if (LibRAW_Result == LIBRAW_SUCCESS)
			{
				processing_state = 2;
				LibRAW_Result = RawConverter.unpack();
				if (LibRAW_Result == LIBRAW_SUCCESS)
				{
					RawConverter.raw2image();
                }
            }
        }
    return 0;
}