#ifndef LISEMMATH_H
#define LISEMMATH_H

#include <math.h>


class LISEMMath
{
public:
    LISEMMath();

    /* Utility to swap to integers */
    static void swap(int *a, int *b)
    {
        int temp;
        temp = *a;
        *a = *b;
        *b = temp;
    }

    /* Utility function that puts all
    non-positive (0 and negative) numbers on left
    side of arr[] and return count of such numbers */
    static int segregate (int arr[], int size)
    {
        int j = 0, i;
        for(i = 0; i < size; i++)
        {
            if (arr[i] <= 0)
            {
                swap(&arr[i], &arr[j]);
                j++; // increment count of non-positive integers
            }
        }

        return j;
    }

    /* Find the smallest positive missing number
    in an array that contains all positive integers */
    static int findMissingPositive(int arr[], int size)
    {
        int i;

        // Mark arr[i] as visited by making arr[arr[i] - 1] negative.
        // Note that 1 is subtracted because index start
        // from 0 and positive numbers start from 1
        for(i = 0; i < size; i++)
        {
            if(abs(arr[i]) - 1 < size && arr[ abs(arr[i]) - 1] > 0)
            {
                arr[ abs(arr[i]) - 1] = -arr[ abs(arr[i]) - 1];
            }
        }

        // Return the first index value at which is positive
        for(i = 0; i < size; i++)
        {
            if (arr[i] > 0)
            {
                // 1 is added because indexes start from 0
                return i+1;
            }
        }

        return size+1;
    }

    /* Find the smallest positive missing
    number in an array that contains
    both positive and negative integers */
    static int findMissing(int arr[], int size)
    {
        // First separate positive and negative numbers
        int shift = segregate (arr, size);

        // Shift the array and call findMissingPositive for
        // positive part
        return findMissingPositive(arr + shift, size - shift);
    }

    // t is a value that goes from 0 to 1 to interpolate in a C1 continuous way across uniformly sampled data points.
    // when t is 0, this will return B.  When t is 1, this will return C.  Inbetween values will return an interpolation
    // between B and C.  A and B are used to calculate slopes at the edges.
    static float CubicHermite (float A, float B, float C, float D, float t)
    {
        float a = -A / 2.0f + (3.0f*B) / 2.0f - (3.0f*C) / 2.0f + D / 2.0f;
        float b = A - (5.0f*B) / 2.0f + 2.0f*C - D / 2.0f;
        float c = -A / 2.0f + C / 2.0f;
        float d = B;

        return a*t*t*t + b*t*t + c*t + d;
    }
};




inline float absMax(float a, float b)
{
    a = std::fabs(a);
    if(std::fabs(b) < a)
    {
        return b<0.0? -a:a;
    }
    return b;
}
inline float absMin(float a, float b)
{
    return abs(a) < abs(b) ? a : b;
}

inline float negSign(float v)
{
    return v < 0.0 ? -1.0 : 1.0;
}



#endif // LISEMMATH_H
