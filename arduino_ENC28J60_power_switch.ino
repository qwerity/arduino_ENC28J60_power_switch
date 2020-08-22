// #include "http_server.h"
#include "snmp_agent.h"
#include <EEPROM.h>

int pins_status_eeAddress = 0;

static byte _mac[] = {0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
static byte _ip[] = {192, 168, 77, 21};

const int led_pins_size = 4;
int led_pins[led_pins_size] = {2, 3, 4, 5};
boolean pins_status[led_pins_size] = {LOW, LOW, LOW, LOW};

void setup()
{
    Serial.begin(9600);

    //http_start_setup(_mac, 10, _ip);
    snmp_agent_setup(_mac, _ip);

    //Get the pins statuses from the EEPROM at position 'eeAddress'
    EEPROM.get(pins_status_eeAddress, pins_status);
    Serial.println("pins_status");
    Serial.println(pins_status[0]);
    Serial.println(pins_status[1]);
    Serial.println(pins_status[2]);
    Serial.println(pins_status[3]);

    for (auto i = 0; i < led_pins_size; ++i)
    {
        pinMode(led_pins[i], OUTPUT);
        digitalWrite(led_pins[i], pins_status[i]);
    }
}

void loop()
{
    //http_server_loop(led_pins, led_pins_size, pins_status, pins_status_eeAddress);
    snmp_agent_loop();
}
