/*
 * motor_driver.h
 *
 * Created: 08.11.2025 11:09:36
 *  Author: flaucht
 */ 


#ifndef MOTOR_DRIVER_H_
#define MOTOR_DRIVER_H_

void motor_init(void);
void motor_move(int speed, int dir);
void motor_move_left(int speed);
void motor_move_right(int speed);
void motor_move_for_time(int speed, int dir, int onTime);
void motor_stop(void);



#endif /* MOTOR_DRIVER_H_ */