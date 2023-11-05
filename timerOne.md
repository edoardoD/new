```cpp
```cpp
#include <TimerOne.h>

// Definisci i pin dei LED
#define LED1 3
#define LED2 5
#define LED3 6
#define LED4 9

// Definisci il periodo del timer in microsecondi
#define PERIOD 1000000

// Definisci una variabile globale che conta i LED spenti
int ledOff = 0;

void setup() {
  // Inizializza i pin dei LED come output e li accende
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, HIGH);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, HIGH);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED3, HIGH);
  pinMode(LED4, OUTPUT);
  digitalWrite(LED4, HIGH);

  // Inizializza il timer con il periodo definito
  Timer1.initialize(PERIOD);

  // Collega la funzione di interrupt al timer
  Timer1.attachInterrupt(blink);
}

void loop() {
  // Non fa nulla, il timer gestisce il lampeggio dei LED
}

void blink() {
  // Questa funzione viene eseguita ogni volta che il timer scade
  // Spegne un LED alla volta in base al valore di ledOff
  switch (ledOff) {
    case 0:
      digitalWrite(LED1, LOW); // Spegne il LED1
      break;
    case 1:
      digitalWrite(LED2, LOW); // Spegne il LED2
      break;
    case 2:
      digitalWrite(LED3, LOW); // Spegne il LED3
      break;
    case 3:
      digitalWrite(LED4, LOW); // Spegne il LED4
      break;
    case 4:
      Timer1.stop(); // Ferma il timer
      break;
  }
  // Incrementa il contatore dei LED spenti
  ledOff++;
}

```