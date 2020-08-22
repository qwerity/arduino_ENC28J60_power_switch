#pragma once

#include <EtherCard.h>
#include <EEPROM.h>

#include "http_responses.h"

byte Ethernet::buffer[900];
BufferFiller bfill;

void homePage(boolean *pins_status)
{
    bfill.emit_p(PSTR("$F"
                      "<title>ArduinoPIN Webserver</title>"
                      "Air 1   : <a href=\"?ArduinoPIN1=$F\">$F</a><br />"
                      "Air 2   : <a href=\"?ArduinoPIN2=$F\">$F</a><br />"
                      "NVR Main: <a href=\"?ArduinoPIN3=$F\">$F</a><br />"
                      "NVR Cash: <a href=\"?ArduinoPIN4=$F\">$F</a>"),
                 http_OK,
                 
                 pins_status[0] ? PSTR("off") : PSTR("on"),
                 pins_status[0] ? PSTR("<font color=\"green\"><b>ON</b></font>") : PSTR("<font color=\"red\">OFF</font>"),
                 
                 pins_status[1] ? PSTR("off") : PSTR("on"),
                 pins_status[1] ? PSTR("<font color=\"green\"><b>ON</b></font>") : PSTR("<font color=\"red\">OFF</font>"),
                 
                 pins_status[2] ? PSTR("off") : PSTR("on"),
                 pins_status[2] ? PSTR("<font color=\"green\"><b>ON</b></font>") : PSTR("<font color=\"red\">OFF</font>"),
                 
                 pins_status[3] ? PSTR("off") : PSTR("on"),
                 pins_status[3] ? PSTR("<font color=\"green\"><b>ON</b></font>") : PSTR("<font color=\"red\">OFF</font>")
             );
}

void http_start_setup(byte *mac, int mac_size, byte *ip)
{
    if (0 == ether.begin(sizeof Ethernet::buffer, mac, mac_size))
    {
        Serial.println("ether.begin error");
    }

    if (! ether.dhcpSetup())
    {
        Serial.println("ether.dhcpSetup error");
    }

    ether.printIp("My Router IP: ", ether.myip);
    ether.staticSetup(ip);
    ether.printIp("My SET IP: ", ether.myip);
}

void http_server_loop(int *led_pins, int led_pins_size, boolean *pins_status, int pins_status_eeAddress)
{
    word packet_len = ether.packetReceive();
    word pos = ether.packetLoop(packet_len);

    if (pos)
    {
        bfill = ether.tcpOffset();
        auto *data = (char *) Ethernet::buffer + pos;
        if (0 != strncmp("GET /", data, 5))
        {
            bfill.emit_p(http_Unauthorized);
        }
        else
        {
            data += 5;
            if (' ' == data[0])
            {
                homePage(pins_status);
                for (auto i = 0; i < led_pins_size; ++i) {
                  digitalWrite(led_pins[i], ! pins_status[i]);
                }
            }
            else if (strncmp("?ArduinoPIN1=on ", data, 16) == 0)
            {
                pins_status[0] = HIGH;
                bfill.emit_p(http_Found);
            }
            else if (strncmp("?ArduinoPIN2=on ", data, 16) == 0)
            {
                pins_status[1] = HIGH;
                bfill.emit_p(http_Found);
            }
            else if (strncmp("?ArduinoPIN3=on ", data, 16) == 0)
            {
                pins_status[2] = HIGH;
                bfill.emit_p(http_Found);
            }
            else if (strncmp("?ArduinoPIN4=on ", data, 16) == 0)
            {
                pins_status[3] = HIGH;
                bfill.emit_p(http_Found);
            }

            //------------------------------------------------------  
            else if (strncmp("?ArduinoPIN1=off ", data, 17) == 0)
            {
                pins_status[0] = LOW;
                bfill.emit_p(http_Found);
            }
            else if (strncmp("?ArduinoPIN2=off ", data, 17) == 0)
            {
                pins_status[1] = LOW;
                bfill.emit_p(http_Found);
            }
            else if (strncmp("?ArduinoPIN3=off ", data, 17) == 0)
            {
                pins_status[2] = LOW;
                bfill.emit_p(http_Found);
            }
            else if (strncmp("?ArduinoPIN4=off ", data, 17) == 0)
            {
                pins_status[3] = LOW;
                bfill.emit_p(http_Found);
            }
            else
            {
                bfill.emit_p(http_Unauthorized);
            }

            EEPROM.put(pins_status_eeAddress, pins_status);
        }
        ether.httpServerReply(bfill.position());
    }
}
