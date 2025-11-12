/*
 * PI_controller.c
 *
 * Created: 12.11.2025 11:49:39
 *  Author: flaucht
 */ 

#include "PI_controller.h"

void PI_init(PI_controller *pi){
	pi->term_i = 0;
	pi->term_p = 0;
	pi->error_prev = 0;
	pi->out = 0;
	
}

float PI_out(PI_controller *pi, float ref_value, float pos_current){
	float error = ref_value - pos_current;
	
	pi->term_p = pi->kp * error;
	pi->term_i = pi->term_i + 0.5 * pi->ki * pi->t_sample * (error + pi->error_prev);
	
	//printf("pi->term_i: %f;  pi->term_p: %f\n", pi->term_i, pi->term_p);
	
	// anti wind up of i_term
	if (pi->term_i > pi->term_i_max){
		pi->term_i = pi->term_i_max;
	}	
	if (pi->term_i < pi->term_i_min){
		pi->term_i = pi->term_i_min;
	} 
		
	pi->out = pi->term_p + pi->term_i;
	// keep output values in limits
	if (pi->out > pi->out_max){
		pi->out = pi->out_max;
	}
	if (pi->out < pi->out_min){
		pi->out = pi->out_min;
	}
			
	return pi->out;	
	pi->error_prev = error;
}