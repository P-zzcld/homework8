#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Magic7";
const char* password = "123shitou";
const int LED_PIN = 2;    // 板载LED引脚
const int TOUCH_PIN = T0; // 触摸引脚GPIO4

WebServer server(80);
bool armState = false;
bool alarmLock = false;

String makePage() {
  String statusText;
  if(!armState) statusText = "未布防";
  else if(alarmLock) statusText = "布防中，报警已锁定";
  else statusText = "布防待机（触摸引脚触发报警）";

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>作业8 安防报警主机</title>
</head>
<body style="font-family:Arial; text-align:center; margin-top:60px;">
  <h1>物联网安防报警器</h1>
  <p style="font-size:22px;">当前系统状态：<b>)rawliteral" + statusText + R"rawliteral(</b></p>
  <br>
  <button onclick="fetch('/arm');location.reload()" style="padding:14px 32px; font-size:20px; margin:10px;">布防 Arm</button>
  <button onclick="fetch('/disarm');location.reload()" style="padding:14px 32px; font-size:20px; margin:10px;">撤防 Disarm</button>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleArm() {
  armState = true;
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleDisarm() {
  armState = false;
  alarmLock = false;
  digitalWrite(LED_PIN, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("正在连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(WiFi.status() == WL_CONNECT_FAILED){
      Serial.println("\nWiFi密码错误！");
    }else if(WiFi.status() == WL_NO_SSID_AVAIL){
      Serial.println("\n找不到热点 Magic7！");
    }
  }
  Serial.println("\nWiFi连接成功");
  Serial.print("网页访问地址：http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

void loop() {
  server.handleClient();
  uint16_t touchValue = touchRead(TOUCH_PIN);

  if(armState && !alarmLock && touchValue < 500){
    alarmLock = true;
  }

  if(alarmLock){
    digitalWrite(LED_PIN, HIGH);
    delay(80);
    digitalWrite(LED_PIN, LOW);
    delay(80);
  }
}