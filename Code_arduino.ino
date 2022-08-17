/* 
Universal CoMBI (c)Yuki Tajika, 2021 (Oct.7)
Hall sensor type (D11):
  DIP1=OFF(HIGH):  TexasInstruments DRV5032-FA unipolar, no-magnet=HIGH, magnet=LOW
  DIP1=ON(LOW):    Unisonic SK8552 unipolar,             no-magnet=LOW, magnet=HIGH
Shutter timing (D12):
  DIP2=OFF(HIGH): passed from the magnet (recommended)
  DIP2=ON(LOW):   approached to the magnet
Microtome type (D9):   Sliding or Rotary
Serial imaging (D10):  ON or OFF
LED resistance (Green-Aqua-Lavender-Lemon)(510-120-120-120 Bright), (670-220-220-220 moderate)

2022-8-10
Bug fix: delay (200) within interuption does not work. Instead, delayMicroseconds (1000) was used repeatedly.
Improve: Focus fold temporally as needed, instead of continuously. Camera controll can be done during Serial ON.
  
*/

volatile byte state = LOW;  // for memo of handle movement; Forward or Backward. Use for interruption.
int sensorVal;              //
int contactTime = 150; 

void setup() {

  pinMode(2, INPUT);         //Hall Sensor Input
  pinMode(3, OUTPUT);        //Optocouplar_Focus (3 and 5)
  pinMode(4, OUTPUT);        //LED Green-510 ohm, Sensor HIGH/LOW when rotary mode, and Serial imaging ON
  pinMode(5, OUTPUT);        //LED Aqua-120 ohm, Optocouplar_Focus (3 and 5)
  pinMode(6, OUTPUT);        //LED Labender-120 ohm, Sensor HIGH/LOW when sliding mode, and Serial imaging ON
  pinMode(7, OUTPUT);        //LED Lemon-120 ohm, Optocouplar_Release (7 and 8)
  pinMode(8, OUTPUT);        //Optocouplar_Release (7 and 8)
  pinMode(9, INPUT_PULLUP);  //SW Mode, sliding or rotary (SW ON=GND=LOW= sliding mode)
  pinMode(10, INPUT_PULLUP); //SW Serial imaging ON/OFF (SW ON=GND=LOW= serial ON)
  pinMode(11, INPUT_PULLUP); //SW Sensor Type, 0-1-0(Unisonic) or 1-0-1(TexasInstruments)
  pinMode(12, INPUT_PULLUP); //Shutter timing

  attachInterrupt(digitalPinToInterrupt(2), release, CHANGE); //CHANGE allow to select HIGH or LOW in void release. Release when reached or passed.
}

void loop() {                    //Visualize sensor value using LED4 and 6.
  sensorVal = digitalRead(2);

  if (digitalRead(11) == HIGH) {   //SW sensor type, TexasInst-FA, 1->0

      if (digitalRead(9) == LOW) {
        digitalWrite(4, !sensorVal);            //Sliding mode (LOW=SWmode ON=GND), LED4 ON
        digitalWrite(6, sensorVal);
      } else {
        digitalWrite(4, sensorVal);           //Rotary mode (HIGH=SWmode OFF=5V), LED6 ON
        digitalWrite(6, !sensorVal);
      }
  }

  if (digitalRead(11) == LOW) {   //SW sensor type, Unisonic, 0->1 display inverted pattern using "!".

      if (digitalRead(9) == LOW) {
        digitalWrite(4, sensorVal);            //Sliding mode(LOW=SWmode ON=GND), LED4 ON
        digitalWrite(6, !sensorVal);
      } else {
        digitalWrite(4, !sensorVal);             //Rotary mode(HIGH=SWmode OFF=5V), LED6 ON
        digitalWrite(6, sensorVal);
      }
  }

  if (digitalRead(10) == LOW) {                  //Serial ON, Focus pin must connects with GND pin contineously, for stable release.
    digitalWrite(5, HIGH); //LED blue ON
  } else {
    digitalWrite(5, LOW);  //LED blue OFF
  }
}

