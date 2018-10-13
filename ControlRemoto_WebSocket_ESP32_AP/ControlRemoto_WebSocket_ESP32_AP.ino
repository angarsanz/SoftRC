/*
  Titulo:"Control remoto coche rc basado en webSocket"
  Autor y fecha: Angarsanz - 06/09/2018
  fuentes: Se incluye contenido de "https://techtutorialsx.com/2017/12/16/esp32-arduino-async-http-server-serving-a-html-page-from-flash-memory/"
  Descirpcion: Des de el servidor web se realiza un envio de una pagian ".html" a un cliente.
              En el mismo documento se incluye un cliente WebSocket, tras realiza rle envio se activa el servidor webSocket para recivir las peticiones del cliente  
*/


#include <WiFi.h>
#include <WebSocketServer.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>


#include <WiFiClient.h> //ClienteWEB
#include <WebServer.h> //ClienteWEB
#include <ESPmDNS.h> //ClienteWEB


 
WiFiServer server(8080);
WebSocketServer webSocketServer;

WebServer serverWEB(80); //ClienteWEB
 
const char* ssid = "mando";
const char* password =  "clave1234";




//Variablres I/O y PWM
Servo servoDireccion;
int servoPin = 13;

int in1Motor = 27;
int in2Motor = 26;
int PWMMotor = 32;
int freq = 5000;

int resolution = 8;


int centroDireccion = 97; //Grados
int direccionMax = 120; // Grados
int direccionMin = 80; //Grados


/*Inicio funciones servidor web*/
void handleRoot() {
  //Esta funcion es la que envia la pagina web con el cliente WebSocket al solicitante.

  //Pagina Web que sera enviada.
  //Se compone de codigo HTML y javaScript el cual debera ser pasado por estas webs en cadena_
  //1)https://www.regextester.com/94245
  //2)https://www.willpeavy.com/minifier/
  //3)http://tomeko.net/online_tools/cpp_text_escape.php?lang=en
  //const char HTML[] = "<!DOCTYPE HTML><html> <head> <script type=\"text/javascript\">function WebSocketTest(){if (\"WebSocket\" in window){alert(\"WebSocket is supported by your Browser!\"); var ws=new WebSocket(\"ws://192.168.1.45:8080/\"); ws.onopen=function(){ws.send(\"Message to send\"); alert(\"Message is sent...\");}; ws.onmessage=function (evt){var received_msg=evt.data; alert(\"Message is received...\");}; ws.onclose=function(){alert(\"Connection is closed...\");};}else{alert(\"WebSocket NOT supported by your Browser!\");}}</script> </head> <body> <div id=\"sse\"> <a href=\"javascript:WebSocketTest()\">Run WebSocket</a> </div></body></html>";

  String paginaHTML = "<html> <head> <title>Mando coche RC/WiFi</title> <meta name=\"viewport\" content=\"width=device-width, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no\"><style type=\"text/css\">body{margin: 0px; overflow: hidden;}canvas{border: 0px solid black;}</style><script type=\"text/javascript\">var canvas;var ctx;var w=0;var h=0;var timer;var timer2;var updateStarted=false;var touches=[];var valorDireccion=0;var valorVelocidad=0;var numActiveTouch=0;function update(){if (updateStarted) return;updateStarted=true;var nw=window.innerWidth;var nh=window.innerHeight;if ((w !=nw) || (h !=nh)){w=nw;h=nh;canvas.style.width=w+'px';canvas.style.height=h+'px';canvas.width=w;canvas.height=h;resetCanvas();}ctx.clearRect(0, 0, w, h);valorDireccion=0;valorVelocidad=0;var i, len=touches.length;ctx.textAlign='left'; if(len > 0){}for (i=0; i<len; i++){var touch=touches[i];var px=touch.pageX;var py=touch.pageY;ctx.beginPath();ctx.arc(px, py, 20, 0, 2*Math.PI, true);ctx.fillStyle=\"rgba(0, 0, 200, 100)\";ctx.fill(); if(px < nw/2){ctx.font=\"30px Arial\"; var maxVelocidad=h/2; valorVelocidad=py-(h/2);valorVelocidad=(valorVelocidad*100)/maxVelocidad;valorVelocidad *=-1; if(py<(h/2)){ctx.fillText(\"F: \" +parseInt(valorVelocidad)+ \" %\", 0, 0+60);}else{ctx.fillText(\"R: \" +parseInt(valorVelocidad)+ \" %\", 0, 0+60);}}else{ctx.font=\"30px Arial\"; var maxGiro=w/4; if(px<w-(w/4)){valorDireccion=px-(w-(w/4)); valorDireccion=(valorDireccion*100)/maxGiro;ctx.fillText(\"L: \"+parseInt(valorDireccion)+ \" %\", w/2, 0+60);}else{valorDireccion=px-(w-(w/4));valorDireccion=(valorDireccion*100)/maxGiro;ctx.fillText(\"R: \"+parseInt(valorDireccion) +\" %\", w/2, 0+60);}}}ctx.textAlign='center';ctx.fillStyle=\"rgba(115, 115, 115, 100)\";ctx.font=\"30px Arial\"; ctx.fillText(\"Velocidad\", (nw/4), 30); ctx.fillText(\"Direccion\", (nw/2)+(nw/4), 30); ctx.lineWidth=3;ctx.strokeStyle=\"#f00\"; ctx.beginPath();ctx.moveTo(nw/2, 0);ctx.lineTo(nw/2, nh);ctx.stroke(); updateStarted=false;};function ol(){initComunicacion();canvas=document.getElementById('canvas');ctx=canvas.getContext('2d');timer=setInterval(update, 15); timer2=setInterval(enviar,50);resetCanvas(); canvas.addEventListener('touchend', function(){ctx.clearRect(0, 0, w, h);numActiveTouch--;console.log(\"Strop: \"+ event.targetTouches.length);toucles=[];touches=event.targetTouches;});canvas.addEventListener('touchmove', function(event){console.log(\"change: \"+event.targetTouches.length); touches=event.targetTouches;event.preventDefault();});canvas.addEventListener('touchstart', function(event){numActiveTouch++; console.log(\"start: \"+ numActiveTouch);});canvas.addEventListener('touchleave', function(event){numActiveTouch++; console.log(\"touchleave: \"+ numActiveTouch);});};function resetCanvas(){devicePixelRatio=window.devicePixelRatio || 5;canvas.width=window.innerWidth * devicePixelRatio; canvas.height=window.innerHeight * devicePixelRatio; canvas.style.width=window.innerWidth + 'px'; canvas.style.height=window.innerHeight + 'px'; ctx.scale(devicePixelRatio, devicePixelRatio);}var ws;function initComunicacion(){if (\"WebSocket\" in window){ws=new WebSocket(\"ws://";
  paginaHTML+=WiFi.softAPIP().toString()+":8080/\"); ws.onopen=function (){ws.send(\"Mando conectado\");};}else{alert(\"El explorador no soporta WebSocket !\");}}function enviar(){var orden={velocidad: parseInt(valorVelocidad),direccion: parseInt(valorDireccion),luces: 0}; if(ws.readyState===1){ws.send(JSON.stringify(orden));}else{console.log(\"todavia no\");}}</script></head><body onload=\"ol()\"><canvas id=\"canvas\" width=\"100\" height=\"100\" style=\"top:0px; left:0px; width: 100; height: 100;\"></canvas><object id=\"tuio\" type=\"application/x-tuio\">Plugin FAILED to load</object></body></html>";

 
  char HTML[paginaHTML.length()];
  paginaHTML.toCharArray(HTML,paginaHTML.length());
  //Realiza el envio de la pagina
  serverWEB.send(200, "text/html", HTML); 
}

