#include <LowPower.h>
#include <Dusk2Dawn.h>
#include "RTClib.h"
RTC_DS3231 rtc;
int alba = 420;
int tramonto = 1250;
int now_mins;
int motorPinUp = 2;
int motorPinDown = 4;
int doorSwitch = 7; //interruttore magnetico
bool switchStatus; //  0:chiuso / 1:aperto

int offset_alba = 60;
int offset_tramonto = 30;
float timeUp = 20; //seconds motors pull up door

//your location
float gpsLAT = 34.8;
float gpsLONG = 18.10; 
int gpsDST = 1;
/////////////////////////////////////////////////////////////

//----------------------- SETUP ---
void setup() {
  Serial.begin(9600);
  if (! rtc.begin()) {
  Serial.println("RTC non trovato"); }
  
  pinMode(doorSwitch, INPUT_PULLUP);
  pinMode(motorPinUp, OUTPUT);
  pinMode(motorPinDown, OUTPUT);
  //pinMode(ledPin, OUTPUT);

  switchStatus = digitalRead(doorSwitch);
  delay(250);
  Serial.println("---= AVVIO =---");
  delay(250);
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //rtc.adjust(DateTime(2014, 1, 21, 0, 0, 0));
  }
  
  Serial.print("DoorSwitch all'avvio: ");Serial.println(switchStatus); // 0:chiuso, 1:aperto
  
  digitalWrite(motorPinUp, LOW); 
  digitalWrite(motorPinDown, LOW);
}
//====================== FINE SETUP =============================


int checkMins() {
  DateTime now = rtc.now();
  now_mins = (now.hour()*60+now.minute());
  return now_mins;
}



//----------------------- OPEN --- 
void openDoor(){
 digitalWrite(motorPinUp, HIGH);
 digitalWrite(motorPinDown, LOW);
 int intApertura = (int) (timeUp * 1000.0) ;
 delay(intApertura);
 digitalWrite(motorPinUp, LOW);
 digitalWrite(motorPinDown, LOW);
 delay(1000);
 //dormi();
}

//----------------------- CLOSE ---
void closeDoor(){
 digitalWrite(motorPinUp, LOW);
 digitalWrite(motorPinDown, HIGH);
 switchStatus = digitalRead(doorSwitch);
 Serial.println(" Chiudo porta --> : " + switchStatus); //debug
 
 while (switchStatus == HIGH) {
  switchStatus = digitalRead(doorSwitch);
  //Serial.println(" .... in chiusura  --> : " + switchStatus); //debug
  delay(250); //per evitare errori lettura switch magnetico
 }
 Serial.println(" Ultimo pezzetto >>> " + switchStatus); //debug
 delay(900); 
 
 digitalWrite(motorPinUp, LOW);
 digitalWrite(motorPinDown, LOW);
 delay(500); 
 Serial.println("CHIUSO. ");
 delay(1000);
 //dormi();
}
//..........................................



//===================== LOOP ===============================
void loop() {
  switchStatus = digitalRead(doorSwitch);
  //Serial.print("doorSwitch @ main-loop: ");Serial.println(switchStatus);
  
  DateTime now = rtc.now();
  Dusk2Dawn location(gpsLAT, gpsLONG, gpsDST);
  alba = location.sunrise(now.year(),now.month(),now.day(),true);
  tramonto = location.sunset(now.year(),now.month(),now.day(),true);
  if(alba==-1 or tramonto ==-1){
    alba= 420;
    tramonto=1200;
  }
  Serial.print(now.hour()); Serial.print(":"); Serial.print(now.minute()); Serial.print(":"); Serial.println(now.second()); 
  char time_alba[] = "00:00";
  Dusk2Dawn::min2str(time_alba, alba);
  Serial.print(time_alba); Serial.print(" // "); Serial.println(alba);  

  char time_tramonto[] = "00:00";
  Dusk2Dawn::min2str(time_tramonto, tramonto); 
  Serial.print(time_tramonto); Serial.print(" // "); Serial.println(tramonto);  
 
  Serial.print("Mins adesso "); Serial.println(checkMins());
  delay(100); 

 //,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
  if ( (checkMins() > alba+offset_alba) && (checkMins() < tramonto) && switchStatus==0) {   
   Serial.println("Segnale di apertura!");
    openDoor(); 
  }
 
  if ( ((checkMins() > tramonto+offset_tramonto) && switchStatus==1)  || ( (checkMins() < alba) && switchStatus==1 ) ) {  
    Serial.println("Segnale di chiusura!");
    closeDoor();
  }
 //,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,SPI_OFF, USART0_OFF, TWI_OFF);
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,SPI_OFF, USART0_OFF, TWI_OFF); 
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,SPI_OFF, USART0_OFF, TWI_OFF);
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,SPI_OFF, USART0_OFF, TWI_OFF); 
}
