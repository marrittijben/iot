const int trigPin = 9;
const int echoPin = 10;
const int led = 7;
const int resistor = A0;

float duration;
float distance;

int value;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(resistor, INPUT);
  Serial.begin(9600);
}

void loop() {
  value = analogRead(resistor);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.034) / 2;

  Serial.print("Afstand: ");
  Serial.println(distance);
  Serial.print("Lichtinval: ");
  Serial.println(value);

  delay(100);

  if (distance < 20 && value < 700){
    digitalWrite(led, HIGH);
    delay(10000);
  }
  else{
    digitalWrite(led, LOW);
  }
}
