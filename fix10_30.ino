//#include <Servo.h>
#include <ESP32Servo.h>
#include <WiFi.h>

// 创建伺服对象来控制伺服
static const int claw_Pin = 25;     //夹子
static const int arm_Pin = 33;      //旋转
static const int upraise_Pin = 32;  //抬升
static const int upraise_lift = 1000;
static const int upraise_down = 1500;
static const int arm_back = 700;
static const int arm_front = 2350;
static const int claw_open = 1700;
static const int claw_close = 2000;
static const int AA = 16;
static const int BB = 17;
Servo Servo_claw;
Servo Servo_arm;
Servo Servo_upraise;
unsigned char color_Red[6] = {0X7E, 0X04, 0X41, 0X00, 0X01, 0XEF};
unsigned char color_Green[6] = {0X7E, 0X04, 0X41, 0X00, 0X02, 0XEF};
unsigned char color_Blue[6] = {0X7E, 0X04, 0X41, 0X00, 0X03, 0XEF};
// 替换为您的网络凭据

const char* ssid = "Miku";           //你的无线账号
const char* password = "450881538";  //你的无线账号密码
IPAddress staticIP(192, 168, 12, 88);
IPAddress gateway(192, 168, 12, 1);
IPAddress subnet(255, 255, 255, 0);
// 设置 Web 服务器端口号为 80
WiFiServer server(80);

// 用于存储 HTTP 请求的变量
String header;

// 解码 HTTP GET 值
String valueString1;
String valueString2;
String valueString3;
int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
int pos4 = 0;

// 当前时间
unsigned long cuBBentTime = millis();
// 上一次
unsigned long previousTime = 0;
// 以毫秒为单位定义超时时间（例如：2000ms = 2s）
const long timeoutTime = 2000;



