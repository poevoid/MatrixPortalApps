#pragma once
#include "vars.h"


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "north-america.pool.ntp.org", -18000, 3600);

Adafruit_Protomatter matrix(
  WIDTH, 4, 1, rgbPins, NUM_ADDR_PINS, addrPins,
  clockPin, latchPin, oePin, true);

Adafruit_LIS3DH accel = Adafruit_LIS3DH();

Adafruit_SGP40 mox;                      //metal oxide sensor
Adafruit_SHTC3 envi = Adafruit_SHTC3();  //environmental sensor (temp/humidity)


void resetGame() {
  player.score = 0;
  oppo.score = 0;
  player.y = HEIGHT / 2 - BOX_HEIGHT;
  ball.x = 45;
  ball.right = false;
}

void bip() {
  tone(buzzer, 988, 20);
}
void beep() {
  tone(buzzer, 880, 20);
}
void boop() {
  tone(buzzer, 784, 40);
}
void ding() {
  tone(buzzer, 1175, 60);
}
void err(int x) {
  uint8_t i;
  pinMode(LED_BUILTIN, OUTPUT);        // Using onboard LED
  for (i = 1;; i++) {                  // Loop forever...
    digitalWrite(LED_BUILTIN, i & 1);  // LED on/off blink to alert user
    delay(x);
  }
}
int concatenate(int i, int j) {
    int result = i * 100 + j; 
    return result;
}


