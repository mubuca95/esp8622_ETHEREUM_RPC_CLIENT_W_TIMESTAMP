#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
class geth{  
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
  public: String eth_sendTransaction(String baseAdd, String targetAdd, int ethAmmount, String sensor_data){
      String baseAdd_1 = String("\"")+String(baseAdd)+String("\"");
      String targetAdd_1 = String("\"")+String(targetAdd)+String("\""); 
      String value_1 = String("\"")+String("0x9184e72a")+String("\"");
      String message_hex = String("\"0x")+str2hex(sensor_data)+String("\"");
      String s = String("{\"jsonrpc\":\"2.0\",\"method\":\"eth_sendTransaction\",\"params\":[{\"from\":")+String(baseAdd_1);
      s = s+String(",\"to\":")+String(targetAdd_1)+String(",\"gas\":\"0x76c0\",\"gasPrice\":\"0x9184e72a000\",\"value\":")+value_1+String(",\"data\":")+message_hex+String("}],\"id\":1}");
      return s;
    };        
};

void setup(){
  Serial.begin(9600);
  WiFi.begin("WLANID", "PASSWORD");
  while(WiFi.status() != WL_CONNECTED) {  
    delay(500);
    Serial.println("Waiting for connection");
  }
  Serial.println("CONNECTED");
}

void loop(){
  if(WiFi.status()== WL_CONNECTED){
    int sensor_value = random(0,1000); //random(0,1000);
    String data = "sensor:"+String(sensor_value);
    geth gethClient;
    HTTPClient http;
    http.begin("http://192.168.0.16:8545");
    http.addHeader("Content-Type","application/json");
    int httpCode = http.POST(gethClient.eth_sendTransaction("0x624e28be19f341834c79516010ceb682b0c6ae52","0x653ae78635157181c0f6e5f6354a477bada7151a",1,data));
    //int httpCode = http.POST(gethClient.eth_blockNumber());
    Serial.println(gethClient.eth_sendTransaction("0x624e28be19f341834c79516010ceb682b0c6ae52","0x653ae78635157181c0f6e5f6354a477bada7151a",1,data));
    //Serial.println(gethClient.eth_blockNumber());
    Serial.println(httpCode); 
    Serial.println("posted");
    String payload = http.getString();
    Serial.println("Response:");
    Serial.println(payload);
    http.end();
    
  }else{
    Serial.println("Error in WiFi connection"); 
  }
  delay(10000);

  
}
