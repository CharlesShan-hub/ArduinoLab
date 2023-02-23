//库文件
#include<Wire.h>
#include<LiquidCrystal_I2C.h>
#include <IRremote.h>
#include"pitches.h"

// 定义每个音的引脚
#define p_do1 A1
#define p_re1 A2
#define p_mi1 A3
#define p_fa1 A4
#define p_so1 A5
#define p_la1 A6
#define p_xi1 A7
#define p_do2 A8
#define output 2

// 用于判断开关机
#define Open_button 14
int Open = 0;

// 定义每个音的默认频率
int do1 = NOTE_C4;
int re1 = NOTE_D4;
int mi1 = NOTE_E4;
int fa1 = NOTE_F4;
int so1 = NOTE_G4;
int la1 = NOTE_A4;
int xi1 = NOTE_B4;
int do2 = NOTE_C5;

// 定义默认音高与持续时间
int tune = 3;
int Dura = 200;

// 用于限定播放第一个音
int first = 0;
int ifread = 0;

// 用于检测火焰
int fire = A9;    //定义A9为模拟量读取引脚
int val = 0;      //定义一个变量

// 用于判断是否进行颤音
int iftre = 0;
int ifmolo=0;

int RECV_PIN = 11;  //定义接受遥控器信号的引脚
int Rec_Play = 8;   //定义录播芯片的播放引脚
int Rec_Record = 7; //定义录播芯片的录音引脚

int MusicMode = 1;
LiquidCrystal_I2C lcd(0x27, 16, 2);
IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  // 显示器
  lcd.init();
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver

  // 设置引脚
  pinMode(output, OUTPUT); //2
  pinMode(p_do1, INPUT); //A1
  pinMode(p_re1, INPUT); //A2
  pinMode(p_mi1, INPUT); //A3
  pinMode(p_fa1, INPUT); //A4
  pinMode(p_so1, INPUT); //A5
  pinMode(p_la1, INPUT); //A6
  pinMode(p_xi1, INPUT); //A7
  pinMode(p_do2, INPUT); //A8
  pinMode(fire, INPUT);  //A9
}

void loop() {
  // 开关机代码
  if (digitalRead(Open_button))//按一下开机，再按一下关机
  {
    delay(300);
    
    if (Open)
    { Open = 0;
      Serial.print("close");
      
      tone(output,NOTE_C6,100);
      delay(100);
      tone(output,NOTE_G5,100);
      delay(100);
      tone(output,NOTE_E5,100);
      delay(100);
      tone(output,NOTE_C5,100);
      lcd.setBacklight(0);
    }
    else
    { Open = 1;
      Serial.print("open");
      tone(output,NOTE_C5,100);
      delay(100);
      tone(output,NOTE_E5,100);
      delay(100);
      tone(output,NOTE_G5,100);
      delay(100);
      tone(output,NOTE_C6,100);
    }
  }

  if (Open)
  {
    // LCD
    lcd.backlight();
    lcd.setCursor(0, 1);
    if (MusicMode == 1)
      lcd.print("Normal Mode ");
    else if (MusicMode == 2)
      lcd.print("Tiktok Mode ");
    else if (MusicMode == 3)
      lcd.print("Tremolo Mode");
    else
      lcd.print("Shaking Mode");

    // 音乐
    Play();
    RemoteControl();

    //检测火焰
    val = analogRead(fire);
    //Serial.println(val);
    if (val >= 450) 
    happy_birthday(); // 生日歌（报警）
  }
}

int aa = 0;
//遥控器模块
void RemoteControl()
{
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    switch (results.value)
    {
      case 0xFF22DD://<<倒带
        SwitchVoice(0);
        Change_Mode(0);
        break;
        
      case 0xFF02FD://>>快进
        SwitchVoice(1);
        Change_Mode(1);
        break;

      case 0xFFA25D: //CH-
        SwitchVoice(0);
        if (tune > 1)
          tune = tune - 1;
        break;

      case 0xFFE21D: //CH+
        SwitchVoice(1);
        if (tune < 5)
          tune = tune + 1;
        break;

      case 0xFF629D: //CH
      tone(output,629,100);
delay(120);
      tone(output,629,100);
    
        tune = 3;
        break;

      case 0xFF6897://0(随机播放)
        randomSeed(analogRead(12));//选择一个没有空接口(pin)
        switch(random(1,3))
        {
          case 1:
          demo_play1();
          break;
          case 2:
          demo_play2();
          break;
          case 3:
          demo_play3();
          break;
          case 4:
          demo_play4();
          break;
          case 5:
          demo_play5();
          break;
          case 6:
          demo_play6();
          break;

        }
        break;

      case 0xFF30CF://1
        demo_play1();
        break;

      case 0xFF18E7://2
        demo_play2();
        break;

      case 0xFF7A85://3
        demo_play3();
        break;
       case 0xFF10EF://4
       demo_play4();
       break;
       case 0xFF38C7://5
       demo_play5();
       break;
       
       case 0xFF5AA5://6
       demo_play6();
       break;
       case 0xFF42BD://7
       break;
       case 0xFF4AB5://8
       break;
       case 0xFF52AD://9
       break;
       
      case 0xFFC23D://play & pause
        digitalWrite(Rec_Play, HIGH);
        break;

      case 0xFF906F://EQ：Record
        if (digitalRead(Rec_Record))
          digitalWrite(Rec_Record, LOW);
        else
          digitalWrite(Rec_Record, HIGH);
        break;

      default:
        break;

    }
    irrecv.resume(); // Receive the next value
  }
  delay(100);
}

