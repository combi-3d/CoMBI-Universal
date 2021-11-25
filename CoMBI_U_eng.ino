/* 
Universal CoMBI (c)Yuki Tajika, 2021 (Oct.7)

Hall sensor type (D11):
  DIP1=OFF(HIGH):  TexasInstruments DRV5032-FA unipolar, no-magnet=HIGH, magnet=LOW
  DIP1=ON(LOW):    Unisonic SK8552 unipolar,             no-magnet=LOW, magnet=HIGH

Shutter timing (D12):
  DIP2=OFF(HIGH): passed from the magnet (recommended)
  DIP2=ON(LOW):   approached to the magnet

Microtome type (D9):   v11 for breadboard, this v12 for PCB. Replace D9 and D10, LED 4 and 6 (Sliding or Rotary)

Serial imaging (D10):
  ON or OFF

LED resistance (Green-Aqua-Lavender-Lemon)(510-120-120-120 Bright), (670-220-220-220 moderate)
*/

volatile byte state = LOW;  // for memo of handle movement; Forward or Backward. Use for interruption.
int sensorVal;              //

void setup() {

  pinMode(2, INPUT);         //Hall Sensor Input
  pinMode(3, OUTPUT);        //Optocouplar_Focus (3 and 5)
  pinMode(4, OUTPUT);        //LED Green-510 ohm, Sensor HIGH/LOW when rotary mode, and Serial imaging ON
  pinMode(5, OUTPUT);        //LED Aqua-120 ohm, Optocouplar_Focus (3 and 5)
  pinMode(6, OUTPUT);        //LED Labender-120 ohm, Sensor HIGH/LOW when sliding mode, and Serial imaging ON
  pinMode(7, OUTPUT);        //LED Lemon-120 ohm, Optocouplar_Release (7 and 8)
  pinMode(8, OUTPUT);        //Optocouplar_Release (7 and 8)
  pinMode(9, INPUT_PULLUP);  //SW Mode, sliding or rotary (SWつながるとGND = LOW、滑走式に設定する)
  pinMode(10, INPUT_PULLUP); //SW Serial imaging ON/OFF (SWつながるとGND = LOW、撮影開始にする)
  pinMode(11, INPUT_PULLUP); //SW Sensor Type, 0-1(Uni) or 1-0(TI), どちらでも通過後にShutterをきるように、統一させる。
  pinMode(12, INPUT_PULLUP); //割り込み、 FALLING or RISING

  attachInterrupt(digitalPinToInterrupt(2), release, CHANGE); //ファイル１１。ここをCHANGEにして、releaseのなかで、HIGH, LOWを区別してみる。通過後、通過前を選択できるように。
}

void loop() {                    //センサー値をLEDで可視化。モードによってLED4赤と6緑
  sensorVal = digitalRead(2);

  if (digitalRead(11) == HIGH) {   //SW sensor type, TexasIーFAは、1->0

      if (digitalRead(9) == LOW) {
        digitalWrite(4, !sensorVal);            //滑走式モード(LOW=switchON=GND)は、LED4赤をON
        digitalWrite(6, sensorVal);
      } else {
        digitalWrite(4, sensorVal);           //回転式モード(HIGH=switchOFF=5V)は、LED6緑をON。磁気を感知すると赤と緑が反転する。
        digitalWrite(6, !sensorVal);
      }
  }

  if (digitalRead(11) == LOW) {   //SW sensor type, Unisonicは、0->1、反転して利用する。

      if (digitalRead(9) == LOW) {
        digitalWrite(4, sensorVal);            //滑走式モード(LOW=switchON=GND)は、LED4赤をON
        digitalWrite(6, !sensorVal);
      } else {
        digitalWrite(4, !sensorVal);             //回転式モード(HIGH=switchOFF=5V)は、LED6緑をON。磁気を感知すると赤と緑が反転する。
        digitalWrite(6, sensorVal);
      }
  }

  if (digitalRead(10) == LOW) {                  //連続撮影ONのとき focusつなぎっぱなし（安定したシャッターに必要）。連続OFFではOFF。
    digitalWrite(3, HIGH); //OC_focus ON
    digitalWrite(5, HIGH); //LED blue ON
  } else {
    digitalWrite(3, LOW);  //OC_focus OFF
    digitalWrite(5, LOW);  //LED blue OFF
  }
}

