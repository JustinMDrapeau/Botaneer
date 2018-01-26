/**
Gregory Ho, Aidan Ha, Justin Drapeau
The Botaneer ECE 150 (FINAL OMEGA PROJECT) (FALL 2017)
Paul Ward
Automated Plant Watering System - Arduino Sketch
*/

int humidity = 0; //moisture level 0-1000
float temperature = 0; //Temperature (-15 ~135 C)
int State=0; //For sensing Omega Output
int soilPin = A0;//Declare a variable for the soil moisture sensor 
int soilPower = 7;//Pin for powering humidity sensor
int tempPin = A1; //Pin to Read Temperature Sensor (TMP36)
int tempPower=8;  // Pin for powering temperatur sensor
int pump=9; //Pumping pin (supply voltage)
int OMEGA=A2; //Input pin to detect when to turn on motor

//Time Variables
unsigned long qtime;
unsigned long newtime=0;
void setup(){
  Serial.begin(9600);   // open serial over USB 
  while (!Serial){} //Wait for Serial to open
  pinMode(soilPower, OUTPUT);//Set 7 as an OUTPUT (soil sensor power supply)
  pinMode(tempPower,OUTPUT); //Set 8 as an OUTPUT (temp sensor power supply)
  pinMode(pump,OUTPUT); //Set 9 as an OUTPUT (turn on motor)
  
  digitalWrite(soilPower, LOW);//Set to LOW so no power is flowing through the sensor and motor
  digitalWrite(tempPower,LOW);
  digitalWrite(pump,LOW);
}
void loop() {
State=analogRead(OMEGA); //Check if input pin has voltage <400 is off, >400 is on
if(State<400){
  qtime =millis();  //Take system time
  if ((qtime-newtime)>=6000){ //If time elapsed greater than multiple of 6000, write values of sensor and set newtime basis to ensure 6 second uniform delay
    Serial.println(String(readSoil()) +"&"+ String(readTemp()));  
    Serial.flush();
    newtime=qtime;
    qtime=0;
  }
  State=analogRead(OMEGA);
  delay (100);
}
if (State>400){ //Input pin on, turn on motor
  pumper();
}
} 
void pumper(){
  digitalWrite(pump,HIGH); //Turn on the pump, delay 6 seconds then turn the pump off
  delay(6000);
  digitalWrite(pump,LOW);
}
  
//This is a function used to get the soil moisture content
int readSoil(){
  // pin 7 (soil) gnd middle A0 (right/sensor)
    digitalWrite(soilPower, HIGH);//turn D7 "On"
    delay(10);//wait 10 milliseconds 
    humidity = analogRead(soilPin);//Read the SIG value form sensor 
    digitalWrite(soilPower, LOW);//turn D7 "Off"
    return humidity;//send current moisture value
}
int readTemp(){
  //left of little box tag = (8)5V/3.3V, Signal A1, GND right
    digitalWrite(tempPower,HIGH); //Turn on the sensor
    delay (10);
    temperature=((float)analogRead(tempPin)*0.004882814); //Convert value Read to Celsius
    temperature =((float)temperature -0.5)*100;
    temperature =(int)temperature; //Cast to integer
    return temperature;
}

