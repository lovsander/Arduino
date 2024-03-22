//#include "CyberLib.h"
#define trigPin 4 // общий пин для двух выводов дальномера
int floorPosition; //  расстояние до пола
int currHeight, prevHeight, tweakCntrUp, tweakCntrDn;
int tuneOverall; //общая поправка
#define smooth 5 // диапазон усреднения в фильтре
//////////////////////////////////////////
int mode = 0; // 5 mesure mode 0...4 режим, задает номер элемента во всех массивах
String modeName[5] = {"santim", "metro", "inches",  "foots",    "yards"};
// пересчетные коэффициенты для каждого режима
// базовая еденица измерения - сантиметры, все коэфф-ты надо умножать на нее
float modeKoeff[5] = {1, 0.01,  0.39370079, 0.032808399, 0.010936133};
int modeBtnPin[5] = {A0, A1, A2, A3, A4}; // buttons on analog pins
int modeLedPin[5] = {8, 9, 10, 11, 12}; // индикаторы на пинах порта В
//битовая маска для включения индикаторов режимов
//byte ledOnMask[5] = {B00000001, B00000010, B00000100, B00001000, B00010000};

////////////////////////////////
int DS_pin = 5; //data
int STCP_pin = 6; // latch
int SHCP_pin = 7; // sck
int t = 50;
// сочетания сегментов, отвечающие за цифры, точку, тире и пустоту
const uint8_t DIGITS_7SEG[13] = {
  //  .gfedcba
  0b11111111, //0// empty
  0b10100000, //1// 0
  0b11110110, //2// 1
  0b01100001, //3// 2
  0b01100100, //4// 3
  0b00110110, //5// 4
  0b00101100, //6// 5
  0b00101000, //7// 6
  0b11100110, //8// 7
  0b00100000, //9// 8
  0b00100100, //10// 9
  0b11011111, //11// .
  0b01111111, //12// -
};

int n;
/////////////////////////////
void setup() { // настраиваем пины на вход с подтягом и выход
  for (int i = 0; i < 5; i++) {
    pinMode (modeBtnPin[i], INPUT_PULLUP);
    pinMode (modeLedPin[i], OUTPUT);
  }
  ledOnOff(mode);
  //Serial.begin(9600);
  // настройка пинов для драйвера индикаторов
  pinMode(DS_pin, OUTPUT);
  pinMode(STCP_pin, OUTPUT);
  pinMode(SHCP_pin, OUTPUT);
  //////////////////////////
  // настройка пина для дальномера
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  //D4_Out;
  //D4_Low;// — установка низкого уровня на пине 4
  floorPosition = measure(10, 500);// при включении определяем расстояние до пола
  writeSm(floorPosition); // индикация расстояния до пола
  delay(1000);
}

