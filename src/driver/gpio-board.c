/*!
 * \file      gpio-board.c
 *
 * \brief     Target board GPIO driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#if defined(WIFI_LORA_32_V3)||defined(WIFI_LORA_32_V2)||defined(WIRELESS_TRACKER)||defined(WIRELESS_STICK_V3)||defined(WIRELESS_STICK_LITE_V3)||defined(WIRELESS_PAPER)||defined(CAPSULE_SENSOR_V3)||defined(WIRELESS_SHELL_V3)


#include "../loramac/utilities.h"
#include "../driver/rtc-board.h"
#include "../driver/gpio-board.h"
#include "../driver/gpio-board.h"
#include "board-config.h"

static GpioIrqHandler *GpioIrq[16];

void GpioMcuInit( Gpio_t *obj, uint8_t pin, uint8_t mode, PinConfigs config, PinTypes type, uint32_t value )
{
	obj->pin = pin;
	pinMode(pin,mode);
    if( mode == OUTPUT )
    {
        GpioMcuWrite( obj, value );
    }
}

bool radio_nss_inited=false;
void GpioMcuSetInterrupt( Gpio_t *obj, int irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
	attachInterrupt(obj->pin,irqHandler,RISING);
}

void GpioMcuRemoveInterrupt( Gpio_t *obj )
{
	detachInterrupt(obj->pin);
}

void GpioMcuWrite( Gpio_t *obj, uint32_t value )
{
	if(obj->pin==RADIO_NSS && radio_nss_inited==false)
	{
		pinMode(obj->pin,OUTPUT);
		radio_nss_inited=true;
	}
	digitalWrite(obj->pin,value);
}

void GpioMcuToggle( Gpio_t *obj )
{

}

uint32_t GpioMcuRead( Gpio_t *obj )
{
	return digitalRead(obj->pin);
}

#endif