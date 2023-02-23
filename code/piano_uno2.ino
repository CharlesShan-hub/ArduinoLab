/*
   IRrecvDemo
   红外控制，接收红外命令控制板载LED灯亮灭
*/
#include<Wire.h>
#include <IRremote.h>
#include"pitches.h"

// 定义每个音的引脚
#define p_do1 6
#define p_re1 7
#define p_mi1 8
#define p_fa1 9
#define p_so1 10
#define p_la1 11
#define p_xi1 12
#define p_do2 13
#define output 2

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

 // 用于限定播放第二个音
int last = 0; // 记录上一个， 0为普通模式
int ifread = 0;

// 用于检测火焰
int fire=A4; //定义A4为模拟量读取引脚
int val=0; //定义一个变量

// 用于判断开关机
#define Open_button 3
int Open = 0;

// 用于判断是否进行颤音
int iftre = 0;
int ifmolo=0;

int RECV_PIN = 5;
int MusicMode = 1;
int randomSong;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  // 显示器
  Serial.begin(19200);
  irrecv.enableIRIn(); // Start the receiver

  // 设置引脚
  pinMode(output,OUTPUT);//2
  pinMode(p_do1, INPUT); //A1
  pinMode(p_re1, INPUT); //A2
  pinMode(p_mi1, INPUT); //A3
  pinMode(p_fa1, INPUT); //A4
  pinMode(p_so1, INPUT); //A5
  pinMode(p_la1, INPUT); //A6
  pinMode(p_xi1, INPUT); //A7
  pinMode(p_do2, INPUT); //A8
  pinMode(fire, INPUT);  //A9
  Serial.begin(9600); // 测试用
}

void loop() {
  // 开关机代码
  if (digitalRead(Open_button))//按一下开机，再按一下关机
  {
    delay(300);
    // 这里写个函数播放开机音乐
    if (Open){ Open = 0;}
    else{ Open = 1;}
  }
  
  if(Open)
  {
    // 音乐
    Play();
    RemoteControl();
  
    //检测火焰
    val = analogRead(fire);
    if(val >= 450) happy_birthday2(); // 生日歌（报警）
  }
}

void RemoteControl()
{
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    switch (results.value)
    {
      case 0xFF22DD://<<倒带
        Change_Mode(0);
        break;
        
      case 0xFF02FD://>>快进
        Change_Mode(1);
        break;

      case 0xFFA25D: //ch-
        if (tune > 1)
          tune = tune - 1;
        break;

      case 0xFFE21D: //ch+
        if (tune < 5)
          tune = tune + 1;
        break;

      case 0xFF629D: //CH
        tune = 3;
        break;

      case 0xFF6897://0
        break;

      case 0xFF30CF://1

        break;

      case 0xFF18E7://2

        break;

      case 0xFF7A85://3

      default:
        break;

    }
    irrecv.resume(); // Receive the next value
  }
  delay(100);
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
    do1 = NOTE_C5;
    re1 = NOTE_D5;
    mi1 = NOTE_E5;
    fa1 = NOTE_F5;
    so1 = NOTE_G5;
    la1 = NOTE_A5;
    xi1 = NOTE_B5;
    do2 = NOTE_C6;
  }
  if (tune == 2)
  {
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
    do1 = NOTE_C7;
    re1 = NOTE_D7;
    mi1 = NOTE_E7;
    fa1 = NOTE_F7;
    so1 = NOTE_G7;
    la1 = NOTE_A7;
    xi1 = NOTE_B7;
    do2 = NOTE_C8;
  }

  // 2 重置 - 没有按下任何键 或 按下了一个键

  ifread = int(digitalRead(p_do1)) + int(digitalRead(p_re1)) + int(digitalRead(p_mi1))
         + int(digitalRead(p_fa1)) + int(digitalRead(p_so1)) + int(digitalRead(p_la1))
         + int(digitalRead(p_xi1)) + int(digitalRead(p_do2));
  if (ifread == 0 || ifread == 1)
    last = 0;

  // 3 保存上一个音
  if(last==0){
    if(digitalRead(p_do1)==HIGH){last=p_do1;}
    else if(digitalRead(p_re1)==HIGH){last=p_re1;}
    else if(digitalRead(p_mi1)==HIGH){last=p_mi1;}
    else if(digitalRead(p_fa1)==HIGH){last=p_fa1;}
    else if(digitalRead(p_so1)==HIGH){last=p_so1;}
    else if(digitalRead(p_la1)==HIGH){last=p_la1;}
    else if(digitalRead(p_xi1)==HIGH){last=p_xi1;}
    else if(digitalRead(p_do2)==HIGH){last=p_do2;}
  }

  // 4 再进行声音发音 - 进行第二个音的播出
  if(digitalRead(p_do1) && (last!=p_do1))
  {
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
  }
  else if(digitalRead(p_re1) && (last!=p_re1))
  {
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
    else tone(output, re1, Dura);}
  else if(digitalRead(p_mi1) && (last!=p_mi1))
  {
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
  }
  else if(digitalRead(p_fa1) && (last!=p_fa1))
  {
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
  }
  else if(digitalRead(p_so1) && (last!=p_so1))
  {
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
  }
  else if(digitalRead(p_la1) && (last!=p_la1))
  {
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
  }
  else if(digitalRead(p_xi1) && (last!=p_xi1))
  {
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
  }
  else if(digitalRead(p_do2) && (last!=p_do2))
  {
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
  }
}

void happy_birthday2(){
    // 解决同步问题
    int melody[] = {
    NOTE_E4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_E4, NOTE_D4,
    NOTE_E4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_F4, NOTE_E4,
    NOTE_E4, NOTE_E4, NOTE_E4, NOTE_C4, NOTE_A3, NOTE_G3, NOTE_F3,
    NOTE_A4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_B3, NOTE_C4
    };
    
    // note durations: 4 = quarter note, 8 = eighth note, etc.:
    int noteDurations[] = {/*64个音*/
    4,4,2,2,2,1,
    4,4,2,2,2,1,
    4,4,2,2,2,2,1,
    4,4,2,2,2,1
    };
    
    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 25; thisNote++) {
      // 两个板子进行时间同步
      delay(3);
      // to calculate the note duration, take one second divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(output, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      if(int(digitalRead(p_do1)) + int(digitalRead(p_re1)) + int(digitalRead(p_mi1))
           + int(digitalRead(p_fa1)) + int(digitalRead(p_so1)) + int(digitalRead(p_la1))
           + int(digitalRead(p_xi1)) + int(digitalRead(p_do2))){delay(pauseBetweenNotes);break;}
      // stop the tone playing:
      noTone(output);
  }
}
