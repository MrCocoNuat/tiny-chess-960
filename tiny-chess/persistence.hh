#pragma once

#include <Arduino.h>

void put(uint8_t address, uint16_t content);
uint16_t get(uint8_t address);
