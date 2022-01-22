#ifndef _FILE_NAMES_H_
#define _FILE_NAMES_H_

#define CONFIG_DIR F("/cnfg")
#define PAGES_DIR F("/pages")

#define WEATHER_CONFIG_PATH String(CONFIG_DIR) + F("/Weather.json")
#define WIFI_CONFIG_PATH String(CONFIG_DIR) + F("/Wifi.json")
#define MAIN_SCREEN_CONFIG_PATH String(CONFIG_DIR) + F("/MainScreen.json")
#define SPECTRUM_CONFIG_PATH String(CONFIG_DIR) + F("/Spectrum.json")

#define INDEX_PAGE_PATH String(PAGES_DIR) + F("/index.html")
#define CONNECT_PAGE_PATH String(PAGES_DIR) + F("/connect.html")
#define EDIT_PAGE_PATH String(PAGES_DIR) + F("/edit.html")
#define MONITOR_PAGE_PATH String(PAGES_DIR) + F("/monitor.html")
#define CONFIG_PAGE_PATH String(PAGES_DIR) + F("/editConfig.html")

#endif
