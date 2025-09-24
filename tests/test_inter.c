#include "tests/test_inter.h"
#include <avr/io.h>
#include <stdio.h>

void test_inter(void)
{
    if (INTF1 == 1)
    {
        printf("Hello!");
    } else {
        printf("Hey\n");
    }
}