void SwitchVoice(int NextorLast)
{
  if (NextorLast)
  { for (aa = 500; aa < 1000; aa++)
      tone(output, aa, 1);

  }
  else
  { for (aa = 1000; aa > 500; aa--)
      tone(output, aa, 1);
  }

}


void Change_Mode(int NextorLast)
{
  if ( NextorLast)
  {
    if ( MusicMode == 4 )
      MusicMode = 1;
    else
      MusicMode++;
  }
  else
  {
    if (MusicMode == 1)
      MusicMode = 4;
    else
      MusicMode--;
  }

  switch (MusicMode)
  {
    case 1://正常模式
      iftre=0;
      Dura = 200;
      ifmolo=0;
      break;

    case 2://跳音模式
      iftre=0;
      Dura = 30;
      ifmolo=0;
      break;

    case 3://颤音模式
      iftre=1;
      ifmolo=0;
      Dura =200;
      break;

    case 4://震音模式
      iftre=0;
      Dura = 200;
      ifmolo=1;
      break;
  }
}
void Play()
{
  // 1 先进行音域选择
  if (tune == 3)
  {
    lcd.setCursor(0, 0);
    lcd.print("Current Tune:3");
    lcd.setCursor(13, 1);
    do1 = NOTE_C5;
    re1 = NOTE_D5;
    mi1 = NOTE_E5;
    fa1 = NOTE_F5;
    so1 = NOTE_G5;
    la1 = NOTE_A5;
    xi1 = NOTE_B5;
    do2 = NOTE_C6;
    /* 保留一段原来的代码（tune=2,4时类似）
      if (digitalRead(p_do1)) {tone (output, NOTE_C5, Dura);lcd.print("Do");}
      if (digitalRead(p_re1)) {tone (output, NOTE_D5, Dura);lcd.print("Re");}
      if (digitalRead(p_mi1)) {tone (output, NOTE_E5, Dura);lcd.print("Mi");}
      if (digitalRead(p_fa1)) {tone (output, NOTE_F5, Dura);lcd.print("Fa");}
      if (digitalRead(p_so1)) {tone (output, NOTE_G5, Dura);lcd.print("So");}
      if (digitalRead(p_la1)) {tone (output, NOTE_A5, Dura);lcd.print("La");}
      if (digitalRead(p_xi1)) {tone (output, NOTE_B5, Dura);lcd.print("Ti");}
      if (digitalRead(p_do2)) {tone (output, NOTE_C6, Dura);lcd.print("Do");}
    */
  }
  if (tune == 2)
  {
    lcd.setCursor(0, 0);
    lcd.print("Current Tune:2");
    lcd.setCursor(13, 1);
    do1 = NOTE_C4;
    re1 = NOTE_D4;
    mi1 = NOTE_E4;
    fa1 = NOTE_F4;
    so1 = NOTE_G4;
    la1 = NOTE_A4;
    xi1 = NOTE_B4;
    do2 = NOTE_C5;
  }
  if (tune == 4)
  {
    lcd.setCursor(0, 0);
    lcd.print("Current Tune:4");
    lcd.setCursor(13, 1);
    do1 = NOTE_C6;
    re1 = NOTE_D6;
    mi1 = NOTE_E6;
    fa1 = NOTE_F6;
    so1 = NOTE_G6;
    la1 = NOTE_A6;
    xi1 = NOTE_B6;
    do2 = NOTE_C7;
  }
  if (tune == 1)
  {
    lcd.setCursor(0, 0);
    lcd.print("Current Tune:1");
    lcd.setCursor(13, 1);
    do1 = NOTE_C3;
    re1 = NOTE_D3;
    mi1 = NOTE_E3;
    fa1 = NOTE_F3;
    so1 = NOTE_G3;
    la1 = NOTE_A3;
    xi1 = NOTE_B3;
    do2 = NOTE_C4;
  }
  if (tune == 5)
  {
    lcd.setCursor(0, 0);
    lcd.print("Current Tune:5");
    lcd.setCursor(13, 1);
    do1 = NOTE_C7;
    re1 = NOTE_D7;
    mi1 = NOTE_E7;
    fa1 = NOTE_F7;
    so1 = NOTE_G7;
    la1 = NOTE_A7;
    xi1 = NOTE_B7;
    do2 = NOTE_C8;
  }

  // 4 重置
  ifread = int(digitalRead(p_do1)) + int(digitalRead(p_re1)) + int(digitalRead(p_mi1))
           + int(digitalRead(p_fa1)) + int(digitalRead(p_so1)) + int(digitalRead(p_la1))
           + int(digitalRead(p_xi1)) + int(digitalRead(p_do2));
  if (ifread == 0 || ifread == 1)
    first = 0;

  // 2 再进行声音发音 - 保存上一个音
  if (first == 0) {
    if (digitalRead(p_do1) == HIGH) {
      first = p_do1;
    }
    else if (digitalRead(p_re1) == HIGH) {
      first = p_re1;
    }
    else if (digitalRead(p_mi1) == HIGH) {
      first = p_mi1;
    }
    else if (digitalRead(p_fa1) == HIGH) {
      first = p_fa1;
    }
    else if (digitalRead(p_so1) == HIGH) {
      first = p_so1;
    }
    else if (digitalRead(p_la1) == HIGH) {
      first = p_la1;
    }
    else if (digitalRead(p_xi1) == HIGH) {
      first = p_xi1;
    }
    else if (digitalRead(p_do2) == HIGH) {
      first = p_do2;
    }
  }

  // 3 再进行声音发音 - 进行第一个音的播出
  if (first == p_do1) {
    if(iftre){
      tone(output, do1, Dura/2);
      delay(Dura/2);
      tone(output, re1, Dura/2);
      }
    else if(ifmolo){
      tone(output, do1, Dura/3);
      delay(Dura/3);
      tone(output, mi1, Dura/3);
      delay(Dura/3);
      tone(output, so1, Dura/3);
      }
    else tone(output, do1, Dura);
    lcd.print("Do");
  }
  else if (first == p_re1) {
    if(iftre){
      tone(output, re1, Dura/2);
      delay(Dura/2);
      tone(output, mi1, Dura/2);
      }
    else if(ifmolo){
      tone(output, re1, Dura/3);
      delay(Dura/3);
      tone(output, fa1, Dura/3);
      delay(Dura/3);
      tone(output, la1, Dura/3);
      }
    else tone(output, re1, Dura);
    lcd.print("Re");
  }
  else if (first == p_mi1) {
    if(iftre){
      tone(output, mi1, Dura/2);
      delay(Dura/2);
      tone(output, fa1, Dura/2);
      }
    else if(ifmolo){
      tone(output, mi1, Dura/3);
      delay(Dura/3);
      tone(output, so1, Dura/3);
      delay(Dura/3);
      tone(output, xi1, Dura/3);
      }
    else tone(output, mi1, Dura);
    lcd.print("Mi");
  }
  else if (first == p_fa1) {
    if(iftre){
      tone(output, fa1, Dura/2);
      delay(Dura/2);
      tone(output, so1, Dura/2);
      }
    else if(ifmolo){
      tone(output, fa1, Dura/3);
      delay(Dura/3);
      tone(output, la1, Dura/3);
      delay(Dura/3);
      tone(output, do2, Dura/3);
      }
    else tone(output, fa1, Dura);
    lcd.print("Fa");
  }
  else if (first == p_so1) {
    if(iftre){
      tone(output, so1, Dura/2);
      delay(Dura/2);
      tone(output, la1, Dura/2);
      }
    else if(ifmolo){
      tone(output, so1, Dura/3);
      delay(Dura/3);
      tone(output, xi1, Dura/3);
      delay(Dura/3);
      tone(output, re1*2, Dura/3);
      }
    else tone(output, so1, Dura);
    lcd.print("So");
  }
  else if (first == p_la1) {
    if(iftre){
      tone(output, la1, Dura/2);
      delay(Dura/2);
      tone(output, xi1, Dura/2);
      }
    else if(ifmolo){
      tone(output, la1, Dura/3);
      delay(Dura/3);
      tone(output, do2, Dura/3);
      delay(Dura/3);
      tone(output, mi1*2, Dura/3);
      }
    else tone(output, la1, Dura);
    lcd.print("La");
  }
  else if (first == p_xi1) {
    if(iftre){
      tone(output, xi1, Dura/2);
      delay(Dura/2);
      tone(output, do2, Dura/2);
      }
    else if(ifmolo){
      tone(output, xi1, Dura/3);
      delay(Dura/3);
      tone(output, re1*2, Dura/3);
      delay(Dura/3);
      tone(output, fa1*2, Dura/3);
      }
    else tone(output, xi1, Dura);
    lcd.print("Ti");
  }
  else if (first == p_do2) {
    if(iftre){
      tone(output, do2, Dura/2);
      delay(Dura/2);
      tone(output, re1*2, Dura/2);
      }
    else if(ifmolo){
      tone(output, do2, Dura/3);
      delay(Dura/3);
      tone(output, mi1*2, Dura/3);
      delay(Dura/3);
      tone(output, so1*2, Dura/3);
      }
    else tone(output, do2, Dura);
    lcd.print("Do");
  }
}

