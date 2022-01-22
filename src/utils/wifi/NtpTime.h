#pragma once

#include <WiFiUdp.h>
#include <WiFi.h>

namespace NtpTime
{
    unsigned long AskNTP(String pool, bool &isOk);
}