#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_READ_BYTES 1024

struct MessageHeader
{
  uint16_t msg_len;
  uint16_t msg_type;
};

enum MsgType
{
  _,
  WRITE,
  READ,
};

class MessageData
{
public:
  char *buff;
  size_t buff_len;
  size_t buff_size;

  MessageData(char *buff, size_t buff_len, size_t buff_size = 0)
  {
    this->buff = buff;
    this->buff_len = buff_len;
    this->buff_size = buff_size;
  }

  bool isValid()
  {
    return buff != NULL && buff < (buff + buff_len);
  }

  MessageData &operator++()
  {
    if (isValid())
    {
      buff += strlen(buff) + 1;
    }
    return *this;
  }

  const char *toString()
  {
    return buff;
  }

  const char *toStr()
  {
    return buff;
  }

  int toInt()
  {
    if (isValid())
    {
      return atoi(buff);
    }
    return 0;
  }

  float toFloat()
  {
    if (isValid())
    {
      return atof(buff);
    }
    return 0;
  }

  void put(const char *str, size_t slen)
  {
    memcpy(buff + buff_len, str, slen);
    buff_len += slen;
    buff[buff_len] = 0;
  }

  void put(const char *str)
  {
    put(str, strlen(str) + 1);
  }

  void put(int val)
  {
    buff_len += snprintf(buff + buff_len, buff_size - buff_len, "%d", val) + 1;
    buff[buff_len] = 0;
  }

  void put(float val)
  {
    buff_len += snprintf(buff + buff_len, buff_size - buff_len, "%f", val) + 1;
    buff[buff_len] = 0;
  }

  void put(double val)
  {
    buff_len += snprintf(buff + buff_len, buff_size - buff_len, "%lf", val) + 1;
    buff[buff_len] = 0;
  }

  const char *end()
  {
    return buff + buff_len;
  }

  size_t length()
  {
    return buff_len;
  }

  bool operator<(const char *data) const
  {
    return buff < data;
  }
  bool operator>=(const char *data) const
  {
    return buff >= data;
  }
};

WebSocketsServer websockets(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{

  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED:
  {
    IPAddress ip = websockets.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
  }
  break;
  case WStype_BIN:

    if (length < sizeof(MessageHeader))
    {
      return;
    }

    MessageHeader hdr;

    memcpy(&hdr, payload, sizeof(MessageHeader));

    hdr.msg_len = ntohs(hdr.msg_len);
    hdr.msg_type = ntohs(hdr.msg_type);

    if (hdr.msg_len > MAX_READ_BYTES)
    {
      Serial.println("Message too long");
      return;
    }

    MessageData data((char *)(payload + sizeof(MessageHeader)), hdr.msg_len);

    switch (hdr.msg_type)
    {
    case WRITE:

      const int pin = data.toInt();

      if (++data >= data.end())
      {
        return;
      }

      digitalWrite(pin, data.toInt());

      break;
    }
  }
}

void setup()
{
  pinMode(2, OUTPUT);

  Serial.begin(115200);

  WiFi.begin("Thrux@wifi.", "LKJHGFDSA");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi network");
  Serial.println(WiFi.localIP());

  websockets.begin();
  websockets.onEvent(webSocketEvent);
}

void loop()
{
  websockets.loop();
}