void happy_birthday()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Happy Birthday!");
  lcd.setCursor(0, 1);
  lcd.print("Happy Everyday!");
  int melody[] = {
    NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_C6, NOTE_B5,
    NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D6, NOTE_C6,
    NOTE_G5, NOTE_G5, NOTE_G6, NOTE_E6, NOTE_C6, NOTE_B5, NOTE_A5,
    NOTE_F6, NOTE_F6, NOTE_E6, NOTE_C6, NOTE_D6, NOTE_C6
  };

  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  int noteDurations[] = {/*64个音*/
    4, 4, 2, 2, 2, 1,
    4, 4, 2, 2, 2, 1,
    4, 4, 2, 2, 2, 2, 1,
    4, 4, 2, 2, 2, 1
  };

  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 25; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(output, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop
    if (int(digitalRead(p_do1)) + int(digitalRead(p_re1)) + int(digitalRead(p_mi1))
        + int(digitalRead(p_fa1)) + int(digitalRead(p_so1)) + int(digitalRead(p_la1))
        + int(digitalRead(p_xi1)) + int(digitalRead(p_do2)) ) {
      delay(pauseBetweenNotes);
      lcd.clear();
      break;
    }
    // stop the tone playing:
    noTone(output);
  }
  lcd.clear();
}

