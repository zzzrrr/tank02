#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Simple_HCSR04.h>
//請修改以下參數--------------------------------------------
char ssid[] = "無線網路SSID";
char password[] = "無線網路密碼";
String url = "http://api.thingspeak.com/update?api_key=***************"; //請修改為你自己的API Key
const int ECHO_PIN = 4; //ECHO在wemos為D2
const int TRIG_PIN = 5; //TRIG在wemos為D1

int water;

Simple_HCSR04 *sensor; //記得要先加入此函式庫Simple_HCSR04

void setup()
{
  Serial.begin(115200);
  sensor = new Simple_HCSR04(ECHO_PIN, TRIG_PIN);
  Serial.print("開始連線到無線網路SSID:");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("連線完成");
}

void loop()
{
  unsigned long distance = sensor->measure()->cm();
  int water = (130 - distance); //出水口到偵測元件的垂直高度為130cm
  delay(3000);

  if (distance > 0) {


    Serial.print("distance: ");
    Serial.print(water);
    Serial.print("cm\n");
    delay(2000);

    //開始傳送到thingspeak
    Serial.println("啟動網頁連線");
    HTTPClient http;
    //將溫度及濕度以http get參數方式補入網址後方
    String url1 = url + "&field1=" + water;
    //http client取得網頁內容
    http.begin(url1);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)      {
      //讀取網頁內容到payload
      String payload = http.getString();
      //將內容顯示出來
      Serial.print("網頁內容=");
      Serial.println(payload);
    } else {
      //讀取失敗
      Serial.println("網路傳送失敗");
    }
    http.end();
  }
//  ESP.deepSleep(150e6);  //睡眠150秒，記得寫入前先移除喚醒電路，適用18650
  delay(150000);  //休息150秒，較適用於行動電源
}
