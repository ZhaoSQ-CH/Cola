#include "image_debug.h"

#include <Arduino.h>

void image_debug_init(void) {
    Serial.begin(115200);
    printf("image debug start ... \n\r");
}