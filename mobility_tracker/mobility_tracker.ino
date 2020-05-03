#include <EEPROM.h>

#define passage_tolerance 40 //cm from horizen considered as mistakes
#define horizen_stability_multiplier 0.999 //part of the horizen that is kept
#define smooth_counter_factor 7 //higher = number of cycles with different value to count a passage
#define one_hour 3600000
#define one_minute 60000
#define ten_seconds 6000

const int pingPin = 7; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor
long duration;
int passages,triger,smooth_counter,address;
float horizen;
unsigned long elapsed_time_record, elapsed_passage, debug;

void setup() {
  Serial.begin(9600);
  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //get address pointer from last run. [pos0 = Pointer, pos1 hour passages, ..., posK 255 new run, ]
  address = EEPROM.read(0);
  address = writeToEEPROM(address,255);
  //reset parameters
  horizen = 0;
  triger = 0;
  passages = 0;
  debug = 0;
  smooth_counter = 0;
}

void loop() {  
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  long current_distance = microsecondsToCentimeters(duration);
//  Serial.println(cm);

//set horizen
  if (current_distance < 500){ //max 5 meters measure
    horizen = horizen_stability_multiplier*horizen + (1-horizen_stability_multiplier)*current_distance;
  }
  else {
    horizen = horizen_stability_multiplier*horizen + (1-horizen_stability_multiplier)*5000;
  }
  if ((horizen - current_distance) > passage_tolerance){   //if distance change is greater than tolerance and current is smaller than horizen
    smooth_counter += 1;
    if (triger == 0 && smooth_counter > smooth_counter_factor) {//and triger is reset, count 1 passage
      passages += 1;
      triger = 1;
      smooth_counter = 0;
      elapsed_passage =  millis();
      Serial.print("pass: ");
      Serial.println(current_distance);
    }
  }
  if (abs(horizen - current_distance) < passage_tolerance){ //return from a sudden blip
    smooth_counter = 0;
  }
  if ((millis() - elapsed_passage) > 2000){ //if more than 2 seconds passed reset trigger (how much time it takes someone to pass)
    triger = 0;
  }

  if (millis() - elapsed_time_record > (one_minute)){  //if elapsed_time_record == 1 hour, reset it and write to EEPROM
    Serial.print("writing to EEPROM: ");
    Serial.println(passages);
    address = writeToEEPROM(address,passages);
    passages = 0;
    triger = 0;
    elapsed_time_record = millis();
  }
  
  if ((millis() - debug) > 1000){
    debug = millis();
    Serial.print("triger: ");
    Serial.println(triger);
    Serial.print("passages: ");
    Serial.println(passages);
    Serial.print("horizen: ");
    Serial.println(horizen);
  }
//  Serial.println(horizen); //debug for plotter
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}

int writeToEEPROM(int address,int value){
  //gets value to put in address, updates main pointer, forwards address and modulo by size of EEPROM
    EEPROM.update(address, value); //update address with value
    EEPROM.update(0, address); //update pointer to current pointer
    address = address + 1;
    if (address == EEPROM.length()) {
      address = 1;
    }
    return address;
}