void demo_play1() //抬棺
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("demo play1");
  int melody[] = {
    /*PRE*/
    NOTE_C5, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_E5, NOTE_E5, NOTE_E5,  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_D5,  NOTE_D5, NOTE_G5, NOTE_G5, NOTE_G5, NOTE_G5,
    NOTE_A5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_A5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_G4,
    /*enter*/
    NOTE_A4, NOTE_A4, NOTE_E5, NOTE_D5, NOTE_C5,  NOTE_B4, NOTE_B4, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_B4,
    NOTE_A4, NOTE_A4, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_B5, NOTE_C6,  NOTE_A4, NOTE_A4, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_B5, NOTE_C6,
    NOTE_A4, NOTE_A4, NOTE_E5, NOTE_D5, NOTE_C5,  NOTE_B4, NOTE_B4, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_B4,
    NOTE_A4, NOTE_A4, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_B5, NOTE_C6,  NOTE_A4, NOTE_A4, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_B5, NOTE_C6,
    NOTE_A4, NOTE_A4, NOTE_E5, NOTE_D5, NOTE_C5,  NOTE_B4, NOTE_B4, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_B4,
    NOTE_A4, NOTE_A4, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_B5, NOTE_C6,  NOTE_A4, NOTE_A4, NOTE_C6, NOTE_B5, NOTE_C6, NOTE_B5, NOTE_C6
  };

  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  int noteDurations[] = {/*64个音*/
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    4, 8, 8, 4, 4,  4, 8, 8, 4, 8, 8,
    4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8,
    4, 8, 8, 4, 4,  4, 8, 8, 4, 8, 8,
    4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8,
    4, 8, 8, 4, 4,  4, 8, 8, 4, 8, 8,
    4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8

  };

  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 107; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(output, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop
    if (int(digitalRead(p_do1)) + int(digitalRead(p_re1)) + int(digitalRead(p_mi1))
        + int(digitalRead(p_fa1)) + int(digitalRead(p_so1)) + int(digitalRead(p_la1))
        + int(digitalRead(p_xi1)) + int(digitalRead(p_do2)) ) {
      delay(pauseBetweenNotes);
      lcd.clear();
      break;
    }
    // stop the tone playing:
    noTone(output);
  }
  lcd.clear();
}

