#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {
    sf_mem_init();

    double *ptr4 = sf_malloc(32);
    double* ptr = sf_malloc(32);
    //sf_show_heap();
    //sf_free(ptr);
    double* ptr1 = sf_malloc(32);
    //sf_show_heap();
    double* ptr2 = sf_malloc(32);
    //sf_show_heap();
    double* ptr3 = sf_malloc(32);
    double* ptr5 = sf_malloc(32);
    sf_show_heap();
    /*ptr = sf_malloc(32);
    sf_show_heap();*/

    // *ptr1 = 320320320e-320;

    // printf("%f\n", *ptr);
    sf_free(ptr4);
     //sf_show_heap();
    sf_free(ptr);
    //sf_show_heap();
    sf_free(ptr1);
    ///sf_show_heap();
    sf_free(ptr2);
    //sf_show_heap();
    sf_free(ptr3);
    sf_show_heap();
    sf_free(ptr5);
    sf_show_heap();

    sf_mem_fini();

    return EXIT_SUCCESS;
}
