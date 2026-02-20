#include <Arduino.h>
#include <ESP8266mDNS.h>

#include "mdns_support.h"
#include "wifi_support.h" /* for get_hostname() */

static void query_mdns(const char* service, const char* proto, uint16_t timeout_ms)
{
  if (service == NULL || proto == NULL)
  {
    Serial.printf("mdns_query_and_print: invalid arguments: %p, %p\n", service, proto);
    return;
  }

  uint32_t count = MDNS.queryService(service, proto, timeout_ms);
  Serial.printf("%s.%s count: %u\n", service, proto, count);

  for (uint32_t i = 0; i < count; ++i)
  {
    const char* host = MDNS.answerHostname(i);
    IPAddress ip = MDNS.answerIP(i);
    uint16_t port = MDNS.answerPort(i);

    Serial.printf("\t[%u] Address: %s:%u,\tHost: %s\n", i, ip.toString().c_str(), port, (host) ? host : "(none)");
  }

  MDNS.removeQuery(); // Free the internal static answer set allocated by queryService()
}

static void dump_mdns()
{
  const uint16_t timeout_ms = 800;
  Serial.printf("\n==== mDNS service ====\n");
  Serial.printf("Responder running (hostname: %s): %d\n", get_hostname(), MDNS.isRunning());

  MDNS.announce();
  Serial.printf("Discovering common services on the LAN...\n");

  query_mdns("http", "tcp", timeout_ms);        // _http._tcp
  query_mdns("arduino", "tcp", timeout_ms);     // _arduino._tcp
  query_mdns("workstation", "tcp", timeout_ms); // _workstation._tcp
  query_mdns("hap", "tcp", timeout_ms);         // Apple HomeKit
  query_mdns("ssh", "tcp", timeout_ms);         // SSH servers
  query_mdns("ipp", "tcp", timeout_ms);         // IPP printers
  query_mdns("smb", "tcp", timeout_ms);         // SMB (rare via mDNS)

  Serial.printf("==== End service details ====\n");
}

void mdns_start()
{
  if (!wifi_is_connected())
  {
    return;
  }

  if (MDNS.isRunning())
  {
    return;
  }

  WiFi.hostname(get_hostname());
  if (MDNS.begin(get_hostname()))
  {
    MDNS.addService("http", "tcp", 80);
    Serial.printf("mDNS responder started on %s\n", get_hostname());
    dump_mdns();
  }
  else
  {
    Serial.printf("mDNS responder has not been started on %s\n", get_hostname());
  }
}

void mdns_tick()
{
  if (wifi_is_connected())
  {
    if (MDNS.isRunning())
    {
      bool mdns_status = MDNS.update();
      if (!mdns_status)
      {
        Serial.printf("mDNS service failed to update\n");
      }
    }
    else
    {
      mdns_start();
    }
  }
  else
  {
    if (MDNS.isRunning())
    {
      MDNS.end();
      Serial.printf("mDNS responder stopped\n");
    }
  }
}
