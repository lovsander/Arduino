

#define DEBUG 1
////////////////////////
#define trigPin 8 // общий пин для двух выводов дальномера
#define smooth 5 // диапазон усреднения в фильтре
const int CLOSE = 40;
const int FAR = 49;
const int tooFAR = 65;
const int LLP = 11;//пин левого поворотника
const int RLP = 12;//пин правого поворотника
bool blinks;
const int sound = 780;
// датчик установлен справа от велика
////////////////////////
const int numLeds = 20; // Количество светодиодов -индикаторов
const int gerkonPin = 4; // Пин с подключенным герконом
const int relayPin = 11; // Пин для реле
const int relayPin1 = 10; // Пин для реле
const int reaperStep = 5; // Расстояние между чекпоинтами в как-бы-пространстве (м)
// 0-лево 1-право
const bool storeDirections[9] = {0, 1, 0, 1, 1, 0, 0, 1, 0};
// номера чекпоинтов с поворотами
const int storeNpnts[9] = {3, 4, 6, 9, 11, 13, 16, 17, 18};
const unsigned long interval = 2000;
const unsigned long breakTime = 9000;
////////////////////////////////
bool statePrev = 0; // предыдущее состояние геркона. Нужно для сравнения в цикле с наст. сост.
unsigned int Noborotov = 0; // здесь будет количество оборотов храниться
long previousMillis = 0;
int NobTurn;
bool firstTime = 1;
int nPntPrev = -1;
// Пины для управления микросхемами 74НС595
int DS_pin = 5; //data
int STCP_pin = 6; // latch
int SHCP_pin = 7; // sck

