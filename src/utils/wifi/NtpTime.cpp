#include "NtpTime.h"

#define UDP_PORT 2390
#define NTP_PACKET_SIZE 48

namespace NtpTime
{
    void sendNTPpacket(WiFiUDP &udp, IPAddress &address);

    unsigned long AskNTP(String pool, bool &isOk)
    {
        WiFiUDP udp;
        udp.begin(UDP_PORT);

        int cb;
        while (udp.parsePacket() > 0)
        {
            delay(10);
        }
        IPAddress timeServerIP;
        WiFi.hostByName(pool.c_str(), timeServerIP);
        sendNTPpacket(udp, timeServerIP);

        unsigned long beginWait = millis();
        while (millis() - beginWait < 5000)
        {
            delay(10);
            cb = udp.parsePacket();
            if (cb >= NTP_PACKET_SIZE)
            {
                byte packetBuffer[NTP_PACKET_SIZE];
                udp.read(packetBuffer, NTP_PACKET_SIZE);
                unsigned long secsSince1900 = (unsigned long)packetBuffer[40] << 24;
                secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
                secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
                secsSince1900 |= (unsigned long)packetBuffer[43];
                isOk = true;
                return secsSince1900 - 2208988800UL;
            }
        }
        isOk = false;
        return 0;
    }

    void sendNTPpacket(WiFiUDP &udp, IPAddress &address)
    {
        // set all bytes in the buffer to 0
        byte packetBuffer[NTP_PACKET_SIZE];
        // Initialize values needed to form NTP request
        // (see URL above for details on the packets)
        packetBuffer[0] = 0b11100011; // LI, Version, Mode
        packetBuffer[1] = 0;          // Stratum, or type of clock
        packetBuffer[2] = 6;          // Polling Interval
        packetBuffer[3] = 0xEC;       // Peer Clock Precision
        // 8 bytes of zero for Root Delay & Root Dispersion
        packetBuffer[12] = 49;
        packetBuffer[13] = 0x4E;
        packetBuffer[14] = 49;
        packetBuffer[15] = 52;
        // all NTP fields have been given values, now
        // you can send a packet requesting a timestamp:
        udp.beginPacket(address, 123); //NTP requests are to port 123
        udp.write(packetBuffer, NTP_PACKET_SIZE);
        udp.endPacket();
    }
}