void playPong() {
  uint32_t t;
  while (((t = micros()) - prevTime) < (1000000L / MAX_FPS))
    ;
  prevTime = t;
  matrix.fillScreen(0x0);
  switch (screen) {
    case CurrentScreen::Title:  //menu
      matrix.setCursor(15, 10);
      matrix.setTextSize(0);
      matrix.println("PONG!");
      resetGame();
      uint8_t tap;
      tap = accel.getClick();
      // if (tap == 0) return;
      //if (!(tap & 0x30)) return;
      //if (tap & 0x10) screen = CurrentScreen::Game;  //single tap
      if (tap & 0x20) screen = CurrentScreen::Game;  //double tap
      //if (!digitalRead(NEXT_BUTTON) || !digitalRead(BACK_BUTTON)) {
      // screen = CurrentScreen::Game;
      // }
      break;

    case CurrentScreen::Game:  //game


      //----player interface-----//
      sensors_event_t event;
      accel.getEvent(&event);
      player.y += event.acceleration.y;

      //-----player bounds-----------|
      if (player.y > HEIGHT - 8) {  //|
        player.y = HEIGHT - 8;      //|
      }                             //|
      if (player.y < 0) {           //|
        player.y = 0;               //|
      }                             //|
      //---------------------------//|

      //-------Ball Physics(hehehe)--------------------//|
      if (ball.right) {
        ball.x++;
      } else {
        ball.x--;
      }
      if (ball.down) {
        ball.y++;
      }
      if (ball.up) {
        ball.y--;
      }

      if (ball.x == player.x + BOX_WIDTH && player.y < ball.y + BALL_SIZE && player.y + BOX_HEIGHT > ball.y) {
        bip();
        if (player.y + BOX_HEIGHT / 2 == ball.y + BALL_SIZE / 2) {  //if player hits dead center
          ball.down = false;
          ball.up = false;
        }
        if (player.y + BOX_HEIGHT / 2 > ball.y + BALL_SIZE / 2) {
          ball.down = false;
          ball.up = true;
        }
        if (player.y + BOX_HEIGHT / 2 < ball.y + BALL_SIZE / 2) {
          ball.up = false;
          ball.down = true;
        }

        ball.right = true;

      }  //if player paddle touches the ball, change directions



      if (ball.x + BALL_SIZE == oppo.x && oppo.y < ball.y + BALL_SIZE && oppo.y + BOX_HEIGHT > ball.y) {
        beep();

        if (oppo.y + BOX_HEIGHT / 2 == ball.y + BALL_SIZE / 2) {
          ball.down = false;
          ball.up = false;
        }
        if (oppo.y + BOX_HEIGHT / 2 > ball.y + BALL_SIZE / 2) {
          ball.down = false;
          ball.up = true;
        }
        if (oppo.y + BOX_HEIGHT / 2 < ball.y + BALL_SIZE / 2) {
          ball.down = true;
          ball.up = false;
        }

        ball.right = false;



      }  //if opponent touches the ball, change directions




      if (ball.y == 0) {
        ball.down = true;
        ball.up = false;
        /*if (random(0,3)==1){
          ball.right=true;
        } else {ball.right=false;}*/
      }
      if (ball.y == HEIGHT - BALL_SIZE) {
        ball.down = false;
        ball.up = true;
        /*if (random(0,3)==1){
          ball.right=true;
        }
        else {ball.right=false;}*/
      }
      //-----------------------------------------------------------------------------------------------------------------|


      //-------------scoring---------------------------|
      if (ball.x + BALL_SIZE <= 0) {
        oppo.score++;
        ball.x = 29;
        boop();
      }
      if (ball.x >= 64) {
        player.score++;
        ball.x = 29;
        beep();
        delay(18);
        bip();
        delay(18);
        beep();
        delay(18);
        bip();
        delay(25);
        ding();
      }
      if (oppo.score == 11) {
        boop();
        delay(30);
        tone(buzzer, 698, 60);
        delay(30);
        tone(buzzer, 587, 60);
        delay(30);
        tone(buzzer, 554, 70);
        delay(30);
        screen = CurrentScreen::Gameover;
      }
      if (player.score == 11) {
        ding();
        delay(30);
        tone(buzzer, 880, 60);
        delay(30);
        ding();
        delay(30);
        tone(buzzer, 1397, 60);
        delay(30);
        tone(buzzer, 1568, 60);
        delay(30);
        screen = CurrentScreen::Win;
      }
      //-------------------------------------------------|

      //-----------Opponent "AI" --------------------|
      if (ball.x > 40 || random(0, 20) == 1) {
        if (ball.y < oppo.y || random(0, 20) == 8 && oppo.y > 1)
          oppo.y--;
        if (ball.y + BALL_SIZE > oppo.y + BOX_HEIGHT || random(0, 20) == 7 && oppo.y < HEIGHT - BOX_HEIGHT)
          oppo.y++;
      }
      //---------------------------------------------|

      //draw objects
      matrix.setCursor(10, 0);
      matrix.println(player.score);
      matrix.setCursor(49, 0);
      matrix.print(oppo.score);
      for (int i = 1; i < 32; i += 4) {
        matrix.drawFastVLine(WIDTH / 2, i, 2, WHITE);
      };
      matrix.fillRect(player.x, player.y, BOX_WIDTH, BOX_HEIGHT, 0XFFFF);
      matrix.fillRect(ball.x, ball.y, BALL_SIZE, BALL_SIZE, ball.color);
      matrix.fillRect(oppo.x, oppo.y, BOX_WIDTH, BOX_HEIGHT, WHITE);

      break;

    case CurrentScreen::Gameover:  //gameover
      matrix.setCursor(0, 0);
      matrix.print("HAHA! \nI WIN, \nLOSER!");
      //matrix.println("Tap to play again")

      tap = accel.getClick();
      // if (tap == 0) return;
      //if (!(tap & 0x30)) return;
      if (tap & 0x10) screen = CurrentScreen::Title;  //single tap
      if (tap & 0x20) screen = CurrentScreen::Title;  //double tap
      break;

    case CurrentScreen::Win:  //winner
      matrix.setCursor(0, 8);
      matrix.println("YOU FUCKIN DID IT,\n CHAMP!");


      tap = accel.getClick();
      // if (tap == 0) return;
      //if (!(tap & 0x30)) return;
      if (tap & 0x10) screen = CurrentScreen::Title;  //single tap
      if (tap & 0x20) screen = CurrentScreen::Title;  //double tap
      break;
  }
  matrix.show();
}

