
#include <SoftwareSerial.h>

SoftwareSerial BT(10, 11);  

const int LED_PIN = 12;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(9600);     
  BT.begin(9600);         
}

void loop() {
  if (BT.available()) {
    char c = BT.read();
    Serial.print("Received: ");
    Serial.println(c);
    Serial.print("ASCII: ");
    Serial.println(c, DEC);

    if (c == '1') {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ON");
    } 
    else if (c == '0') {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED OFF");
    }
  }
}