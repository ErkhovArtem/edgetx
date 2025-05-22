/*
 * Copyright (C) EdgeTX
 *
 * Based on code named
 *   opentx - https://github.com/opentx/opentx
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "hal/serial_port.h"
#include "hal/gpio.h"
#include "stm32_serial_driver.h"
#include "board.h"

#include "debug.h"
#include "lcd.h"

#if !defined(BOOT)
  #include "edgetx.h"
#endif

#define LCD_SERIAL_BAUDRATE 1000000 

// extern "C" void LCD_DMA_Stream_IRQHandler(void)
// {
//   stm32_usart_tx_dma_isr(&lcdUART);
// }

static void* _lcd_usart_ctx = nullptr;

void lcdInit()
{
  etx_serial_init cfg = {
    .baudrate = LCD_SERIAL_BAUDRATE,
    .encoding = ETX_Encoding_8N1,
    .direction = ETX_Dir_TX,
    .polarity = ETX_Pol_Normal,
  };

  _lcd_usart_ctx = STM32SerialDriver.init(auxSerialGetPort(0)->hw_def, &cfg);
}


void lcdOff()
{
  if (_lcd_usart_ctx) {
    STM32SerialDriver.deinit(_lcd_usart_ctx);
    _lcd_usart_ctx = nullptr;
  }
}

void lcdWriteCommand(uint8_t byte){
    if (!_lcd_usart_ctx) return;
    STM32SerialDriver.sendByte(_lcd_usart_ctx, byte);
}

void lcdRefresh(bool wait)
{
uint8_t * p = displayBuf;
  for (uint8_t y=0; y < 8; y++, p+=LCD_W) {
    lcdWriteCommand(0x10); // Column addr 0
    lcdWriteCommand(0xB0 | y); // Page addr y
    STM32SerialDriver.sendBuffer(_lcd_usart_ctx, (const uint8_t*)p, LCD_W);
  }
}

// Заглушки для совместимости (если требуются)
// void lcdInit() {}
// void lcdOff() {}
// void lcdRefresh(bool wait) {}
void lcdSetRefVolt(uint8_t val) {}
void lcdSetInvert(bool invert) {}