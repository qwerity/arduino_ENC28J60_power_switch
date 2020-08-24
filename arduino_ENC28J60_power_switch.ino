#include <UIPAgentuino.h>
#include <UIPEthernet.h>
#include <EEPROM.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EEPROM
int pinsStatusEEAddress = 0;

// Ethernet
static byte mac[] = {0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
static byte ip[] = {192, 168, 77, 21};

// led pins
const int led_pins_size = 4;
int ledPins[led_pins_size] = {2, 3, 4, 5};
boolean pins_status[led_pins_size] = {LOW, LOW, LOW, LOW};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mib description // tkmib - linux mib browser
//
// RFC1213-MIB OIDs
// .iso (.1)
// .iso.org (.1.3)
// .iso.org.dod (.1.3.6)
// .iso.org.dod.internet (.1.3.6.1)
// .iso.org.dod.internet.mgmt (.1.3.6.1.2)
// .iso.org.dod.internet.mgmt.mib-2 (.1.3.6.1.2.1)
// .iso.org.dod.internet.mgmt.mib-2.system (.1.3.6.1.2.1.1)

// .iso.org.dod.internet.mgmt.mib-2.system.sysDescr (.1.3.6.1.2.1.1.1)
static const char sysDescr[] PROGMEM = "1.3.6.1.2.1.1.1.0";  // read-only  (DisplayString)

// .iso.org.dod.internet.mgmt.mib-2.system.sysUpTime (.1.3.6.1.2.1.1.2)
static const char sysUpTime[]  PROGMEM = "1.3.6.1.2.1.1.2.0";  // read-only  (TimeTicks)

// .iso.org.dod.internet.mgmt.mib-2.system.sysContact (.1.3.6.1.2.1.1.3)
static const char sysContact[] PROGMEM = "1.3.6.1.2.1.1.3.0";  // read-write (DisplayString)

// .iso.org.dod.internet.mgmt.mib-2.system.sysName (.1.3.6.1.2.1.1.4)
static const char sysName[] PROGMEM = "1.3.6.1.2.1.1.4.0";  // read-write (DisplayString)

// .iso.org.dod.internet.mgmt.mib-2.system.sysLedPinsStatus (.1.3.6.1.2.1.1.5)
static const char sysLedPinsStatus[] PROGMEM = "1.3.6.1.2.1.1.5.0";  // read-write (byte)

// RFC1213 local values
static char locDescr[] = "EP SNMP Agent."; // read-only (static)
static uint32_t locUpTime = 0;             // read-only (static)
static char locContact[20] = "qwerity";    // should be stored/read from EEPROM - read/write (not done for simplicity)
static char locName[20] = "EP_SNMP_Agent"; // should be stored/read from EEPROM - read/write (not done for simplicity)
static uint32_t locLedPinsStatus{0};       // should be stored/read from EEPROM - read/write (not done for simplicity)

uint32_t prevMillis = millis();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void preparePins()
{
    for (int ledPin : ledPins)
    {
        pinMode(ledPin, OUTPUT);
    }
}

void setPinsStatus(uint32_t pinsStatus, boolean saveInEE = false)
{
    for (unsigned i = 0; i < led_pins_size; ++i)
    {
        digitalWrite(ledPins[i], ! (pinsStatus & (1U << i)));
    }

    if (saveInEE) {
        EEPROM.put(pinsStatusEEAddress, locLedPinsStatus);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void pduReceived() {
    SNMP_PDU pdu{};
    SNMP_ERR_CODES status;
    char oid[SNMP_MAX_OID_LEN];
    SNMP_API_STAT_CODES api_status = Agentuino.requestPdu(&pdu);
    //
    if ((pdu.type == SNMP_PDU_GET || pdu.type == SNMP_PDU_GET_NEXT || pdu.type == SNMP_PDU_SET)
        && pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS) {
        pdu.OID.toString(oid);
        // Implementation SNMP GET NEXT
        if (pdu.type == SNMP_PDU_GET_NEXT) {
            char tmpOIDfs[SNMP_MAX_OID_LEN];
            if (strcmp_P(oid, sysDescr) == 0) {
                strcpy_P(oid, sysUpTime);
                strcpy_P(tmpOIDfs, sysUpTime);
                pdu.OID.fromString(tmpOIDfs);
            } else if (strcmp_P(oid, sysUpTime) == 0) {
                strcpy_P(oid, sysContact);
                strcpy_P(tmpOIDfs, sysContact);
                pdu.OID.fromString(tmpOIDfs);
            } else if (strcmp_P(oid, sysContact) == 0) {
                strcpy_P(oid, sysName);
                strcpy_P(tmpOIDfs, sysName);
                pdu.OID.fromString(tmpOIDfs);
            } else if (strcmp_P(oid, sysName) == 0) {
                strcpy_P(oid, sysLedPinsStatus);
                strcpy_P(tmpOIDfs, sysLedPinsStatus);
                pdu.OID.fromString(tmpOIDfs);
            } else if (strcmp_P(oid, sysLedPinsStatus) == 0) {
                strcpy_P(oid, "1.0");
            } else {
                auto ilen = strlen(oid);
                if (strncmp_P(oid, sysDescr, ilen) == 0) {
                    strcpy_P(oid, sysDescr);
                    strcpy_P(tmpOIDfs, sysDescr);
                    pdu.OID.fromString(tmpOIDfs);
                } else if (strncmp_P(oid, sysUpTime, ilen) == 0) {
                    strcpy_P(oid, sysUpTime);
                    strcpy_P(tmpOIDfs, sysUpTime);
                    pdu.OID.fromString(tmpOIDfs);
                } else if (strncmp_P(oid, sysContact, ilen) == 0) {
                    strcpy_P(oid, sysContact);
                    strcpy_P(tmpOIDfs, sysContact);
                    pdu.OID.fromString(tmpOIDfs);
                } else if (strncmp_P(oid, sysName, ilen) == 0) {
                    strcpy_P(oid, sysName);
                    strcpy_P(tmpOIDfs, sysName);
                    pdu.OID.fromString(tmpOIDfs);
                } else if (strncmp_P(oid, sysLedPinsStatus, ilen) == 0) {
                    strcpy_P(oid, sysLedPinsStatus);
                    strcpy_P(tmpOIDfs, sysLedPinsStatus);
                    pdu.OID.fromString(tmpOIDfs);
                }
            }
        }
        // End of implementation SNMP GET NEXT / WALK

        if (strcmp_P(oid, sysDescr) == 0) {
            // handle sysDescr (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read-only
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_READ_ONLY;
            } else {
                // response packet from get-request - locDescr
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locDescr);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
        } else if (strcmp_P(oid, sysUpTime) == 0) {
            // handle sysName (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read-only
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_READ_ONLY;
            } else {
                // response packet from get-request - locUpTime
                status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, locUpTime);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
        } else if (strcmp_P(oid, sysName) == 0) {
            // handle sysName (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(locName, strlen(locName));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locName
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locName);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
        } else if (strcmp_P(oid, sysContact) == 0) {
            // handle sysContact (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(locContact, strlen(locContact));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locContact
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locContact);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
        } else if (strcmp_P(oid, sysLedPinsStatus) == 0) {
            // handle sysLedPinsStatus (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(&locLedPinsStatus);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;

                if (locLedPinsStatus > 7) {
                    pdu.error = SNMP_ERR_WRONG_VALUE;
                }

                if (SNMP_ERR_NO_ERROR == pdu.error) {
                    setPinsStatus(locLedPinsStatus, true);
                }
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_UINT32, locLedPinsStatus);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
        } else {
            // oid does not exist
            // response packet - object not found
            pdu.type = SNMP_PDU_RESPONSE;
            pdu.error = SNMP_ERR_NO_SUCH_NAME;
        }
        Agentuino.responsePdu(&pdu);
    }
    Agentuino.freePdu(&pdu);
}

// setup
void setup()
{
    Ethernet.begin(mac, ip);

    //Get the pins statuses from the EEPROM at position 'eeAddress'
    EEPROM.get(pinsStatusEEAddress, locLedPinsStatus);
//    Serial.begin(9600);
//    Serial.print("pins status: ");
//    Serial.println(locLedPinsStatus);

    preparePins();
    setPinsStatus(locLedPinsStatus);

    if (SNMP_API_STAT_SUCCESS == Agentuino.begin())
    {
        Agentuino.onPduReceive(pduReceived);
        delay(10);
        return;
    }
    delay(10);
}

// main loop
void loop()
{
    // listen/handle for incoming SNMP requests
    Agentuino.listen();

    // sysUpTime - The time (in hundredths of a second) since
    // the network management portion of the system was last
    // re-initialized.
    if (millis() - prevMillis > 1000) {
        // increment previous milliseconds
        prevMillis += 1000;
        //
        // increment up-time counter
        locUpTime += 100;
    }
}
