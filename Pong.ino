#include "LedControl.h"
#define BUTTON_ONE_UP A1
#define BUTTON_ONE_DOWN A3
#define BUTTON_TWO_UP 5
#define BUTTON_TWO_DOWN 2
#define BUTTON_START_STOP 4
#define BUZZER A5
LedControl lc = LedControl(12, 11, 10, 1); //DIN, CLK, LOAD, No. DRIVER

// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

short pozPadOne;
short pozPadTwo;


short pozBallX;
short pozBallY;
short speedBall = 0;

short directionBall = 5;//Numpad directions, 5 - center

bool scored;// semafor care verifica daca a fost castigata runda

bool played;// semafor care verifica ca a fost jucata o runda

short scorePlayerOne;
short scorePlayerTwo;

unsigned long padMillis = 0;
unsigned long ballMillis = 0;

short delayAnimationTime = 20;
short padDelay = 50;
short ballDelay = 300;


const byte START[][8] = {
  {
    B00000000,
    B00111100,
    B01100110,
    B01100000,
    B00111100,
    B00000110,
    B01100110,
    B00111100
  }, {
    B00000000,
    B01111110,
    B01011010,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000
  }, {
    B00000000,
    B00111100,
    B01100110,
    B01100110,
    B01111110,
    B01100110,
    B01100110,
    B01100110
  }, {
    B00000000,
    B01111100,
    B01100110,
    B01100110,
    B01111100,
    B01111000,
    B01101100,
    B01100110
  }, {
    B00000000,
    B01111110,
    B01011010,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000
  }
};

const byte WIN[][8] = {
  {
    B00000000,
    B10000001,
    B11000011,
    B11000011,
    B11011011,
    B11111111,
    B01100110,
    B00100100
  }, {
    B00000000,
    B00111100,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00011000,
    B00111100
  }, {
    B00000000,
    B01100011,
    B01110011,
    B01111011,
    B01101111,
    B01100111,
    B01100011,
    B01100011
  }
};

const byte IMAGES[][8] = {
  {
    B00000000,
    B00111100,
    B01100110,
    B01101110,
    B01110110,
    B01100110,
    B01100110,
    B00111100
  }, {
    B00000000,
    B00011000,
    B00011000,
    B00111000,
    B00011000,
    B00011000,
    B00011000,
    B01111110
  }, {
    B00000000,
    B00111100,
    B01100110,
    B00000110,
    B00001100,
    B00110000,
    B01100000,
    B01111110
  }, {
    B00000000,
    B00111100,
    B01100110,
    B00000110,
    B00011100,
    B00000110,
    B01100110,
    B00111100
  }, {
    B00000000,
    B00001100,
    B00011100,
    B00101100,
    B01001100,
    B01111110,
    B00001100,
    B00001100
  }, {
    B00000000,
    B01111110,
    B01100000,
    B01111100,
    B00000110,
    B00000110,
    B01100110,
    B00111100
  }, {
    B00000000,
    B00111100,
    B01100110,
    B01100000,
    B01111100,
    B01100110,
    B01100110,
    B00111100
  }, {
    B00000000,
    B01111110,
    B01100110,
    B00001100,
    B00001100,
    B00011000,
    B00011000,
    B00011000
  }, {
    B00000000,
    B00111100,
    B01100110,
    B01100110,
    B00111100,
    B01100110,
    B01100110,
    B00111100
  }, {
    B00000000,
    B00111100,
    B01100110,
    B01100110,
    B00111110,
    B00000110,
    B01100110,
    B00111100
  }
};

void setup()
{
  scored = 0;
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 2); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

  pinMode(BUTTON_ONE_UP, INPUT_PULLUP);
  pinMode(BUTTON_TWO_UP, INPUT_PULLUP);
  pinMode(BUTTON_TWO_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_ONE_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_START_STOP, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  randomSeed(analogRead(0));

  bool side = random(2); //variabila aleatoare care hotaraste de unde se porneste llea
  directionBall = random(1, 4) * 3 - side * 2; //directie hotarata aleator in spre paleta adversa

  pozBallX = 0 + 7 * side;
  pozBallY = random(8);
  speedBall = 0; //viteza adaugata a mingei care la inceput este 0

  pozPadOne = random(5) + 2;
  pozPadTwo = random(5) + 2;

  lc.setLed(0, 0, pozPadOne + 1, true);
  lc.setLed(0, 0, pozPadOne,  true);
  lc.setLed(0, 0, pozPadOne - 1, true);

  lc.setLed(0, 7, pozPadTwo + 1, true);
  lc.setLed(0, 7, pozPadTwo,  true);
  lc.setLed(0, 7, pozPadTwo - 1, true);


}

void movePadOneUp() {
  pozPadOne++;
  lc.setLed(0, 0, pozPadOne + 1, true);
  lc.setLed(0, 0, pozPadOne - 2, false);
}
void movePadOneDown() {
  pozPadOne--;
  lc.setLed(0, 0, pozPadOne - 1, true);
  lc.setLed(0, 0, pozPadOne + 2, false);
}

