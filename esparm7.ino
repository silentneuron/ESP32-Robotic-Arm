/* 
nrml 3DOF robot with Arduino ide with web control by silentneuron
dwnld the required libraries might have some problems downloading and running 
might have to dwnld specific version of libraries like ESPAsyncWebServer one
*/


#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>
#include <iostream>
#include <sstream>
#include <vector>


struct ServoPins
{
  Servo servo;
  int servoPin;
  String servoName;
  int initialPosition;  
};
std::vector<ServoPins> servoPins = 
{
  { Servo(), 33 , "Base", 90},
  { Servo(), 27 , "Shoulder", 90},
  { Servo(), 25 , "Elbow", 90},
  { Servo(), 14 , "Gripper", 90},
};



// put all ur respective pin number less work on wiring!



const char* ssid     = "RobotArm3";
const char* password = "123456789";// very secure password 

// put the ssid and password dont forget them, (unlike how she forgot u) the ssid is supposed to spawn in ur device when running



AsyncWebServer server(80);
AsyncWebSocket wsRobotArmInput("/RobotArmInput");



// html arc starts from here i have kept it simple so the esp32 doesn't get cooked (too lazy) u can change it however u like


const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
    <style>
    input[type=button]

    {
      background-color:red;color:white;border-radius:30px;width:100%;height:40px;font-size:20px;text-align:center;
    } 


   // put any colour u like

    .noselect {
      -webkit-touch-callout: none;
      -webkit-user-select: none;
       -khtml-user-select: none;
         -moz-user-select: none;
          -ms-user-select: none;
              user-select: none;
    }

    //bit of UI

    .slidecontainer {
      width: 100%;
    }
    .slider {
      -webkit-appearance: none;
      width: 100%;
      height: 20px;
      border-radius: 5px;
      background: #d3d3d3;
      outline: none;
      opacity: 0.7;
      -webkit-transition: .2s;
      transition: opacity .2s;
    }
    .slider:hover {
      opacity: 1;
    }
    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 40px;
      height: 40px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }
    .slider::-moz-range-thumb {
      width: 40px;
      height: 40px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }


    </style>
  </head>
  <body class="noselect" align="center" style="background-color:white">

     // give any name u like or background colour 

    <h1 style="color: teal;text-align:center;">3DOF Robotic arm by silentneuron</h1>
    <table id="mainTable" style="width:400px;margin:auto;table-layout:fixed" CELLSPACING=10>
      <tr/><tr/>
      <tr/>
      <tr/>
      <tr>
        <td style="text-align:left;font-size:25px"><b>Gripper:</b></td> //gripper part
        <td colspan=2>



         <div class="slidecontainer">
            <input type="range" min="0" max="180" value="90" class="slider" id="Gripper" oninput='sendButtonInput("Gripper",value)'>
          </div>
        </td>
      </tr> 
      <tr/><tr/>
      <tr>
        <td style="text-align:left;font-size:25px"><b>Elbow:</b></td> // elbow part
        <td colspan=2>


         <div class="slidecontainer">
            <input type="range" min="0" max="180" value="90" class="slider" id="Elbow" oninput='sendButtonInput("Elbow",value)'>
          </div>
        </td>
      </tr> 
      <tr/><tr/>      
      <tr>
        <td style="text-align:left;font-size:25px"><b>Shoulder:</b></td> // shoulder part
        <td colspan=2>



         <div class="slidecontainer">
            <input type="range" min="0" max="180" value="90" class="slider" id="Shoulder" oninput='sendButtonInput("Shoulder",value)'>
          </div>
        </td>
      </tr>  
      <tr/><tr/>      
      <tr>
        <td style="text-align:left;font-size:25px"><b>Base:</b></td> // base part
        <td colspan=2>




         <div class="slidecontainer">
            <input type="range" min="0" max="180" value="90" class="slider" id="Base" oninput='sendButtonInput("Base",value)'>
          </div>
        </td>
      </tr> 
    </table>


  
    <script>
      var webSocketRobotArmInputUrl = "ws:\/\/" + window.location.hostname + "/RobotArmInput";      
      var websocketRobotArmInput;
      
      function initRobotArmInputWebSocket() 
      {
        websocketRobotArmInput = new WebSocket(webSocketRobotArmInputUrl);
        websocketRobotArmInput.onopen    = function(event){};
        websocketRobotArmInput.onclose   = function(event){setTimeout(initRobotArmInputWebSocket, 2000);};
      }
      
      function sendButtonInput(key, value) 
      {
        var data = key + "," + value;
        websocketRobotArmInput.send(data);
      }
            
      window.onload = initRobotArmInputWebSocket;
      document.getElementById("mainTable").addEventListener("touchend", function(event){
        event.preventDefault()
      });      
    </script>
  </body>    
</html>
)HTMLHOMEPAGE";



void handleRoot(AsyncWebServerRequest *request) 
{
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "File Not Found");
}

void sendCurrentRobotArmState()
{
  for (int i = 0; i < servoPins.size(); i++)
  {
    wsRobotArmInput.textAll(servoPins[i].servoName + "," + servoPins[i].servo.read());
  }
}

void writeServoValues(int servoIndex, int value)
{
  servoPins[servoIndex].servo.write(value);   
}

void onRobotArmInputWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client, 
                      AwsEventType type,
                      void *arg, 
                      uint8_t *data, 
                      size_t len) 


{                      
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      sendCurrentRobotArmState();
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
    {
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        std::istringstream ss(myData);
        std::string key, value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');
        Serial.printf("Key [%s] Value[%s]\n", key.c_str(), value.c_str()); 
        int valueInt = atoi(value.c_str()); 
        
        if (key == "Base")
        {
          writeServoValues(0, valueInt);            
        } 
        else if (key == "Shoulder")
        {
          writeServoValues(1, valueInt);           
        } 
        else if (key == "Elbow")
        {
          writeServoValues(2, valueInt);           
        }         
        else if (key == "Gripper")
        {
          writeServoValues(3, valueInt);     
        }     
      }
      break;
    }
    default:
      break;  
  }
}



void setUpPinModes()
{
  for (int i = 0; i < servoPins.size(); i++)
  {
    servoPins[i].servo.attach(servoPins[i].servoPin);
    servoPins[i].servo.write(servoPins[i].initialPosition);    
  }
}

// terminal side, IP will be required for OpenCV and all

void setup(void) 
{
  setUpPinModes();
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
      
  wsRobotArmInput.onEvent(onRobotArmInputWebSocketEvent);
  server.addHandler(&wsRobotArmInput);

  server.begin();
  Serial.println("HTTP server started");

}



void loop() 
{
  wsRobotArmInput.cleanupClients();
}



// Thank u! please say if any changes required!