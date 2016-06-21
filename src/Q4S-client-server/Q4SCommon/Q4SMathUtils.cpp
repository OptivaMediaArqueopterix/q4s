#include "Q4SMathUtils.h"

#include <algorithm>

float EMathUtils_median( std::vector <unsigned long> &vector)
{
    float median = 0.f;

    std::vector <unsigned long> vectorToSort (vector);

    std::sort( vectorToSort.begin(), vectorToSort.end());

    int size = vectorToSort.size();

    // size is even
    if ( size%2 == 0 )
    {
        median = vectorToSort[size/2] + vectorToSort[(size/2) - 1] / 2.0f;
    }
    else
    {
        median = (float)vectorToSort[size/2];
    }
            
    return median;
}

unsigned long EMathUtils_mean( std::vector <unsigned long> &vector)
{
    unsigned long mean = 0;

    unsigned long total = 0;

    std::for_each(vector.begin(), vector.end(), [&](int element) {
        total += element;
    });

    mean = total/vector.size();
            
    return mean;
}