void release(){            //When sensor value CHANGE, interrupt. Choose combination of states; Mode D10, sensor D11, timing D12, and serial D9. 

if (digitalRead(11) == HIGH && digitalRead(12) == HIGH || digitalRead(11) == LOW && digitalRead(12) == LOW){   //TI_FA Release when passed, or Unisonic Release when reached, (&& is prior)

if (digitalRead(2) == HIGH){      //TI_FA Release when passed, or Unisonic Release when reached

  if (digitalRead(10) == LOW){     //Serial ON(＝LOW)
    
      if (digitalRead(9) == LOW){      //Sliding (10, LOW), Use "state". When primary interuption, default state = LOW.
        state = !state;                 //Invert "state". Primary interuption, LOW to HIGH releases shutter. 2nd interuption, HIGH to LOW does not release.
        digitalWrite(3, state); 
        digitalWrite(8, state);              //OC_release
        digitalWrite(7, state);              //LED OC_release
          for(int j=0;j<contactTime;j++){
            delayMicroseconds(1000);
            }
        digitalWrite(8, LOW);
        digitalWrite(7, LOW);
        digitalWrite(3, LOW);               //OC_focus OFF
        } else {                        //Rotary mode(10, HIGH) release shutter anyway. No need to use "state".
        digitalWrite(3, HIGH);              //OC_focus ON
        digitalWrite(7, HIGH);              //OC_release
        digitalWrite(8, HIGH);              //LED_OC_release
          for(int j=0;j<contactTime;j++){
            delayMicroseconds(1000);
            }
        digitalWrite(8, LOW);
        digitalWrite(7, LOW);
        digitalWrite(3, LOW);               //OC_focus OFF
        }
   }
  
  if (digitalRead(10) == HIGH){     //Serial OFF(＝HIGH), only LEDs indicate, optocouplars do nothing.
  
      if (digitalRead(9) == LOW){             //Slinding mode (10, LOW)
        state = !state;
        digitalWrite(5, state);  //LED bleu
        digitalWrite(7, state);  //LED yellow
          for(int j=0;j<contactTime;j++){
            delayMicroseconds(1000);
            }
        digitalWrite(5, LOW);
        digitalWrite(7, LOW);
        } else {                               //Rotary mode (10, HIGH)
        digitalWrite(5, HIGH);  //LED blue
        digitalWrite(7, HIGH);  //LED yellow
          for(int j=0;j<contactTime;j++){
            delayMicroseconds(1000);
            }
        digitalWrite(5, LOW);
        digitalWrite(7, LOW);
        }
   }
}
}

if (digitalRead(11) == HIGH && digitalRead(12) == LOW || digitalRead(11) == LOW && digitalRead(12) == HIGH){      //TI_FA releases when reached or Unisonic releases when passed

if (digitalRead(2) == LOW){

  if (digitalRead(10) == LOW){     //Serial ON＝LOW
    
      if (digitalRead(9) == LOW){      //Sliding (10, LOW) use "state". When primary interruption, default state = LOW
        state = !state;                 //Invert "state". Primary interuption, LOW to HIGH releases shutter. 2nd interuption, HIGH to LOW does not release.
        digitalWrite(3, state);              //OC_focus
        digitalWrite(8, state);              //OC_release
        digitalWrite(7, state);              //LED OC_release
          for(int j=0;j<contactTime;j++){
            delayMicroseconds(1000);
            }
        digitalWrite(8, LOW);
        digitalWrite(7, LOW);
        digitalWrite(3, LOW);
        } else {                        //Rotary (10, HIGH) release shutter anytime of passing. no need to use "state"
        digitalWrite(3, HIGH);              //OC_focus
        digitalWrite(7, HIGH);              //OC_release
        digitalWrite(8, HIGH);              //LED_OC_release
          for(int j=0;j<contactTime;j++){
            delayMicroseconds(1000);
            }
        digitalWrite(8, LOW);               //LED_OC_release OFF
        digitalWrite(7, LOW);               //OC_release OFF
        digitalWrite(3, LOW);
        }
   }
  
  if (digitalRead(10) == HIGH){     //Serial OFF(＝HIGH), LEDs indicate, but optocouplars do nothing
  
      if (digitalRead(9) == LOW){             //Sliding mode(10, LOW)
        state = !state;
        digitalWrite(5, state);  //LED bleu
        digitalWrite(7, state);  //LED yellow
          for(int j=0;j<contactTime;j++){
            delayMicroseconds(1000);
            }
        digitalWrite(5, LOW);
        digitalWrite(7, LOW);
        } else {                                       //Rotary mode(10, HIGH)
        digitalWrite(5, HIGH);  //LED blue
        digitalWrite(7, HIGH);  //LED yellow
          for(int j=0;j<contactTime;j++){
            delayMicroseconds(1000);
            }
        digitalWrite(5, LOW);
        digitalWrite(7, LOW);
        }
   }
}
}

}
