#include <Servo.h>      //motor
#include <HX711_ADC.h>  //Waage
#include <Wire.h>       
#include <EEPROM.h>     
#include <SPI.h>
#include <SD.h>

HX711_ADC LoadCell(A3, A4);//LoadCell
Servo esc;

//Pin deklearation
int amperemeter = A1;
int referenzVoltage = A0;
int voltmeter = A2;
int lightsensor = A6;

//Variablen
int motorset = 0;
int maxRPM = 60;
int rpm = 0;
float c = 439.83; //calibration factor;
int VpA = 66; // Millivolt pro Ampere (100 für 20A Modul und 66 für 30A Modul)
char filename = "";
int chipSelect = 5;
float R1 = 30000.0; //
float R2 = 7500.0; //
double nullwert = 0;
double value = 0;
float vint = 0;
int increment = 5;

double ampere = 0;
float volt = 0;
float reference = 0;
float weight = 0; 
const int numReadings = 50; //Anzahl Reading von Waage
boolean activateSD = true; //SD Karte aktiviert
bool stopago = 0;


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  delay(10000);
  Serial.begin(19200);
  Serial.println("LOS GEHTS");
  pinMode(voltmeter, INPUT);
  pinMode(referenzVoltage, INPUT);
  pinMode(amperemeter, INPUT);
  pinMode(lightsensor, INPUT);
  //Programme ablaufen
  getampere();
  nullwert = vint;
  Serial.print("Nullwert gesetzt auf ");Serial.println(nullwert);
  delay(5000);
  ESC_arm();
  LoadCell_Initialisieren();
  //if(activateSD)SD_Initialisierung();
  delay(2000);
  
}  

void loop() {

  getampere();
  getvolt();
  getreference();
  getweight();
  if(Serial.available()>0){
    char temp = Serial.read();
    if(temp == '+'){
      motorset ++;
    }
    if(temp == '-'){
      motorset --;
    }
    if(temp == 'a'){
      esc.write(85); while(1); Serial.print("++++++++++++++++++++++++++++++MESSUNG BEENDET++++++++++++++++++++++++++++++");
    }
    }
      String numbers = "";
    numbers += ampere;    numbers += "; ";
    numbers += volt;      numbers += "; ";
    numbers += weight;    numbers += "; ";
    numbers += motorset;
    Serial.println(numbers);
  setSpeed(motorset);
  delay(1);
  

}
//++++++++++++++MOTOR++++++++++++++++++
void ESC_arm() { //ESC initialisieren
  esc.attach(10);
  esc.write(0); Serial.println("initate"); delay(2000);
  esc.write(179); Serial.println("..."); delay(2000);
  esc.write(80); delay(2000); Serial.println("MOTOR ARMED. STAY CLEAR!!!");
}
void setSpeed(int speed) {
  int angle = map(speed, 0, 100, 90, 179); //Motor hat 180° als maximalen Input
  esc.write(angle); //Motorspeed geben
}
void getrpm() {
  boolean lasttime = false;
  unsigned long nowtime = 0;
  unsigned long tme = micros();
  unsigned long starttime = 0;
  unsigned long difference = 0;
  int x = 0;
  int frequency = 0;
  for (int i = 0; i < 100000000000000; i++) {
    starttime = tme;
    difference = micros() - starttime;
    //Serial.print(" Differenz: ");Serial.print(difference);Serial.print(" Zähler ist: ");Serial.print(x);Serial.print(" Frequenz: ");Serial.println(frequency);
    if (difference > 991000) {
      frequency = x;
      rpm = frequency * 30; //60seconds /2 for two blades = 30
      //Serial.print("nowtime ");Serial.print(nowtime);Serial.print("starttime ");Serial.print(starttime);
      break;
    }
    if (analogRead(lightsensor) < 4095 && lasttime == true) {
      lasttime = false;
    }
    if (analogRead(lightsensor) == 4095 && lasttime == false) {
      x++; lasttime = true;
    }
  }
}
//++++++++++++MOTORENDE++++++++++++++++++

//*************SD Karte******************
void SD_Initialisierung(){
  Serial.print("Initializing SD card...");  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present"); // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  //  Serial.println("Please state the name of the file below: ");
  //  boolean stopago = 0;
  //  while (stopago == 0) {
  //    if (Serial.available() > 0) {
  //      filename = + char(Serial.read());
  //      filename = + ".csv";
  //      stopago = 1;
  //    }
  //  }

  //Serial.println("The filename will be: "); Serial.print(filename);
  //SD.mkdir(filename);
  File file = SD.open("/ASH20G.csv", FILE_WRITE);
  if (file) {
    String header = "ampere; volt; weight; motorset";
    file.println(header);
    file.close();
    Serial.println(header);
  } else {
    Serial.println("Couldn't open log file");
  }
  delay(1000);
}
void filewrite() {
  File file = SD.open("/ASH20G.csv", FILE_WRITE);
  if (file) {
    String numbers = "";
    //numbers += rpm;       numbers += "; ";
    numbers += ampere;    numbers += "; ";
    //numbers += ampereroh;    numbers += "; ";
    //numbers += amperespannung;    numbers += "; ";
    numbers += volt;      numbers += "; ";
    numbers += weight;    numbers += "; ";
    numbers += motorset;
    file.println(numbers);
    file.close();
    Serial.println(numbers);
  } else {
    //Serial.println("Couldn't open log file");
  }
}
//***********SD Karte Ende***************

//*************Load Cell*****************
void LoadCell_Initialisieren(){
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(2000); // load cells gets 2000ms of time to stabilize
  LoadCell.setCalFactor(439.83); // calibration factor for load cell => strongly dependent on your individual setup
}
//***********Load Cell Ende**************

//*********Voltage and Ampere************
void getampere() {
  float vout = 0.0;
  value = analogRead(amperemeter);
  vout = (value * 5.0) / 1024.0; // see text
  vint = vout / (R2 / (R1 + R2));
  ampere = (vint-nullwert)*1000/VpA;
}

void getvolt() {
  float vout = 0.0;
  value = analogRead(voltmeter);
  vout = (value * 5.0) / 1024.0; // see text
  volt = vout / (R2 / (R1 + R2));
}

void getreference() {
  float vout = 0.0;
  value = analogRead(referenzVoltage);
  vout = (value * 5.0) / 1024.0; // see text
  reference = vout / (R2 / (R1 + R2));
}

void getweight() {
  int intervall = 0;
  float total = 0;  
  long t;
  LoadCell.setCalFactor(439.83);
  LoadCell.update();
  float j = LoadCell.getData();
  weight = j;
}
//*******Voltage and Ampere Ende*********