void release(){            //センサー値がCHANGEしたとき、割り込む。モードpin10、センサーpin11、タイミングpin12、連続pin9で条件分け。

if (digitalRead(11) == HIGH && digitalRead(12) == HIGH || digitalRead(11) == LOW && digitalRead(12) == LOW){      //TI_FAで通過後、または、Unisonicで通過前, &&は優先

if (digitalRead(2) == HIGH){      //TI_FA通過後、Unisonic通過前

  if (digitalRead(10) == LOW){     //連続撮影ON＝LOWのとき
    
      if (digitalRead(9) == LOW){      //滑走式(10, LOW)なら、stateをつかう。最初の割り込みのときはデフォルトのstate = LOW。
        state = !state;                 //stateを反転。はじめの割り込みでは、LOWからHIGHになってシャッターおりる。次の割り込みでは、HIGHからLOWになり、シャッターおりない。
        digitalWrite(8, state);              //OC_release
        digitalWrite(7, state);              //LED OC_release
        delay(200);
        digitalWrite(8, LOW);
        digitalWrite(7, LOW);
        } else {                        //回転式(10, HIGH)なら区別しない、stateを使わず。通過時にとにかくシャッター
        digitalWrite(8, HIGH);              //OC_release
        digitalWrite(7, HIGH);              //LED_OC_release
        delay(200);
        digitalWrite(8, LOW);
        digitalWrite(7, LOW);
        }
   }
  
  if (digitalRead(10) == HIGH){     //連続撮影OFF＝HIGHのとき、LEDだけは光らせる。フォトカプラはなにもしない。
  
      if (digitalRead(9) == LOW){             //モードが滑走式(10, LOW)
        state = !state;
        digitalWrite(5, state);  //LED bleu
        digitalWrite(7, state);  //LED yellow
        delay(200);
        digitalWrite(5, LOW);
        digitalWrite(7, LOW);
        } else {                                       //モードが回転式(10, HIGH)
        digitalWrite(5, HIGH);  //LED blue
        digitalWrite(7, HIGH);  //LED yellow
        delay(200);
        digitalWrite(5, LOW);
        digitalWrite(7, LOW);
        }
   }
}
}

if (digitalRead(11) == HIGH && digitalRead(12) == LOW || digitalRead(11) == LOW && digitalRead(12) == HIGH){      //TI_FAで通過前、または、Unisonicで通過後

if (digitalRead(2) == LOW){

  if (digitalRead(10) == LOW){     //連続撮影ON＝LOWのとき
    
      if (digitalRead(9) == LOW){      //滑走式(10, LOW)なら、stateをつかう。最初の割り込みのときはデフォルトのstate = LOW。
        state = !state;                 //stateを反転。はじめの割り込みでは、LOWからHIGHになってシャッターおりる。次の割り込みでは、HIGHからLOWになり、シャッターおりない。
        digitalWrite(8, state);              //OC_release
        digitalWrite(7, state);              //LED OC_release
        delay(200);
        digitalWrite(8, LOW);
        digitalWrite(7, LOW);
        } else {                        //回転式(10, HIGH)なら区別しない、stateを使わず。通過時にとにかくシャッター
        digitalWrite(8, HIGH);              //OC_release
        digitalWrite(7, HIGH);              //LED_OC_release
        delay(200);
        digitalWrite(8, LOW);
        digitalWrite(7, LOW);
        }
   }
  
  if (digitalRead(10) == HIGH){     //連続撮影OFF＝HIGHのとき、LEDだけは光らせる。フォトカプラはなにもしない。
  
      if (digitalRead(9) == LOW){             //モードが滑走式(10, LOW)
        state = !state;
        digitalWrite(5, state);  //LED bleu
        digitalWrite(7, state);  //LED yellow
        delay(200);
        digitalWrite(5, LOW);
        digitalWrite(7, LOW);
        } else {                                       //モードが回転式(10, HIGH)
        digitalWrite(5, HIGH);  //LED blue
        digitalWrite(7, HIGH);  //LED yellow
        delay(200);
        digitalWrite(5, LOW);
        digitalWrite(7, LOW);
        }
   }
}
}

}

//通り過ぎるときにShutterを切ったほうが操作しやすい。従来、通り過ぎたことを確認するのが難しかった。１）音、シャッター音を利用する。２）視角、磁気があるエリアは、LEDで知らせる。３）試料や刃の位置。
//CHANGEにして、void release内で区別する。