void demo_play2()//卡农
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("demo play2"); 
    int melody[] = {
    /*PRE*/
    NOTE_C5, NOTE_B4, NOTE_C5, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_B4,  NOTE_C5, NOTE_B4, NOTE_C5, NOTE_E4,  NOTE_G5, NOTE_E5, NOTE_G5, NOTE_A5, 
    NOTE_F5, NOTE_E5, NOTE_D5, NOTE_F5,  NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4,  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_C5,  NOTE_B4,NOTE_G4, NOTE_C5, NOTE_B4,
   /*enter*/
    NOTE_G5,NOTE_E5,NOTE_F5,  NOTE_G5,NOTE_E5,NOTE_F5,    NOTE_G5,NOTE_G4,NOTE_A4, NOTE_B4,NOTE_C5,NOTE_D5, NOTE_E5,NOTE_F5, 
 NOTE_E5,NOTE_C5,NOTE_D5,    NOTE_E5,NOTE_E4,NOTE_F4,    NOTE_G4,NOTE_A4,NOTE_G4,    NOTE_E4,NOTE_G4,NOTE_C5,    NOTE_B4,NOTE_C5,
 NOTE_A4,NOTE_C5,NOTE_B4, NOTE_A4,NOTE_G4,NOTE_F4,  NOTE_G4,NOTE_F4,NOTE_E4,    NOTE_F4,NOTE_G4,NOTE_A4,    NOTE_B4,NOTE_C5,    
  NOTE_A4,NOTE_C5,NOTE_B4,   NOTE_C5,NOTE_B4,NOTE_A4,   NOTE_B4,NOTE_A4,NOTE_B4,   NOTE_C5,NOTE_D5,NOTE_E5,   NOTE_F5,NOTE_G5,   
/*repeat*/
 NOTE_G5,NOTE_E5,NOTE_F5,  NOTE_G5,NOTE_E5,NOTE_F5,    NOTE_G5,NOTE_G4,NOTE_A4, NOTE_B4,NOTE_C5,NOTE_D5, NOTE_E5,NOTE_F5, 
 NOTE_E5,NOTE_C5,NOTE_D5,    NOTE_E5,NOTE_E4,NOTE_F4,    NOTE_G4,NOTE_A4,NOTE_G4,    NOTE_E4,NOTE_G4,NOTE_C5,    NOTE_B4,NOTE_C5,
 NOTE_A4,NOTE_C5,NOTE_B4, NOTE_A4,NOTE_G4,NOTE_F4,  NOTE_G4,NOTE_F4,NOTE_E4,    NOTE_F4,NOTE_G4,NOTE_A4,    NOTE_B4,NOTE_C5,    
  NOTE_A4,NOTE_C5,NOTE_B4,   NOTE_C5,NOTE_B4,NOTE_A4,   NOTE_B4,NOTE_A4,NOTE_B4,   NOTE_C5,NOTE_D5,NOTE_E5,   NOTE_F5,NOTE_G5,  
/*ending*/
NOTE_E5,NOTE_C5,NOTE_D4,NOTE_E5,NOTE_D5,NOTE_C5,NOTE_D5,NOTE_B4,NOTE_C5,NOTE_D5,NOTE_E5,NOTE_D5,NOTE_C5,NOTE_B4,
NOTE_C5,NOTE_A4,NOTE_B4,NOTE_C5,NOTE_E4,NOTE_F4,NOTE_G4,NOTE_A4,NOTE_G4,NOTE_F4,NOTE_G4,NOTE_C5,NOTE_B4,NOTE_C5,
NOTE_A4,NOTE_C5,NOTE_B4,NOTE_A4,NOTE_G4,NOTE_F4,NOTE_G4, NOTE_F4,NOTE_E4,NOTE_F4,NOTE_G5,NOTE_A4,NOTE_B4,NOTE_C5,
NOTE_A4,NOTE_C5,NOTE_B4,NOTE_C5,NOTE_B4,NOTE_A4,NOTE_B4, NOTE_C5,NOTE_D5,NOTE_C5,NOTE_B4,NOTE_C5,NOTE_A4,NOTE_B4,
NOTE_C5
    };
    
    // note durations: 4 = quarter note, 8 = eighth note, etc.:
    int noteDurations[] = {/*64个音*/
   
    4,4,4,4,2,4,4, 4,4,4,4, 4,4,4,4, 4,4,4,4, 4,4,4,4, 4,4,4,4, 4,4,4,4, 
    4,8,8,4,8,8,8,8,8,8,8,8,8,8,
    4,8,8,4,8,8,8,8,8,8,8,8,8,8,
    4,8,8,4,8,8,8,8,8,8,8,8,8,8,
    4,8,8,4,8,8,8,8,8,8,8,8,8,8,
    4,8,8,4,8,8,8,8,8,8,8,8,8,8,
    4,8,8,4,8,8,8,8,8,8,8,8,8,8,
    4,8,8,4,8,8,8,8,8,8,8,8,8,8,
    4,8,8,4,8,8,8,8,8,8,8,8,8,8,
    4,8,8,4,8,8,8,8,8,8,8,8,8,8,
    4,8,8,4,8,8,8,8,8,8,8,8,8,8,
    4,8,8,4,8,8,8,8,8,8,8,8,8,8,
    4,8,8,4,8,8,8,8,8,8,8,8,8,8,
    2

    };
    
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 200; thisNote++) {
      // to calculate the note duration, take one second divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(output, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop
    if (int(digitalRead(p_do1)) + int(digitalRead(p_re1)) + int(digitalRead(p_mi1))
        + int(digitalRead(p_fa1)) + int(digitalRead(p_so1)) + int(digitalRead(p_la1))
        + int(digitalRead(p_xi1)) + int(digitalRead(p_do2)) ) {
      delay(pauseBetweenNotes);
      lcd.clear();
      break;
    }
      // stop the tone playing:
      noTone(output);
  }
    lcd.clear();
}

