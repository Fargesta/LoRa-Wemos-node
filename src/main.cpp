#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>
#include <main.h>

//Init driver and encrytion
RH_RF95 rf95(RFM95_CS, RFM95_INT);
Speck msgCipher;
RHEncryptedDriver encDriver(rf95, msgCipher);

//Network join status
bool isJoined = false;

//Millis timer setup
int delayPeriod = 3000;
unsigned long timeNow = 0;

void SendResponse(String responseText, String code)
{
  //reply
  String response = GW_PASSWORD;
  response += PASSWORD;
  response += code;
  response += responseText;
  short messageLength = response.length() + 1; //must be +1 for eof symbol
  char radioMessage[messageLength + 1];
  response.toCharArray(radioMessage, messageLength);

  Serial.print(">>Sending command: ");
  Serial.println(radioMessage);
  radioMessage[messageLength] = 0;
  delay(10);
  encDriver.send((uint8_t *)radioMessage, messageLength);

  delay(10);
  encDriver.waitPacketSent();
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
  msgCipher.setKey(encryptKey, sizeof(encryptKey));
  encDriver.setHeaderTo(GW_NETWORK_ID);
  encDriver.setThisAddress(NETWORK_ID);
}

void loop()
{
  if(encDriver.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if(encDriver.recv(buf, &len))
    {
      String cmd = (char*)buf;
      Serial.println(cmd);

      if(cmd.substring(0, 8).equals(PASSWORD))
      {
        //parse command
        cmd = cmd.substring(8, 10);
        if(cmd.equals(ON)) 
        {
          //WiFi ON
          SendResponse(ON, OK);
        }
        else if (cmd.equals(OFF))
        {
          //WiFi OFF
          SendResponse(OFF, OK);
        }
        else if (cmd.equals(ACK))
        {
          isJoined = true;
        }
        else
        {
          SendResponse("Unknown command", ERROR);
        }
      }
    }
    else
    {
      SendResponse("Read buffer", ERROR);
    }
  }
  else if (!isJoined)
  {
    //Send SYN request each second until ACK received
    if(millis() > timeNow + delayPeriod)
    {
      timeNow = millis();
      SendResponse(SYN, OK);
    }
  }
}