#include <Wire.h>
#include <BH1750.h>
BH1750 lightMeter;

const int ledPin =  LED_BUILTIN;// the number of the LED pin
int ledState = LOW;             // ledState used to set the LED
int read_delay = 2000;

int motorPinUp = 2;
int motorPinDown = 4;
int doorSwitch = 7;
int doorStatus = -1; //1:open ,0:moving , -1:closed

unsigned int avgLight; //unsigned int to extend positive range

int dayLevel = 400; //lux threashold to open door
int nightLevel = 120; //lux threashold to close door
float timeUp = 15; //seconds motors pull up door

int intApertura = (int) (timeUp * 1000.0) ;
bool switchStatus; // it's a pull_up --> 0:closed / 1:open

/////////////////////////////////////////////////////////////


void setup() {
  pinMode(doorSwitch, INPUT_PULLUP);
  pinMode(motorPinUp, OUTPUT);
  pinMode(motorPinDown, OUTPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); //
  Wire.begin();
  lightMeter.begin();
  switchStatus = digitalRead(doorSwitch);
  delay(250);
  Serial.println("---= START =---");
  delay(250);
  Serial.print("doorSwitch @start: ");Serial.println(switchStatus);
  
  digitalWrite(motorPinUp, LOW); //just to make sure it's not moving.
  digitalWrite(motorPinDown, LOW);
}

void readLight () {
  unsigned int lux_a = lightMeter.readLightLevel(true);
  delay(2500);
  unsigned int lux_b = lightMeter.readLightLevel(true);
  if (lux_a >=0 and lux_b >= 0) {
      avgLight = (lux_a+lux_b)/2;
    }
  Serial.println("Avg Lux: " + avgLight);
}

void openDoor(){
 doorStatus=0; //door is in movement
 digitalWrite(motorPinUp, HIGH);
 digitalWrite(motorPinDown, LOW);
 delay(intApertura);
 digitalWrite(motorPinUp, LOW);
 digitalWrite(motorPinDown, LOW);
 doorStatus= 1; //door is up
 delay(2000); //stay here to avoid reading light again too soon
}

void closeDoor(){
 doorStatus=0; //door is in movement
 digitalWrite(motorPinUp, LOW);
 digitalWrite(motorPinDown, HIGH);
 switchStatus = digitalRead(doorSwitch);
 
 while (switchStatus == HIGH) {
  switchStatus = digitalRead(doorSwitch);
  Serial.println("doorSwitch in while loop: " + switchStatus); //debug
  delay(250); //to avoid switch rebounce
 }
 delay(850); //necessary to close final centimeters
 
 digitalWrite(motorPinUp, LOW);
 digitalWrite(motorPinDown, LOW);
 doorStatus= -1; //door is down
 delay(20000); //stay here to avoid reading light again too soon
}

void loop() {
  digitalWrite(ledPin, HIGH);
  
  switchStatus = digitalRead(doorSwitch);
  Serial.print("doorSwitch @ main-loop: ");Serial.println(switchStatus);
  
  readLight();

  if (avgLight>dayLevel and doorStatus==-1 and switchStatus==0) {
    Serial.println("Day! >> openDoor !");
    openDoor(); 
  }
  
  if (avgLight<nightLevel and doorStatus==1 and switchStatus==1) {
    Serial.println("Night! >> closeDoor !");
    closeDoor();
  }
  
  digitalWrite(ledPin, LOW);
  delay(2000);
}
