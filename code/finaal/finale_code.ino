#include <QTRSensors.h>
#include <EEPROM.h>


// Motor pins

#define LEFT_IN1   6
#define LEFT_IN2   5
#define RIGHT_IN1 10
#define RIGHT_IN2  9

#define BT Serial1


// QTR-8A

#define NUM_SENSORS 8
QTRSensors qtr;
uint16_t sensorValues[NUM_SENSORS];

// A7 = links, A0 = rechts
const uint8_t sensorPins[NUM_SENSORS] =
{ A7, A6, A5, A4, A3, A2, A1, A0 };

//Variabele

bool robotRunning = false;

int baseSpeed = 80;     
int maxSpeed  = 255;

float Kp = 0.15;
float Ki = 0.0;
float Kd = 0.8;

float iTerm = 0;
int lastError = 0;


#define EE_KP     0
#define EE_KI     4
#define EE_KD     8
#define EE_SPEED 12


float readFloat(int addr) {
  float v;
  EEPROM.get(addr, v);
  return v;
}

void writeFloat(int addr, float v) {
  EEPROM.put(addr, v);
}

int readInt(int addr) {
  int v;
  EEPROM.get(addr, v);
  return v;
}

void writeInt(int addr, int v) {
  EEPROM.put(addr, v);
}



void setLeftMotor(int speed) {
  speed = constrain(speed, -255, 255);

  if (speed >= 0) {
    analogWrite(LEFT_IN1, speed);
    analogWrite(LEFT_IN2, 0);
  } else {
    analogWrite(LEFT_IN1, 0);
    analogWrite(LEFT_IN2, speed);
  }
}

void setRightMotor(int speed) {
  speed = constrain(speed, -255, 255);

  if (speed >= 0) {
    analogWrite(RIGHT_IN1, speed);
    analogWrite(RIGHT_IN2, 0);
  } else {
    analogWrite(RIGHT_IN1, 0);
    analogWrite(RIGHT_IN2, speed);
  }
}



void calibrateSensors() {
  BT.println("CAL START");
  for (int i = 0; i < 300; i++) {
    qtr.calibrate();
    delay(20);
  }
  BT.println("CAL OK");
}



void handleBluetooth() {
  while (BT.available()) {
    char c = toupper((char)BT.read());
    BT.setTimeout(300);

    if (c == '1') {
      robotRunning = true;
      iTerm = 0;
      lastError = 0;
      BT.println("START");
    }

    else if (c == '0') {
      robotRunning = false;
      setLeftMotor(0);
      setRightMotor(0);
      BT.println("STOP");
    }

    else if (c == 'S') {
      int v = BT.parseInt();
      if (v >= 0 && v <= 255) {
        baseSpeed = v;
        writeInt(EE_SPEED, baseSpeed);
        BT.print("SPD=");
        BT.println(baseSpeed);
      }
    }

    else if (c == 'P') {
      Kp = BT.parseFloat();
      writeFloat(EE_KP, Kp);
      BT.print("KP=");
      BT.println(Kp);
    }

    else if (c == 'I') {
      Ki = BT.parseFloat();
      writeFloat(EE_KI, Ki);
      BT.print("KI=");
      BT.println(Ki);
    }

    else if (c == 'D') {
      Kd = BT.parseFloat();
      writeFloat(EE_KD, Kd);
      BT.print("KD=");
      BT.println(Kd);
    }

    else if (c == 'C') {
      calibrateSensors();
    }

    else if (c == 'T') {
      BT.print("SPD="); BT.println(baseSpeed);
      BT.print("KP=");  BT.println(Kp);
      BT.print("KI=");  BT.println(Ki);
      BT.print("KD=");  BT.println(Kd);
    }
  }
}



void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  qtr.setTypeAnalog();
  qtr.setSensorPins(sensorPins, NUM_SENSORS);
  qtr.setTimeout(2500);

  // Load EEPROM
  Kp = readFloat(EE_KP); if (isnan(Kp)) Kp = 0.15;
  Ki = readFloat(EE_KI); if (isnan(Ki)) Ki = 0.0;
  Kd = readFloat(EE_KD); if (isnan(Kd)) Kd = 0.8;

  int sp = readInt(EE_SPEED);
  if (sp > 0 && sp <= 255) baseSpeed = sp;

  delay(200);
  calibrateSensors();
}



void loop() {
  handleBluetooth();

  if (!robotRunning) {
    setLeftMotor(0);
    setRightMotor(0);
    return;
  }

  int position = qtr.readLineBlack(sensorValues);

  // Setpoint = midden = 3500
  int error = 3500 - position;

  // PID
  float output = Kp * error;

  iTerm += Ki * error;
  iTerm = constrain(iTerm, -510, 510);
  output += iTerm;

  output += Kd * (error - lastError);
  lastError = error;

  output = constrain(output, -510, 510);

  int leftSpeed  = baseSpeed - output / 2;
  int rightSpeed = baseSpeed + output / 2;

  leftSpeed  = constrain(leftSpeed,  -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  setLeftMotor(leftSpeed);
  setRightMotor(rightSpeed);
}
