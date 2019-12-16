#ifdef MGOSMYLIB_H
#define MGOSMYLIB_H

#include <mgos.h>
#include <Arduino.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>

bool mgos_mgosmylib_init(void);

#endif