// Motor R (A-channel DRV8833)
const int IN1 = 6;   // AIN1 - PWM
const int IN2 = 5;   // AIN2 - PWM

// Motor L (B-channel DRV8833)
const int IN3 = 10;  // BIN1 - PWM
const int IN4 = 9;   // BIN2 - PWM

const int rampDelay = 10;

void setup()
{
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors();
}

// --- Motor control functions ---

void motorR_forward(int s) {
  analogWrite(IN1, s);
  analogWrite(IN2, 0);
}

void motorR_backward(int s) {
  analogWrite(IN1, 0);
  analogWrite(IN2, s);
}

void motorL_forward(int s) {
  analogWrite(IN3, s);
  analogWrite(IN4, 0);
}

void motorL_backward(int s) {
  analogWrite(IN3, 0);
  analogWrite(IN4, s);
}

void stopMotors() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}

// --- Ramping helper (both motors) ---
void rampBoth(void (*funcR)(int), void (*funcL)(int)) {
  for (int i = 0; i <= 255; i++) {
    funcR(i);
    funcL(i);
    delay(rampDelay);
  }
  for (int i = 255; i >= 0; i--) {
    funcR(i);
    funcL(i);
    delay(rampDelay);
  }
}

void loop()
{
  // VOORUIT
  rampBoth(motorR_forward, motorL_forward);
  stopMotors();
  delay(2000);

  // ACHTERUIT
  rampBoth(motorR_backward, motorL_backward);
  stopMotors();
  delay(2000);

  // RECHTS DRAAIEN (R vooruit, L achteruit)
  rampBoth(motorR_forward, motorL_backward);
  stopMotors();
  delay(2000);

  // LINKS DRAAIEN (R achteruit, L vooruit)
  rampBoth(motorR_backward, motorL_forward);
  stopMotors();
  delay(2000);
}