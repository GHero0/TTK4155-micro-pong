#include "tests/test_inter.h"
#include <avr/io.h>
#include <stdio.h>

void test_inter(void)
{
    if (GIFR & (1 << INTF1))
    {
        printf("Hello!");
    } else {
        printf("Hey\n");
    }
}
