/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, hathach for Adafruit
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "stm32f4xx_hal.h"
#include "tusb_option.h"
#include "tusb.h"

#if defined(ARDUINO_ARCH_STM32) && TUSB_OPT_DEVICE_ENABLED

#include <stdint.h>

//--------------------------------------------------------------------+
// Forward USB interrupt events to TinyUSB IRQ Handler
//--------------------------------------------------------------------+
extern "C" void OTG_FS_IRQHandler(void)
{
  tud_int_handler(0);
}

extern "C" void OTG_HS_IRQHandler(void)
{
  tud_int_handler(1);
}

void TinyUSB_Port_EnterDFU(void) {}

void TinyUSB_Port_InitDevice(uint8_t rhport) {
  (void)rhport;
  tusb_init();
}

uint8_t TinyUSB_Port_GetSerialNumber(uint8_t serial_id[16]) {
  (void)serial_id;
  return 7;
}

#endif
