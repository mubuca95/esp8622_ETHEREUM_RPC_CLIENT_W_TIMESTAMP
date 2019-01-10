#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "coap_client.h"
#include "EmonLib.h"
#include <WiFiUDP.h>
#include <NTPClient.h>
#include <ESP8266HTTPClient.h>

class geth{  
   /* Converts utf-8 String to HEXADECIMAL form(A ascii 65 -> HEX 42 )*/
  public: String str2hex(String s){
    String s_hex;
    char str[(s.length()+1)];
    s.toCharArray(str,sizeof(str));
      for(int c = 0; c < sizeof(str)-1; c++){
        int a = (int)str[c];
        s_hex += String(a,HEX);
      }
    return s_hex;
    }  
  public: String eth_blockNumber(){
    String s = "{\"jsonrpc\":\"2.0\",\"method\":\"eth_blockNumber\",\"params\":[],\"id\":1}";
    return s;
  };

  /* Creates data form for consistency between other scripts in project  */
  public: String eth_data(String Name, String Value, String Timestamp){
    String s = "{\"data\":{\"name\":\""+Name+"\",\"value\":\""+Value+"\",\"timestamp\":\""+Timestamp+"\"}}";
    return s;
  }
  /* Send Transaction */
  public: String eth_sendTransaction(String baseAdd, String targetAdd, int ethAmmount, String sensor_data){
      String baseAdd_1 = String("\"")+String(baseAdd)+String("\"");
      String targetAdd_1 = String("\"")+String(targetAdd)+String("\""); 
      String value_1 = String("\"")+String("0x9184e72a")+String("\"");
      //String value_1 = String("\"")+ethAmmount+String("\"");
      String message_hex = String("\"0x")+str2hex(sensor_data)+String("\"");
      String s = String("{\"jsonrpc\":\"2.0\",\"method\":\"eth_sendTransaction\",\"params\":[{\"from\":")+String(baseAdd_1);
      s = s+String(",\"to\":")+String(targetAdd_1)+String(",\"gas\":\"0x76c0\",\"gasPrice\":\"0x9184e72a000\",\"value\":")+value_1+String(",\"data\":")+message_hex+String("}],\"id\":1}");
      return s;
    };        
};

//instance for coapclient
coapClient coap;
WiFiUDP Client;
EnergyMonitor emon1;

WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP);
// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, "192.168.137.145", 0, 60000);

//WiFi connection info
const char* ssid = "tekir-wlan";
const char* password = "12121212";
String DEVICE_SECRET_KEY  = "d_sk_u9VmUPktEAtNMLj16yiCnNNG";

WiFiClient client;
double inputVal = 0;
//ip address and default port of coap server in which your interested in
IPAddress ip(52, 17, 209, 228);
int port = 5683;
char* path = "events";

StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();

// coap client response callback
void callback_response(coapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;

  //response from coap server
  if (packet.type == 3 && packet.code == 0) {
    Serial.println("ping ok");
  }

  Serial.println(p);
}

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println(" ");
  emon1.current(0,29); //Current: input pin, calibration
  // Connection info to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    yield();
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address of client
  Serial.println(WiFi.localIP());
  Serial.print("connecting to ");
  
  //Client.begin(81);

  

  // client response callback.
  // this endpoint is single callback.
  coap.response(callback_response);

  // start coap client
  coap.start();
  timeClient.begin();
  
}

void loop() {  
  //timeClient.update();
  geth gethClient;
  double Irms = emon1.calcIrms(200);
  root["name"] = "current30A";
  root["data"] =  Irms;
  root["accessToken"] =  DEVICE_SECRET_KEY;
  //geth start
  long Timestamp = timeClient.getEpochTime();
  String eth_data = gethClient.eth_data("current30A", String (Irms), String (Timestamp));
  HTTPClient http;
  http.begin("http://192.168.137.145:8081");
  http.addHeader("Content-Type","application/json");
  int httpCode = http.POST(gethClient.eth_sendTransaction("0x09bcf739fb7c2b3ba6f6c65ce2343e92b6e212db", "0x4f6fa0b0393988f7cb488954a9da6ef0e2670085", 0, eth_data));
  Serial.println(httpCode); 
  Serial.println("posted");
  String payload = http.getString();
  Serial.println("HTTP Response:");
  Serial.println(payload);
  //geth end
  String data;
  root.printTo(data);
  char dataChar[data.length() + 1];
  data.toCharArray(dataChar, data.length() + 1);
  bool state;
  String Stringdata = String(Irms, 10);
  
  //New Char Array for UDP Comms
  char udp_Array[10];
  Stringdata.toCharArray(udp_Array, 10);
  Serial.println(Stringdata);
  
  //send UDP Packet
  udpsend(udp_Array);
  //udpsend2(udp_Array);
  //post CoAP request
  //arguments server ip address,default port,resource name, payload,payloadlength
  //int msgid = coap.post(ip, port, path, dataChar, data.length());
  //state = coap.loop();
  
  delay(8);
}

void udpsend(char a[]){
    const char ip[]="172.17.33.199";
  Client.beginPacket(ip,80);
  Client.write(a, 9);
  Client.endPacket();
  
  return;
}

void udpsend2(char a[]){
  const char ip[] = "172.17.33.199";
  Client.beginPacket(ip,90);
  Client.write(a,9);
  Client.endPacket();
}
