 
/*************************************************************
2015.07.25 13:57
针对新的协议，修改了程序
2015.07.26 17:29
添加了亮灯之前解除绑定舵机
2015.07.29 12:32
修改了结束符为0X0D 0X0A
2015.08.02 14:22
修改了舵机的引脚编号
2015.08.02 19:36
修改了A指令舵机的速度

*************************************************************/



//#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <Servo.h>
#define PIN 17

Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, PIN, NEO_GRB + NEO_KHZ800);


//舵机控制
Servo Servos[9];
int MIN_DELAY = 5;
int SERVO_NUM = 9;
int Pins[10] = {4, 5, 6, 7, 8, 9, 13, 12, 11, 10};
int Positions[10] = {90, 90, 90, 90, 90, 90, 90, 90, 90, 90};
int GoalPositions[10] = {90, 90, 90, 90, 90, 90, 90, 90, 90, 90 };
int Speeds[10] = {5, 5, 5, 5, 5, 5, 5, 5 , 5, 5};
int Directions[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int position_temp[10]={0};
int speeds_temp[10]={0};
int positions_calculator[10]={0};
int servo_count=0;
int cycle_account=0;


void setup()  
{
  //打开串行通信，等待端口打开：
  Serial.begin(9600);
  Serial.println("Hello MAIR!");

  //Servo init
  for(int i;i<SERVO_NUM;i++)
  {
     Servos[i].attach(Pins[i]);
     Servos[i].write(Positions[i]);
  }

  //RGB light init
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  //Touch module
  attachInterrupt(0, touch, FALLING);
  pinMode(2,INPUT);
  digitalWrite(2,HIGH);
}

int number=0;
int angle=0;
int speeds=0;
int D_number=0;
int end_table=0;
int a_account=0;
String inString = "";    // string to hold input
String comdata = "";
int currentColor = 0;
int red, green, blue = 0;
unsigned char usart_table=0;
unsigned char usart_val=0; 
void loop() // 循环
{ 
  int inChar;
  if (Serial.available())
  { 
    usart_val=Serial.read();
    comdata += char(usart_val);
    //end of command
    if(usart_val == 0x0d) end_table=1;  
    
    if (usart_val == 0x41 && usart_table == 0) 
    {
      usart_table=1;
      usart_val=0;
    }
    //C type command
    if (usart_val == 0x42 && usart_table == 0) 
    {
      usart_table=2;
      usart_val=0;
    }    
    if (usart_val == 0x43 && usart_table == 0) 
    {
      usart_table=3;
      usart_val=0;
    }     
    if (usart_val == 0x44 && usart_table == 0) 
    {
      usart_table=4;
      usart_val=0;
    }             
    //A type command
    if (usart_table==1 && usart_val)
    {
      inChar=usart_val;     
      if (isDigit(inChar)) 
      {
        inString += (char)inChar;
      }
      if (inChar == ',') 
      {
        if(a_account == 0)
          number = inString.toInt();
        if(a_account == 1)
          angle = inString.toInt();
        inString = "";
        a_account++;
      }
      if (inChar == 0x0a && end_table ==1) 
      {
        speeds = inString.toInt();
        inString = "";   
        
        position_temp[number]=angle;
        speeds_temp[number]=speeds;
        go(position_temp,speeds_temp);
        usart_table=0;
        usart_val=0;   
        Serial.println("ACK");
        comdata = ""; 
        end_table=0;      
        a_account=0; 
      } 
    }
    //B type command
    if (usart_table==2 && usart_val)
    { 
      inChar=usart_val;     
      if (isDigit(inChar)) 
      {
         inString += (char)inChar;
      }
      if (inChar == ',') 
      {
        switch (currentColor) 
        {
          case 0:    // 0 = red
          red = inString.toInt();
          inString = "";
          break;
          case 1:    // 1 = green:
          green = inString.toInt();
          // clear the string for new input:
          inString = "";
          break;
        }
        currentColor++;
      }
      if (inChar == 0x0a  && end_table ==1) 
      {
        //2015/07/26  cjw disattach servo pin for problem of interfacing.
        for(int i;i<SERVO_NUM;i++)
        {
          Servos[i].detach();
        }   
        
        blue = inString.toInt();
        colorWipe(strip.Color(green,red,blue), 50);
        // clear the string for new input:
        inString = "";
        // reset the color counter:
        currentColor = 0;  
        usart_table=0;
        usart_val=0;   
        Serial.println("ACK");
        comdata = "";
        end_table ==0;
        //2015/07/26  cjw reattach servo pin
        for(int i;i<SERVO_NUM;i++)
        {
          Servos[i].attach(Pins[i]);
        }   
      }  
    }
    //C type command
    if (usart_table==3 && usart_val)
    {
      inChar=usart_val;     
      if (isDigit(inChar)) 
      {
         inString += (char)inChar;
      }
      if (inChar == ',') 
      {
        if(servo_count % 2 == 0)
          position_temp[servo_count/2]=inString.toInt();
        else
          speeds_temp[servo_count/2]=inString.toInt();
        inString = "";
        servo_count++;
      }
      if (inChar == 0x0a  && end_table ==1) 
      {
        speeds_temp[servo_count/2]=inString.toInt();
        inString = "";
        servo_count=0;
        go(position_temp,speeds_temp);
        usart_table=0;
        usart_val=0;   
        Serial.println("ACK");
        comdata = "";        
        end_table == 0;   
      }
    }
    //D type command
    if (usart_table==4 && usart_val)
    {
      inChar=usart_val;     
      if (isDigit(inChar)) 
      {
         inString += (char)inChar;
      }
      if (inChar == ',') 
      {
        if(servo_count % 3 == 0)
          D_number = inString.toInt();
        else if(servo_count % 3 == 1)
          position_temp[D_number]=inString.toInt();
        else
          speeds_temp[D_number]=inString.toInt();
        inString = "";
        servo_count++;
      }
      if (inChar == 0x0a  && end_table ==1) 
      {
        speeds_temp[D_number]=inString.toInt();
        inString = "";
        servo_count=0;
        D_number=0;
        go(position_temp,speeds_temp);
        usart_table=0;
        usart_val=0;   
        Serial.println("ACK");
        comdata = "";        
        end_table == 0;   
      }
    }    
  }
}


void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
    strip.setPixelColor(i, c);
    strip.setBrightness(35);
    strip.show();
    delay(wait);
  }
}
void touch()
{
  if(digitalRead(2) == LOW)
  {
    delay(100);
    if(digitalRead(2) == LOW)
    {
    Serial.println('T');
    }
  }
}

