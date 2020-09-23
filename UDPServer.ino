#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h> 
#include "udp.h"
#include "arduino_secrets.h"
#include <Arduino_LSM6DS3.h>

AHcode code;

double roll = 0.00, pitch = 0.00;
IPAddress ip(10, 0, 0, 10);
IPAddress dnServer(10, 0, 0, 1);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 0, 0, 0);
IPAddress ips(10, 0, 0, 12);
IPAddress ips2(10, 0, 0, 13);
IPAddress remoteIp;
int keyindex = 0;
int status = WL_IDLE_STATUS;
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP)
unsigned int localPort = 4001;
char packetBuffer[255];
char packetBuffer2[255];
bool check_bool = false;
WiFiUDP Udp;
WiFiUDP Udp1;
WiFiServer server(4000);
String ReplyBuffer1 = "";
int readSpeed();
int readHeight();
enum States{start,mu,md,go,st,bl} state;
int speed = 0;
int height = 0;
void setup() {
  Serial.begin(9600);
  

  while (!Serial)
    if (WiFi.status() == WL_NO_MODULE) {
      Serial.println("Communication with WiFi module failed!");
      while (true);
    }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  WiFi.config(ip, dnServer, gateway, subnet);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    while (true);
  }
  IMU.begin();
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }
  server.begin();
  Udp.begin(localPort);
  Udp1.begin(4010);
  state =start;
}

