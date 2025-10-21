//192.168.1.6
#include <WiFi.h>
#include <ESP32Servo.h>

Servo ObjServo1; // Make object of Servo motor from Servo library
Servo ObjServo2;
Servo ObjServo3;
Servo ObjServo4;
Servo ObjServo5;
Servo ObjServo6;

// Objects are made for every servo motor we want to control through this library
static const int ServoGPIO1 = 33; // define the GPIO pin with which servo 1 is connected
static const int ServoGPIO2 = 25;
static const int ServoGPIO3 = 26;
static const int ServoGPIO4 = 27;
static const int ServoGPIO5 = 14;
static const int ServoGPIO6 = 12;


// Variables to store network name and password
const char* ssid = ""; // Enter your network name
const char* password = ""; // Enter your network password
// Set the server port number to default 80

WiFiServer server(80);
// Variables to store slider positions
String valueString1 = String(0);
String valueString2 = String(0);
String valueString3 = String(0);
String valueString4 = String(0);
String valueString5 = String(0);
String valueString6 = String(0);
void setup() {
  Serial.begin(115200); // Define serial communication with baud rate of 115200
  ObjServo1.attach(ServoGPIO1); // Attach ObjServo1 to ServoGPIO1 pin
  ObjServo2.attach(ServoGPIO2);
  ObjServo3.attach(ServoGPIO3);
  ObjServo4.attach(ServoGPIO4);
  ObjServo5.attach(ServoGPIO5);
  ObjServo6.attach(ServoGPIO6);
  Serial.print("Making connection to "); // Display message on serial monitor
  Serial.println(ssid);
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print the IP address value on serial monitor
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin(); // Start the server
}
void loop() {
  WiFiClient client = server.available();                                    // Listen for incoming clients
  if (client) {                                                                                    // If a new client connects
    String header = client.readStringUntil('\r');
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    // Display the HTML web page
    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\">");
    client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
    client.println(".slider { width: 300px; }");
    client.println(".button { font-size: 24px; padding: 10px 20px; }</style>");
    client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
    client.println("</head><body>");
    //Slider 1
    client.println("<h1>ESP32 RoboArm Conrol</h1>");
    client.println("<p>Position 1: <span id=\"servoPos1\"></span></p>");
    client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider1\" onchange=\"servo(this.value, 1)\" value=\"" + valueString1 + "\"/>");
    client.println("<button class=\"button\" onclick=\"decrementValue(1)\"style=\"font-size: 24px; padding: 10px 20px;\">-</button>");
    client.println("<button class=\"button\" onclick=\"incrementValue(1)\"style=\"font-size: 24px; padding: 10px 20px;\">+</button>");
    // Slider 2
    client.println("<p>Position 2: <span id=\"servoPos2\"></span></p>");
    client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider2\" onchange=\"servo(this.value, 2)\" value=\"" + valueString2 + "\"/>");
    client.println("<button class=\"button\" onclick=\"decrementValue(2)\"style=\"font-size: 24px; padding: 10px 20px;\">-</button>");
    client.println("<button class=\"button\" onclick=\"incrementValue(2)\"style=\"font-size: 24px; padding: 10px 20px;\">+</button>");
    // Slider 3
    client.println("<p>Position 3: <span id=\"servoPos3\"></span></p>");
    client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider3\" onchange=\"servo(this.value, 3)\" value=\"" + valueString3 + "\"/>");
    client.println("<button class=\"button\" onclick=\"incrementValue(3)\"style=\"font-size: 24px; padding: 10px 20px;\">-</button>");
    client.println("<button class=\"button\" onclick=\"decrementValue(3)\"style=\"font-size: 24px; padding: 10px 20px;\">+</button>");
    // Slider 4
    client.println("<p>Position 4: <span id=\"servoPos4\"></span></p>");
    client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider4\" onchange=\"servo(this.value, 4)\" value=\"" + valueString4 + "\"/>");
    client.println("<button class=\"button\" onclick=\"incrementValue(4)\"style=\"font-size: 24px; padding: 10px 20px;\">-</button>");
    client.println("<button class=\"button\" onclick=\"decrementValue(4)\"style=\"font-size: 24px; padding: 10px 20px;\">+</button>");
    // Slider 5
    client.println("<p>Position 5: <span id=\"servoPos5\"></span></p>");
    client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider5\" onchange=\"servo(this.value, 5)\" value=\"" + valueString5 + "\"/>");
    client.println("<button class=\"button\" onclick=\"decrementValue(5)\"style=\"font-size: 24px; padding: 10px 20px;\">-</button>");
    client.println("<button class=\"button\" onclick=\"incrementValue(5)\"style=\"font-size: 24px; padding: 10px 20px;\">+</button>");
    // Slider 6
    client.println("<p>Position 6: <span id=\"servoPos6\"></span></p>");
    client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider6\" onchange=\"servo(this.value, 6)\" value=\"" + valueString6 + "\"/>");
    client.println("<button class=\"button\" onclick=\"decrementValue(6)\"style=\"font-size: 24px; padding: 10px 20px;\">-</button>");
    client.println("<button class=\"button\" onclick=\"incrementValue(6)\"style=\"font-size: 24px; padding: 10px 20px;\">+</button>");
    client.println("<script>");
    client.println("var slider1 = document.getElementById(\"servoSlider1\");");
    client.println("var servoP1 = document.getElementById(\"servoPos1\");");
    client.println("slider1.oninput = function() { servoP1.innerHTML = this.value; }");
    client.println("var slider2 = document.getElementById(\"servoSlider2\");");
    client.println("var servoP2 = document.getElementById(\"servoPos2\");");
    client.println("slider2.oninput = function() { servoP2.innerHTML = this.value; }");
    client.println("var slider3 = document.getElementById(\"servoSlider3\");");
    client.println("var servoP3 = document.getElementById(\"servoPos3\");");
    client.println("slider3.oninput = function() { servoP3.innerHTML = this.value; }");
    client.println("var slider4 = document.getElementById(\"servoSlider4\");");
    client.println("var servoP4 = document.getElementById(\"servoPos4\");");
    client.println("slider4.oninput = function() { servoP4.innerHTML = this.value; }");
    client.println("var slider5 = document.getElementById(\"servoSlider5\");");
    client.println("var servoP5 = document.getElementById(\"servoPos5\");");
    client.println("slider5.oninput = function() { servoP5.innerHTML = this.value; }");
    client.println("var slider6 = document.getElementById(\"servoSlider6\");");
    client.println("var servoP6 = document.getElementById(\"servoPos6\");");
    client.println("slider6.oninput = function() { servoP6.innerHTML = this.value; }");
    client.println("$.ajaxSetup({timeout: 1000});");
    client.println("function servo(pos, servoNum) {");
    client.println("$.get(\"/?servo=\" + servoNum + \"&value=\" + pos + \"&\");");
    client.println("}");
    client.println("function incrementValue(servoNum) {");
    client.println("var slider = document.getElementById(\"servoSlider\" + servoNum);");
    client.println("slider.value = parseInt(slider.value) + 5;");
    client.println("servo(slider.value, servoNum);");
    client.println("}");
    client.println("function decrementValue(servoNum) {");
    client.println("var slider = document.getElementById(\"servoSlider\" + servoNum);");
    client.println("slider.value = parseInt(slider.value) - 5;");
    client.println("servo(slider.value, servoNum);");
    client.println("}");  
    client.println("</script>");
    client.println("</body></html>");
     if (header.indexOf("GET /?servo=") >= 0) {
      int servoPosition = header.indexOf("servo=") + 6;
      int valuePosition = header.indexOf("&value=");
      int servoNum = header.substring(servoPosition, valuePosition).toInt();
      int value = header.substring(valuePosition + 7).toInt();
      // Set the servo position
      setServoPosition(servoNum, value);
    }
    client.stop(); // Disconnect the client
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
void setServoPosition(int servoNum, int value) {
  switch (servoNum) {
    case 1:
      ObjServo1.write(value);
      valueString1 = String(value);
      break;
    case 2:
      ObjServo2.write(value);
      valueString2 = String(value);
      break;
    case 3:
      ObjServo3.write(value);
      valueString3 = String(value);
      break;
    case 4:
      ObjServo4.write(value);
      valueString4 = String(value);
      break;
    case 5:
      ObjServo5.write(value);
      valueString5 = String(value);
      break;
    case 6:
      ObjServo6.write(value);
      valueString6 = String(value);
      break;
  }
}