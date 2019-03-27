/* Used to time matrix mulitplication operation
    Authors: Hwei-Shin Harriman, Enmo Ren, and Cassandra Overney
    Adapted from How to Optimize Gemm Tutorial
      created by Prof. Robert van de Geijn and Jianyu Huang
*/

#include <sys/time.h>
#include <time.h>

static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

double dclock()
{
        double         the_time, norm_sec;
        struct timeval tv;

        gettimeofday( &tv, NULL );

        if ( gtod_ref_time_sec == 0.0 )
                gtod_ref_time_sec = ( double ) tv.tv_sec;

        norm_sec = ( double ) tv.tv_sec - gtod_ref_time_sec;

        the_time = norm_sec + tv.tv_usec * 1.0e-6;

        return the_time;
}
