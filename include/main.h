//Node Keys
#define NETWORK_ID 14             //uint8_t
#define PASSWORD "QV?+p,(j"       //8 symbols
#define GW_NETWORK_ID 13          //uint8_t
#define GW_PASSWORD "F!;Gx[+G"    //8 symbols

//Commands
#define SYN "sy"           //Initial SYN request
#define ACK "ac"           //Network join confirmation after SYN
#define ON "on"             //Turn on WiFi
#define OFF "of"             //Turn off WiFi

//Response coded
#define ERROR "er"
#define OK "ok"

//LoRa frequency channels
#define RF95_FREQ 868.1
//#define RF95_FREQ 868.3
//#define RF95_FREQ 868.5
//#define RF95_FREQ 867.1
//#define RF95_FREQ 867.3
//#define RF95_FREQ 867.5
//#define RF95_FREQ 867.7
//#define RF95_FREQ 867.9
//#define RF95_FREQ 868.8
//#define RF95_FREQ 869.525

#define RFM95_RST D0
#define RFM95_INT D1
#define RFM95_CS D8

//encryption key
unsigned char encryptKey[16] = {'E', 'B', 3, 'v', 'J', 'k', 'Y', '#', 9, 5, 'y', 's', 'W', '!', '9', 'J'};