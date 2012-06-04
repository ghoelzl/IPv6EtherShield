#include <HardwareSerial.h>
#include "uip.h"
#include "arduino-debug.h"

void arduino_debug_init() {
    Serial.begin(9600);
}

void arduino_debug(char* s) {
    Serial.println(s);
}

void arduino_debug_hex(int b) {
    Serial.println(b, HEX);
}
    
void arduino_debug_address(uip_ip6addr_t* address) {
    int i;
    for (i=0; i<16; i++) {    
      if (address->u8[i]<0xf) 
        Serial.print("0");   
      Serial.print(address->u8[i], HEX);
      if (((i%2) == 1) && (i != 15))
        Serial.print(":");
    }    
    Serial.println();  
}

    
    