void loop() {

  //print gyroscope data


  
  char * sub;
  char comm[255] = {};
  switch(state){
    case start:{
      readpack(Udp,comm);
      if(strlen(comm) != 0){
        Serial.println("packet cont:");
        Serial.println(comm);
        sub = strtok(comm,";");
        String s(sub);
        if(s.equals("mu")){
          state = mu;
          speed = atoi(sub = strtok (NULL, ";"));
        }
        if(s.equals("md")){
          state = md;
          speed = atoi(sub = strtok (NULL, ";"));
        }
        if(s.equals("go")){
          state = go;
          speed = atoi(sub = strtok (NULL, ";"));
          height = atoi(sub = strtok (NULL, ";"));
        }
        if(s.equals("bl")){
          state = bl;
        }
        if(s.equals("st")){
          
        }
        Serial.println("comm: "+s);
        if(s.equals("ch")){
          remoteIp = Udp.remoteIP();
          Udp1.beginPacket(remoteIp,4005);
          Udp1.write("ack");
          Udp1.endPacket();
          Udp1.flush();
          //memset(packetBuffer2, 0, sizeof(packetBuffer2));
          Udp1.beginPacket(remoteIp,4005);
          String h = String("h;1;"+String(code.getCurrentHeight()));
          Serial.println(h);
          h.toCharArray(packetBuffer2,sizeof(packetBuffer2));
          //Serial.println("packetbuffer2: ");
          //Serial.println(packetBuffer2);
          Udp1.write(packetBuffer2);
          Udp1.endPacket();
          Udp1.flush();
          memset(packetBuffer2, 0, sizeof(packetBuffer2));
          //Udp1.begin(4010);
          Serial.println("test packet from: "+remoteIp);
          //Udp.beginPacket(remoteIp, 4005);
          //Udp.println("ack;"+localPort);
          //Udp.endPacket();
          //Udp.flush();
        }
      }
      break;
    }
    case mu:{
      Udp1.beginPacket(remoteIp,4005);
      String h = String("h;1;"+String(code.getCurrentHeight()));
      h.toCharArray(packetBuffer2,sizeof(packetBuffer2));
      Udp1.write(packetBuffer2);
      Udp1.endPacket();
      Udp1.flush();
      memset(packetBuffer2, 0, sizeof(packetBuffer2));
      if(code.getCurrentHeight() != 29){
        code.moveUp(speed);
      }else{
        speed = 0;
        state = start;
        code.setMotor("stop",0,0);
        Udp1.beginPacket(remoteIp,4005);
        String h = String("h;1;"+String(code.getCurrentHeight()));
        h.toCharArray(packetBuffer2,sizeof(packetBuffer2));
        Udp1.write(packetBuffer2);
        Udp1.endPacket();
        Udp1.flush();
        memset(packetBuffer2, 0, sizeof(packetBuffer2));  
      }
      readpack(Udp,comm);
      if(strlen(comm) != 0){
        Serial.println(comm);
        sub = strtok(comm,";");
        String s(sub);
        if(s.equals("st")){
          state = start; 
          code.setMotor("stop",0,0);     
        }
      }
      break;
    }
    case md:{
      Udp1.beginPacket(remoteIp,4005);
      String h = String("h;1;"+String(code.getCurrentHeight()));
      h.toCharArray(packetBuffer2,sizeof(packetBuffer2));
      Udp1.write(packetBuffer2);
      Udp1.endPacket();
      Udp1.flush();
      memset(packetBuffer2, 0, sizeof(packetBuffer2));
      Serial.println(state);  
      if(code.getCurrentHeight() != 0){
        code.moveDown(speed);
      }else{
        speed = 0;
        state = start;
        code.setMotor("stop",0,0);
        Udp1.beginPacket(remoteIp,4005);
        String h = String("h;1;"+String(code.getCurrentHeight()));
        h.toCharArray(packetBuffer2,sizeof(packetBuffer2));
        Udp1.write(packetBuffer2);
        Udp1.endPacket();
        Udp1.flush();
        memset(packetBuffer2, 0, sizeof(packetBuffer2));   
      }
      readpack(Udp,comm);
      if(strlen(comm) != 0){
        Serial.println(comm);
        sub = strtok(comm,";");
        String s(sub);
        if(s.equals("st")){
          state = start; 
          code.setMotor("stop",0,0);   
          code.stop();  
        }
      }
      break;
    }
    case go:{
      Udp1.beginPacket(remoteIp,4005);
      //memset(packetBuffer2, 0, sizeof(packetBuffer2));
      String h = String("h;1;"+String(code.getCurrentHeight()));
      h.toCharArray(packetBuffer2,sizeof(packetBuffer2));
      Udp1.write(packetBuffer2);
      Udp1.endPacket();
      Udp1.flush();
      memset(packetBuffer2, 0, sizeof(packetBuffer2));

      if(code.getCurrentHeight() < height){
        code.moveUp(speed);
      }else if(code.getCurrentHeight() > height){
        code.moveDown(speed);
      }else{
        code.setMotor("stop",0,0);   
        speed = 0;
        height = -1;
        state = start;
        Udp1.beginPacket(remoteIp,4005);
         //memset(packetBuffer2, 0, sizeof(packetBuffer2));
        String h = String("h;1;"+String(code.getCurrentHeight()));
        h.toCharArray(packetBuffer2,sizeof(packetBuffer2));
        Udp1.write(packetBuffer2);
        Udp1.endPacket();
        Udp1.flush();
      }
      
      readpack(Udp,comm);
      Serial.println(state);
      if(strlen(comm) != 0){
        Serial.println(comm);
        sub = strtok(comm,";");
        String s(sub);
        if(s.equals("st")){
          state = start; 
          code.stop();     
        }
      }
      break;
    }
    case st:{
      break;
    }
    case bl:{
      float x, y, z;

      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(x, y, z);
        z = 0.98-z;
        Serial.print(x);
        Serial.print('\t');
        Serial.print(y);
        Serial.print('\t');
        Serial.println(z);
        double x_Buff = float(x);
        double y_Buff = float(y);
        double z_Buff = float(z);
        roll = atan2(y_Buff , z_Buff) * 57.3;
        pitch = atan2((- x_Buff) , sqrt(y_Buff * y_Buff + z_Buff * z_Buff)) * 57.3;
        Serial.print('\t');
        Serial.print(roll);
        Serial.print('\t');
        Serial.println(pitch);
      }
      state=start;
      break;
    }
  } 
}
void readpack(WiFiUDP udp,char* comm){
  char pb[255]; //buffer to hold incoming packet
  int packetSize = udp.parsePacket();
  if (packetSize) {
    //Serial.print("Received packet of size ");
    //Serial.println(packetSize);
    //Serial.print("From ");
    //IPAddress remoteIp = udp.remoteIP();
    //Serial.print(remoteIp);
    //Serial.print(", port ");
    //Serial.println(udp.remotePort());
    // read the packet into packetBufffer
    int len = udp.read(pb, 255);
    if (len > 0) {
      pb[len] = 0;
    }
    Serial.println("Contents:");
    strncpy(comm,pb,strlen(pb));
  }
}
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your board's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