// подпрограмма включающая один указанный в скобках лед а остальные выключает
void ledOnOff(int whichOn) {
  for (int i = 0; i < 5; i++) {
    if (i == whichOn) digitalWrite (modeLedPin[i], HIGH);
    else digitalWrite (modeLedPin[i], LOW);
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

void loop() {
  for (int i = 0; i < 5; i++) {
    //считываем состояния всех кнопок, притянутых к плюсу через встроенный резистор
    if (analogRead(modeBtnPin[i]) < 800) {
      //digitalWrite(13,HIGH);
      delay(100);
      //digitalWrite(13,LOW);
      // и назначаем номер режима
      mode = i;
      // включаем один из светодиодов, а остальные выключаем
      ledOnOff(mode);
    }
  }

  // считаем рост вычитанием от пола того что измерили (x) раз по (y) миллисек
  currHeight = floorPosition - measure(4, 50);
  //нейтрализуем погрешность измерения, тк малый рост показывает верно,а высокий рост заменьшает
  // дробим линейку на 4 равные части
  // и назначаем для каждой части свою поправку tune- нелинейная поправка
  int tune;
  if (currHeight < (floorPosition / 4) )tune = 0;
  else if (currHeight < (floorPosition / 2) ) tune = 1;
  else if (currHeight < (3 * floorPosition / 4) ) tune = 3;
  else  tune = 5;
  // учитываем нелинейную и общую (жестовую) поправки
  currHeight = currHeight + tune + tuneOverall;
  // если есть что-то выше 10 см от пола, считаем это значимым объектом...
  if (currHeight > 10) {
    ////////////////////////GOTO INDICATION\\\\\\\\\\\\\\\\
    if (mode == 0) writeSm(currHeight);
    else writeFloat(float(currHeight) * modeKoeff[mode]);

    //================ жесты для волшебного управления настройками
    // взмах вверх на 4 пункта подряд включает void tweakUP()
    // взмах вниз на 4 пункта подряд включает void tweakDN()
    // жесты широкие, 5см для каждого шага = от 20 см хода, иначе не сработает
    if (currHeight - prevHeight > 5) {// если сделан шаг 5 см
      tweakCntrUp++; // увеличиваем счетчик
      tweakCntrDn = 0; // а другой обнуляем
    } // и наоборот
    if (prevHeight - currHeight > 5) {
      tweakCntrDn++;
      tweakCntrUp = 0;
    } // если есть 4 шага, то жест сработал
    if (tweakCntrUp >= 4) tweakUP(); // взмах вверх
    if (tweakCntrDn >= 4) tweakDN(); // взмах вниз
    // если движение остановлено, не дойдя до срабатывания жеста, то обнуляем счетчики
    if (currHeight == prevHeight) tweakCntrDn = tweakCntrUp = 0;
    delay(100);
    // сохраняем текущее значение для следущего прохода цикла
    prevHeight = currHeight;

    // если нету ничего выше 10 см от пола, то ждущий режим...
  } else { //(currHeight > 10)
    writeToAll(11);
    //обнуляем счетчики
    tweakCntrDn = tweakCntrUp = 0;
    delay(100);
  }
}
/////////////////////////////////////////////////////////
//====================== К чему приводят жесты========
/////////////////////////////////////////////////////////
void tweakUP() {
  writeSm(999);
  //обнуляем счетчики
  tweakCntrDn = tweakCntrUp = 0;
  tuneOverall++;// прибавляем поправку tuneOverall
  delay(1300);
}
void tweakDN() {
  writeSm(111);
  //обнуляем счетчики
  tweakCntrDn = tweakCntrUp = 0;
  tuneOverall--;// уменьшаем поправку (она может быть отрицательной)
  delay(1300);
}
/////////////////////////////////////////////////////////
//     =====================INDICATION===============
/////////////////////////////////////////////////////////
// индикация целочисленных чисел в сантиметрах
void writeSm(int sm) {
  // переменная для вывода на 3 цифры
  uint8_t symb[3];
  // получаем разряд Сотни
  int pos100 = sm / 100;
  // если сотни есть, то берем комбинацию сегментов для этой цифры
  if (pos100 != 0) symb[2] = DIGITS_7SEG[pos100 + 1]; //+1 тк комбинации начинаются с пустоты
  // если сотен нет, то выводим пустоту на индикаторе сотен
  else symb[2] = DIGITS_7SEG[pos100];
  // получаем разряд Десятки
  int pos10 = (sm % 100) / 10;
  // если сотен и десятков нет, то выводим пустоту
  if (pos10 == 0 && pos100 == 0) symb[1] = DIGITS_7SEG[pos10];
  // иначе выводим цифру
  else symb[1] = DIGITS_7SEG[pos10 + 1];
  // получаем разряд Еденицы
  int pos1 = sm % 10;
  // выводим еденицы
  symb[0] = DIGITS_7SEG[pos1 + 1];

  //отправляем выводимое на драйвера
  digitalWrite(STCP_pin, LOW);
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, symb[0]);
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, symb[1]);
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, symb[2]);
  digitalWrite(STCP_pin, HIGH);
}
// индикация чисел с точкой
void writeFloat(float fl) {
  String fstring = (String)fl; //переводим число в строку
  int flPntIndex = fstring.indexOf('.'); //находим позицию точки
  Serial.print(fstring);
  Serial.print('\t');
  if (flPntIndex < 4 && flPntIndex != -1) { //значит это float xxx.xx
    uint8_t fcharBuf[5]; //берем 5 первых символа включая точку
    fstring.getBytes(fcharBuf, 5);
    if (flPntIndex == 3) { //xxx.
      fcharBuf[3] = 3; // в последнем элементе массива позиция точки
    }
    if (flPntIndex == 2) { //xx.x
      fcharBuf[2] = fcharBuf[3]; //передвигаем цифры влево
      fcharBuf[3] = 2; //позиция точки
    }
    if (flPntIndex == 1) { //x.xx
      fcharBuf[1] = fcharBuf[2];
      fcharBuf[2] = fcharBuf[3];
      fcharBuf[3] = 1;
    }
    for (int i = 0; i < 3; i++) {
      Serial.print (fcharBuf[i] - 48);
      fcharBuf[i] = DIGITS_7SEG[(fcharBuf[i] - 48) + 1];
    }
    writeSymbol(fcharBuf);
    Serial.println (fcharBuf[3]);
  }
}
// индикация напрямую того, что лежит в массиве
void writeSymbol(uint8_t symb[4]) {
  digitalWrite(STCP_pin, LOW);
  // добавляем к нужной цифре точку
  // позиция точки указана в последней ячейке массива
  if (symb[3] > 0) bitWrite(symb[symb[3] - 1], 5, LOW); //add dot
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, symb[0]);
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, symb[1]);
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, symb[2]);
  digitalWrite(STCP_pin, HIGH);
}
// индикация на всех экранах одного и того же символа
void writeToAll(uint8_t symb) {
  digitalWrite(STCP_pin, LOW);
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, DIGITS_7SEG[symb]);
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, DIGITS_7SEG[symb]);
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, DIGITS_7SEG[symb]);
  digitalWrite(STCP_pin, HIGH);
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

