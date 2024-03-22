#define DEBUG 0   // 1 включить режим отладки, 0 выключить нах
const int buttonPin[4] = {2, 3, 4, 5};
const int relayPin =  8;
const int relayPin2 =  9;
const int pause = 1; // пауза перед вкл реле
const bool relaySignal = 0; // на что реагирует реле? 1 - на HIGH, 0 - на LOW
int pointer = 0;

void setup() {
  if (DEBUG == 1)  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  pinMode(relayPin2, OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(buttonPin[i], INPUT);
  }
}

void loop() {
  bool btn[4];
  for (int i = 0; i < 4; i++) {
    btn[i] = digitalRead(buttonPin[i]);
  }
  // берем случай когда лишь одна из кнопок нажата. или та или иная.

  if (btn[0] == 1 && btn[1] == 0 && btn[2] == 0 && btn[3] == 0) {
    pointer = 1;
    if (DEBUG == 1)  Serial.println("1");
  } else  if (btn[0] == 0 && btn[1] == 1 && btn[2] == 0 && btn[3] == 0 && (pointer == 1 || pointer == 2)) {
    pointer = 2;// кнопку можно жать несколько секунд - даже несколько раз одну и ту же - зачтется
    if (DEBUG == 1)  Serial.println("2");
  } else if (btn[0] == 0 && btn[1] == 0 && btn[2] == 1 && btn[3] == 0 && (pointer == 2 || pointer == 3)) {
    pointer = 3;
    if (DEBUG == 1)  Serial.println("3");
  } else if (btn[0] == 0 && btn[1] == 0 && btn[2] == 0 && btn[3] == 1 && (pointer == 3 || pointer == 4)) {
    pointer = 4;
    if (DEBUG == 1) Serial.println("4");
  } else if (btn[0] == 0 && btn[1] == 0 && btn[2] == 0 && btn[3] == 0) {
    //none
  } else {
    pointer = 0;
    if (DEBUG == 1) {
      Serial.print(btn[0]);
      Serial.print(btn[1]);
      Serial.print(btn[2]);
      Serial.print(btn[3]);
      Serial.println("fuck");
    }
  }

  delay(50);// анти дребезг

  if (pointer == 4) {
    delay(1000 * pause);
    if (relaySignal) {
      digitalWrite(relayPin, HIGH);
      digitalWrite(relayPin2, HIGH);
    } else {
      digitalWrite(relayPin, LOW);
      digitalWrite(relayPin2, LOW);
    }
    while (1) {}
  } else {
    if (relaySignal) {
      digitalWrite(relayPin, LOW);
      digitalWrite(relayPin2, LOW);
    } else {
      digitalWrite(relayPin, HIGH);
      digitalWrite(relayPin2, HIGH);
    }
  }
}