void demo_play3()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("demo play3");
    tone(2,880,750);//la
    noTone(1);
    delay(750);
    tone(2,784,250);//so
    noTone(1);
    delay(250);
    tone(2,659,500);//mi
    noTone(1);
    delay(500);
    tone(2,587,500);//re
    noTone(1);
    delay(500);
    tone(2,523,1000);//do
    noTone(1);
    delay(2000);
    tone(2,659,750);//mi
    noTone(1);
    delay(750);
    tone(2,587,250);//re
    noTone(1);
    delay(250);
    tone(2,523,500);//do
    noTone(1);
    delay(500);
    tone(2,440,500);//la*0.5
    noTone(1);
    delay(500);
    tone(2,392,1000);//so*0.5
    noTone(1);
    delay(2000);
    tone(2,392,750);//so*0.5
    noTone(1);
    delay(750);
    tone(2,440,250);//la*0.5
    noTone(1);
    delay(250);
    tone(2,392,750);//so*0.5
    noTone(1);
    delay(750);
    tone(2,440,250);//la*0.5
    noTone(1);
    delay(250);
    tone(2,523,750);//do
    noTone(1);
    delay(750);
    tone(2,587,250);//re
    noTone(1);
    delay(250);
    tone(2,659,500);//mi
    noTone(1);
    delay(500);
    tone(2,784,500);//so
    noTone(1);
    delay(500);
    tone(2,880,750);//la
    noTone(1);
    delay(750);
    tone(2,784,250);//so
    noTone(1);
    delay(250);
    tone(2,659,250);//mi
    noTone(1);
    delay(250);
    tone(2,587,250);//re
    noTone(1);
    delay(250);
    tone(2,523,500);//do
    noTone(1);
    delay(500);
    tone(2,587,1000);//re
    noTone(1);
    delay(2000);
    tone(2,880,500);//6
    noTone(1);
    delay(500);
    tone(2,784,500);//5
    noTone(1);
    delay(500);
    tone(2,659,500);//3
    noTone(1);
    delay(500);
    tone(2,587,500);//2
    noTone(1);
    delay(500);
    tone(2,523,1000);//1
    noTone(1);
    delay(2000);
    tone(2,659,750);//3
    noTone(1);
    delay(750);
    tone(2,587,250);//2
    noTone(1);
    delay(250);
    tone(2,523,500);//1
    noTone(1);
    delay(500);
    tone(2,440,500);//6*0.5
    noTone(1);
    delay(500);
    tone(2,392,1000);//5*0.5
    noTone(1);
    delay(2000);
    tone(2,392,750);//5*0.5
    noTone(1);
    delay(750);
    tone(2,440,250);//6*0.5
    noTone(1);
    delay(250);
    tone(2,392,750);//5*0.5
    noTone(1);
    delay(750);
    tone(2,440,250);//6*0.5
    noTone(1);
    delay(250);
    tone(2,523,750);//1
    noTone(1);
    delay(750);
    tone(2,587,250);//2
    noTone(1);
    delay(250);
    tone(2,659,500);//3
    noTone(1);
    delay(500);
    tone(2,784,500);//5
    noTone(1);
    delay(500);
    tone(2,880,750);//6
    noTone(1);
    delay(750);
    tone(2,784,250);//5
    noTone(1);
    delay(250);
    tone(2,659,250);//3
    noTone(1);
    delay(250);
    tone(2,587,250);//2
    noTone(1);
    delay(250);
    tone(2,523,480);//1
    noTone(1);
    delay(500);
    tone(2,523,1500);//1
    noTone(1);
    delay(2000);   
    lcd.clear();
}

void demo_play4()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("demo play4");
    tone(2,784,1000);//5
    noTone(1);
    delay(1000);
    tone(2,880,500);//6
    noTone(1);
    delay(500);
    tone(2,784,1000);//5
    noTone(1);
    delay(1000);
    tone(2,659,500);//3
    noTone(1);
    delay(500);
    tone(2,784,500);//5
    noTone(1);
    delay(500);
    tone(2,659,250);//3
    noTone(1);
    delay(250);
    tone(2,587,250);//2
    noTone(1);
    delay(250);
    tone(2,523,500);//1
    noTone(1);
    delay(500);
    tone(2,392,1000);//5*0.5
    noTone(1);
    delay(1500);
    tone(2,440,1000);//6*0.5
    noTone(1);
    delay(1000);
    tone(2,523,500);//1
    noTone(1);
    delay(500);
    tone(2,587,1000);//2
    noTone(1);
    delay(1000);
    tone(2,784,500);//5
    noTone(1);
    delay(500);
    tone(2,659,2000);//3
    noTone(1);
    delay(3000);
    tone(2,784,1000);//5
    noTone(1);
    delay(1000);
    tone(2,880,500);//6
    noTone(1);
    delay(500);
    tone(2,784,1000);//5
    noTone(1);
    delay(1000);
    tone(2,659,500);//3
    noTone(1);
    delay(500);
    tone(2,784,500);//5
    noTone(1);
    delay(500);
    tone(2,659,250);//3
    noTone(1);
    delay(250);
    tone(2,587,250);//2
    noTone(1);
    delay(250);
    tone(2,523,500);//1
    noTone(1);
    delay(500);
    tone(2,392,1000);//5*0.5
    noTone(1);
    delay(1500);
    tone(2,440,1000);//6*0.5
    noTone(1);
    delay(1000);
    tone(2,523,500);//1
    noTone(1);
    delay(500);
    tone(2,392,1000);//5*0.5
    noTone(1);
    delay(1000);
    tone(2,587,500);//2
    noTone(1);
    delay(500);
    tone(2,523,2000);//1
    noTone(1);
    delay(3000);
    lcd.clear();
}

