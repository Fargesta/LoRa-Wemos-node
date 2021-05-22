#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <main.h>

RH_RF95 rf95(RFM95_CS, RFM95_INT);
int16_t packetNum = 0;

//Network join status
bool isJoined = false;

//Millis timer setup
int delayPeriod = 2000;
unsigned long timeNow = 0;

void SendResponse(String responseText, String code)
{
  //reply
  String response = GW_NAME;
  response += GW_ID;
  response += ID;
  response += code;
  response += responseText;
  short messageLength = response.length();
  char radioMessage[messageLength + 1];
  response.toCharArray(radioMessage, messageLength);

  Serial.print(">>Sending command: ");
  Serial.println(response);
  radioMessage[messageLength] = 0;
  delay(10);
  rf95.send((uint8_t *)radioMessage, messageLength);

  delay(10);
  rf95.waitPacketSent();
}

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
  if (!isJoined)
  {
    //Send SYN request each second until ACK received
    if(millis() > timeNow + delayPeriod)
    {
      timeNow = millis();
      SendResponse(SYN, OK);
    }
  }
  
  if(rf95.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if(rf95.recv(buf, &len))
    {
      //RH_RF95::printBuffer("Received:", buf, len);
      String cmd = (char*)buf;
      String response = GW_NAME;
      response += GW_ID;
      if(cmd.substring(0, 4).equals(NAME))
      {
        cmd = cmd.substring(4);
        //decrypt here
        //...
        if(cmd.substring(4, 8).equals(ID))
        {
          //serial debug
          Serial.print("Got: ");
          Serial.println((char*)buf);
          //Serial.print("RSSI: ");
          //Serial.println(rf95.lastRssi(), DEC);
          delay(10);

          //parse command
          cmd = cmd.substring(8, 10);
          if(cmd.equals(ON)) 
          {
            //Wifi ON
            SendResponse("wifion", OK);
          }
          else if (cmd.equals(OF))
          {
            //Wifi OFF
            SendResponse("wifioff", OK);
          }
          else if (cmd.equals(ACK))
          {
            isJoined = true;
            SendResponse("joined", OK);
          }
          else
          {
            SendResponse("Unknown command", ERROR);
          }
        }
      }
    }
    else
    {
      SendResponse("Read buffer", ERROR);
    }
  }
}