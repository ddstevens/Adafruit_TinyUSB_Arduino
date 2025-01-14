# STM32F401 "port" of Adafruit TinyUSB Library for Arduino

I wanted to use the Adafruit TinyUSB Arduino library for a project but it doesn't currently support STM MCUs.

This is my attempt to add support. Keep your expectations in check, it wasn't long ago I was learning how to blink a LED. I just implemented the methods outlined in the library section of their porting guide.

You have to modify one of the core Arduino files to build this library which will most likely **break STM's USB libraries** when used with TinyUSB.

To use copy this project to your local Arduino library directory and change the function signature `void OTG_FS_IRQHandler(void)` to `__weak void OTG_FS_IRQHandler(void)` in `cores/arduino/stm32/usb/usbd_conf.c` so it doesn't conflict with TinyUSB.

This port has the same limitations (and maybe more) as other cores without built-in support. I wasn't sure how to handle runtime DFU support so that's missing. Anyway see below for the limitations. Good luck.

# Adafruit TinyUSB Library for Arduino

[![Build Status](https://github.com/adafruit/Adafruit_TinyUSB_Arduino/workflows/Build/badge.svg)](https://github.com/adafruit/Adafruit_TinyUSB_Arduino/actions) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)

This library is a Arduino-friendly version of [TinyUSB](https://github.com/hathach/tinyusb) stack.
It is designed with structure and APIs that are easily integrated to an Arduino Core.

## Features

Currently only support device mode only, supported class drivers are:

- Communication (CDC): which is used to implement `Serial` monitor
- Human Interface Device (HID): Generic (In & Out), Keyboard, Mouse, Gamepad etc ...
- Mass Storage Class (MSC): with multiple LUNs
- Musical Instrument Digital Interface (MIDI)
- WebUSB with vendor specific class

## Supported Cores

There are 2 type of supported cores: with and without built-in support for TinyUSB. Built-in support provide seamless integration but requires extra code added to core's source code. Unfortunately it is not always easy or possible to make those modification.

### Cores with built-in support

Following core has TinyUSB as either the primary usb stack or selectable via menu `Tools->USB Stack`. You only need to include `<Adafruit_TinyUSB.h>` in your sketch to use.

- [adafruit/Adafruit_nRF52_Arduino](https://github.com/adafruit/Adafruit_nRF52_Arduino)
- [adafruit/ArduinoCore-samd](https://github.com/adafruit/ArduinoCore-samd)
- [earlephilhower/arduino-pico](https://github.com/earlephilhower/arduino-pico)
- [espressif/arduino-esp32](https://github.com/espressif/arduino-esp32)

  ESP32 port relies on Espressif's [esp32-hal-tinyusb.c](https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-tinyusb.c) for building usb descriptors which requires all descriptors must be specified in usb objects declaration i.e constructors. Therefore all descriptor-related fields must be part of object declaration and descriptor-related API have no effect afterwards for this port. 

### Cores without built-in support

Following is cores without built-in support

- **mbed_rp2040**

It is still possible to use TinyUSB but with some limits such as:

- `TinyUSB_Device_Init()` need to be manually called in setup()
- `TinyUSB_Device_Task()` and/or `TinyUSB_Device_FlushCDC()` may (or not) need to be manually called in loop()
- Use `SerialTinyUSB` name instead of Serial for serial monitor
- And there could be more other issues, using on these cores should be considered as experimental

## Class Driver API

More document to write ... 

## Porting Guide

To integrate TinyUSB library to a Arduino core, you will need to make changes to the core for built-in support and library for porting the mcu/platform.

### Arduino Core Changes

If possible, making changes to core will allow it to have built-in which make it almost transparent to user sketch 

1. Add this repo as submodule (or have local copy) at your ArduioCore/libraries/Adafruit_TinyUSB_Arduino (much like SPI).
2. Since Serial as CDC is considered as part of the core, we need to have `#include "Adafruit_USBD_CDC.h"` within your `Arduino.h`. For this to work, your `platform.txt` include path need to have `"-I{runtime.platform.path}/libraries/Adafruit_TinyUSB_Arduino/src/arduino"`.
3. In your `main.cpp` before setup() invoke the `TinyUSB_Device_Init(rhport)`. This will initialize usb device hardware and tinyusb stack and also include Serial as an instance of CDC class.
4. `TinyUSB_Device_Task()` must be called whenever there is new USB event. Depending on your core and MCU with or without RTOS. There are many ways to run the task. For example:
  - Use USB IRQn to set flag then invoke function later on after exiting IRQ.
  - Just invoke function after the loop(), within yield(), and delay()
5. `TinyUSB_Device_FlushCDC()` should also be called often to send out Serial data as well.
6. Note: For low power platform that make use of WFI()/WFE(), extra care is required before mcu go into low power mode. Check out my PR to circuipython for reference https://github.com/adafruit/circuitpython/pull/2956

### Library Changes

In addition to core changes, library need to be ported to your platform. Don't worry, tinyusb stack has already done most of heavy-lifting. You only need to write a few APIs

1. `TinyUSB_Port_InitDevice()` hardware specific (clock, phy) to enable usb hardware then call tud_init(). This API is called as part of TinyUSB_Device_Init() invocation.
2. `TinyUSB_Port_EnterDFU()` which is called when device need to enter DFU mode, usually by touch1200 feature
3. `TinyUSB_Port_GetSerialNumber()` which is called to get unique MCU Serial ID to used as Serial string descriptor.