void demo_play5()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("demo play5");
    tone(2,659,400);//3
    noTone(1);
    delay(400);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,523,400);//1
    noTone(1);
    delay(400);
    tone(2,523,200);//1
    noTone(1);
    delay(200);
    tone(2,440,200);//6*0.5
    noTone(1);
    delay(200);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,392,800);//5*0.5
    noTone(1);
    delay(800);
    tone(2,440,400);//6*0.5
    noTone(1);
    delay(400);
    tone(2,440,200);//6*0.5
    noTone(1);
    delay(200);
    tone(2,392,200);//5*0.5
    noTone(1);
    delay(200);
    tone(2,440,400);//6*0.5
    noTone(1);
    delay(400);
    tone(2,523,400);//1
    noTone(1);
    delay(400);
    tone(2,784,200);//5
    noTone(1);
    delay(200);
    tone(2,880,200);//6
    noTone(1);
    delay(200);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,784,200);//5
    noTone(1);
    delay(200);
    tone(2,587,800);//2
    noTone(1);
    delay(800);

    tone(2,659,400);//3
    noTone(1);
    delay(400);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,659,400);//3
    noTone(1);
    delay(400);
    tone(2,784,400);//5
    noTone(1);
    delay(400);
    tone(2,880,200);//6
    noTone(1);
    delay(200);
    tone(2,880,200);//6
    noTone(1);
    delay(200);
    tone(2,880,200);//6
    noTone(1);
    delay(200);
    tone(2,1046,200);//1*2
    noTone(1);
    delay(200);
    tone(2,880,400);//6
    noTone(1);
    delay(400);
    tone(2,794,200);//5
    noTone(1);
    delay(200);
    tone(2,659,200);//3
    noTone(1);
    delay(200);

    tone(2,587,400);//2
    noTone(1);
    delay(400);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,784,400);//5
    noTone(1);
    delay(400);
    tone(2,392,400);//5*0.5
    noTone(1);
    delay(400);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,523,800);//1
    noTone(1);
    delay(800);

    tone(2,659,400);//3
    noTone(1);
    delay(400);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,659,400);//3
    noTone(1);
    delay(400);
    tone(2,784,400);//5
    noTone(1);
    delay(400);
    tone(2,880,200);//6
    noTone(1);
    delay(200);
    tone(2,1046,200);//1*2
    noTone(1);
    delay(200);
    tone(2,880,200);//6
    noTone(1);
    delay(200);
    tone(2,784,200);//5
    noTone(1);
    delay(200);
    tone(2,880,400);//6
    noTone(1);
    delay(400);
    tone(2,794,200);//5
    noTone(1);
    delay(200);
    tone(2,659,200);//3
    noTone(1);
    delay(200);

    tone(2,587,400);//2
    noTone(1);
    delay(400);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,784,400);//5
    noTone(1);
    delay(400);
    tone(2,392,400);//5*0.5
    noTone(1);
    delay(400);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,523,800);//1
    noTone(1);
    delay(800);

    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,587,200);//2
    noTone(1);
    delay(200);
    tone(2,659,200);//3
    noTone(1);
    delay(200);
    tone(2,784,400);//5
    noTone(1);
    delay(400);
    tone(2,784,200);//5
    noTone(1);
    delay(200);
    tone(2,880,200);//6
    noTone(1);
    delay(200);
    tone(2,1046,400);//1*2
    noTone(1);
    delay(400);
    tone(2,880,400);//6
    noTone(1);
    delay(400);
    tone(2,1046,800);//1*2
    noTone(1);
    delay(800);
    lcd.clear();
}

