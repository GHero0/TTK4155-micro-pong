#include "encoder.h"
#include "types.h"
#include "sam.h"

uint32_t hand_position = 0;

void Update_Hand_Position(void)
{
    hand_position = TC2->TC_CHANNEL[0].TC_CV;
}
