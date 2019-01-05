#include <Servo.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// ** DFPlayer_Miniモジュールを使う場合のメモ
// 1、Arduinoで使う場合、ライブラリはDFRobotDFPlayerMiniを。
//    DFPlayerでググると出てくるブログなどは現在非推奨となっているDFPlayer-Mini-mp3を使用している場合があるので、作例を探す時は注意。
// 2、再生ファイルの指定に使う数値はおそらくFATエントリ順(平たく言えば書き込んだ順)なので、そのあたりを気を付けておけばどんなファイル名でも大丈夫。
// 3、Arduinoなどのコントローラが無くても電源／スピーカー／スイッチなど外付けするだけでも使用できます。アンプを内蔵していますのでスピーカーへの直接出力も可能ですが、その場合、モノラル再生になります。
//
//  参考サイト : https://www.dfrobot.com/wiki/index.php/DFPlayer_Mini_SKU:DFR0299

#define HITOSAHI_PIN 6
#define KUSURI_PIN 8
#define TEKUBI_PIN 5
#define BUTTON_PIN 3
#define SW_RX_PIN 10
#define SW_TX_PIN 11

Servo hitosashi, kusuri, tekubi;
SoftwareSerial mySoftwareSerial(SW_RX_PIN, SW_TX_PIN);
DFRobotDFPlayerMini myDFPlayer;

bool buttonState = LOW;
bool buttonStete_old = LOW;

uint32_t pushStart_time = 0;
uint32_t push_time = 0;
bool     cmd_active = false;
bool     jyanken_start = false;



void setup() {
 Serial.begin(115200);
 mySoftwareSerial.begin(9600);

  // put your setup code here, to run once:
  hitosashi.attach( HITOSAHI_PIN );
  kusuri.attach( KUSURI_PIN );
  tekubi.attach( TEKUBI_PIN );

  // 指の位置のdefault (パーの位置)
  hitosashi.write(0);
  kusuri.write(150);
  tekubi.write(90);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  //スイッチボタンの割り込み処理
  attachInterrupt(1, interruptsw, CHANGE);    // Pin3をスイッチ割込みとして使用

  //スピーカ出力開始
  myDFPlayer.begin(mySoftwareSerial);
  delay(100); //設定待ち時間は仮置き

  myDFPlayer.volume(30);  //Set volume value. From 0 to 30

}

void loop() {
  if(jyanken_start == true){
    // ①グーからスタート(待機)
    hitosashi.write(130);
    kusuri.write(20);
      
    // ②「じゃんけんぽん」に応じて手首を振る
    tekubi.write(90);
    delay(500);
    
    tekubi.write(60);
    delay(300);
    myDFPlayer.play(1);  //Play the first mp3 (0001:じゃんけん合図)
    tekubi.write(90);
    delay(500);
    
    tekubi.write(60);
    delay(300);
    tekubi.write(90);
    delay(500);
    
    tekubi.write(60);
    delay(300);
    tekubi.write(90);
    
    // ③じゃんけんの手(グー/チョキ/パー)を出す
    //   (1) ボタン長押し時間が500ms以下ならグー
    if(push_time < 500){
       // グーの位置
      hitosashi.write(130);
      kusuri.write(20);
      //tekubi.write(120);
      
      delay(500);
      myDFPlayer.play(3);  //Play the first mp3 (0003:グー)
      Serial.println("goo");
   
    // (2) ボタン長押し時間が1000ms以下ならチョキー    
    }else if(push_time < 1000){
     // チョキの位置
      hitosashi.write(0);
      kusuri.write(20);    
      
      delay(500);
      myDFPlayer.play(4);  //Play the first mp3 (0004:チョキ)
      Serial.println("choki"); 
    // (3) ボタン長押し時間が1000ms以上ならチョキー
    }else{
     hitosashi.write(0);
      kusuri.write(150);   

      delay(500);
      myDFPlayer.play(5);  //Play the first mp3 (0005:パー)
      Serial.println("pa");
    }
      
    jyanken_start = false;  
  }
}


// スイッチボタンの割り込み処理
void interruptsw(){

  delay(5); // 5ms待ち (チャタリング対策)
  buttonState = digitalRead(BUTTON_PIN);

  //ボタン押し判定　(押し時間の測定開始)
  if(buttonState == LOW && buttonStete_old == HIGH){
      pushStart_time = millis();
      jyanken_start = true;
  //ボタン離し判定 (押し続けた時間を測定)
  }else if(buttonState == HIGH && buttonStete_old == LOW){
      push_time = millis() - pushStart_time;
      Serial.println(push_time);
  }

  buttonStete_old = buttonState;
}
