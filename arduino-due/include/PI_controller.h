/*
 * IncFile1.h
 *
 * Created: 12.11.2025 11:49:50
 *  Author: flaucht
 */ 



#ifndef PI_CONTROLLER_H_
#define PI_CONTROLLER_H_

typedef struct  
{
	float kp;
	float ki;
	
	float term_p;
	float term_i;
	
	float term_i_max;
	float term_i_min;
	
	float error_prev;
	float t_sample;
	
	float out_max;
	float out_min;
	
	float out;	
} PI_controller;

void	PI_init(PI_controller *pi);
float	PI_out(PI_controller *pi, float ref_value, float pos_current);


#endif /* PI_CONTROLLER_H_ */