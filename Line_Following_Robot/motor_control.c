/*
 * PWM.c
 *
 *  Created on: Oct 28, 2014
 *      Author: VBrzeski
 */
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_pwm.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"

#define switchingfreq 50
void Board_PWM_init(void)
{
	/* Set clock for PWM */
	SysCtlPWMClockSet(SYSCTL_PWMDIV_8);
	/* Enable PWM on PWM0 */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
	/* Set PWM output on PB6, and PB7 */
	GPIOPinTypePWM(GPIO_PORTB_BASE, (GPIO_PIN_6|GPIO_PIN_7));
	/* Set PWM Pins for output or input */
	GPIOPinConfigure(GPIO_PB6_M0PWM0);
	GPIOPinConfigure(GPIO_PB7_M0PWM1);
	/* Calculate Period */
	uint32_t period = (SysCtlPWMClockGet())/switchingfreq;
	/* Configure PWM Generators */
	PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN);
	PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_UP_DOWN);
	/* Set period of PWM module */
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0,period);
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1,period);
	/* Set Initial DC to 0% */
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 0);
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 0);
	/* Enable PWM Output */
	PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
	PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
	PWMGenEnable(PWM0_BASE, PWM_GEN_0);
	PWMGenEnable(PWM0_BASE, PWM_GEN_1);

	/* Unlock PTD_PIN7 from NMI Function */
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;
	HWREG(GPIO_PORTD_BASE + GPIO_O_AFSEL) &= ~0x80;
	HWREG(GPIO_PORTD_BASE + GPIO_O_DEN) |= 0x80;
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;

	/* Set GPIO pins for output */
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);


}
void fw_motors(unsigned int duty)
{
	/* calculate Load value */
	uint32_t pwm_clk = SysCtlPWMClockGet();
	uint32_t load = (pwm_clk / switchingfreq) - 1;
	/* Set duty cycle on both motors */
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, ((duty * load)/100));
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ((duty * load)/100));
	/* Set PTA_3, and PTD_6 high, for forward movement */
	GPIOPinWrite(GPIO_PORTC_BASE, (GPIO_PIN_6 | GPIO_PIN_7),GPIO_PIN_7);
	GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_6|GPIO_PIN_7),GPIO_PIN_6);
}
void rv_motors(unsigned int duty)
{
	/* calculate Load value */
	uint32_t pwm_clk = SysCtlPWMClockGet();
	uint32_t load = (pwm_clk / switchingfreq) - 1;
	/* Set duty cycle on both motors */
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, ((duty * load)/100));
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ((duty * load)/100));
	/* Set PTA_2, and PTD_7 high, for reverse movement */
	GPIOPinWrite(GPIO_PORTC_BASE, (GPIO_PIN_6 | GPIO_PIN_7),GPIO_PIN_7);
	GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_6|GPIO_PIN_7),GPIO_PIN_7);
}
void rr_motors(unsigned int duty)
{
	/* calculate Load value */
	uint32_t pwm_clk = SysCtlPWMClockGet();
	uint32_t load = (pwm_clk / switchingfreq) - 1;
	/* Set duty cycle on both motors */
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, ((duty * load)/100));
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ((duty * load)/100));
	/* Set PTA_2, and PTD_6 high, for rotate right movement */
	GPIOPinWrite(GPIO_PORTC_BASE, (GPIO_PIN_6 | GPIO_PIN_7),GPIO_PIN_6);
	GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_6|GPIO_PIN_7),GPIO_PIN_6);
}
void rl_motors(unsigned int duty)
{
	/* calculate Load value */
	uint32_t pwm_clk = SysCtlPWMClockGet();
	uint32_t load = (pwm_clk / switchingfreq) - 1;
	/* Set duty cycle on both motors */
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, ((duty * load)/100));
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ((duty * load)/100));
	/* Set PTA_3, and PTD_7 high, for rotate left movement */
	GPIOPinWrite(GPIO_PORTC_BASE, (GPIO_PIN_6 | GPIO_PIN_7),GPIO_PIN_7);
	GPIOPinWrite(GPIO_PORTD_BASE,(GPIO_PIN_6|GPIO_PIN_7),GPIO_PIN_7);
}