void monitorEnvironment() {
#ifndef SETUPSENSORS
  matrix.fillScreen(0x0);
  if (!envi.begin()) {
    matrix.println("Couldn't Find SHTC3");
  }
  if (!mox.begin()) {
    matrix.println("couldn't find SGP40 either");
     matrix.show();
    return ;
  }
  matrix.println("Found sensors!");
#define SETUPSENSORS
 
#endif

  matrix.fillScreen(0x0);
  uint16_t color;
  sensors_event_t humidity, temp;
  int32_t voc_index;
  envi.getEvent(&humidity, &temp);  // populate temp and humidity objects with fresh data
  voc_index = mox.measureVocIndex(temp.temperature, humidity.relative_humidity);
  int tread = temp.temperature * 1.8 + 32;
  int hread = humidity.relative_humidity;
  matrix.setCursor(0, 0);
  matrix.print(tread);
  matrix.println("F");
  matrix.print(hread);
  matrix.println("%rH");
  matrix.print("VOC: ");
  matrix.println(voc_index);
  matrix.show();
  delay(100);
}


void drawCistercian0() {
  matrix.drawFastHLine(zeroX, zeroY, zeroLength, AMBER);
}

void drawCistercian1() {
  matrix.drawFastVLine(zeroX, zeroY - legth, legth, AMBER);
}

void drawCistercian2() {
  matrix.drawFastVLine(zeroX + legth, zeroY - legth, legth, AMBER);
}
void drawCistercian3() {
  matrix.drawLine(zeroX + legth, zeroY - legth, zeroX, zeroY, AMBER);
}
void drawCistercian4() {
  matrix.drawLine(zeroX, zeroY - legth, zeroX + legth, zeroY, AMBER);
}
void drawCistercian5(){
  drawCistercian1();
  drawCistercian4();
}
void drawCistercian6() {
  matrix.drawFastHLine(zeroX, zeroY - legth, legth, AMBER);
}
void drawCistercian7(){
  drawCistercian1();
  drawCistercian6();
}
void drawCistercian8(){
  drawCistercian2();
  drawCistercian6();
}
void drawCistercian9(){
  drawCistercian1();
  drawCistercian2();
  drawCistercian6();
}
void drawCistercian10() {
  matrix.drawFastVLine(zeroX, zeroY, legth, AMBER);
}
void drawCistercian20() {
  matrix.drawFastVLine(zeroX + legth, zeroY, legth + 1, AMBER);
}
void drawCistercian30() {
  matrix.drawLine(zeroX, zeroY, zeroX + legth, zeroY + legth, AMBER);
}
void drawCistercian40() {
  matrix.drawLine(zeroX, zeroY + legth, zeroX + legth, zeroY, AMBER);
}
void drawCistercian50(){
  drawCistercian10();
  drawCistercian40();
}
void drawCistercian60() {
  matrix.drawFastHLine(zeroX, zeroY + legth-1, legth, AMBER);
}
void drawCistercian70(){
  drawCistercian10();
  drawCistercian60();
}
void drawCistercian80(){
  drawCistercian20();
  drawCistercian60();
}
void drawCistercian90(){
  drawCistercian10();
  drawCistercian20();
  drawCistercian60();
}
void drawCistercian1h() {
  matrix.drawFastVLine(zeroLength + 1, zeroY - legth, legth, AMBER);
}
void drawCistercian2h() {
  matrix.drawFastVLine(zeroLength - legth+1, zeroY - legth, legth, AMBER);
}
void drawCistercian3h() {
  matrix.drawLine(zeroLength - legth + 1, zeroY - legth, zeroLength + 1, zeroY, AMBER);
}
void drawCistercian4h() {
  matrix.drawLine(zeroLength - legth+1, zeroY, zeroLength+1, zeroY - legth, AMBER);
}
void drawCistercian5h(){
  drawCistercian1h();
  drawCistercian4h();
}
void drawCistercian6h() {
  matrix.drawFastHLine(zeroLength - legth +1, zeroY - legth, legth, AMBER);
}
void drawCistercian7h(){
  drawCistercian1h();
  drawCistercian6h();
}
void drawCistercian8h(){
  drawCistercian2h();
  drawCistercian6h();
}
void drawCistercian9h(){
  drawCistercian1h();
  drawCistercian2h();
  drawCistercian6h();
}
void drawCistercian1th() {
  matrix.drawFastVLine(zeroLength+1, zeroY, legth, AMBER);
}
void drawCistercian2th() {
  matrix.drawFastVLine(zeroLength-legth+1, zeroY, legth, AMBER);
}
void drawCistercian3th() {
  matrix.drawLine(zeroLength-legth+1, zeroY+legth, zeroLength+1, zeroY, AMBER);
}
void drawCistercian4th() {
  matrix.drawLine(zeroLength-legth+1, zeroY, zeroLength, zeroY+legth-1, AMBER);
}

