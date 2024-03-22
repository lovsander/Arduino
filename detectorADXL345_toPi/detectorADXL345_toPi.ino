#include "defines.h"
#include "globals.h"
void setup() {
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  pinMode(13, OUTPUT);
  LOG_INIT(57600);
  LOGLN("BEGIN");
  /* Initialise the sensor */
  if (!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1) {
      digitalWrite(13, !digitalRead(13));
      delay(1000);
    }
  }
  accel.setRange(ADXL345_RANGE_16_G);
  accel.setDataRate(ADXL345_DATARATE_3200_HZ);
  delay(2000);
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
#ifdef NDEBUG
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
#endif

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    // Serial.print(inputString);
    Serial.println(analiz());
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  //****************************************************
  dtc1.update();

  //if (dtc1.punchMax()) punchMax = punchToScore(dtc1.punchMax());
  //if (dtc1.punch()) punch = punchToScore(dtc1.punch());
  //if (dtc1.ovfs()) ovfs = dtc1.ovfs();
#ifdef NDEBUG
  if (dtc1.punchMax()) punchMax = dtc1.punchMax();
  if (dtc1.punch()) punch = dtc1.punch();
  if (dtc1.ovfs()) ovfs = dtc1.ovfs();
#else
  /*
    LOG("x = ");
    LOGV(accel.GetRawAccX());
    LOG(" y = ");
    LOGV(accel.GetRawAccY());
    LOG(" z = ");
    LOGV(accel.GetRawAccZ());
    LOG(" xyz = ");
    LOGVLN(accel.calcAcc());
  */
  LOG(" got = ");
  LOGV(dtc1.got());
  LOG(" stills = ");
  LOGV(dtc1.stills());
  LOG(" force = ");
  LOGV(dtc1.force());
  LOG(" punchMax = ");
  LOGV(dtc1.punchMax());
  LOG(" punch = ");
  LOGVLN(dtc1.punch());

  delay(50);
#endif
}
