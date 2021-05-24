#include <Wire.h>
#include <BH1750.h>
#include <LowPower.h>
BH1750 lightMeter;

//const int ledPin =  LED_BUILTIN;// the number of the LED pin
//int ledState = LOW;             // ledState used to set the LED

int motorPinUp = 2;
int motorPinDown = 4;
int doorSwitch = 7;
int doorStatus = -1; //1:open ,0:moving , -1:closed

unsigned int avgLight; //unsigned int to extend positive range

int dayLevel = 200; //lux threashold to open door
int nightLevel = 90; //lux threashold to close door
float timeUp = 20; //seconds motors pull up door

int read_delay = 2500;

int intApertura = (int) (timeUp * 1000.0) ;
bool switchStatus; // it's a pull_up --> 0:closed / 1:open

/////////////////////////////////////////////////////////////


void setup() {
  pinMode(doorSwitch, INPUT_PULLUP);
  pinMode(motorPinUp, OUTPUT);
  pinMode(motorPinDown, OUTPUT);
  //pinMode(ledPin, OUTPUT);
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
  delay(read_delay);
  unsigned int lux_b = lightMeter.readLightLevel(true);
  if (lux_a >=0 and lux_b >= 0) {
      avgLight = (lux_a+lux_b)/2;
    }
  
}

void openDoor(){
 doorStatus=0; //door is in movement
 digitalWrite(motorPinUp, HIGH);
 digitalWrite(motorPinDown, LOW);
 delay(intApertura);
 digitalWrite(motorPinUp, LOW);
 digitalWrite(motorPinDown, LOW);
 doorStatus= 1; //door is up
 delay(5000); //stay here to avoid reading light again too soon
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
 delay(5000); //stay here to avoid reading light again too soon
}

void loop() {
  //digitalWrite(ledPin, HIGH);
  switchStatus = digitalRead(doorSwitch);
  //Serial.print("doorSwitch @ main-loop: ");Serial.println(switchStatus);
  
  readLight();
  Serial.print("Avg Lux: ");Serial.println(avgLight);

  if (avgLight>dayLevel and doorStatus==-1 and switchStatus==0) {
    Serial.println("Day! >> openDoor !");
    openDoor(); 
  }
  
  if (avgLight<nightLevel and doorStatus==1 and switchStatus==1) {
    Serial.println("Night! >> closeDoor !");
    closeDoor();
  }

  Serial.println("--> SLEEP 8sec");
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
                 SPI_OFF, USART0_OFF, TWI_OFF);
  delay(1000);
  Serial.println("Arduino wakeup");
  
  //digitalWrite(ledPin, LOW);
  //delay(2000);
}
