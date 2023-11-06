#include <Arduino.h>
#include <EnableInterrupt.h>
#include <avr/sleep.h>
#include <timerOne.h>
#include <time.h>
#include <avr8-stub.h>

#define NUM_BUTTONS 4
#define BTN1 13
#define BTN2 12
#define BTN3 8
#define BTN4 7

#define NUM_LEDS 4
#define RED_LED 6
#define GREEN_LED1 5
#define GREEN_LED2 4
#define GREEN_LED3 3
#define GREEN_LED4 2
#define SLEEP_TIME 10000
#define DEBOUNCE_TLR 50

#define T2 2000000
#define T3 30000

enum GameState
{
  START,
  MC,
  PLAY,
  GAMEOVER,
  END
} state;

bool runningTimer = false;
unsigned long sleepStartTime;
long playTime;
unsigned long lastInterrupt = 0;
int brightness = 0; // how bright the LED is
int fadeAmount = 5;
const int LED_PINS[NUM_LEDS] = {GREEN_LED1, GREEN_LED2, GREEN_LED3, GREEN_LED4};
int LED_STATES[NUM_LEDS] = {HIGH, HIGH, HIGH, HIGH};
const int BUTTON_PINS[NUM_BUTTONS] = {BTN1, BTN2, BTN3, BTN4};
int LED_SOLUTION[NUM_LEDS];
int score = 0;
int count_mc = 0;
const int ANALOG_PIN = A1;

int random_leds[NUM_LEDS];

/**
 * @brief funzione che serve per controllare il debounce dei pulsanti
 * @param lastInterrupt puntatore all'ultima volta in cui è stato premuto il pulsante
 * @return true se è passato il tempo di debounce, false altrimenti
 */
bool check_debouncing(unsigned long *lastInterrupt)
{
  unsigned long currentMillis = millis();
  if (millis() - *lastInterrupt > DEBOUNCE_TLR)
  {
    *lastInterrupt = currentMillis;
    return true;
  }
  else
  {
    return false;
  }
}

/**
 * @brief incapusla le modifiche di stato in cui si trova il gioco
 * @param Newstate nuovo stato
 */
void nextState(GameState Newstate)
{
  state = Newstate;
}

/*play led metrod _____________________________________________________________________*/

void game_logic(int id)
{
  if (LED_SOLUTION[count_mc] == id)
  {
    LED_STATES[id] = !LED_STATES[id];
    LED_SOLUTION[count_mc] = -1;

    if (count_mc == 0)
    {
      score++;
      nextState(END);
    }
    else
    {
      count_mc--;
    }
  }
  else
  {
    nextState(GAMEOVER);
  }
}

void play_led_0()
{
  static unsigned long lastInterrupt = 0;
  int id = 0;

  if (check_debouncing(&lastInterrupt))
  {
    Serial.println("led 0");
    game_logic(id);
  }
}

void play_led_1()
{
  static unsigned long lastInterrupt = 0;
  int id = 1;
  if (check_debouncing(&lastInterrupt))
  {
    Serial.println("led 1");

    game_logic(id);
  }
}

void play_led_2()
{
  static unsigned long lastInterrupt = 0;
  int id = 2;

  if (check_debouncing(&lastInterrupt))
  {
    Serial.println("led 2");
    game_logic(id);
  }
}

void play_led_3()
{
  static unsigned long lastInterrupt = 0;
  int id = 3;
  if (check_debouncing(&lastInterrupt))
  {
    Serial.println("led 3");
    game_logic(id);
  }
}



void switch_on_led()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    LED_STATES[i] = HIGH;
  }
}

void switch_off_led()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    LED_STATES[i] = LOW;
  }
}

/*____________________________________________________________________________________*/

void mc_play()
{

  if (count_mc < NUM_LEDS)
  {
    LED_STATES[random_leds[count_mc]] = LOW;
    LED_SOLUTION[count_mc] = random_leds[count_mc];

    Serial.println(LED_SOLUTION[count_mc]);
    count_mc++;
    if (count_mc == NUM_LEDS)
    {
      disableInterrupt(BUTTON_PINS[0]);
      enableInterrupt(BUTTON_PINS[0], play_led_0, RISING);
      enableInterrupt(BUTTON_PINS[1], play_led_1, RISING);
      enableInterrupt(BUTTON_PINS[2], play_led_2, RISING);
      enableInterrupt(BUTTON_PINS[3], play_led_3, RISING);
      Timer1.detachInterrupt();
      Timer1.stop();
      count_mc--;
      nextState(PLAY);
      playTime = millis();
    }
  }
}
void ledOutput()
{
  digitalWrite(GREEN_LED1, LED_STATES[0]);
  digitalWrite(GREEN_LED2, LED_STATES[1]);
  digitalWrite(GREEN_LED3, LED_STATES[2]);
  digitalWrite(GREEN_LED4, LED_STATES[3]);
}
void start_game()
{
  static unsigned long lastInterrupt = 0;
  if (check_debouncing(&lastInterrupt))
  {
    digitalWrite(RED_LED, LOW);

    for (int i = 0; i < NUM_LEDS; i++)
    {
      disableInterrupt(BUTTON_PINS[i]);
    }

    for (int i = 0; i < NUM_LEDS; i++)
    {
      random_leds[i] = i;
    }
    // Mescola l'array
    srand(millis());
    for (int i = NUM_LEDS - 1; i > 0; i--)
    {
      int j = rand() % (i + 1);

      int temp = random_leds[i];
      random_leds[i] = random_leds[j];
      random_leds[j] = temp;
    }

    switch_on_led();
    ledOutput();
    nextState(MC);
    Timer1.stop();
    Timer1.detachInterrupt();
  }
}

