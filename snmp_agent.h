#pragma once

#include <UIPAgentuino.h>
#include <UIPEthernet.h>

#include "mib.h"

// Arduino defined OIDs
// .iso.org.dod.internet.private (.1.3.6.1.4)
// .iso.org.dod.internet.private.enterprises (.1.3.6.1.4.1)
// .iso.org.dod.internet.private.enterprises.arduino (.1.3.6.1.4.1.36582)
//
// RFC1213 local values
static char locDescr[] = "light-weight SNMP Agent."; // read-only (static)
//static char locObjectID[] = "1.3.6.1.3.2009.0";    // read-only (static)
static uint32_t locUpTime = 0;                       // read-only (static)
static char locContact[20] = "qwerity";              // should be stored/read from EEPROM - read/write (not done for simplicity)
static char locName[20] = "EP_SNMP_Agent";           // should be stored/read from EEPROM - read/write (not done for simplicity)
static char locLocation[20] = "Armenia";             // should be stored/read from EEPROM - read/write (not done for simplicity)
static int32_t locServices = 6;                      // read-only (static)

uint32_t prevMillis = millis();

void pduReceived() {
    SNMP_PDU pdu;
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
                strcpy_P(oid, sysLocation);
                strcpy_P(tmpOIDfs, sysLocation);
                pdu.OID.fromString(tmpOIDfs);
            } else if (strcmp_P(oid, sysLocation) == 0) {
                strcpy_P(oid, sysServices);
                strcpy_P(tmpOIDfs, sysServices);
                pdu.OID.fromString(tmpOIDfs);
            } else if (strcmp_P(oid, sysServices) == 0) {
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
                } else if (strncmp_P(oid, sysLocation, ilen) == 0) {
                    strcpy_P(oid, sysLocation);
                    strcpy_P(tmpOIDfs, sysLocation);
                    pdu.OID.fromString(tmpOIDfs);
                } else if (strncmp_P(oid, sysServices, ilen) == 0) {
                    strcpy_P(oid, sysServices);
                    strcpy_P(tmpOIDfs, sysServices);
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
        } else if (strcmp_P(oid, sysLocation) == 0) {
            // handle sysLocation (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read/write
                status = pdu.VALUE.decode(locLocation, strlen(locLocation));
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            } else {
                // response packet from get-request - locLocation
                status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locLocation);
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = status;
            }
        } else if (strcmp_P(oid, sysServices) == 0) {
            // handle sysServices (set/get) requests
            if (pdu.type == SNMP_PDU_SET) {
                // response packet from set-request - object is read-only
                pdu.type = SNMP_PDU_RESPONSE;
                pdu.error = SNMP_ERR_READ_ONLY;
            } else {
                // response packet from get-request - locServices
                status = pdu.VALUE.encode(SNMP_SYNTAX_INT, locServices);
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

void snmp_agent_setup(byte *mac, byte *ip)
{
    Ethernet.begin(mac, ip);

    if (SNMP_API_STAT_SUCCESS == Agentuino.begin()) 
    {
        Agentuino.onPduReceive(pduReceived);
        delay(10);
        return;
    }
    delay(10);
}

void snmp_agent_loop()
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
