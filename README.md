IPv6EtherShield - IPv6 for the Arduino platform
===============================================

This library works with [ENC28J60](http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en022889) based Ethernet Shields like that from [Electronic Kits Zone](http://www.ekitszone.com).

The library is a port of the [uipv6 network stack](http://www.sics.se/contiki/). The [ENC28J60](http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en022889) library is from Guido Socher. The uipv6 library was isolated by [Shapeshifter](http://www.shapeshifter.se/code/uipv6/).

I wrote an additional wrapper class with initialization methods, so the sketch files should be pretty simple. Included in the library you will find the following examples:

* IPv6TelnetServer, where you can put/get the status of an attached LED and where you can get information about the free RAM bytes.
* IPv6WebServer, where you can get the current values of the analog input pins.

The library should be copied into the libraries folder of the Arduino software. After that you can find the above sketches in the "Examples" section.
