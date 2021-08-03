/*
Universal CoMBI

Etsumi release cable convert release pins of any campany's camera into 2.5 mm, 3-pins plug.
Hall-effect sensor, unipolar-type, internal pull-up resistor. Unisonic SK8552
Arduino UNO, ATmega328P
Optocouplar Toshiba TLP785GB 2x
LED 4x
(c)2021 Yuki Tajika, CC-BY-NC
 */
 
volatile byte state = LOW;  //handle direction for sliding microtome
int sensorVal;              //Value of Hall-effect sensor 

void setup() {

  pinMode(2, INPUT);        //Hall-effect Sensor Input
  pinMode(3, OUTPUT);       //Optocouplar_Focus (3 and 5)
  pinMode(4, OUTPUT);       //LED red, Rotary mode, !sensorVal
  pinMode(5, OUTPUT);       //LED blue, Optocouplar_Focus (3 and 5)
  pinMode(6, OUTPUT);       //LED green, Sliding mode, !sensorVal
  pinMode(7, OUTPUT);       //LED yellow, Optocouplar_Release (7 and 8)
  pinMode(8, OUTPUT);       //Optocouplar_Release (7 and 8)
  pinMode(9, INPUT_PULLUP);  //SW Serial imaging ON/OFF
  pinMode(10, INPUT_PULLUP); //SW Mode, Sliding (GND=LOW, SW ON) or Rotary (5V=HIGH)
  
  attachInterrupt(digitalPinToInterrupt(2), release, RISING); //interupt upon sensorVal(LOW to HIGH), note; FALLING, RISING, CHANGE, LOW
}

void loop() {
  sensorVal = digitalRead(2);    //Read sensor value
  sensorVal = !sensorVal;        //invert the value

  if (digitalRead(10) == LOW) {                   //Sliding mode (LOW=switchON=GND), LED red(4) indicates sensorVal
      digitalWrite(4, sensorVal);
      digitalWrite(6, LOW);
  }

  if (digitalRead(10) == HIGH) {                   //Rotary mode (HIGH=switchOFF=5V), LED green(6) indicates sensorVal
      digitalWrite(4, LOW);
      digitalWrite(6, sensorVal);
  }
  
  if (digitalRead(9) == HIGH) {                 //Serial OFF
    digitalWrite(3, LOW);                           //OC_focus OFF
    digitalWrite(5, LOW);                           //LED blue OFF
  }
  
  if (digitalRead(9) == LOW) {                  //Serial ON, OC_focus ON continuously
    digitalWrite(3, HIGH);                          //OC_focus ON
    digitalWrite(5, HIGH);                          //LED blue ON
  }
}

void release(){            //interupt

  if (digitalRead(9) == LOW){     //serial ON(9, LOW)
  
    if (digitalRead(10) == LOW){        //Sliding mode (10, LOW), use state. default state is LOW
      state = !state;                   //invert state. Primary = HIGH, release shutter, secondary = LOW, no shutter.

      digitalWrite(8, state);    //OC_release
      digitalWrite(7, state);    //LED yellow
      delay(200);
      digitalWrite(8, LOW);
      digitalWrite(7, LOW);
    }

    if (digitalRead(10) == HIGH){        //Rotary mode (10, HIGH), shutter release, every detection.

      digitalWrite(8, HIGH);    //OC_release
      digitalWrite(7, HIGH);    //LED yellow
      delay(200);
      digitalWrite(8, LOW);
      digitalWrite(7, LOW);

    }
  }
  
  if (digitalRead(9) == HIGH){     //Serial OFF(9, HIGH), visualize by LED only, no actions on optocouplar
  
    if (digitalRead(10) == LOW){        //Sliding mode (10, LOW)
      state = !state;

      digitalWrite(5, state);  //LED bleu
      digitalWrite(7, state);  //LED yellow
      delay(200);
      digitalWrite(5, LOW);
      digitalWrite(7, LOW);
    }

    if (digitalRead(10) == HIGH){        //Rotary mode (10, HIGH)

      digitalWrite(5, HIGH);  //LED blue
      digitalWrite(7, HIGH);  //LED yellow
      delay(200);
      digitalWrite(5, LOW);
      digitalWrite(7, LOW);

    }
  }
}

// Continous ON of focus optocouplar is needed to make compatible for all cameras (Canon 60D, Nikon D810, Sony a7RIII). 
// When focus optocoupler is included in interuption code (release), shutter release is missed often by Canon, occasionally by Nikon.
