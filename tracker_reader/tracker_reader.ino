
#include <EEPROM.h>

int address = 0;
byte value;
 
void setup() {
  Serial.begin(9600);
  value = EEPROM.read(address);
  Serial.print("Last Pointer:");
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();
}
 
void loop() {
  value = EEPROM.read(address);
  Serial.print("Time:");
  Serial.print(address);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();
  address = address + 1;
  if (address == EEPROM.length()) {
    address = 0;
    delay(100000);
  }
}
