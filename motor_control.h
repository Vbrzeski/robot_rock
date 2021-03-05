/*
 * PWM.h
 *
 *  Created on: Oct 28, 2014
 *      Author: User
 */

#ifndef PWM_H_
#define PWM_H_
/*
 * @brief : This function initializes the PWM generator for use on
 * ECE4437 robot
 * @param : none
 * @return : none
 */
void Board_PWM_init(void);
/*
 * @brief : This function start motors forward
 * @param : unsigned int duty - Duty cycle desired
 * @return : none
 */
void fw_motors(unsigned int duty);
/*
 * @brief : This function start motors reverse
 * @param : unsigned int duty - Duty cycle desired
 * @return : none
 */
void rv_motors(unsigned int duty);
/*
 * @brief : This function rotates robot clockwise
 * @param : unsigned int duty - Duty cycle desired
 * @return : none
 */
void rr_motors(unsigned int duty);
/*
 * @brief : This function rotates robot counter clockwise
 * @param : unsigned int duty - Duty cycle desired
 * @return : none
 */
void rl_motors(unsigned int duty);
#endif /* PWM_H_ */
