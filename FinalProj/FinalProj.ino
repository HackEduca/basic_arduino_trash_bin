#include <SoftwareSerial.h>
#include<Servo.h>
Servo servo;

// 사용자 설정을 위한 변수 설정
int sensitivity = 50;
int ledmode = 1;

// 블루투스 모듈 연결 핀 설정
int const blueTx = 12;
int const blueRx = 13;

// 사용자의 접근을 감지하는 초음파 센서 핀 설정
int const trigPin = 6;
int const echoPin = 5;

// 쓰레기통 내의 잔량을 측정하는 초음파 센서 핀 설정
int const trigPin2 = 11;
int const echoPin2 = 10;

// 잔량을 표시해줄 LED pin 설정
int const lowLED = 7;
int const middleLED = 8;
int const highLED = 9;

SoftwareSerial mySerial(blueTx, blueRx);

void setup()
{
  pinMode(trigPin, OUTPUT); // trig핀을 출력모드로 설정
  pinMode(echoPin, INPUT);  // echo핀을 입력모드로 설정
  pinMode(trigPin2, OUTPUT); // trig핀을 출력모드로 설정
  pinMode(echoPin2, INPUT);  // echo핀을 입력모드로 설정
  pinMode(lowLED, OUTPUT);
  pinMode(middleLED, OUTPUT);  
  pinMode(highLED, OUTPUT);

  servo.attach(3); // 모터 핀 지정
  servo.write(0);
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop()
{
  int duration, distance;
  digitalWrite(trigPin, HIGH); // trig 핀 High
  delayMicroseconds(10); // 1ms동안 유지
  digitalWrite(trigPin, LOW); // trig 핀 Low

  // Echo 핀으로 들어오는 시간 측정
  duration = pulseIn(echoPin, HIGH); // pulsein 함수가 호출되고 펄스가 입력될 때까지의 시간
  distance = (duration/2) / 29.1; // 거리를 cm 단위로 환산
  
  if (mySerial.available() == 1) 
    {
      char c = mySerial.read();
    
      if (c == 'i') {
        sendSettings();
        while (!mySerial.available()) {
          if (mySerial.available())
            break;
        }
      
        if (mySerial.available())
        { 
          Serial.println("data input");
          // 한 byte 읽기
        
          char c = mySerial.read();
          
          if (c == 'l') {
            Serial.print("ledmode: ");
            ledmode = mySerial.parseInt(); 
            Serial.print(ledmode);
          } 
          c = mySerial.read();
          if (c == 'd') {
            Serial.print("distance: ");
            sensitivity = mySerial.parseInt();
            Serial.println(sensitivity);
          }
        }
        Serial.println("data end");
      }
      
      if (c == 'O') {
        servo.write(50); //opened
        while (!mySerial.available()) {
          if (mySerial.available())
            break;
        }
        c = mySerial.read();
        if (c == 'C')
          servo.write(160);
      }

      while (mySerial.available()) {
        Serial.print('f');
        mySerial.read();
      }
    }
  
    distance = constrain(distance, 0, 100);
    if (distance <= sensitivity && distance > 0) // 50cm 이하일 경우 servo 모터 동작
    { 
      servo.write(50); //opened
    } 
    else 
    {
      servo.write(160); // closed
      delay(500);
      showStatus();
    }
  delay(60);
}

void showStatus()
{
  int duration, distance;
  int value = 0;
  digitalWrite(trigPin2, LOW); // trig 핀 Low
  delayMicroseconds(2); // 1ms동안 유지
  digitalWrite(trigPin2, HIGH); // trig 핀 High
  delayMicroseconds(10); // 1ms동안 유지
  digitalWrite(trigPin2, LOW); // trig 핀 Low

  // Echo 핀으로 들어오는 시간 측정
  duration = pulseIn(echoPin2, HIGH); // pulsein 함수가 호출되고 펄스가 입력될 때까지의 시간
  distance = (duration/2) / 29.1; // 거리를 cm 단위로 환산
  //distance = constrain(distance, 0, 100);
  //Serial.println(distance);
  distance = map(distance, 30, 0, 0, 100); // 쓰레기통의 잔량 (센서까지의 길이)을 측정해 0~100 안의 값으로 맵핑
  mySerial.print(constrain(distance, 0, 100));
  delay(100);
  
  if (distance <= 50 && ledmode == 1) // 잔량을 표시
  {
    digitalWrite(lowLED, HIGH);
    delay(500);
    digitalWrite(lowLED, LOW);
    //Serial.println("high");
  }
  else if (distance <= 80 && distance > 50 && ledmode == 1) 
  {
    digitalWrite(middleLED, HIGH); 
    delay(500);
    digitalWrite(middleLED, LOW);
    //Serial.println("middle");
  }
  else if (ledmode == 1)
  {
    digitalWrite(highLED, HIGH); 
    delay(500);
    digitalWrite(highLED, LOW);
    //Serial.println("high");
  }
}

void sendSettings() {
  Serial.print('0');
  mySerial.print('0');
  Serial.print('|');
  mySerial.print('|');
  Serial.print(ledmode);
  mySerial.print(ledmode);
  Serial.print('|');
  mySerial.print('|');
  Serial.print(sensitivity);
  mySerial.print(sensitivity);
  Serial.println('|');
  mySerial.println('|');
}
