#include <stdint.h>

#ifndef BLYNK_H_
#define BLYNK_H_

// ----------------------------------- TM4C_to_Blynk ------------------------------
// Send data to the Blynk App
// It uses Virtual Pin numbers between 70 and 99
// so that the ESP8266 knows to forward the data to the Blynk App
void TM4C_to_Blynk(uint32_t pin, uint32_t value);

#endif // BLYNK_H_