void wakeUpNow()
{
  // just want the thing to wake up
}

void sleepNow() // here we put the arduino to sleep
{
  disableInterrupt(BUTTON_PINS[0]);
  Serial.println("GOING TO SLEEP");
  delay(30);
  digitalWrite(RED_LED, HIGH);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();                      // enables the sleep bit in the mcucr register
  // so sleep is possible. just a safety pin
  enableInterrupt(BUTTON_PINS[0], wakeUpNow, RISING); // use interrupt 0 (pin 2) and run function
  // wakeUpNow when pin 2 gets LOW
  sleep_mode(); // here the device is actually put to sleep!!
  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable(); // first thing after waking from sleep:
  // disable sleep...
  disableInterrupt(BUTTON_PINS[0]); // wakeUpNow code will not be executed
                                    // during normal running time.
  enableInterrupt(BUTTON_PINS[0], start_game, RISING);
}

void fade()
{
  // set the brightness of pin 9:
  analogWrite(RED_LED, brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness == 0 || brightness == 255)
  {
    fadeAmount = -fadeAmount;
  }
  delay(20);
}

void start_faiding()
{
  Timer1.detachInterrupt();
  Timer1.initialize(6000);
  Timer1.attachInterrupt(fade);
  Timer1.start();
}

void boardInit()
{
  count_mc = 0;
  start_faiding();
  switch_off_led();
  runningTimer = false;
}

void setup()
{

  state = START;
  sleepStartTime = millis();

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED1, OUTPUT);
  pinMode(GREEN_LED2, OUTPUT);
  pinMode(GREEN_LED3, OUTPUT);
  pinMode(GREEN_LED4, OUTPUT);

  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);
  pinMode(BTN3, INPUT);
  pinMode(BTN4, INPUT);

  enableInterrupt(BTN1, start_game, HIGH);
  Serial.begin(9600);
  boardInit();
  // Inizializzo il timer
  start_faiding();
  Serial.println("Welcome to the Restore the Light Game. Press Key B1 to Start”. ");
}

void loop()
{
  int sensorValue;
  int outputValue;
  switch (state)
  {

  case START:

    sensorValue = analogRead(ANALOG_PIN);
    outputValue = map(sensorValue, 0, 1023, 1, 4);
    if (millis() - sleepStartTime > SLEEP_TIME)
    {
      sleepNow();
      sleepStartTime = millis();
    }
    break;
  case MC:
    /* while tutte le luci */
    /* ne spefgni una a caso ogni t2 tempo*/
    /* ti salvi quella spenta nel vettore orinato*/
    /* case = PLAYER */
    ledOutput();

    if (!runningTimer)
    {

      Timer1.attachInterrupt(mc_play);
      delay(10);
      Serial.println("start timer");
      /*devo ricordarmi che Timer1 accetta microsecondi*/
      Timer1.initialize(T2 - (outputValue * 100000 * score > 0 ? score : 1));
      runningTimer = true;
    }
    break;
  case PLAY:
    
    ledOutput();
    if (millis() - playTime > T3 - (outputValue * 100 * score > 0 ? score : 1))
    {
      Serial.println("time finisced");
      if (LED_SOLUTION[0] != -1)
      {
        nextState(GAMEOVER);
      }
    }
    break;
  case GAMEOVER:
    Serial.println("GAMEOVER");
    Serial.print("your score is: ");
    Serial.print(score);
    Serial.println(" press B1 to restart, your score will be reset");
    score = 0;
    nextState(START);
    boardInit();
    ledOutput();
    sleepStartTime = millis();
    disableInterrupt(BTN1);
    disableInterrupt(BTN1);
    disableInterrupt(BTN2);
    disableInterrupt(BTN3);
    disableInterrupt(BTN4);
    enableInterrupt(BTN1, start_game, HIGH);
    break;
  case END:
    Serial.print("your score is: ");
    Serial.println(score);
    Serial.println("press B1 play another game and encrease your score");
    nextState(START);
    boardInit();
    ledOutput();
    sleepStartTime = millis();
    disableInterrupt(BTN1);
    disableInterrupt(BTN2);
    disableInterrupt(BTN3);
    disableInterrupt(BTN4);
    enableInterrupt(BTN1, start_game, HIGH);
    break;
  }
}
