#include <Arduino.h>
#include "EEpromWriteAnything.h"
#include <Ethernet.h>
#include <ArduinoJson.h>
JsonDocument doc;

// #define DEBUG
#define DHCP

struct NetConfig
{
  char state[10];
  uint8_t mac[6];
  uint8_t usedhcp;
  uint8_t serverip[4];
  uint8_t ip[4];
  uint8_t subnet[4];
  uint8_t gateway[4];
  uint8_t dnsserver[4];
  uint16_t serverport;
} conf;

void initEthernet()
{
#ifdef DHCP
  if (conf.usedhcp == 1)
  {
    Serial.println(F("Ethernet configure useing DHCP"));
    Ethernet.begin(conf.mac);
    Ethernet.maintain();
  }
  else
  {
#endif
    Serial.println(F("Ethernet configure using fix IP"));
    Ethernet.begin(conf.mac, conf.ip, conf.dnsserver, conf.gateway, conf.subnet);
#ifdef DHCP
  }
#endif
}

String getMACasString(uint8_t *mac)
{
  char macno[32];
  sprintf(macno, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(macno);
}

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

#ifdef DEBUG
  while (!Serial)
  {
    ;
  }
#endif
  EEPROM_readAnything(1, conf);
  bool configured = String(conf.state).startsWith("CONFIG");
  if (configured)
  {
    initEthernet();
  }
  Serial.println(configured ? F("Configured") : F("Not confugured yet"));

  if (configured)
  {
    Serial.print(F("reader ip: "));
    Serial.println(Ethernet.localIP());
  }
}

void getMACFromString(String mac, uint8_t *result)
{
  for (uint8_t i = 0; i < 6; i++)
  {
    String part = mac.substring(0, mac.indexOf("-"));
    char *pEnd;
    result[i] = strtol(part.c_str(), &pEnd, 16);
    mac.remove(0, part.length() + 1);
  }
}

void printIPToSerial(String ipname, IPAddress addr)
{
  char ipno[16];
  sprintf(ipno, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
  Serial.print(F("\""));
  Serial.print(ipname);
  Serial.print(F("\":\""));
  Serial.print(ipno);
  Serial.print(F("\""));
};

void printMACToSerial(String ipname, uint8_t *result)
{
  Serial.print(F("\""));
  Serial.print(ipname);
  Serial.print(F("\":\""));
  Serial.print(getMACasString(result));
  Serial.print(F("\""));
}

void getIPFromString(String ip, uint8_t *result, byte length = 4)
{
  for (uint8_t i; i < length; i++)
  {
    String part = ip.substring(0, ip.indexOf("."));
    result[i] = part.toInt();
    ip.remove(0, part.length() + 1);
  }
}

void printConfigToSerial()
{
  Serial.print(F("{"));
  Serial.print(F("\"state\":\""));
  Serial.print(String(conf.state));
  Serial.print(F("\",\""));
  Serial.print(F("usedhcp\":"));
  Serial.print(conf.usedhcp == 1);
  Serial.print(F(","));
  printIPToSerial(F("ip"), conf.ip);
  Serial.print(",");
  printIPToSerial(F("serverip"), conf.serverip);
  Serial.print(",\"");
  Serial.print(F("serverport\":"));
  Serial.print(conf.serverport);
  Serial.print(",");
  printIPToSerial(F("gateway"), conf.gateway);
  Serial.print(",");
  printIPToSerial(F("dnsserver"), conf.dnsserver);
  Serial.print(",");
  printIPToSerial(F("subnet"), conf.subnet);
  Serial.print(",");
  printMACToSerial(F("mac"), conf.mac);
  Serial.println(F("}"));
};

void updateConf()
{
  strcpy(conf.state, doc["state"].as<String>().c_str());
  getIPFromString(doc[F("ip")].as<String>(), conf.ip);
  getIPFromString(doc[F("serverip")].as<String>(), conf.serverip);
  conf.usedhcp = doc[F("usedhcp")].as<int>();
  getIPFromString(doc[F("subnet")].as<String>(), conf.subnet);
  getIPFromString(doc[F("gateway")].as<String>(), conf.gateway);
  getIPFromString(doc[F("dnsserver")].as<String>(), conf.dnsserver);
  getMACFromString(doc[F("mac")].as<String>(), conf.mac);
  conf.serverport = doc[F("serverport")].as<int>();
}

String command = "";

void loop()
{

  while (Serial.available())
  {
    char buff[16];
    int cnt = Serial.readBytes(buff, sizeof(buff));
    if (cnt > 0)
    {
      String part = String(buff).substring(0, cnt);
      command += part;
#ifdef DEBUG
      Serial.print(F("\ncommand part:"));
      Serial.println(part);
#endif
    }
  };
  if (command.endsWith("}"))
  {
    Serial.print(F("Command received:"));
    Serial.println(command);
    deserializeJson(doc, command);
#ifdef DEBUG
    Serial.print(F("state:"));
    Serial.println(doc[F("state")].as<String>());

    Serial.print(F("ip:"));
    Serial.println(doc[F("ip")].as<String>());

    Serial.print(F("serverip:"));
    Serial.println(doc[F("serverip")].as<String>());

    Serial.print(F("serverport:"));
    Serial.println(doc[F("serverport")].as<int>());

    Serial.print(F("usedhcp:"));
    Serial.println(doc[F("usedhcp")].as<int>());

    Serial.print(F("subnet:"));
    Serial.println(doc[F("subnet")].as<String>());

    Serial.print(F("gateway:"));
    Serial.println(doc[F("gateway")].as<String>());

    Serial.print(F("dnsserver:"));
    Serial.println(doc[F("dnsserver")].as<String>());

    Serial.print(F("mac:"));
    Serial.println(doc[F("mac")].as<String>());
#endif
    if (doc["action"].as<String>().equals(F("configure")))
    {
      updateConf();
      EEPROM_writeAnything(1, conf);
    }
    else if (doc["action"].as<String>().equals(F("readconfig")))
    {
      EEPROM_readAnything(1, conf);
      printConfigToSerial();
    };
    command = "";
  };
}
