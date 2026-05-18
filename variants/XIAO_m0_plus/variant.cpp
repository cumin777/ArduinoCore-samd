/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "variant.h"

/*
 * Pin descriptions for XIAO SAMD21 Plus
 *
 * Pin mapping derived from schematic: 202004620_XIAO SAMD21-Plus_V1.0
 * SAMD21G18A-MU (U5, QFN-48) -> XIAO-Add-On-Plus header (U3, 23-pin)
 *
 * SERCOM allocation:
 *   SERCOM0 -> SPI  (SCK=PA07/PAD3, MISO=PA05/PAD1, MOSI=PA06/PAD2)
 *   SERCOM1 -> I2C1 (SDA1=PA16/PAD0, SCL1=PA17/PAD1)
 *   SERCOM2 -> I2C0 (SDA0=PA08/PAD0, SCL0=PA09/PAD1)
 *   SERCOM4 -> UART (TX=PB08/PAD0, RX=PB09/PAD1)
 */
const PinDescription g_APinDescription[]=
{
  // 0..10 - Digital & Analog pins (same as old XIAO M0)
  // -----------------------------------------------
  // D0/A0 - PA02
  { PORTA,  2, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel0, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_2 },
  // D1/A1 - PA04
  { PORTA,  4, PIO_ANALOG, (PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel4, PWM0_CH0, TCC0_CH0, EXTERNAL_INT_4 },
  // D2/A2 - PA10
  { PORTA, 10, PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), ADC_Channel18, PWM0_CH2, TCC0_CH2, EXTERNAL_INT_10 },
  // D3/A3 - PA11
  { PORTA, 11, PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), ADC_Channel19, PWM0_CH3, TCC0_CH3, EXTERNAL_INT_11 },

  // 4..5 - I2C0 on SERCOM2 (SDA0/SCL0)
  // -----------------------------------------------
  // D4/A4/SDA0 - PA08: SERCOM2/PAD[0]
  { PORTA,  8, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), ADC_Channel16, PWM1_CH2, TCC1_CH2, EXTERNAL_INT_NMI },
  // D5/A5/SCL0 - PA09: SERCOM2/PAD[1]
  { PORTA,  9, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), ADC_Channel17, PWM1_CH3, TCC1_CH3, EXTERNAL_INT_9 },

  // 6..7 - UART on SERCOM4
  // -----------------------------------------------
  // D6/A6/TX - PB08: SERCOM4/PAD[0]
  { PORTB,  8, PIO_SERCOM_ALT, (PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel2, PWM4_CH0, TC4_CH0, EXTERNAL_INT_8 },
  // D7/A7/RX - PB09: SERCOM4/PAD[1]
  { PORTB,  9, PIO_SERCOM_ALT, (PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel3, PWM4_CH1, TC4_CH1, EXTERNAL_INT_9 },

  // 8..10 - SPI on SERCOM0
  // -----------------------------------------------
  // D8/A8/SCK - PA07: SERCOM0/PAD[3]
  { PORTA,  7, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel7, PWM1_CH1, TCC1_CH1, EXTERNAL_INT_7 },
  // D9/A9/MISO - PA05: SERCOM0/PAD[1]
  { PORTA,  5, PIO_SERCOM_ALT, (PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel5, PWM0_CH1, TCC0_CH1, EXTERNAL_INT_5 },
  // D10/A10/MOSI - PA06: SERCOM0/PAD[2]
  { PORTA,  6, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel6, PWM1_CH0, TCC1_CH0, EXTERNAL_INT_6 },

  // 11 - RGB LED (WS2812B)
  // -----------------------------------------------
  // D11/RGB - PA27
  { PORTA, 27, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },

  // 12 - GPIO
  // -----------------------------------------------
  // D12 - PA28
  { PORTA, 28, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },

  // 13..14 - I2C1 on SERCOM1
  // -----------------------------------------------
  // D13/SCL1 - PA17: SERCOM1/PAD[1]
  { PORTA, 17, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM2_CH1, TCC2_CH1, EXTERNAL_INT_1 },
  // D14/SDA1 - PA16: SERCOM1/PAD[0]
  { PORTA, 16, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM2_CH0, TCC2_CH0, EXTERNAL_INT_NONE },

  // 15..18 - Additional GPIOs
  // -----------------------------------------------
  // D15 - PA15 (TCC3/WO[3])
  { PORTA, 15, PIO_TIMER_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), No_ADC_Channel, PWM3_CH3, TC3_CH3, EXTERNAL_INT_15 },
  // D16 - PA14 (TCC3/WO[2])
  { PORTA, 14, PIO_TIMER_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), No_ADC_Channel, PWM3_CH2, TC3_CH2, EXTERNAL_INT_14 },
  // D17 - PA13 (TCC2/WO[1])
  { PORTA, 13, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM2_CH1, TCC2_CH1, EXTERNAL_INT_13 },
  // D18 - PA12 (TCC2/WO[0])
  { PORTA, 12, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM2_CH0, TCC2_CH0, EXTERNAL_INT_12 },

  // 19..21 - I2S
  // -----------------------------------------------
  // D19/I2S_SD - PA19
  { PORTA, 19, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM3_CH1, TC3_CH1, EXTERNAL_INT_3 },
  // D20/I2S_SCK - PA20
  { PORTA, 20, PIO_TIMER_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), No_ADC_Channel, PWM0_CH4, TCC0_CH4, EXTERNAL_INT_4 },
  // D21/I2S_WS - PA21
  { PORTA, 21, PIO_TIMER_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER_ALT), No_ADC_Channel, PWM0_CH5, TCC0_CH5, EXTERNAL_INT_5 },

  // 22..23 - Additional GPIOs
  // -----------------------------------------------
  // D22 - PB10 (TC5)
  { PORTB, 10, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM5_CH0, TC5_CH0, EXTERNAL_INT_10 },
  // D23 - PB11 (TC5)
  { PORTB, 11, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM5_CH1, TC5_CH1, EXTERNAL_INT_11 },

  // 24..27 - More GPIOs
  // -----------------------------------------------
  // D24 - PB23
  { PORTB, 23, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_7 },
  // D25 - PA23
  { PORTA, 23, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_7 },
  // D26 - PA22
  { PORTA, 22, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_6 },
  // D27 - PA18 (TC3)
  { PORTA, 18, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM3_CH0, TC3_CH0, EXTERNAL_INT_2 },

  // 28 - User Button (PB22, active low)
  // -----------------------------------------------
  { PORTB, 22, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_6 },

  // 29..30 - USB
  // -----------------------------------------------
  // USB_DM - PA24
  { PORTA, 24, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },
  // USB_DP - PA25
  { PORTA, 25, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }
} ;

const void* g_apTCInstances[TCC_INST_NUM+TC_INST_NUM]={ TCC0, TCC1, TCC2, TC3, TC4, TC5 } ;

// Multi-serial objects instantiation
SERCOM sercom0( SERCOM0 ) ;
SERCOM sercom1( SERCOM1 ) ;
SERCOM sercom2( SERCOM2 ) ;
SERCOM sercom3( SERCOM3 ) ;
SERCOM sercom4( SERCOM4 ) ;
SERCOM sercom5( SERCOM5 ) ;

#ifndef NO_USART_INTERFACE

Uart Serial1( &sercom4, PIN_SERIAL1_RX, PIN_SERIAL1_TX, PAD_SERIAL1_RX, PAD_SERIAL1_TX ) ;

void SERCOM4_Handler()
{
  Serial1.IrqHandler();
}

#endif