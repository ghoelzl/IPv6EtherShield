#include "IPv6EtherShield.h"

// IPv6TelnetServer
//
// 2010, 2012 by Guenther Hoelzl
//
// The program implements the following commands: 
// get: read status of pin 3
// put0: set pin 3 to low
// put1: set pin 3 to high
// mem: print number of free RAM bytes
// exit: disconnect from client
//
// The circuit:
// LED connected from digital pin 3 to a 1k resistor and afterwards to ground.
//
// Note: Works on an Arduino Uno with an ATMega328 and 
// an ENC28J60 Ethershield
//
// Please modify the following line. The MAC-address has to be unique
// in your local area network.
static uint8_t mMAC[6] = {0x00,0x22,0x15,0x01,0x02,0x04};
static int ledPin = 3;

IPv6EtherShield ipv6ES = IPv6EtherShield();

void setup() {
  pinMode(ledPin, OUTPUT);
  // init network-device
  ipv6ES.initENC28J60(mMAC);
  ipv6ES.initTCPIP(mMAC, processIncomingData);  
  // add "Link Local Unicast" Address
  // for testing under Linux: ping6 -I eth0 fe80::1234
  ipv6ES.addAddress(0xfe80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1234);
  // add "Global Unicast Address"
  // for testing under Linux: ping6 2a00:eb0:100:15::1234
  ipv6ES.addAddress(0x2a00, 0xeb0, 0x100, 0x15, 0x00, 0x00, 0x00, 0x1234); 
  // telnet listen
  ipv6ES.tcpListen(23);    
}

//from http://www.arduino.cc/playground/Code/AvailableMemory
int availableMemory() {
  uint8_t * heapptr, * stackptr;
  stackptr = (uint8_t *)malloc(4);
  heapptr = stackptr;
  free(stackptr);               
  stackptr = (uint8_t *)(SP);
  return stackptr - heapptr;
} 

void processIncomingData() {  
  char *newData;  
  char message[20];
  int message_length;
  if (ipv6ES.newDataAvailable()) {
    newData = ipv6ES.getNewData();
    if (ipv6ES.newDataLength()>=4) {
      if (strncmp(newData, "get", 3) == 0) {
        if (digitalRead(ledPin) == LOW) 
          ipv6ES.sendData("LOW\n", 4);
        else  
          ipv6ES.sendData("HIGH\n", 5);        
      }
      else 
        if (strncmp(newData, "put0", 4) == 0) {
        ipv6ES.sendData("ok\n", 3);
        digitalWrite(ledPin, LOW);
      }
      else
        if (strncmp(newData, "put1", 4) == 0) {
          ipv6ES.sendData("ok\n", 3);
          digitalWrite(ledPin, HIGH);
        }
      else 
        if (strncmp(newData, "mem", 3) == 0) {
          message_length = sprintf(message, "%u bytes free\n", availableMemory());
          ipv6ES.sendData(message, message_length);
        }
      else 
        if (strncmp(newData, "exit", 4) == 0) {
          ipv6ES.closeConnection();
        }        
      else 
      {
        ipv6ES.sendData("IPv6TelnetServer\ncommands: get, put0, put1, mem, exit\n", 54);
      }  
    }  
  }    
}

void loop() {
  ipv6ES.receivePacket();
  if (ipv6ES.newDataLength() != 0) {
    if (ipv6ES.isIPv6Packet()) {
      ipv6ES.processTCPIP();
    }    
  }  
  ipv6ES.pollTimers();
}