void go(int* positions,int* speeds)
{
  
  int i;
  int j;
  for(j=0;j<SERVO_NUM;j++)
  {
    GoalPositions[j] = positions[j];
    Speeds[j] = speeds[j];
  }

  //calc the biggest cycle account
  for(j=0;j<SERVO_NUM;j++)
  {
    positions_calculator[j]=Speeds[j]*abs(GoalPositions[j]-Positions[j]);
  }
  cycle_account=positions_calculator[0];
  
  for(j=1;j<SERVO_NUM;j++)
  {
    if(cycle_account < positions_calculator[j])
    {
      cycle_account=positions_calculator[j];
    }
  }

  //get the servo's turn direction
  for(j=0;j<SERVO_NUM;j++)
  {
    if(GoalPositions[j]>Positions[j]) Directions[j]=1;
    else if(GoalPositions[j]<Positions[j]) Directions[j]=-1;
    else Directions[j]=0;
  }
  
  for(i=0;i<cycle_account;i++)
  {
    delay(MIN_DELAY);
    for(j=0;j<SERVO_NUM;j++)
    {
      if(Positions[j]!=GoalPositions[j])
      {
        if(i%Speeds[j]==0)
        {
          Positions[j]+=Directions[j];
          Servos[j].write(Positions[j]);
        }
      }
    }
  }

}

//灯的变化
// Fill the dots one after the other with a color
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
