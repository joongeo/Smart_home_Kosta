#define TRIG_PIN 7
#define ECHO_PIN 6
#define LED_RED_PIN 9

void setup() {
Serial.begin(9600);
pinMode(TRIG_PIN, OUTPUT);
pinMode(ECHO_PIN, INPUT);
pinMode(LED_RED_PIN, OUTPUT);

}

void loop()
{
int distance = 0;

digitalWrite(TRIG_PIN, HIGH);
delayMicroseconds(10);
digitalWrite(TRIG_PIN, LOW);
distance = pulseIn(ECHO_PIN, HIGH)/58.2;

if (distance <= 65) {
digitalWrite(LED_RED_PIN, HIGH);
Serial.println("on");

}

else {
digitalWrite(LED_RED_PIN, LOW);
Serial.println("off");
}
delay(100);
}