#include <stddef.h>

double
array_average(double *array,
              size_t  repetitions,
              size_t  length,
              size_t  offset,
              size_t  skip)
{
    size_t i;
    size_t j;

    double average = 0.0;

    for (i = 0; i < repetitions; i++) {
        for (j = 0; j < length; j++) {
            average +=
                array[(i *
                       (offset + length +
                        skip)) + offset + j] / (repetitions * length);
        }
    }

    return average;
}
