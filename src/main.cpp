#include <Arduino.h>
#include <EnableInterrupt.h>
#include <avr/sleep.h>
#include <TimerOne.h>

#define BTN1 13
#define BTN2 12
#define BTN3 8
#define BTN4 7

const int NUM_BUTTONS = 4;
const int BUTTON_PINS[NUM_BUTTONS] = {BTN1, BTN2, BTN3, BTN4};

#define RED_LED 6
#define GREEN_LED1 5
#define GREEN_LED2 4
#define GREEN_LED3 3
#define GREEN_LED4 2
#define SLEEP_TIME 10000
#define DEBOUNCE_TLR 300

enum GameState
{
  START,
  PLAY,
  END
} state;

unsigned long sleepStartTime;
unsigned long playTime;
unsigned long lastInterrupt = 0;
int brightness = 0; // how bright the LED is
int fadeAmount = 5;
const int NUM_LEDS = 4;
const int LED_PINS[NUM_LEDS] = {GREEN_LED1, GREEN_LED2, GREEN_LED3, GREEN_LED4};
int LED_STATES[NUM_LEDS] = {HIGH, HIGH, HIGH, HIGH};


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
void play_led_0()
{
  static unsigned long lastInterrupt = 0;

  if (check_debouncing(&lastInterrupt))
  {
    LED_STATES[0] = !LED_STATES[0];
  }
}

void play_led_1(){
  static unsigned long lastInterrupt = 0;

  if (check_debouncing(&lastInterrupt))
  {
    LED_STATES[1] = !LED_STATES[1];
  }
}
void play_led_2(){
  static unsigned long lastInterrupt = 0;

  if (check_debouncing(&lastInterrupt))
  {
    LED_STATES[2] = !LED_STATES[2];
  }
}
void play_led_3(){
  static unsigned long lastInterrupt = 0;

  if (check_debouncing(&lastInterrupt))
  {
    LED_STATES[3] = !LED_STATES[3];
  }
}

/*____________________________________________________________________________________*/

void start_game()
{
  static unsigned long lastInterrupt = 0;
  if (check_debouncing(&lastInterrupt))
  {
    Serial.println("debouncing funziona");
    delay(30);
    nextState(PLAY);
    for (int i = 0; i < NUM_LEDS; i++)
    {
      disableInterrupt(BUTTON_PINS[i]);
    }
    playTime = millis();
    enableInterrupt(BUTTON_PINS[0], play_led_0, RISING);
    enableInterrupt(BUTTON_PINS[1], play_led_1, RISING);
    enableInterrupt(BUTTON_PINS[2], play_led_2, RISING);
    enableInterrupt(BUTTON_PINS[3], play_led_3, RISING);
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
  delay(30);
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

  // Inizializzo il timer 
  Timer1.initialize(6000);
  Timer1.attachInterrupt(fade);
  Serial.println("Welcome to the Restore the Light Game. Press Key B1 to Start”. ");
}

void loop()
{

  switch (state)
  {

  case START:
    if (millis() - sleepStartTime > SLEEP_TIME)
    {
      sleepNow();
      sleepStartTime = millis();
    }
    break;
  case PLAY:
    Timer1.stop();
    digitalWrite(RED_LED, LOW);

    /*Accendo tutti i led verdi e una volta aspettato un tempo t 
    inizio a farli spegnere, daje*/

    for (int i = 0; i < NUM_LEDS; i++)
    {
      digitalWrite(LED_PINS[i], LED_STATES[i]);
    }
    break;
  case END:
    Serial.println("END");
    state = START;
    Timer1.start();
    sleepStartTime = millis();
    disableInterrupt(BTN1);
    enableInterrupt(BTN1, start_game, RISING);
    break;
  }
}
