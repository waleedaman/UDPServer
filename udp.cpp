#import<string.h>
#import "udp.h"
void AHcode::init(){
    Serial.begin(9600);
    delay(1000);
    pinMode(dir1PinA,OUTPUT);
    pinMode(speedPinA,OUTPUT);
    pinMode (A3, INPUT);
}
 float AHcode::getCurrent(){
    return current=0.00296*analogRead(A0);
}
int AHcode::mapValue (int value){
    uint32_t u32Value = analogRead(A3);
    Serial.print("PotValue: ");
   Serial.println(u32Value);
   Serial.println("Height in cm = ");
    Serial.println(map(u32Value, minPot, maxPot, minHeight, maxHeight));
    int height123 =  map(u32Value, minPot, maxPot, minHeight, maxHeight);
    return height123;
}
void AHcode::readPacket(WiFiUDP Udp,char* comm){
    char * sub;
    char packetBuffer[255];
    int packetSize = Udp.parsePacket();
    if (packetSize) {
        // read the packet into packetBufffer
        int len = Udp.read(packetBuffer, 255);
        comm = packetBuffer;
        if (len > 0) {
        packetBuffer[len] = 0;
        comm[len] = 0;
        }
    }
}
int AHcode::getCurrentHeight(){
  return AHcode::mapValue(value);
}
void AHcode::moveUp(int speed){
  analogWrite(speedPinA, speed);
  digitalWrite(dir1PinA, HIGH);
}
void AHcode::moveDown(int speed){
  analogWrite(speedPinA, speed);
  digitalWrite(dir1PinA, LOW);
}
void AHcode::stop(){
    analogWrite(speedPinA, 0);
    digitalWrite(brakeA, HIGH);
}
void AHcode::setMotor(String dir,int speed, int Height){
    if(speed >=0 && speed <=255){
        analogWrite(speedPinA, speed);
       if(dir.equals("up")){
            digitalWrite(dir1PinA, HIGH);
            while (1){
              delay(250);
              int height_cm = AHcode::mapValue(value);
              if (Height == height_cm){
                 Serial.println("Height in cm = ");
                 Serial.println(height_cm);
                 digitalWrite (brakeA, HIGH);
                 break;
              }
            }
            delay (500);
            Serial.print(current);
            Serial.print(" amps");
            Serial.println ("");
        }else if(dir.equals("down")){
            digitalWrite(dir1PinA, LOW);
            while (1){
              delay(250);
              int height_cm = AHcode::mapValue(value);
              if (Height == height_cm){
                 Serial.println("Height in cm = ");
                 Serial.println(height_cm);
                 digitalWrite (brakeA, HIGH);
                 break;
              }
            }
            delay (500);
            Serial.print(current);
            Serial.print(" amps");
            Serial.println ("        ");
        }
       else if(dir.equals("stop")){
            analogWrite(speedPinA, 0);
            digitalWrite(brakeA, HIGH);
        }
        delay (200);
        if(getCurrent()>=2.0){
            digitalWrite(brakeA, HIGH);
            delay(500);
            if(dir.equals("up")){
                digitalWrite(dir1PinA, HIGH);
            }else if(dir.equals("down")){
                digitalWrite(dir1PinA, LOW);
            }
        }
        if(dir.equals("go")){
          while(1){
            int height_cm = AHcode::mapValue(value);
            delay(100);
            Serial.println("Current height");
            Serial.println(height_cm);
            Serial.println("Given height");
            Serial.println(Height);
            if(Height<height_cm){
              digitalWrite(dir1PinA, LOW);
              Serial.println("Height is less than current position. Moving down");
            }
            if(Height>height_cm){
              Serial.println("Height is more than current position. Moving up");
              digitalWrite(dir1PinA, HIGH);
            }
            if(Height == height_cm){
              Serial.println("Height is equal to current position. Stopping");
              setMotor("stop",0,0);
              break;
            }
          }
        }
        delay (500);
    }
}
