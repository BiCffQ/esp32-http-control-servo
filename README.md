# esp32-http-control-servo
```cpp
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
 ```
 
 ```cpp
 if (header.indexOf("GET /?") >= 0) {
              pos1 = header.indexOf('?');              
              pos2 = header.indexOf('=');
              pos3 = header.indexOf('&');
              valueString1 = header.substring(pos1 + 1, pos2);
              valueString2 = header.substring(pos2 + 1, pos3);
}
              
```






```cpp
IPAddress staticIP(192, 168, 12, 88);
IPAddress gateway(192, 168, 12, 1);
IPAddress subnet(255, 255, 255, 0);
// 设置 Web 服务器端口号为 80
WiFiServer server(80);
```
###### http://192.168.12.88/?move=0,2000&
-----------------
###### http://192.168.12.88/?arm=back&
-----------------
###### http://192.168.12.88/?arm=front&
-----------------
###### http://192.168.12.88/?claw=open&
-----------------
###### http://192.168.12.88/?claw=close&
-----------------
###### http://192.168.12.88/?upraise=lift&
-----------------
###### http://192.168.12.88/?upraise=down&
-----------------
###### http://192.168.12.88/?color=RB&
-----------------
