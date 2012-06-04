/*
  IPv6EtherShield library for "ENC28J60 Arduino Ethernet Shield" 
  from http://www.ekitszone.com

  We use the uipv6 network stack from http://www.sics.se/contiki/.
  The ENC28J60 library is from Guido Socher.
  
  The library was isolated by http://www.shapeshifter.se/code/uipv6/ 
  and ported to the Arduino Platform by Guenther Hoelzl, 
  see http://sites.google.com/site/ghoelzl/  

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.
*/

#ifndef IPV6ETHERSHIELD_H
#define IPV6ETHERSHIELD_H

#ifdef __cplusplus

extern "C" {
    #include <inttypes.h>
    #include "uip.h"
}    

class IPv6EtherShield
{
  public:
    
    IPv6EtherShield();
	void initENC28J60(uint8_t* macAddress);
    void initTCPIP(uint8_t* macAddress, void (*processingFunction)());
    void addAddress(uint16_t addr0, uint16_t addr1, uint16_t addr2, uint16_t addr3, uint16_t addr4, uint16_t addr5, uint16_t addr6, uint16_t addr7);
    void receivePacket();    
    uint8_t isIPv6Packet();
    uint8_t getBuffer(uint16_t position);
    void processTCPIP();    
    void pollTimers();
    void tcpListen(uint16_t port);
    uint8_t newDataAvailable();     
    uint16_t newDataLength();
    char* getNewData();
    void sendData(char* data, int len); 
    void closeConnection();
    uint8_t gotAck();
};	

#endif

extern void ipv6_ethershield_process_data();
		
#endif

