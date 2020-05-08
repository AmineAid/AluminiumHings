
#include<avr/wdt.h> 
#include<arduino.h>

int Ncylinder = 6;
int Nsensor = 14;
int Nmux = 6;
int stage = 1;
int laststage = 1;
int Try = 0;
int errornomorebare=0;
int nomorebar = 0;
int nomorebarStep = 0;
int reset_button = 5;
int error_led_relay = 16;
int cylinderpin[6] = {19, 4, 3, 2, 18, 17};
int sensorpin24v[2] = {9, 13};
int muxpins[6] = {8, 7, 6, 15, 14, 12};
int muxpin1 = 10;
int muxpin2 = 11;
int started = 0;
unsigned long lasttimereset;
int longpress=0;
unsigned long time;


int steps[8][6] = {
  {0, 0, 0, 0, 1, 0},
  {0, 1, 0, 0, 1, 0},
  {1, 1, 0, 0, 1, 0},
  {1, 1, 1, 0, 1, 0},
  {1, 0, 1, 0, 1, 1},
  {0, 0, 1, 0, 1, 1},
  {0, 1, 1, 0, 1, 0},
  {0, 1, 1, 1, 1, 0},
};

int conditions[8][14] = {
  {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
  {0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
  {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
  {1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1},
  {1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1},
  {0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1},
  {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1},
  {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1},
};

int sensorstate[14] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int muxselect[15][6] = {
  {1, 1, 1, 0, 0, 0}, //MUX1:0 MUX2:7    Sensor 1
  {1, 1, 0, 0, 0, 0}, //MUX1:1 MUX2:7    Sensor 2
  {1, 0, 1, 0, 0, 0}, //MUX1:2 MUX2:7    Sensor 3
  {1, 0, 0, 0, 0, 0}, //MUX1:3 MUX2:7    Sensor 4
  {0, 1, 1, 0, 0, 0}, //MUX1:4 MUX2:7    Sensor 5
  {0, 1, 0, 0, 0, 0}, //MUX1:5 MUX2:7    Sensor 6
  {0, 0, 1, 0, 0, 0}, //MUX1:6 MUX2:7    Sensor 7
  {0, 0, 0, 1, 1, 1}, //MUX1:7 MUX2:0    Sensor 8
  {0, 0, 0, 1, 1, 0}, //MUX1:7 MUX2:1    Sensor 9
  {0, 0, 0, 1, 0, 1}, //MUX1:7 MUX2:2    Sensor 10
  {0, 0, 0, 1, 0, 0}, //MUX1:7 MUX2:3    Sensor 11
  {0, 0, 0, 0, 1, 1}, //MUX1:7 MUX2:4    Sensor 12
  {0, 0, 0, 0, 1, 0}, //MUX1:7 MUX2:5    Sensor 13
  {0, 0, 0, 0, 0, 1}, //MUX1:7 MUX2:6    Sensor 14
  {0, 0, 0, 0, 0, 0}, //MUX1:7 MUX2:7    No Sensor
};



void setup() {

  pinMode(reset_button, INPUT_PULLUP);
  pinMode(cylinderpin[0], OUTPUT);
  pinMode(cylinderpin[1], OUTPUT);
  pinMode(cylinderpin[2], OUTPUT);
  pinMode(cylinderpin[3], OUTPUT);
  pinMode(cylinderpin[4], OUTPUT);
  pinMode(cylinderpin[5], OUTPUT);


  digitalWrite(cylinderpin[0], LOW);
  digitalWrite(cylinderpin[1], LOW);
  digitalWrite(cylinderpin[2], HIGH);
  digitalWrite(cylinderpin[3], LOW);
  digitalWrite(cylinderpin[4], LOW);
  digitalWrite(cylinderpin[5], LOW);

  pinMode(error_led_relay, OUTPUT);
  blink_error(500);


  pinMode(sensorpin24v[0], INPUT);
  pinMode(sensorpin24v[1], INPUT);
  pinMode(muxpin1, INPUT);
  pinMode(muxpin2, INPUT);


  pinMode(muxpins[0], OUTPUT);
  pinMode(muxpins[1], OUTPUT);
  pinMode(muxpins[2], OUTPUT);
  pinMode(muxpins[3], OUTPUT);
  pinMode(muxpins[4], OUTPUT);
  pinMode(muxpins[5], OUTPUT);
  Serial.begin(9600);
  Serial.println("ON");
wdt_disable();
    delay(2000);
    wdt_enable(WDTO_4S);

}


void loop() {
  wdt_reset();
 
  switch (stage) {
    case 1:
    
      laststage != 1 ? Try = 0 : 1;
      laststage = 1;
      Step(1);
      Try < 1 ? delay(10) : delay(5);
      condition(1) ? stage = 2 : Try++;
      Try > 5 ? stage = 100 : 1;
      wdt_reset();
      break;
    case 2:
      wdt_reset();
      laststage != 2 ? Try = 0 : 1;
      laststage = 2;
      Step(2);
      Try < 1 ? delay(10) : delay(5);
      condition(2) ? stage = 3 : Try++;
      Try > 5 ? stage = 100 : 1;
      wdt_reset();
      break;
    case 3:
      laststage != 3 ? Try = 0 : 1;
      laststage = 3;
      Step(3);
      Try < 1 ? delay(10) : delay(5);
      condition(3) ? stage = 4 : Try++;
      Try > 5 ? stage = 100 : 1;
      wdt_reset();
      break;
    case 4:
      laststage != 4 ? Try = 0 : 1;
      laststage = 4;
      Step(4);
      Try < 1 ? delay(10) : delay(5);
      condition(4) ? stage = 5 : Try++;
      Try > 5 ? stage = 100 : 1;
      wdt_reset();
      break;
    case 5:
      laststage != 5 ? Try = 0 : 1;
      laststage = 5;
      Step(5);
      Try < 1 ? delay(10) : delay(5);
      condition(5) ? stage = 6 : Try++;
      Try > 5 ? stage = 100 : 1;
      wdt_reset();
      break;
    case 6:
      laststage != 6 ? Try = 0 : 1;
      laststage = 6;
      Step(6);
      Try < 1 ? delay(500) : delay(200);
      condition(6) ? stage = 7 : Try++;
      Try > 5 ? stage = 100 : 1;
      wdt_reset();
      break;
    case 7:
      laststage != 7 ? Try = 0 : 1;
      laststage = 7;
      if(Try == 0){
      Step(7);
      mux(7);
      delay(30);
      time = millis();
      time = time + 2000;
      while ( digitalRead(muxpin2) == 1 && millis() < time) {
        delay(5);
      }
      digitalWrite(cylinderpin[3], LOW);
      wdt_reset();
      }
      delay(1000);
      Try < 1 ? delay(10) : delay(5);
      condition(7) ? stage = 1 : Try++;
      Try > 5 ? stage = 100 : 1;
      wdt_reset();
      break;
    case 100:
      digitalWrite(error_led_relay, 1);
      Try = 0;
      Serial.print("error 0x00");
      Serial.println(laststage);
      while (longpress==0){
      while (digitalRead(reset_button)) {
      delay(100);
      wdt_reset();
      }
      lasttimereset=millis();
      lasttimereset = lasttimereset + 1000;
      longpress=1;
       while (millis() < lasttimereset ) {
      delay(30);
      if(digitalRead(reset_button) == HIGH){
        longpress=0;
      }
        wdt_reset();
      }
      }
      longpress=0;
      digitalWrite(error_led_relay, 0);
      stage = laststage;
      errornomorebare=0;
       nomorebar = 0;
       nomorebarStep = 0;
      wdt_reset();
      break;
  }
}

void Step(int number) {
  wdt_reset();
  for (int i = 0; i < Ncylinder; i++) {
    //new 
    digitalWrite(cylinderpin[i], steps[number][i]);
    
    delay(50);
  }

}

boolean condition (int number) {
  wdt_reset();
  readsensor();
  if(errornomorebare==1){
    return (false);
  }

  for (int i = 0; i < Nsensor; i++) {
    if (sensorstate[i] != conditions[number][i]) {
      return (false);
    } else {

    }
  }
  return (true);
}

void readsensor() {
wdt_reset();
  sensorstate[12] = digitalRead(sensorpin24v[0]);
  sensorstate[13] = 1;
  if (nomorebar > 25 && stage == 5) {
    sensorstate[13] = 0;
    nomorebar = 0;
    errornomorebare=1;
   // Serial.print("here");
  } else if (nomorebar < 50 && digitalRead(sensorpin24v[1]) == 0  && nomorebarStep != stage) {
    nomorebar++;
    nomorebarStep = stage;
  }
   else if (nomorebar == 0 && digitalRead(sensorpin24v[1]) == 0){
    nomorebar = 1;
    nomorebarStep = stage;
  }



  
  for (int i = 0; i < 12; i++) {
    mux(i);
    delay(30);
    i < 7 ? sensorstate[i] = digitalRead(muxpin1) : sensorstate[i] = digitalRead(muxpin2);
  }
  if(stage==4){
    sensorstate[10]=1;
    sensorstate[11]=0;
  }
  mux(0);


}

void displaySensors() {
  for (int i = 0; i < 14; i++) {
    Serial.print(i + 1);
    Serial.print(":");
    Serial.print(sensorstate[i]);
    Serial.print("  ");
  }
  Serial.println("");
}

void mux(int number) {

  for (int i = 0; i < Nmux; i++) {
    digitalWrite(muxpins[i], muxselect[number][i]);

  }
}



void blink_error(int delayy) {
wdt_reset();
  digitalWrite(error_led_relay, HIGH);
  delay(delayy);
  digitalWrite(error_led_relay, LOW);
  delay(delayy);
}

