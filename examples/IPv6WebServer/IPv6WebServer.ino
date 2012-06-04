#include "IPv6EtherShield.h"

// IPv6WebServer
//
// 2010, 2012 by Guenther Hoelzl
//
// The program implements a simple IPv6 http-server
// outputs the current values of the analog input pins
//
// Note: Works on an Arduino Uno with an ATMega328 and 
// an ENC28J60 Ethershield
//
// Please modify the following line. The MAC-address has to be unique
// in your local area network.
static uint8_t mMAC[6] = {0x00,0x22,0x15,0x01,0x02,0x04};

IPv6EtherShield ipv6ES = IPv6EtherShield();

#define WAITING_FOR_REQUEST 0
#define HTTP_OK_SENT        1
#define HEADER_SENT         2
#define DATA_SENT           3

char httpState = WAITING_FOR_REQUEST;
char sendingDataLine;

void setup() {
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
  ipv6ES.tcpListen(80);    
}

void processIncomingData() { 
  char* replyString;
  char* newData;  
  
  switch (httpState) {
    case WAITING_FOR_REQUEST:
      if (ipv6ES.newDataAvailable()) {
        newData = ipv6ES.getNewData();
        // if we got to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so we can send a reply
        boolean currentLineIsBlank = true;
        while (*newData != 0) {
          if (*newData == '\n' && currentLineIsBlank) {      
            replyString = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
            ipv6ES.sendData(replyString, strlen(replyString)); 
            httpState++;
            break;
          }          
          if (*newData == '\n') {
            // we're starting a new line
            currentLineIsBlank = true;
          } else if (*newData != '\r') {
            // we've gotten a character on the current line
            currentLineIsBlank = false;
          }                  
          newData++;
        }
      }
      break;
      
    case HTTP_OK_SENT:
      replyString = "<p><h1>Welcome to <a href=http://sites.google.com/site/"
                     "ghoelzl/ipv6ethershield>IPv6EtherShield</a></h1></p>\r\n";
      ipv6ES.sendData(replyString, strlen(replyString));
      httpState++;
      sendingDataLine = 0;
      break;
      
    case HEADER_SENT:
      // send the values of all analog pins
      char data[40];
      sprintf(data, "Analog pin %i has value %i\r\n<br>\r\n", 
              sendingDataLine, analogRead(sendingDataLine));
      ipv6ES.sendData(data, strlen(data));
      if (++sendingDataLine == 6) 
        httpState++;
      break;
      
    default: 
      // let http-client time to process data
      delay(1);
      httpState = WAITING_FOR_REQUEST;
      ipv6ES.closeConnection();
      break;
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