void drawCistercian5th(){
  drawCistercian1th();
  drawCistercian4th();
}
void drawCistercian6th() {
  matrix.drawFastHLine(zeroLength-legth+1, zeroY+legth-1, legth, AMBER);
}

void drawCistercian7th(){
  drawCistercian1th();
  drawCistercian6th();
}
void drawCistercian8th(){
  drawCistercian2th();
  drawCistercian6th();
}
void drawCistercian9th(){
  drawCistercian1th();
  drawCistercian2th();
  drawCistercian6th();
}
void drawUnits(){
  if (counter == 0){
    drawCistercian0();
  }
  if (counter == 1){
    drawCistercian1();
  }
  if (counter ==2){
    drawCistercian2();
  }
  if (counter == 3){
    drawCistercian3();
  }
  if (counter == 4){
    drawCistercian4();
  }
  if (counter == 5){
    drawCistercian5();
  }
  if (counter == 6){
    drawCistercian6();
  }
  if (counter == 7){
    drawCistercian7();
  }
  if (counter == 8){
    drawCistercian8();
  }
  if (counter == 9){
    drawCistercian9();
  }
}
void drawTens(){
  if (counter >= 10 && counter < 20){
    drawCistercian10();
    counter -=10;
    drawUnits();
    counter+=10;
  }
  if (counter >= 20 && counter < 30){
    drawCistercian20();
    counter -=20;
    drawUnits();
    counter+=20;
  }
  if (counter >= 30 && counter < 40){
    drawCistercian30();
    counter -=30;
    drawUnits();
    counter+=30;
  }
  if (counter >= 40 && counter < 50){
    drawCistercian40();
    counter -=40;
    drawUnits();
    counter+=40;
  }
  if (counter >= 50 && counter < 60){
    drawCistercian50();
    counter -=50;
    drawUnits();
    counter+=50;
  }
  if (counter >= 60 && counter < 70){
    drawCistercian60();
    counter -=60;
    drawUnits();
    counter+=60;
  }
  if (counter >= 70 && counter < 80){
    drawCistercian70();
    counter -=70;
    drawUnits();
    counter+=70;
  }
  if (counter >= 80 && counter < 90){
    drawCistercian80();
    counter -=80;
    drawUnits();
    counter+=80;
  }
  if (counter >= 90 && counter < 100){
    drawCistercian90();
    counter -=90;
    drawUnits();
    counter+=90;
  }
}
void drawHundreds(){
  if (counter >= 100&& counter < 200){
    drawCistercian1h();
    counter -=100;
    drawUnits();
    drawTens();
    counter+=100;
  }
  if (counter >= 200&& counter < 300){
    drawCistercian2h();
    counter -=200;
    drawUnits();
    drawTens();
    counter+=200;
  }
  if (counter >= 300&& counter < 400){
    drawCistercian3h();
    counter -=300;
    drawUnits();
    drawTens();
    counter+=300;
  }
  if (counter >= 400&& counter < 500){
    drawCistercian4h();
    counter -=400;
    drawUnits();
    drawTens();
    counter+=400;
  }
  if (counter >= 500&& counter < 600){
    drawCistercian5h();
    counter -=500;
    drawUnits();
    drawTens();
    counter+=500;
  }
  if (counter >= 600&& counter < 700){
    drawCistercian6h();
    counter -=600;
    drawUnits();
    drawTens();
    counter+=600;
  }
  if (counter >= 700&& counter < 800){
    drawCistercian7h();
    counter -=700;
    drawUnits();
    drawTens();
    counter+=700;
  }
  if (counter >= 800&& counter < 900){
    drawCistercian8h();
    counter -=800;
    drawUnits();
    drawTens();
    counter+=800;
  }
  if (counter >= 900&& counter < 1000){
    drawCistercian9h();
    counter -=900;
    drawUnits();
    drawTens();
    counter+=900;
  }
}
void drawThousands(){
  if (counter >= 1000 && counter < 2000){
    drawCistercian1th();
    counter -=1000;
    drawUnits();
    drawTens();
    drawHundreds();
    counter+=1000;
  }
  if (counter >= 2000 && counter < 3000){
    drawCistercian2th();
    counter -=2000;
    drawUnits();
    drawTens();
    drawHundreds();
    counter+=2000;
  }
  if (counter >= 3000 && counter < 4000){
    drawCistercian3th();
    counter -=3000;
    drawUnits();
    drawTens();
    drawHundreds();
    counter+=3000;
  }
  if (counter >= 4000 && counter < 5000){
    drawCistercian4th();
    counter -=4000;
    drawUnits();
    drawTens();
    drawHundreds();
    counter+=4000;
  }
  if (counter >= 5000 && counter < 6000){
    drawCistercian5th();
    counter -=5000;
    drawUnits();
    drawTens();
    drawHundreds();
    counter+=5000;
  }
  if (counter >= 6000 && counter < 7000){
    drawCistercian6th();
    counter -=6000;
    drawUnits();
    drawTens();
    drawHundreds();
    counter+=6000;
  }
  if (counter >= 7000 && counter < 8000){
    drawCistercian7th();
    counter -=7000;
    drawUnits();
    drawTens();
    drawHundreds();
    counter+=7000;
  }
  if (counter >= 8000 && counter < 9000){
    drawCistercian8th();
    counter -=8000;
    drawUnits();
    drawTens();
    drawHundreds();
    counter+=8000;
  }
  if (counter >= 9000 && counter < 10000){
    drawCistercian9th();
    counter -=9000;
    drawUnits();
    drawTens();
    drawHundreds();
    counter+=9000;
  }
}
void drawDots(){
  if (counter % 2 == 0){
    matrix.fillRect(30, 7, 4, 4, AMBER);
    matrix.fillRect(30, 22, 4, 4, AMBER);
  }
}
void drawCistercian(){
  drawUnits();
  drawTens();
  drawHundreds();
  drawThousands();
}

void cistercianClock(){
  #ifndef WIFISETUP
  WiFi.begin(ssid, pass);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
  }
  timeClient.begin();
  #define WIFISETUP
  #endif
  uint32_t t;
  while (((t = micros()) - prevTime) < (1000000L / MAX_FPS))
    ;
  prevTime = t;
  timeClient.update();
  matrix.fillScreen(0x0);  // blank the screen

  int h = timeClient.getHours();
  int m = timeClient.getMinutes();
  counter = concatenate(h, m);

  drawCistercian();//interpret the counter
  drawCistercian0();//but always shows zero
  matrix.drawPixel(0, 31, MAGENTA);
  matrix.show();  //show objects
  delay(1000);
}