void handleNotFound() {
  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += serverWEB.uri();
  message += "\nMethod: ";
  message += (serverWEB.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += serverWEB.args();
  message += "\n";
  for (uint8_t i = 0; i < serverWEB.args(); i++) {
    message += " " + serverWEB.argName(i) + ": " + serverWEB.arg(i) + "\n";
  }
  serverWEB.send(404, "text/plain", message);
  
}
/*Final funciones servidor web*/

void setup() {
 
  Serial.begin(115200);
 
  WiFi.softAP(ssid, password); 

  Serial.println(WiFi.softAPIP());
 
 
 
  Serial.println("Connected to the WiFi network");

 
  server.begin();
  delay(100);

  //Server WEB

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  serverWEB.on("/", handleRoot);

  serverWEB.on("/inline", []() {
    serverWEB.send(200, "text/plain", "this works as well");
  });

  serverWEB.onNotFound(handleNotFound);

  serverWEB.begin();
  Serial.println("HTTP server started");

  //Codigo I/O y PWM
  servoDireccion.attach(servoPin);

  pinMode(in1Motor,OUTPUT); //determina el sentido de giro
  pinMode(in2Motor,OUTPUT); //determina el sentido de giro

  ledcAttachPin(PWMMotor, 3); //Determina la vlocida de giro
  ledcSetup(3, freq, resolution); //Determina la vlocida de giro
  


}

void interpretarMensaje(String datos){

  StaticJsonDocument<500> doc;
  
  DeserializationError error = deserializeJson(doc,datos);
  if (error) {   //Check for errors in parsing
   
      Serial.println("Parsing failed");
      return;
   
  }
  JsonObject parsed = doc.as<JsonObject>(); //Parse message
  
  int velocidad = parsed["velocidad"];
  int direccion = parsed["direccion"];
  Serial.print("V: ");
  Serial.print(velocidad);
  Serial.print("% ");
  Serial.print("D");
  Serial.print(direccion);
  Serial.print("%");
  
  int valorDireccion=0;

  if(direccion>100){
      direccion=100;
  }
  if(direccion<-100){
      direccion=-100;
  }
   
  
  if(direccion>0){
    valorDireccion=map(direccion,0,100,centroDireccion,direccionMax);
  }else{
    valorDireccion=map(direccion,-100,0,direccionMin,centroDireccion);
  }
  servoDireccion.write(valorDireccion);//Actua soble el servomotor

  int valorVelocidad=0;
  
  if(velocidad>0){
    digitalWrite(in1Motor,HIGH);
    digitalWrite(in2Motor,LOW);
  }else{
    digitalWrite(in1Motor,LOW);
    digitalWrite(in2Motor,HIGH);
    velocidad*=-1;
  }
  valorVelocidad=map(velocidad,0,100,0,255);
  ledcWrite(3, valorVelocidad);//Actua soble el puente H

  
  Serial.print(valorDireccion);
  Serial.println("");
}



void loop() {

 // WiFiClient cliente = server.available();
 // Serial.println(">");
 // if(cliente){
   // Serial.println("hay un cloente conectado");
    serverWEB.handleClient();
    WiFiClient client = server.available();
   
    if (client.connected() && webSocketServer.handshake(client)) {
   
      String data;      
   
      while (client.connected()) {
   
        data = webSocketServer.getData();
   
        if (data.length() > 0) {
          // Serial.println(data);
          interpretarMensaje(data);
           //ledcWrite(1,data.toInt());//Actua soble la salida PWM
           webSocketServer.sendData(data);
        }
  
        delay(10); // Delay needed for receiving the data correctly
     }
   
     Serial.println("The client disconnected");
     delay(100);
    }
 // }
  delay(100);
}
