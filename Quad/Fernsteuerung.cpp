#include "Fernsteuerung.h"
#include "ch.h"
#include "hal.h"

#define RC_IN_RANGE(x) (((x)>900 && (x)<2300))
volatile unsigned short RC_INPUT_CHANNELS[4], RC_INPUT_LAST_TCNT,tmp=0;
char PPM_FRAME_GOOD = 1;

/*
 *  _____       _                             _
 * |_   _|     | |                           | |
 *   | |  _ __ | |_ ___ _ __ _ __ _   _ _ __ | |_ ___
 *   | | | '_ \| __/ _ \ '__| '__| | | | '_ \| __/ __|
 *  _| |_| | | | ||  __/ |  | |  | |_| | |_) | |_\__ \
 * |_____|_| |_|\__\___|_|  |_|   \__,_| .__/ \__|___/
 *                                     | |
 *                                     |_|
 */
void rx_channel1_interrupt(EXTDriver *extp, expchannel_t channel) {
		(void)extp;
		(void)channel;

		chSysLockFromIsr();
		if (palReadPad(GPIOE, 10) == PAL_LOW) {
			unsigned short tmp = TIM4->CNT - RC_INPUT_LAST_TCNT;
			if (RC_IN_RANGE(tmp)) RC_INPUT_CHANNELS[0] = tmp;
		}
		RC_INPUT_LAST_TCNT = TIM4->CNT;
		chSysUnlockFromIsr();
}

void rx_channel2_interrupt(EXTDriver *extp, expchannel_t channel) {
		(void)extp;
		(void)channel;

		chSysLockFromIsr();
		if (palReadPad(GPIOE, 11) == PAL_LOW) {
			unsigned short tmp = TIM4->CNT - RC_INPUT_LAST_TCNT;
			if (RC_IN_RANGE(tmp)) 
			{
				RC_INPUT_CHANNELS[1] = tmp;
			}
		}
		RC_INPUT_LAST_TCNT = TIM4->CNT;
		chSysUnlockFromIsr(); 		  
}
void rx_channel3_interrupt(EXTDriver *extp, expchannel_t channel) {
		(void)extp;
		(void)channel;

		chSysLockFromIsr();
		if (palReadPad(GPIOE, 12) == PAL_LOW) {
			unsigned short tmp = TIM4->CNT - RC_INPUT_LAST_TCNT;
			if (RC_IN_RANGE(tmp)) 
			{
				RC_INPUT_CHANNELS[2] = tmp;
			}
		}
		RC_INPUT_LAST_TCNT = TIM4->CNT;
		chSysUnlockFromIsr(); 
}
void rx_channel4_interrupt(EXTDriver *extp, expchannel_t channel) {
		(void)extp;
		(void)channel;

		chSysLockFromIsr();
		if (palReadPad(GPIOE, 13) == PAL_LOW) {
			unsigned short tmp = TIM4->CNT - RC_INPUT_LAST_TCNT;
			if (RC_IN_RANGE(tmp)) 
			{
				RC_INPUT_CHANNELS[3] = tmp;
			}
		}
		RC_INPUT_LAST_TCNT = TIM4->CNT;
		chSysUnlockFromIsr();	  
}
static const EXTConfig extcfg = {
	{
   	 	{EXT_CH_MODE_DISABLED, NULL},
    	{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
    	{EXT_CH_MODE_DISABLED, NULL},
    	{EXT_CH_MODE_DISABLED, NULL},
    	{EXT_CH_MODE_DISABLED, NULL},
    	{EXT_CH_MODE_DISABLED, NULL},
    	{EXT_CH_MODE_DISABLED, NULL},
	    {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART, rx_channel1_interrupt},
   	 	{EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART, rx_channel2_interrupt},
		{EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART, rx_channel3_interrupt},
		{EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART, rx_channel4_interrupt},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
	},
	EXT_MODE_EXTI(0, 	/* 0 */
	              0, 	/* 1 */
	              0,			 	/* 2 */
	              0, 				/* 3 */
	              0, 				/* 4 */
	              0, 				/* 5 */
	              0, 				/* 6 */
	              0, 				/* 7 */
	              0, 				/* 8 */
	              0, 				/* 9 */
	              EXT_MODE_GPIOE, 				/* 10 */
	              EXT_MODE_GPIOE, 				/* 11 */
	              EXT_MODE_GPIOE, 	/* 12 */
	              EXT_MODE_GPIOE, 	/* 13 */
	              0, /* 14 */
	              0) /* 15 */
};

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup_Fernsteuerung() 
{
	/*
	* Enable Timer 4
	*/
	TIM4->CR1 = 0x00000000;
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->SMCR = 0; // slave mode disabled
	TIM4->PSC = 84; // 84 mhz maximum apb1 bus speed
	TIM4->ARR = 0xffff;
	TIM4->SR = 0;
	TIM4->DIER = 0;
	TIM4->CR1 = 0x00000001;

	extStart(&EXTD1, &extcfg); 
}

float getNick() {return((float(RC_INPUT_CHANNELS[1]) - 1500)/1000);}
float getRoll() {return((float(RC_INPUT_CHANNELS[0]) - 1500)/1000);}
float getSchub() {return((float(RC_INPUT_CHANNELS[2]) - 1100)/1000);}
float getYaw() {return((float(RC_INPUT_CHANNELS[3]) - 1500)/1000);}
