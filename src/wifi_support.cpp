#include <ESP8266WiFi.h>
#include <WiFiManager.h> // "WiFiManager by tzapu"

#include "wifi_support.h"

static WiFiManager g_wm;

static const char* HOSTNAME = "sensor_co2_1";            // must match your OTA hostname
static const int CONNECT_TIMEOUT_S = 15;                 // seconds
static const int CONFIG_PORTAL_TIMEOUT_S = 300;          // seconds
static const char* SETUP_AP_NAME = "sensor_co2_1_setup"; // captive portal SSID
static const char* SETUP_AP_PASS = "12345678";           // captive portal password

const char* get_hostname()
{
  return HOSTNAME;
}

const char* get_setup_ap_pass()
{
  return SETUP_AP_PASS;
}

void wifi_forget_credentials()
{
  g_wm.resetSettings();
}

bool wifi_is_connected()
{
  return WiFi.isConnected();
}

static const char* get_phy()
{
  phy_mode_t phy = wifi_get_phy_mode();
  switch (phy)
  {
  case PHY_MODE_11B:
    return "802.11b";
  case PHY_MODE_11G:
    return "802.11g";
  case PHY_MODE_11N:
    return "802.11n";
  default:
    return "unknown";
  }
}

static void dump_wifi()
{
  Serial.printf("\n==== WI-FI service ====\n");

  Serial.printf("Hostname: %s\n", get_hostname());
  Serial.printf("Interface mode: %d (0=Off,1=STA,2=AP,3=AP+STA)\n", WiFi.getMode());
  Serial.printf("Connection state: %d, SSID: %s\n", wifi_is_connected(), WiFi.SSID().c_str());

  Serial.printf("Channel: %d\n", WiFi.channel());
  Serial.printf("Signal level (RSSI): %d dBm\n", WiFi.RSSI());

  Serial.printf("Physical layer (PHY): %s\n", get_phy());

  wifi_country_t country;
  if (wifi_get_country(&country))
  {
    Serial.printf("Regulatory country: %.2s\n", country.cc);
    Serial.printf("Channel range: %u..%u\n", country.schan, (country.schan + country.nchan - 1));
    Serial.printf("Max TX power (policy): %u\n", country.policy);
  }
  else
  {
    Serial.printf("Regulatory country: (unavailable)\n");
  }

  Serial.printf("IPv4 address: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("Subnet mask: %s\n", WiFi.subnetMask().toString().c_str());
  Serial.printf("Default gateway: %s\n", WiFi.gatewayIP().toString().c_str());
  Serial.printf("DNS[0]: %s\n", WiFi.dnsIP(0).toString().c_str());
  Serial.printf("DNS[1]: %s\n", WiFi.dnsIP(1).toString().c_str());
  Serial.printf("AutoConnect: %d\n", WiFi.getAutoConnect());
  Serial.printf("AutoReconnect: %d\n", WiFi.getAutoReconnect());

  Serial.printf("==== End service details ====\n");
}

void wifi_start()
{
  WiFi.mode(WIFI_STA);
  WiFi.persistent(true);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.hostname(get_hostname());

  g_wm.setConnectTimeout(CONNECT_TIMEOUT_S);
  g_wm.setConfigPortalTimeout(CONFIG_PORTAL_TIMEOUT_S);

  if (g_wm.getWiFiIsSaved())
  {
    g_wm.setEnableConfigPortal(false);
    WiFi.begin();
  }
  else
  {
    Serial.printf("No saved Wi-Fi credentials. Starting setup portal \"%s\"...\n", SETUP_AP_NAME);
    if (g_wm.startConfigPortal(SETUP_AP_NAME, SETUP_AP_PASS))
    {
      if (wifi_is_connected())
      {
        WiFi.hostname(get_hostname());
      }
    }
    else
    {
      Serial.printf("Setup portal failed or timed out. Continue without connection.\n");
    }
  }

  if (wifi_is_connected())
  {
    dump_wifi();
  }

  return;
}

void wifi_tick()
{
  static uint32_t next_retry_ms = 0;

  if (wifi_is_connected())
  {
    return;
  }

  const uint32_t now = millis();
  if (now > next_retry_ms)
  {
    WiFi.reconnect();
    next_retry_ms = now + (CONNECT_TIMEOUT_S * 1000);
  }
}
