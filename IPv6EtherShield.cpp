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

extern "C" {
	#include "enc28j60.h"
	#include "uip.h"
	#include "uip-netif.h"
	#include "uip-nd6.h"
	#include "uip_arp.h"
	#include <string.h>	
}

#include "Arduino.h"
#include "IPv6EtherShield.h"
#include "arduino-debug.h"

struct ethip_hdr {
  struct uip_eth_hdr ethhdr;
  /* IP header */
  u8_t vhl,
    tos,
    len[2],
    ipid[2],
    ipoffset[2],
    ttl,
    proto;
  u16_t ipchksum;
  uip_ipaddr_t srcipaddr, destipaddr;
};

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
#define IPBUF ((struct ethip_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL 0
#endif /* NULL */

void (*tcp_processing_function)();

//constructor
IPv6EtherShield::IPv6EtherShield(){
    tcp_processing_function = 0;
}

void IPv6EtherShield::initENC28J60(uint8_t* macAddress){
    uint8_t counter;

    enc28j60Init(macAddress);

    enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz

    delay (10);

    // 6 times for IPv6 ;-)
    for (counter=0; counter<6; counter++) {

        /* Magjack leds configuration, see enc28j60 datasheet, page 11 */
        // LEDA=green LEDB=yellow

        // 0x880 is PHLCON LEDB=on, LEDA=on
        enc28j60PhyWrite (PHLCON, 0x880);
        delay (100);

        // 0x990 is PHLCON LEDB=off, LEDA=off
        enc28j60PhyWrite (PHLCON, 0x990);
        delay (100);
    }    

    // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
    enc28j60PhyWrite (PHLCON, 0x476);
    delay (100);
}

uint8_t ethOutput(uip_lladdr_t *lladdr) {
    /* Setup MAC address */
    if (lladdr == NULL) {
        (&BUF->dest)->addr[0] = 0x33;
        (&BUF->dest)->addr[1] = 0x33;
        (&BUF->dest)->addr[2] = IPBUF->destipaddr.u8[12];
        (&BUF->dest)->addr[3] = IPBUF->destipaddr.u8[13];
        (&BUF->dest)->addr[4] = IPBUF->destipaddr.u8[14];
        (&BUF->dest)->addr[5] = IPBUF->destipaddr.u8[15];
    }
    else {
        memcpy(&BUF->dest, lladdr, UIP_LLADDR_LEN);
    }
    memcpy(&BUF->src, &uip_lladdr, UIP_LLADDR_LEN);
    BUF->type = HTONS(UIP_ETHTYPE_IPV6);
    uip_len += sizeof(struct uip_eth_hdr);
 
    /* Pass the frame to the ethernet driver */
    // return enc28j60_frame_send(uip_len, (char *)uip_buf);
    enc28j60PacketSend(uip_len, uip_buf);
    return 0;
}

void IPv6EtherShield::initTCPIP(uint8_t* macAddress, void (*processingFunction)()) {
    uint8_t addr_pos;
    #if ARDUINO_DEBUG
    arduino_debug_init();
    #endif
    
    // copy MAC-address
    for (addr_pos = 0; addr_pos<6; addr_pos++) 
        uip_lladdr.addr[addr_pos] = macAddress[addr_pos];
    tcpip_set_outputfunc(ethOutput);
    tcp_processing_function = processingFunction;
    tcpip_init();
    clock_init();    
}

void IPv6EtherShield::addAddress(uint16_t addr0, uint16_t addr1, uint16_t addr2, uint16_t addr3, uint16_t addr4, uint16_t addr5, uint16_t addr6, uint16_t addr7) {
    uip_ipaddr_t ipv6_address;
    uip_ip6addr(&ipv6_address, addr0, addr1, addr2, addr3, addr4, addr5, addr6, addr7);
    uip_netif_addr_add(&ipv6_address, 64, 0, MANUAL);
    addr4 = 0;
    addr5 = 0;
    addr6 = 0;
    addr7 = 0;             
    uip_nd6_prefix_add(&ipv6_address, 64, 0);
}

void IPv6EtherShield::receivePacket() {
    uip_len = enc28j60PacketReceive(UIP_BUFSIZE, uip_buf);
}

uint8_t IPv6EtherShield::isIPv6Packet() {
    return (BUF->type == htons(UIP_ETHTYPE_IPV6));
}    

uint8_t IPv6EtherShield::getBuffer(uint16_t position) {
    return uip_buf[position];
}    

void IPv6EtherShield::processTCPIP() {
    return tcpip_input();
}    

void IPv6EtherShield::pollTimers() {
    etimer_poll();
}

void IPv6EtherShield::tcpListen(uint16_t port) {
    uip_listen(HTONS(port));
}    

uint8_t IPv6EtherShield::newDataAvailable() {
    return uip_newdata();
}    

uint16_t IPv6EtherShield::newDataLength() {
    return uip_datalen();
}

char* IPv6EtherShield::getNewData() {
    return (char *) uip_appdata;
}    

void IPv6EtherShield::sendData(char* data, int len) {
    uip_send(data, len);
}    

void IPv6EtherShield::closeConnection() {
    uip_close();
}    

uint8_t IPv6EtherShield::gotAck() {
    return uip_acked();
}    

// call function with previously set function pointer
void ipv6_ethershield_process_data() { 
  if (tcp_processing_function)
    (*tcp_processing_function)();
}