// сочетания зажженных Led
const uint8_t LEDS[9] = {
  0b11111111, // full
  0b11111110,
  0b11111100,
  0b11111000,
  0b11110000,
  0b11100000,
  0b11000000,
  0b10000000,
  0b00000000, // empty
};
const uint8_t LEDSRed[9] = {
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00010000,
  0b00100000,
  0b01000000,
  0b10000000,
  0b00000000, // empty
};
/////////////////////////////
void setup() {
  // настраиваем пин на вход
  pinMode (gerkonPin, INPUT);
  pinMode (LLP, OUTPUT);
  pinMode (RLP, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(relayPin1, OUTPUT);
  digitalWrite(relayPin, LOW);
  if (DEBUG) Serial.begin(9600);
  // настройка пинов для драйвера индикаторов
  pinMode(DS_pin, OUTPUT);
  pinMode(STCP_pin, OUTPUT);
  pinMode(SHCP_pin, OUTPUT);
  //////////////////////////
  // настройка пина для дальномера
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  //////////////////////////
  delay(1000);
}

// Процедура определения числа оборотов
void howMuchTurns() {
  //считываем состояния геркона, притянутого к земле через резистор 10К
  bool state = digitalRead(gerkonPin);
  //если сост геркона не совпадает с предыдущим, то геркон замкнулся или разомкнулся
  if (state != statePrev) {
    // запоминаем это состояние statePrev
    statePrev = state;
    // и увеличиваем число оборотов на 1, если только state=1 то есть когда геркон замкнулся
    if (state) Noborotov++;
  }
}

void howMuchTurns1() {
  Noborotov++;
  delay(800);
}

void loop() {
  howMuchTurns1(); // число оборотов
  if (DEBUG && Noborotov == 1) Serial.println("Nob--nPnt--FT");
  int nPnt = Noborotov / reaperStep; // узнаем сколько чекпоинтов проехали
  if ( nPnt != nPntPrev) {
    nPntPrev = nPnt;
    firstTime = 1;
    if (DEBUG) Serial.println("____________firstTime");
  }
  if (nPnt < numLeds) { // если не доехали до конца пути (посл чекпотнта)
    if (DEBUG) Serial.print(Noborotov);
    if (DEBUG) Serial.print(" ");
    if (DEBUG) Serial.print(nPnt);
    if (DEBUG) Serial.print(" ");
    if (DEBUG) Serial.print(firstTime);
    if (DEBUG) Serial.println(" ");
    indicate(nPnt); // просто зажигаем новый чекпоинт

    bool isTurn = false;
    bool wantedDir;
    // ПРОВЕРКА ЧЕКПОИНТА НА ОЖИДАЕМЫЙ ПОВОРОТ
    for (int i = 0; i < 9; i++) {
      if (storeNpnts[i] == nPnt) {
        isTurn = true;
        // если есть поворот, то узнаем КУДА 0 лево, 1 право
        wantedDir = storeDirections[i];
      }
    }

    //if (DEBUG) Serial.print(isTurn);
    //if (DEBUG) Serial.println(wantedDir);
    unsigned long startTime = millis();

    // если есть поворот влево
    if (isTurn && wantedDir == 0) {
      if (DEBUG) Serial.println("go LEFT");
      // пока не нажат контакт лево
      if (firstTime) {
        blinks = 1;
        firstTime = 0;
        while (!turnAsk(1)) { ///_left_RightCentre
          // индикация моргающих поворотников
          digitalWrite(LLP, blinks);
          digitalWrite(RLP, LOW);
          blinks = !blinks;
          tone(9, sound, 150);
          if (DEBUG) Serial.println("turn LEFT first Time"); delay(800);
          noTone(9);
          unsigned long nowTime = millis();
          // моргалка
          //if ((nowTime - startTime) % 150 == 0)
          if (nowTime - startTime > breakTime) {
            if (DEBUG) Serial.println("left TIMEOUT");
            Noborotov = 0;
            break;
          }
          if (turnAsk(2)) { ///left_Right_Centre
            if (DEBUG) Serial.println("needed left, pressed Right -> Mistake");
            if (nowTime - startTime > breakTime / 2) {
              Noborotov = 0;
              break;
            }
          }
        }
        // индикация стабильных поворотников
        digitalWrite(LLP, HIGH);
        digitalWrite(RLP, LOW);
      }
    }


    // если есть поворот вправо
    if (isTurn && wantedDir == 1) {
      if (DEBUG) Serial.println("go RIGHT");
      // пока не нажат контакт вправо
      if (firstTime) {
        blinks = 1;
        firstTime = 0;
        while (!turnAsk(2)) { ///left_Right_Centre
          // индикация моргающих поворотников
          digitalWrite(LLP, LOW);
          digitalWrite(RLP, blinks);
          tone(9, sound, 150);
          if (DEBUG) Serial.println("turn RIGHT first Time"); delay(800);
          noTone(9);

          unsigned long nowTime = millis();
          // моргалка
          //if ((nowTime - startTime) % 150 == 0)
          blinks = !blinks;
          if (nowTime - startTime > breakTime) {
            if (DEBUG) Serial.println("right TIMEOUT");
            Noborotov = 0;
            break;
          }
          if (turnAsk(1)) { ///_left_RightCentre
            if (DEBUG) Serial.println("needed right, pressed Left -> Mistake");
            if (nowTime - startTime > breakTime / 2) {
              Noborotov = 0;
              break;
            }
          }
        }
        // индикация стабильных поворотников
        digitalWrite(LLP, LOW);
        digitalWrite(RLP, HIGH);
      }
    }

    // если надо держать руль прямо
    if (!isTurn) {
      digitalWrite(LLP, LOW);
      digitalWrite(RLP, LOW);
      if (DEBUG) Serial.println("go AHEAD");
      if (firstTime) {
        firstTime = 0;
        while (!turnAsk(3)) { ///leftRight_Centre_
          tone(9, sound, 150);

          if (DEBUG) Serial.println("set AHEAD first Time"); delay(800);
          noTone(9);

          unsigned long nowTime = millis();
          if (nowTime - startTime > breakTime) {
            if (DEBUG) Serial.println("ahead TIMEOUT");
            Noborotov = 0;
            break;
          }
        }


      }
    }


  } else { // иначе сообщаем о победе!

    digitalWrite(relayPin, HIGH);// врубаем реле!
    digitalWrite(relayPin1, HIGH);// врубаем реле!
    if (DEBUG) Serial.println("******* Y O U W I N !!! *********");
    YouWin(); // зажигаем все чекпоинты

  }

}




/////////////////////////////////////////////////////////
// =====================INDICATION===============
/////////////////////////////////////////////////////////
void indicate(int nPoint) {
  int wait = 0; // 1-значит что надо подождать перед тем как зажишать зеленый
  // на перекрестве. 0 значит что зеленая сразу загорается
  uint8_t finalRow = LEDSRed[9];
  for (int i = 0; i < 9; i++) {
    if (storeNpnts[i] == nPoint) {
      finalRow = LEDSRed[7 - i];
      wait = 1;
    }
  }
  uint8_t finalRow2 = LEDSRed[9];
  if (nPoint == 3) {
    finalRow2 = LEDSRed[6];
    wait = 1;
  } else if (nPoint == 18) {
    finalRow2 = LEDSRed[7];
    wait = 1;
  }

  if (nPoint <= 8) {
    // Первая восьмерка Leds 1-8
    //if (DEBUG) Serial.print("<9 ");
    //if (DEBUG) Serial.println(nPoint);

    digitalWrite(STCP_pin, LOW);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, finalRow2);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, finalRow);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8 - nPoint + wait]);
    digitalWrite(STCP_pin, HIGH);

  } else if (nPoint <= 16) {
    // Вторая восьмерка Leds 9-16
    //if (DEBUG) Serial.print("<17 ");
    //if (DEBUG) Serial.println(nPoint);


    digitalWrite(STCP_pin, LOW);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, finalRow2);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, finalRow);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[16 - nPoint + wait]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[0]);
    digitalWrite(STCP_pin, HIGH);

  } else {
    // Третья восьмерка Leds 16-24
    //if (DEBUG) Serial.print("<24 ");
    //if (DEBUG) Serial.println(nPoint);


    digitalWrite(STCP_pin, LOW);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, finalRow2);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, finalRow);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[24 - nPoint + wait]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[0]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[0]);
    digitalWrite(STCP_pin, HIGH);

  }
}



