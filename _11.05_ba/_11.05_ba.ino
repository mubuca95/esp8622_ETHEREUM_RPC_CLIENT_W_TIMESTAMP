#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
bool schalter_status = true; 
int optokuplor = D0;
int optokuplor1 = D1;

String setSchalterAus = "{\"jsonrpc\": \"2.0\",\"method\": \"eth_sendTransaction\",\"params\": [{\"data\": \"0x1c9d185c\",\"from\": \"0xf0198100cB8BbBA815aF45803Ce45562e8bF19AF\",\"to\": \"0x643adf2e4c754e33f286138d92a585f496fe0a65\"}],\"id\": 8}";
String callGeth(String inputJSON){
  HTTPClient http;    //Declare object of class HTTPClient
 
    http.begin("http://192.168.0.15:8081/");      //TRAILING SLASH AT END REQUIRED!!!
    http.addHeader("Content-Type", "application/json");  //Specify content-type header
    int httpCode = http.POST(inputJSON);   //Send the request and get http Code
    String JSONResult = http.getString();  //Get the response from Geth JSONRPC
    
    http.end();
    return JSONResult;
}
 
void setSchalterEin_func(){
  pinMode(optokuplor, OUTPUT);
  pinMode(optokuplor1, OUTPUT);
  digitalWrite(optokuplor,HIGH);
  digitalWrite(optokuplor1,LOW);
}
void setSchalterAus_func(){
  pinMode(optokuplor, OUTPUT);
  pinMode(optokuplor1, OUTPUT);
  digitalWrite(optokuplor,LOW);
  digitalWrite(optokuplor1,HIGH);
}

void setup() {
  digitalWrite(optokuplor1,LOW);
  Serial.begin(9600);
  WiFi.begin("tp-link", "12121212");
  while(WiFi.status() != WL_CONNECTED) {  
    delay(500);
    Serial.println("Waiting for connection");
  }
  Serial.println("CONNECTED");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    StaticJsonBuffer<1000> JSONbuffer;   //Declaring static JSON buffer and set high value maybe 500 per call
    JsonObject& gethQueryJSON = JSONbuffer.createObject(); 
    gethQueryJSON["jsonrpc"] = "2.0";
    gethQueryJSON["method"] = "eth_call";
    JsonArray&  gethQueryParams = gethQueryJSON.createNestedArray("params");
    JsonObject& gethCallParams = JSONbuffer.createObject();
    gethCallParams["to"] = "0x643adf2e4c754e33f286138d92a585f496fe0a65";
    gethCallParams["data"] = "0x23a67b6f";
    gethQueryParams.add(gethCallParams);
    gethQueryParams.add("latest");
    gethQueryJSON["id"] = 1;
 
    String gethStringJSON;
    gethQueryJSON.printTo(gethStringJSON);
    Serial.println("Get Schalter Status ");
    Serial.println(gethStringJSON);     
    String gethResult = callGeth(gethStringJSON);  //Get the response from Geth JSONRPC
    JsonObject& gethJSONRPC = JSONbuffer.parseObject(gethResult);
    String lightOnString = gethJSONRPC["result"];
    lightOnString.remove(0,2);
    Serial.println(lightOnString); //wert von schalter
    
    if(lightOnString == "0000000000000000000000000000000000000000000000000000000000000001"){
          StaticJsonBuffer<1000> JSONbuffer;   //Declaring static JSON buffer and set high value maybe 500 per call
          JsonObject& gethQueryJSON = JSONbuffer.createObject(); 
          gethQueryJSON["jsonrpc"] = "2.0";
          gethQueryJSON["method"] = "eth_call";
          JsonArray&  gethQueryParams = gethQueryJSON.createNestedArray("params");
          JsonObject& gethCallParams = JSONbuffer.createObject();
          gethCallParams["to"] = "0x643adf2e4c754e33f286138d92a585f496fe0a65";
          gethCallParams["data"] = "0x5a8d08a6";
          gethQueryParams.add(gethCallParams);
          gethQueryParams.add("latest");
          gethQueryJSON["id"] = 1;
       
          String gethStringJSON;
          gethQueryJSON.printTo(gethStringJSON);
          Serial.println("Get Schalter Status ");
          Serial.println(gethStringJSON);     
          String gethResult = callGeth(gethStringJSON);  //Get the response from Geth JSONRPC
          JsonObject& gethJSONRPC = JSONbuffer.parseObject(gethResult);
          String lightOnString = gethJSONRPC["result"];
          lightOnString.remove(0,2);
          Serial.println(lightOnString);
          long int lightOn = strtol(lightOnString.c_str(), NULL, 16);
          Serial.println(lightOn);
          setSchalterEin_func();
          int a = lightOn*1000*5;          
          delay(a);

          callGeth(setSchalterAus);
          delay(10);
        
          }
      delay(100);            
      setSchalterAus_func();
}
}
