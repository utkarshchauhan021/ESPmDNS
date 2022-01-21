/*
 IPv6Address.h - Base class that provides IPv6Address
 Copyright (c) 2011 Adrian McEwen.  All right reserved.
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef IPv6Address_h
#define IPv6Address_h

#include <stdint.h>
#include <WString.h>
#include <Printable.h>

// A class to make it easier to handle and pass around IP addresses

class IPv6Address: public Printable
{
private:
    union {
        uint8_t bytes[16];  // IPv4 address
        uint32_t dword[4];
    } _address;

    // Access the raw byte array containing the address.  Because this returns a pointer
    // to the internal structure rather than a copy of the address this function should only
    // be used when you know that the usage of the returned uint8_t* will be transient and not
    // stored.
    uint8_t* raw_address()
    {
        return _address.bytes;
    }

public:
    // Constructors
    IPv6Address();
    IPv6Address(const uint8_t *address);
    IPv6Address(const uint32_t *address);
    virtual ~IPv6Address() {}

    bool fromString(const char *address);
    bool fromString(const String &address) { return fromString(address.c_str()); }

    operator const uint8_t*() const
    {
        return _address.bytes;
    }
    operator const uint32_t*() const
    {
        return _address.dword;
    }
    bool operator==(const IPv6Address& addr) const
    {
        return (_address.dword[0] == addr._address.dword[0])
            && (_address.dword[1] == addr._address.dword[1])
            && (_address.dword[2] == addr._address.dword[2])
            && (_address.dword[3] == addr._address.dword[3]);
    }
    bool operator==(const uint8_t* addr) const;

    // Overloaded index operator to allow getting and setting individual octets of the address
    uint8_t operator[](int index) const
    {
        return _address.bytes[index];
    }
    uint8_t& operator[](int index)
    {
        return _address.bytes[index];
    }

    // Overloaded copy operators to allow initialisation of IPv6Address objects from other types
    IPv6Address& operator=(const uint8_t *address);

    virtual size_t printTo(Print& p) const;
    String toString() const;

    friend class UDP;
    friend class Client;
    friend class Server;
};

#endif


/*
ESP8266 Multicast DNS (port of CC3000 Multicast DNS library)
Version 1.1
Copyright (c) 2013 Tony DiCola (tony@tonydicola.com)
ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)
MDNS-SD Suport 2015 Hristo Gochkov (hristo@espressif.com)
Extended MDNS-SD support 2016 Lars Englund (lars.englund@gmail.com)
Rewritten for ESP32 by Hristo Gochkov (hristo@espressif.com)
This is a simple implementation of multicast DNS query support for an Arduino
running on ESP32 chip. 
Usage:
- Include the ESP32 Multicast DNS library in the sketch.
- Call the begin method in the sketch's setup and provide a domain name (without
  the '.local' suffix, i.e. just provide 'foo' to resolve 'foo.local'), and the
  Adafruit CC3000 class instance.  Optionally provide a time to live (in seconds)
  for the DNS record--the default is 1 hour.
- Call the update method in each iteration of the sketch's loop function.
License (MIT license):
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
#ifndef ESP32MDNS_H
#define ESP32MDNS_H

#include "Arduino.h"
#include "IPv6Address.h"
#include "mdns.h"

//this should be defined at build time
#ifndef ARDUINO_VARIANT
#define ARDUINO_VARIANT "esp32"
#endif

class MDNSResponder {
public:
  MDNSResponder();
  ~MDNSResponder();
  bool begin(const char* hostName);
  void end();

  void setInstanceName(String name);
  void setInstanceName(const char * name){
    setInstanceName(String(name));
  }
  void setInstanceName(char * name){
    setInstanceName(String(name));
  }

  bool addService(char *service, char *proto, uint16_t port);
  bool addService(const char *service, const char *proto, uint16_t port){
    return addService((char *)service, (char *)proto, port);
  }
  bool addService(String service, String proto, uint16_t port){
    return addService(service.c_str(), proto.c_str(), port);
  }
  
  bool addServiceTxt(char *name, char *proto, char * key, char * value);
  void addServiceTxt(const char *name, const char *proto, const char *key,const char * value){
    addServiceTxt((char *)name, (char *)proto, (char *)key, (char *)value);
  }
  void addServiceTxt(String name, String proto, String key, String value){
    addServiceTxt(name.c_str(), proto.c_str(), key.c_str(), value.c_str());
  }

  void enableArduino(uint16_t port=3232, bool auth=false);
  void disableArduino();

  void enableWorkstation(esp_interface_t interface=ESP_IF_WIFI_STA);
  void disableWorkstation();

  IPAddress queryHost(char *host, uint32_t timeout=2000);
  IPAddress queryHost(const char *host, uint32_t timeout=2000){
    return queryHost((char *)host, timeout);
  }
  IPAddress queryHost(String host, uint32_t timeout=2000){
    return queryHost(host.c_str(), timeout);
  }
  
  int queryService(char *service, char *proto);
  int queryService(const char *service, const char *proto){
    return queryService((char *)service, (char *)proto);
  }
  int queryService(String service, String proto){
    return queryService(service.c_str(), proto.c_str());
  }

  String hostname(int idx);
  IPAddress IP(int idx);
  IPv6Address IPv6(int idx);
  uint16_t port(int idx);
  int numTxt(int idx);
  bool hasTxt(int idx, const char * key);
  String txt(int idx, const char * key);
  String txt(int idx, int txtIdx);
  String txtKey(int idx, int txtIdx);
  
private:
  String _hostname;
  mdns_result_t * results;
  mdns_result_t * _getResult(int idx);
  mdns_txt_item_t * _getResultTxt(int idx, int txtIdx);
};

extern MDNSResponder MDNS;

#endif //ESP32MDNS_H
