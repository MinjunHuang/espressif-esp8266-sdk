/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_light.c
 *
 * Description: light demo's function realization
 *
 * Modification history:
 *     2014/5/1, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

#include "user_light.h"

#if LIGHT_DEVICE

static struct light_saved_param light_param;

/******************************************************************************
 * FunctionName : user_light_get_duty
 * Description  : get duty of each channel
 * Parameters   : uint8_t channel : LIGHT_RED/LIGHT_GREEN/LIGHT_BLUE
 * Returns      : NONE
*******************************************************************************/
uint8_t ICACHE_FLASH_ATTR
user_light_get_duty(uint8_t channel)
{
    return light_param.pwm_duty[channel];
}

/******************************************************************************
 * FunctionName : user_light_set_duty
 * Description  : set each channel's duty params
 * Parameters   : uint8_t duty    : 0 ~ PWM_DEPTH
 *                uint8_t channel : LIGHT_RED/LIGHT_GREEN/LIGHT_BLUE
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
user_light_set_duty(uint8_t duty, uint8_t channel)
{
    if (duty != light_param.pwm_duty[channel]) {
        pwm_set_duty(duty, channel);

        light_param.pwm_duty[channel] = pwm_get_duty(channel);
    }
}

/******************************************************************************
 * FunctionName : user_light_get_freq
 * Description  : get pwm frequency
 * Parameters   : NONE
 * Returns      : uint16_t : pwm frequency
*******************************************************************************/
uint16_t ICACHE_FLASH_ATTR
user_light_get_freq(void)
{
    return light_param.pwm_freq;
}

/******************************************************************************
 * FunctionName : user_light_set_duty
 * Description  : set pwm frequency
 * Parameters   : uint16_t freq : 100hz typically
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
user_light_set_freq(uint16_t freq)
{
    if (freq != light_param.pwm_freq) {
        pwm_set_freq(freq);

        light_param.pwm_freq = pwm_get_freq();
    }
}

void ICACHE_FLASH_ATTR
user_light_restart(void)
{
	spi_flash_erase_sector(PRIV_PARAM_START_SEC + PRIV_PARAM_SAVE);
	spi_flash_write((PRIV_PARAM_START_SEC + PRIV_PARAM_SAVE) * SPI_FLASH_SEC_SIZE,
	    		(uint32_t *)&light_param, sizeof(struct light_saved_param));

	pwm_start();
}

/******************************************************************************
 * FunctionName : user_light_init
 * Description  : light demo init, mainy init pwm
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
user_light_init(void)
{
    spi_flash_read((PRIV_PARAM_START_SEC + PRIV_PARAM_SAVE) * SPI_FLASH_SEC_SIZE,
    		(uint32_t *)&light_param, sizeof(struct light_saved_param));

    pwm_init(light_param.pwm_freq, light_param.pwm_duty);
}
#endif

