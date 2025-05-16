#ifndef __maxssau__global__
#define __maxssau__global__ 

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef null
#define null 0
#endif

#include <stdio.h>

#include "typedef.h"

#ifdef __use__time_user__
#include "time_user.h"
#endif

#ifdef __use__xml_io__
    #include "xml_io.h"
#endif

#ifdef __use__neural__
    #include "neural.h"
#endif


#endif