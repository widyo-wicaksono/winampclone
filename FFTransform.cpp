#include "FFTransform.h"

float FastSqrt(float x)
{ 
    float xhalf = 0.5f*x;
    int i = *(int*)&x;
    i = 0x5f375a84 - (i>>1);
    x = *(float*)&i;
    
    x = x*(1.5f-xhalf*x*x);
    return 1.0f/x;
} 