void ServoControl(String servo, String posDegrees) {
  int Degrees = 1500;
  if (posDegrees == "lift") {
    Degrees = upraise_lift;
  }
  if (posDegrees == "down") {
    Degrees = upraise_down;
  }
  if (posDegrees == "front") {
    Degrees = arm_front;
  }
  if (posDegrees == "back") {
    Degrees = arm_back;
  }
  if (posDegrees == "open") {
    Degrees = claw_open;
  }
  if (posDegrees == "close") {
    Degrees = claw_close;
  }
  if (servo == "claw") {
    Servo_claw.writeMicroseconds(Degrees);
  }

  if (servo == "arm") {
    Servo_arm.writeMicroseconds(Degrees);
  }

  if (servo == "upraise") {
    Servo_upraise.writeMicroseconds(Degrees);
  }
}
void GD3800(String colors) {
  if (colors == "RB") {
    Serial.write(color_Red, 6);
    delay(700);
    Serial.write(color_Blue, 6);
  }
  if (colors == "RG") {
    Serial.write(color_Red, 6);
    delay(700);
    Serial.write(color_Green, 6);
  }
  if (colors == "BR") {
    Serial.write(color_Blue, 6);
    delay(700);
    Serial.write(color_Red, 6);
  }
  if (colors == "BG") {
    Serial.write(color_Blue, 6);
    delay(700);
    Serial.write(color_Green, 6);
  }
  if (colors == "GR") {
    Serial.write(color_Green, 6);
    delay(700);
    Serial.write(color_Red, 6);
  }
  if (colors == "GB") {
    Serial.write(color_Green, 6);
    delay(700);
    Serial.write(color_Blue, 6);
  }
}
void Mecarun(int16_t velx, int16_t vely, int16_t velr)
{

  uint8_t check_sum = 0;
  uint8_t data[15];
  data[0] = '$';
  data[1] = '$';
  data[2] = 0x05;
  data[3] = 0x01;
  data[4] = velx;
  data[5] = velx >> 8;
  data[6] = vely;
  data[7] = vely >> 8;
  data[8] = velr;
  data[9] = velr >> 8;
  data[10] = 0;
  data[11] = 0;

  for (int i = 2; i < 12; ++i)
  {
    check_sum += data[i];
  }
  data[12] = check_sum;

  data[13] = '\r';
  data[14] = '\n';

  for (int i = 0; i < 15; i++)
  {
    Serial2.write(data[i]);
  }
}
void mecar(String x, String y) {
  Serial2.println("p x" + x + " y" + y);
  Serial.println("p x" + x + " y" + y);
}
void L298(int Times, bool fangxiang) {
  if (fangxiang == 0) {
    digitalWrite(BB, LOW);
    delay(Times);
    digitalWrite(BB, HIGH);
  }
  if (fangxiang == 1) {
    digitalWrite(AA, LOW);
    delay(Times);
    digitalWrite(AA, HIGH);
  }
}
void setup() {
  Serial.begin(9600);
  //  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  pinMode(AA, OUTPUT);
  pinMode(BB, OUTPUT);
  digitalWrite(AA, HIGH);
  digitalWrite(BB, HIGH);
  Servo_claw.attach(claw_Pin);
  Servo_arm.attach(arm_Pin);
  Servo_upraise.attach(upraise_Pin);

  if (WiFi.config(staticIP, gateway, subnet) == false) {
    Serial.println("Configuration failed.");
  }
  // 使用 SSID 和密码连接到 Wi-Fi 网络
  // Serial.print("Connecting to ");
  // Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //打印本地IP地址并启动Web服务器
  // Serial.println("");
  // Serial.println("WiFi connected.");
  // Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Serial.println(WiFi.subnetMask());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();  //

  if (client) {  //
    cuBBentTime = millis();
    previousTime = cuBBentTime;
    // Serial.println("New Client.");                                             //
    String cuBBentLine = "";                                                   //
    while (client.connected() && cuBBentTime - previousTime <= timeoutTime) {  //在客户端连接时循环
      cuBBentTime = millis();
      if (client.available()) {  //
        char c = client.read();  //
        // Serial.write(c);         //
        header += c;
        if (c == '\n') {  //
          //如果当前行为空，则连续有两个换行符。
          //这就是客户端 HTTP 请求的结束，所以发送一个响应：
          if (cuBBentLine.length() == 0) {
            // HTTP 标头始终以响应代码开头（例如 HTTP/1.1 200 OK）
            //和一个内容类型，以便客户端知道接下来会发生什么，然后是一个空行：
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();


            //GET /?value=180& HTTP/1.1
            // if (header.indexOf("GET /?value=") >= 0) {
            //   pos1 = header.indexOf('=');
            //   pos2 = header.indexOf('&');
            //   valueString = header.substring(pos1 + 1, pos2);

            //   //旋转舵机
            //   // myservo.write(valueString.toInt());
            //   Serial.println(valueString);
            // }

            if (header.indexOf("GET /?") >= 0) {
              pos1 = header.indexOf('?');
              pos2 = header.indexOf('=');
              pos3 = header.indexOf('&');
              valueString1 = header.substring(pos1 + 1, pos2);

              valueString2 = header.substring(pos2 + 1, pos3);
              if (valueString1 || "claw" || "arm" || "upraise") {
                ServoControl(valueString1, valueString2);
              }
              if (valueString1 == "color") {
                GD3800(valueString2);
              }
              if (valueString1 == "move")
              {
                pos3 = header.indexOf('_');
                pos4 = header.indexOf('&');
                valueString2 = header.substring(pos2 + 1, pos3);
                valueString3 = header.substring(pos3 + 1, pos4);
                // Mecarun(valueString2.toInt(), valueString3.toInt(), 0);
                // Serial.println(valueString2 + valueString3);
                L298(valueString2.toInt(), valueString3.toInt());
                // mecar(valueString2, valueString3);

              }

            }
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else {  // if you got a newline, then clear cuBBentLine
            cuBBentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a caBBiage return character,
          cuBBentLine += c;      // add it to the end of the cuBBentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    // Serial.println("Client disconnected.");
    // Serial.println("");
  }
}