void movePadTwoUp() {
  pozPadTwo++;
  lc.setLed(0, 7, pozPadTwo + 1, true);
  lc.setLed(0, 7, pozPadTwo - 2, false);
}

void movePadTwoDown() {
  pozPadTwo--;
  lc.setLed(0, 7, pozPadTwo - 1, true);
  lc.setLed(0, 7, pozPadTwo + 2, false);
}

void padMove()
{
  bool buttonOneUpState = digitalRead(BUTTON_ONE_UP);
  bool buttonOneDownState = digitalRead(BUTTON_ONE_DOWN);

  bool buttonTwoUpState = digitalRead(BUTTON_TWO_UP);
  bool buttonTwoDownState = digitalRead(BUTTON_TWO_DOWN);

  if (buttonOneUpState == LOW && pozPadOne < 6)
    movePadOneUp();

  if (buttonOneDownState == LOW && pozPadOne > 1)
    movePadOneDown();

  if (buttonTwoUpState == LOW && pozPadTwo < 6)
    movePadTwoUp();

  if (buttonTwoDownState == LOW && pozPadTwo > 1)
    movePadTwoDown();
}

void ballMove();

void changeDirection(short stepBack, short stepFront = 0) //schimbarea diretiei are la fiecare coliziune
{
  switch (stepBack)
  {
    case 1: {
        pozBallY++;
        pozBallX--;
        break;
      }
    case 3: {
        pozBallY++;
        pozBallX++;
        break;
      }
    case 4: {
        pozBallX--;
        break;
      }
    case 6: {
        pozBallX++;
        break;
      }
    case 7: {
        pozBallY--;
        pozBallX--;
        break;
      }
    case 9: {
        pozBallY--;
        pozBallX++;
        break;
      }
  }
  if (stepFront != 0)
  {
    if (speedBall < 250)
      speedBall += 10;
    directionBall = stepFront;
    ballMove();
  }
}
void ballMove()
{
  if (!(pozBallX == 0 && (pozBallY == pozPadOne || pozBallY == pozPadOne + 1 || pozBallY == pozPadOne - 1) || pozBallX == 7 && (pozBallY == pozPadTwo || pozBallY == pozPadTwo + 1 || pozBallY == pozPadTwo - 1 )))
    lc.setLed(0, pozBallX, pozBallY, false);
  //daca mingea se afla peste paleta la zedurrile paletei nu vor fi stinse
  changeDirection(directionBall);

  short stepBack, stepFront = 0;//daca am depasit marginile , vom face un pas inapoi apoi vom calibra directia

  if (pozBallY < 0)
  {
    if (directionBall == 9)
      changeDirection(1, 3);
    else
      changeDirection(3, 1);
    return 0;
  }

  if (pozBallY > 7)
  {
    if (directionBall == 1)
      changeDirection(9, 7);
    else
      changeDirection(7, 9);
    return 0;
  }
  if (pozBallX < 0)
  {
    scorePlayerTwo++;
    scored++;
    return 0;
  }
  if (pozBallX < 1)
  {
    if (pozBallY == pozPadOne - 1)
      stepFront = 9;
    if (pozBallY == pozPadOne)
      stepFront = 6;
    if (pozBallY == pozPadOne + 1)
      stepFront = 3;
    if (directionBall == 1)
      stepBack = 9;
    if (directionBall == 7)
      stepBack = 3;
    if (directionBall == 4)
      stepBack = 6;
    if (stepBack && stepFront)
    {
      tone(BUZZER, 360);//la intalnirea paletei vom canta o nota
      changeDirection(stepBack, stepFront);
      return 0;
    }
  }

  if (pozBallX > 7)
  {
    scorePlayerOne++;
    scored++;
    return 0;
  }
  if (pozBallX > 6)
  {
    if (pozBallY == pozPadTwo - 1)
      stepFront = 7;
    if (pozBallY == pozPadTwo)
      stepFront = 4;
    if (pozBallY == pozPadTwo + 1)
      stepFront = 1;
    if (directionBall == 3)
      stepBack = 7;
    else if (directionBall != 6)
      stepBack = 1;
    else
      stepBack = 4;
    if (stepBack && stepFront)
    {
      tone(BUZZER, 400);//la intalnirea paletei vom canta o nota
      changeDirection(stepBack, stepFront);
      return 0;
    }

  }
  lc.setLed(0, pozBallX, pozBallY, true);
}
void start()//vom rula aceasta secventa la inceputul fiecarui joc si dupa fiecare partida 
{
  bool buttonStartStopState = digitalRead(BUTTON_START_STOP);
  while ( buttonStartStopState != LOW)
  {
    for (short k = 0; k < 5; k++)
    {
      for (short j = 0; j < 8; j++)
      {
        for (short i = 0; i < 8; i++)
        {
          if (buttonStartStopState == LOW )
            break;
          buttonStartStopState = digitalRead(BUTTON_START_STOP);
          lc.setColumn(0, i, START[k][7 - i] >> 7 - j);
        }
        delay(delayAnimationTime);
      }
      for (short j = 0; j < 8; j++)
      {
        for (short i = 0; i < 8; i++)
        {
          if (buttonStartStopState == LOW )
            break;
          buttonStartStopState = digitalRead(BUTTON_START_STOP);
          lc.setColumn(0, i, START[k][7 - i] << j);
        }
        delay(delayAnimationTime);
      }
    }
  }
  clearScore();
  played++;
   setup();
}
void pause()
{
  bool buttonStartStopState = digitalRead(BUTTON_START_STOP);
  if (buttonStartStopState == LOW )
    while (1)
    {
      if (!digitalRead(BUTTON_ONE_UP) || !digitalRead(BUTTON_ONE_DOWN) || !digitalRead(BUTTON_TWO_UP) || !digitalRead(BUTTON_TWO_DOWN))
        break;
    }

}