void demo_play6()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("demo play6");
    tone(2,392,300);//5*0.5
    noTone(1);
    delay(300);
    tone(2,523,300);//1
    noTone(1);
    delay(300);
    tone(2,523,300);//1
    noTone(1);
    delay(300);

    tone(2,659,300);//3
    noTone(1);
    delay(300);
    tone(2,880,300);//6
    noTone(1);
    delay(300);
    tone(2,659,300);//3
    noTone(1);
    delay(300);
    tone(2,784,600);//5
    noTone(1);
    delay(600);

    tone(2,784,300);//5
    noTone(1);
    delay(300);
    tone(2,880,300);//6
    noTone(1);
    delay(300);
    tone(2,784,300);//5
    noTone(1);
    delay(300);
    tone(2,659,300);//3
    noTone(1);
    delay(300);
    tone(2,698,300);//4
    noTone(1);
    delay(300);
    tone(2,659,300);//3
    noTone(1);
    delay(300);
    tone(2,587,600);//2
    noTone(1);
    delay(600);

    tone(2,440,300);//6*0.5
    noTone(1);
    delay(300);
    tone(2,587,300);//2
    noTone(1);
    delay(300);
    tone(2,587,300);//2
    noTone(1);
    delay(300);
    tone(2,698,300);//4
    noTone(1);
    delay(300);
    tone(2,988,300);//7
    noTone(1);
    delay(300);
    tone(2,988,300);//7
    noTone(1);
    delay(300);
    tone(2,880,300);//6
    noTone(1);
    delay(300);
    tone(2,784,300);//5
    noTone(1);
    delay(300);

    tone(2,698,300);//4
    noTone(1);
    delay(300);
    tone(2,698,300);//4
    noTone(1);
    delay(300);
    tone(2,698,300);//4
    noTone(1);
    delay(300);
    tone(2,659,300);//3
    noTone(1);
    delay(300);
    tone(2,440,300);//6*0.5
    noTone(1);
    delay(300);
    tone(2,494,300);//7*0.5
    noTone(1);
    delay(300);
    tone(2,494,300);//7*0.5
    noTone(1);
    delay(300);
    tone(2,523,300);//1
    noTone(1);
    delay(300);
    tone(2,587,600);//2
    noTone(1);
    delay(2400);

    tone(2,392,300);//5*0.5
    noTone(1);
    delay(300);
    tone(2,523,300);//1
    noTone(1);
    delay(300);
    tone(2,523,300);//1
    noTone(1);
    delay(300);
    tone(2,659,300);//3
    noTone(1);
    delay(300);
    tone(2,880,300);//6
    noTone(1);
    delay(300);
    tone(2,659,300);//3
    noTone(1);
    delay(300);
    tone(2,784,600);//5
    noTone(1);
    delay(600);

    tone(2,784,300);//5
    noTone(1);
    delay(300);
    tone(2,880,300);//6
    noTone(1);
    delay(300);
    tone(2,784,300);//5
    noTone(1);
    delay(300);
    tone(2,659,300);//3
    noTone(1);
    delay(300);
    tone(2,698,300);//4
    noTone(1);
    delay(300);
    tone(2,659,300);//3
    noTone(1);
    delay(300);
    tone(2,587,600);//2
    noTone(1);
    delay(600);

    tone(2,440,300);//6*0.5
    noTone(1);
    delay(300);
    tone(2,587,300);//2
    noTone(1);
    delay(300);
    tone(2,587,300);//2
    noTone(1);
    delay(300);
    tone(2,698,300);//4
    noTone(1);
    delay(300);
    tone(2,988,300);//7
    noTone(1);
    delay(300);
    tone(2,988,300);//7
    noTone(1);
    delay(300);
    tone(2,880,300);//6
    noTone(1);
    delay(300);
    tone(2,784,300);//5
    noTone(1);
    delay(300);

    tone(2,698,600);//4
    noTone(1);
    delay(600);
    tone(2,698,300);//4
    noTone(1);
    delay(300);
    tone(2,659,300);//3
    noTone(1);
    delay(300);
    tone(2,494,600);//7*0.5
    noTone(1);
    delay(600);
    tone(2,587,600);//2
    noTone(1);
    delay(600);
    tone(2,523,600);//1
    noTone(1);
    delay(2400);

    tone(2,880,600);//6
    noTone(1);
    delay(600);
    tone(2,880,300);//6
    noTone(1);
    delay(300);
    tone(2,784,300);//5
    noTone(1);
    delay(300);
    tone(2,698,200);//4
    noTone(1);
    delay(200);
    tone(2,784,200);//5
    noTone(1);
    delay(200);
    tone(2,880,200);//6
    noTone(1);
    delay(200);
    tone(2,784,600);//5
    noTone(1);
    delay(600);
    tone(2,587,300);//2
    noTone(1);
    delay(300);
    tone(2,659,300);//3
    noTone(1);
    delay(300);
    tone(2,698,300);//4
    noTone(1);
    delay(300);
    tone(2,587,300);//2
    noTone(1);
    delay(300);
    tone(2,784,600);//5
    noTone(1);
    delay(1200);

    tone(2,880,600);//6
    noTone(1);
    delay(600);
    tone(2,784,600);//5
    noTone(1);
    delay(600);
    tone(2,698,600);//4
    noTone(1);
    delay(1200);
    tone(2,587,600);//2
    noTone(1);
    delay(600);
    tone(2,988,300);//7
    noTone(1);
    delay(300);
    tone(2,880,300);//6
    noTone(1);
    delay(300);
    tone(2,784,300);//5
    noTone(1);
    delay(300);
    tone(2,880,300);//6
    noTone(1);
    delay(300);
    tone(2,784,300);//5
    noTone(1);
    delay(300);
    tone(2,698,900);//4
    noTone(1);
    delay(900);
    tone(2,784,300);//5
    noTone(1);
    delay(300);
    tone(2,880,300);//6
    noTone(1);
    delay(300);
    tone(2,659,900);//3
    noTone(1);
    delay(900);
    tone(2,587,300);//2
    noTone(1);
    delay(300);
    tone(2,523,600);//1
    noTone(1);
    delay(2400);
    lcd.clear();
}