void YouWin() { // Победа!
  while (1) {// зависаем
    digitalWrite(STCP_pin, LOW);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[0]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[0]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[0]);
    digitalWrite(STCP_pin, HIGH);
    delay(700);
    digitalWrite(STCP_pin, LOW);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[0]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[0]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[0]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    digitalWrite(STCP_pin, HIGH);
    delay(700);
    digitalWrite(STCP_pin, LOW);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[8]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[0]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[0]);
    shiftOut(DS_pin, SHCP_pin, LSBFIRST, LEDS[0]);
    digitalWrite(STCP_pin, HIGH);
  }
}





// ============ Логика Ответа на запрос- что по направлению =======================
bool turnAsk(int leftRightCentre) {
  int m = measure(5, 10);
  if (DEBUG) {
    Serial.print("____________DISTANCE= ");
    Serial.println(m);
  }
  /*
  // калибровочная индикация поворотников
  if (m >= FAR) {
  digitalWrite(LLP, HIGH);
  digitalWrite(RLP, LOW);
  } else if (m <= CLOSE) {
  digitalWrite(LLP, LOW);
  digitalWrite(RLP, HIGH);
  } else {
  digitalWrite(LLP, LOW);
  digitalWrite(RLP, LOW);
  }
  */


  if (leftRightCentre == 1 && m >= FAR && m <= tooFAR) { //LEFT ASK
    if (DEBUG) {
      Serial.print("____________TURNed LEFT ");
      Serial.println(m);
    }
    return 1;
  } else if (leftRightCentre == 2 && m <= CLOSE) {//RIGHT ASK
    if (DEBUG) {
      Serial.print("____________TURNed RIGHT ");
      Serial.println(m);
    }
    return 1;
  } else if (leftRightCentre == 3 && m < FAR && m > CLOSE) { //CENTER ASK
    if (DEBUG) {
      Serial.print("____________TURNed CENTER ");
      Serial.println(m);
    }
    return 1;
  } else {
    if (DEBUG) {
      Serial.print("____________TURN FUCKED!!!!! ");
      Serial.println(m);
    }
    return 0;
  }
}

//============ Расчет расстояний=======================
// сколько раз мерить и по сколько времени тратить на замер?
int measure(uint8_t cycles, uint16_t delayTime) {
  //unsigned int maxDist, sum;
  uint16_t distance[cycles];
  for (uint8_t i = 0; i < cycles; i++) {
    pinMode(trigPin, OUTPUT);
    digitalWrite(trigPin, LOW);
    digitalWrite(trigPin, HIGH);
    //D4_Out; //— установка пина Х как выход
    //D4_Low; //— установка низкого уровня на пине 4
    //D4_High; //— установка низкого уровня на пине 4
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    pinMode(trigPin, INPUT);
    //D4_Low; //— установка низкого уровня на пине 4
    //D4_In; //— установка пина Х как вход
    distance[i] = (pulseIn(trigPin, HIGH) / 58);
    delay(delayTime);

  }
  //Пользуемся фильтром из либы CyberLib
  uint16_t filtered = find_similar(distance, sizeof(distance), smooth);
  return (filtered);
}

uint16_t find_similar(uint16_t *buf, uint8_t size_buff, uint8_t range)
{
  uint8_t maxcomp = 0; //счётчик максимального колличества совпадений
  uint16_t mcn = 0; //максимально часто встречающийся элемент массива
  uint16_t comp; //временная переменная
  range++; //допустимое отклонение

  for (uint8_t i = 0; i < size_buff; i++)
  {
    comp = buf[i]; //кладем элемент массива в comp
    uint8_t n = 0; //счётчик совпадении
    for (uint8_t j = 0; j < size_buff; j++) {
      if (buf[j] > comp - range && buf[j] < comp + range) n++; // ищем повторения элемента comp в массиве buf
    }
    if (n > maxcomp) //если число повторов больше чем было найдено ранее
    {
      maxcomp = n; //сохраняем счетяик повторов
      mcn = comp; //сохраняем повторяемый элемент
    }
  }
  return mcn;
}


