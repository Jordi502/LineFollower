int value;
void setup() 
{
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  Serial.begin(9600);
}

void loop() 
{
  for (int i = 0; i <= 8; i++)
  {
    value = analogRead(i);
    Serial.print(i);
    Serial.print(": ");
    Serial.println(value);
  }
  delay(1000);
}