void clearScore()
{
  scorePlayerOne = 0;
  scorePlayerTwo = 0;
}
void score()//vom afisa scorul, iar daca jocul s-a terminat, adica unul dintre jucatori a atins scorul de 10 puncte, va fi insotit de un mesaj si o mica secventa muzicala
{
  if (scorePlayerOne > 9)
  {
    for (short l = 0; l < 3; l++)
    {
      for (short j = 0; j < 8; j++)
      {
        for (short i = 0; i < 8; i++)
        {
          lc.setColumn(0, i, IMAGES[1][7 - i] << 7 - j);
          tone(BUZZER, 400);
        }
        delay(delayAnimationTime);
      }

      for (short j = 0; j < 8; j++)
      {
        for (short i = 0; i < 8; i++)
        {
          lc.setColumn(0, i, IMAGES[1][7 - i] << j);
          tone(BUZZER, 560);
        }
        delay(delayAnimationTime);
      }
      noTone(BUZZER);

      for (short k = 0; k < 3; k++)
      {
        for (short j = 0; j < 8; j++)
        {
          for (short i = 0; i < 8; i++)
          {
            lc.setColumn(0, i, WIN[k][7 - i] >> 7 - j);
          }
          delay(delayAnimationTime);
        }
        for (short j = 0; j < 8; j++)
        {
          for (short i = 0; i < 8; i++)
          {
            lc.setColumn(0, i, WIN[k][7 - i] << j);
          }
          delay(delayAnimationTime);
        }
      }
    }
    start();
  }
  else if (scorePlayerTwo > 9)
  {
    for (short l = 0; l < 3; l++)
    {
      for (short j = 0; j < 8; j++)
      {
        for (short i = 0; i < 8; i++)
        {
          lc.setColumn(0, i, IMAGES[2][7 - i] >> 7 - j);
          tone(BUZZER, 400);

        }
        delay(delayAnimationTime);
      }

      for (short j = 0; j < 8; j++)
      {
        for (short i = 0; i < 8; i++)
        {
          lc.setColumn(0, i, IMAGES[2][7 - i] >>  j);
          tone(BUZZER, 560);
        }
        delay(delayAnimationTime);
      }
      noTone(BUZZER);

      for (short k = 0; k < 3; k++)
      {
        for (short j = 0; j < 8; j++)
        {
          for (short i = 0; i < 8; i++)
          {
            lc.setColumn(0, i, WIN[k][7 - i] << 7 - j);
          }
          delay(delayAnimationTime);
        }
        for (short j = 0; j < 8; j++)
        {
          for (short i = 0; i < 8; i++)
          {
            lc.setColumn(0, i, WIN[k][7 - i] >> j);
          }
          delay(delayAnimationTime);
        }
      }
    }
    start();
  }
  else {
    for (short j = 0; j < 8; j++)
    {
      for (short i = 0; i < 8; i++)
      {
        lc.setColumn(0, i, IMAGES[scorePlayerOne][7 - i] << 7 - j);
      }
      delay(delayAnimationTime);
    }
    for (short j = 0; j < 8; j++)
    {
      for (short i = 0; i < 8; i++)
      {
        lc.setColumn(0, i, IMAGES[scorePlayerOne][7 - i] << j);
      }
      delay(delayAnimationTime);
    }
    for (short j = 0; j < 8; j++)
    {
      for (short i = 0; i < 8; i++)

      {
        lc.setColumn(0, i, IMAGES[scorePlayerTwo][7 - i] >> 7 - j);
      }
      delay(delayAnimationTime);
    }
    for (short j = 0; j < 8; j++)
    {
      for (short i = 0; i < 8; i++)
      {
        lc.setColumn(0, i, IMAGES[scorePlayerTwo][7 - i] >> j);
      }
      delay(delayAnimationTime);
    }
    setup();
  }
}
void loop()//bucla care ruleaza instructiunile
{
  if (!played)
    start();
  if (millis() - padMillis > padDelay)
  {
    padMove();
    padMillis = millis();
  }
  if (millis() - ballMillis > ballDelay - speedBall)
  {    noTone(BUZZER);

    ballMove();
    ballMillis = millis();
  }
  if (scored)
  {
    
    score();
  }
  pause();
}
