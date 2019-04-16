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
const char* password =  "NUUHPBOY";

//const char* ssid = "MOVISTAR_C0F0";
//const char* password =  "NUUHPBOY";


//Variablres I/O y PWM
Servo servoDireccion;
int servoPin = 13;

int in1Motor = 25;
int in2Motor = 33;
int PWMMotor = 26;

int freq = 35000;
int resolution = 8;

// sevo channel 0
int channel_motor = 3;
int channel_tailLight = 4;
int channel_headLamp = 5;

int centroDireccion = 97; //Grados
int direccionMax = 120; // Grados
int direccionMin = 80; //Grados


int headLamp_PWM = 23;
int tailLight_PWM = 22;


/*Inicio funciones servidor web*/
void handleRoot() {
  //Esta funcion es la que envia la pagina web con el cliente WebSocket al solicitante.

   extern const char mando_html[];
 //extern const char mando_html_Barco[];
 
//  char HTML[paginaHTML.length()];
//  paginaHTML.toCharArray(HTML,paginaHTML.length());
  //Realiza el envio de la pagina
  serverWEB.send(200, "text/html", mando_html); //HTML por 
 // serverWEB.send(200, "text/html", mando_html_Barco); //HTML por 
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

  //////////Server WEB////////

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

  //////////Codigo I/O y PWM ////////
  servoDireccion.attach(servoPin);

  pinMode(in1Motor,OUTPUT); //determina el sentido de giro
  pinMode(in2Motor,OUTPUT); //determina el sentido de giro

  //channel_motor = 3
  ledcAttachPin(PWMMotor, channel_motor); //Determina la vlocida de giro
  ledcSetup(channel_motor, freq, resolution); //Determina la vlocida de giro


  
  //////////Acesories 
  
  //channel_headLamp =5
  ledcAttachPin(headLamp_PWM, channel_headLamp); 
  ledcSetup(channel_headLamp, freq, resolution); 

  //channel_tailLight=4
  ledcAttachPin(tailLight_PWM, channel_tailLight); 
  ledcSetup(channel_tailLight, freq, resolution); 


}

void luces_ON(){

  ledcWrite(channel_headLamp, 250);
  ledcWrite(channel_tailLight, 125);
  
}

void luces_OFF(){

  ledcWrite(channel_headLamp, 0);
  ledcWrite(channel_tailLight, 0);
  
}

void lucesFrenado_ON(){
  ledcWrite(channel_tailLight, 254);
}

void lucesFrenado_OFF(){
  ledcWrite(channel_tailLight, 125); 
}

void interminete_R_ON(){
  
}

void interminete_L_ON(){
  
  
}

void interminete_L_OFF(){
  
  
}

void interminete_R_OFF(){
  
  
}

void Warning_ON(){
  
  }

void Warning_OFF(){
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
  
  if(velocidad>=0){
    digitalWrite(in1Motor,HIGH);
    digitalWrite(in2Motor,LOW);
    lucesFrenado_OFF();
  }else{
    digitalWrite(in1Motor,LOW);
    digitalWrite(in2Motor,HIGH);
    velocidad*=-1;
    lucesFrenado_ON();
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
    
    serverWEB.handleClient();
    delay(100); // Da tiempo a qu el cliente procese la pagina y solicite la conexion
    WiFiClient client = server.available();
   
    if (client.connected() && webSocketServer.handshake(client)) {
   
      String data;      
      luces_ON();
      while (client.connected()) {
   
        data = webSocketServer.getData();
   
        if (data.length() > 0) {
           Serial.println(data);
          interpretarMensaje(data);
           //ledcWrite(1,data.toInt());//Actua soble la salida PWM
           webSocketServer.sendData(data);
        }
  
        delay(10); // Delay needed for receiving the data correctly
     }
   
     Serial.println("The client disconnected");
     
     delay(100);
    }else{
      
      luces_OFF();
     // Serial.print("..");
       
      }
 // }
  delay(100);
}
