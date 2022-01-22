#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#define COMMAND_STOP_CHAR '|'
#define COMMAND_SPLIT_CHAR ';'

#define COMMAND_SET_MODE_SPECTRUM F("startSpectrum")
#define COMMAND_SEND_LINE_COUNT F("line")
#define COMMAND_SEND_SPECTRUM_DATA F("sendSpectrum")
#define COMMAND_SET_MAX_SPECTRUM_DATA F("maxData")

#define COMMAND_RESET F("reset")
#define COMMAND_RELOAD_SCREEN F("reloadScreen")

#define LOG_TOKEN F(" [LOG] ")

#endif