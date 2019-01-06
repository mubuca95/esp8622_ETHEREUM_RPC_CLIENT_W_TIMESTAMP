#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "tekir-wlan";
const char* password = "12121212";

WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP);
// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
 NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

String formattedDate;
String dayStamp;
String timeStamp;

String eth_data(String Name, String Value, String Timestamp){
    String s = "{\"data\":{\"name\":\""+Name+"\",\"value\":\""+Value+"\",\"timestamp\":\""+Timestamp+"\"}}";
    return s;
}
  

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  timeClient.begin();
}

void loop() {
  while(!timeClient.update()) {
    int c;
    for(int c = 0 ; c < 5; c++){
      timeClient.forceUpdate();
      Serial.println("forcing update");}
    break;
  }
  /*formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("T");
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  */
  long timeStamp = timeClient.getEpochTime();
  Serial.println(timeStamp);
  Serial.println(eth_data("sensor","12213123","12:39"));
  delay(1000);

}
