#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>

#define RF95_FREQ 868.0
#define RFM95_RST D0
#define RFM95_INT D1
#define RFM95_CS D8

RH_RF95 rf95(RFM95_CS, RFM95_INT);
int16_t packetNum = 0;

void setup()
{
  pinMode(RFM95_RST, OUTPUT);

  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);    
  while (!Serial);
  delay(100);

  Serial.println();
  Serial.println("Starting LoRa Sender");

  //manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);

  while(!rf95.init())
  {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if(!rf95.setFrequency(RF95_FREQ))
  {
    Serial.println("setFrequency failed");
    while(1);
  }
  Serial.print("Set freq to: ");
  Serial.println(RF95_FREQ);

  rf95.setTxPower(23, false);
}

void loop()
{
  if(rf95.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if(rf95.recv(buf, &len))
    {
      RH_RF95::printBuffer("Received:", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      delay(10);

      //reply
      char response[] = "Hello back";
      rf95.send((uint8_t *)response, sizeof(response));
      rf95.waitPacketSent();
      Serial.print("Response sent: ");
      Serial.println(response);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}