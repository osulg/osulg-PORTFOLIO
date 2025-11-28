#include <Wire.h>              // I2C 통신을 위한 라이브러리
#include <RTClib.h>            // RTC 기능을 구현하기 위한 라이브러리
#include <IRremote.h>          // 적외선 리모컨 수신을 위한 라이브러리
#include <LiquidCrystal_I2C.h> // I2C LCD를 위한 라이브러리

// 적외선 리모컨 버튼 값
// ############## HEX ############## //
// BUTTON 0 : 0xE916FF00
// BUTTON 1 : 0xF30CFF00
// BUTTON 2 : 0xE718FF00
// BUTTON 3 : 0xA15EFF00
// BUTTON 4 : 0xF708FF00
// BUTTON 5 : 0xE31CFF00
// BUTTON 6 : 0xA55AFF00
// BUTTON 7 : 0xBD42FF00
// BUTTON 8 : 0xAD52FF00
// BUTTON 9 : 0xB54AFF00
// BUTTON EQ : 0xF609FF00
// ################################## //

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD : 주소 (0x27), 크기 (16x2)
RTC_DS1307 rtc;                     // RTC 모듈 객체 생성

int RECV_PIN = A0; // 적외선 리모컨 수신 핀을 A0로 설정

bool displayOff = false;  // 디스플레이 ON/OFF flag
bool alarmActive = false; // 알람 상태 flag

int buzzer = 15;                      // 부저 핀 15번(A1)으로 설정
int alarmHour = -1, alarmMinute = -1; // 알람 시/분

void setup()
{
  Serial.begin(9600);                              // 시리얼 통신을 9600 baud rate로 설정
  Wire.begin();                                    // I2C 통신 초기화
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK); // 적외선 리모컨 초기화

  // RTC 모듈 초기화 확인
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC"); // RTC 모듈이 없다면 멈춤
    while (1)
      ;
  }

  // RTC 모듈이 작동 중이 아니라면 초기 시간을 설정
  if (!rtc.isrunning())
  {
    rtc.adjust(DateTime(__DATE__, __TIME__)); // 컴파일 시간 기준으로 설정
  }

  pinMode(buzzer, OUTPUT); // 부저를 OUTPUT으로 설정

  lcd.init();      // LCD 초기화
  lcd.backlight(); // LCD 백라이트 켜기

  lcd.setCursor(0, 0);          // 커서 위치를 (0,0)으로 설정
  lcd.print("Initializing..."); // 초기화 메세지 설정
  delay(2000);
  lcd.clear(); // 초기화 후 화면 지우기
}

void loop()
{
  // 시계 모드
  if (!alarmActive)
  {
    clockState(); // 시간 표시
  }
  // 알람 모드
  else
  {
    alarmState(); // 알람 시계
  }

  AlarmTimeInput(); // 사용자 입력을 통해 알람 시간 설정
  delay(1000);      // 1초 간격으로 실행
}

void clockState()
{
  DateTime now = rtc.now(); // 현재 시간 가져오기

  // 시/분/초 저장
  int h = now.hour();
  int m = now.minute();
  int s = now.second();

  // 시계 모드 (시:분:초) 출력
  Serial.print("Clock State - Time: ");
  Serial.print(formatTime(h));
  Serial.print(":");
  Serial.print(formatTime(m));
  Serial.print(":");
  Serial.println(formatTime(s));

  // LCD에 (시:분:초) 출력
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.setCursor(0, 1);
  lcd.print(formatTime(h));
  lcd.print(":");
  lcd.print(formatTime(m));
  lcd.print(":");
  lcd.print(formatTime(s));

  // 현재 시간이 알람 시간과 같아지면
  if (h == alarmHour && m == alarmMinute)
  {
    alarmActive = true; // 알람 모드
  }
}

void alarmState()
{
  Serial.println("ALARM! Alarm State Active."); // 알람 시간임을 표시

  lcd.setCursor(0, 0); // 커서 위치 (0,0)에 알람 설정 시간 알리기
  lcd.print("ALARM! Time!");

  digitalWrite(buzzer, HIGH); // 부저 울리기

  // 적외선 리모컨 신호 감지
  if (IrReceiver.decode())
  {
    // 0번 버튼이 눌리면 (+반복 신호 방지)
    if (IrReceiver.decodedIRData.decodedRawData != 0xFFFFFFFF && IrReceiver.decodedIRData.decodedRawData == 0xE916FF00)
    {
      Serial.println("Alarm Stopped!"); // 알람 종료 표시

      lcd.setCursor(0, 1); // 커서 위치 (0,1)에 알람 종료 표시
      lcd.print("Alarm Stopped!  ");
      delay(2000);
      alarmActive = false; // 알람 모드 비활성화

      lcd.clear();               // LCD 지우기
      digitalWrite(buzzer, LOW); // 부저 끄기
      lcd.noBacklight();         // LCD 백라이트 끄기
      while (1)
        ; // 종료 및 무한 루프
    }
    IrReceiver.resume(); // 리모커 신호 대기
  }
}

void AlarmTimeInput()
{
  // 알람 시간 시리얼 입력 받기
  if (Serial.available())
  {                                              // 시리얼 포트에 데이터가 존재한다면
    String input = Serial.readStringUntil('\n'); // '\n'(줄바꿈)까지 문자열 읽기

    // HH:MM 형식 및 5자인지 확인
    if (input.length() == 5 && input.charAt(2) == ':')
    {
      int h = input.substring(0, 2).toInt(); // 시(hout)를 정수로 변환
      int m = input.substring(3).toInt();    // 분(minute)을 정수로 변환

      // 유효한 시간인지 확인
      // 시 (23시간 형식) : 0~23
      // 분 0~59
      if (h >= 0 && h < 24 && m >= 0 && m < 60)
      {
        // 알람 시/분(alarmHour/alarmMinute)에 설정 시간 저장
        alarmHour = h;
        alarmMinute = m;

        // 시리얼 모니터에 설정된 알람 시간 출력
        Serial.print("Alarm Time Set: ");
        Serial.print(alarmHour);
        Serial.print(":");
        Serial.println(alarmMinute);

        // LCD에 설정된 알람 시간 출력
        lcd.setCursor(0, 1);
        lcd.print("Alarm Set: ");
        lcd.print(formatTime(alarmHour)); // 0X:0X의 형태로 츌력
        lcd.print(":");
        lcd.print(formatTime(alarmMinute)); // 0X:0X의 형태로 츌력
      }
      // 형식에 맞지 않으면 오류 메세지 출력
      else
      {
        Serial.println("Invalid Time Format!");

        // LCD에 형식에 맞지 않음을 표시
        lcd.setCursor(0, 1);
        lcd.print("Invalid Time!");
      }
    }

    // 입력 문자열이 형식에 맞지 않은 경우 오류 메세지 출력
    else
    {
      // 시리얼 모니터에 형식에 맞지 않음을 표시
      Serial.println("Invalid Time Input!");
      Serial.flush();

      // LCD에 형식에 맞지 않음을 표시
      lcd.setCursor(0, 1);
      lcd.print("Invalid Input!");
    }

    delay(2000);
    lcd.clear();
  }
}

String formatTime(int value)
{
  // 한 자리 수 앞에
  if (value < 10)
  {
    return "0" + String(value); // 0 붙이기
  }
  return String(value); // 두 자리 수는 그대로 반환
}
