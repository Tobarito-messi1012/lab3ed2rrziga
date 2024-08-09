#include <Arduino.h>
// tenemos 3 botones 
// 4 leds de decremento y incremento

#define BOTON_INCREMENTO 12
#define BOTON_DECREMENTO 14
#define BOTON_REINICIO 33
#define LED1_BOTON 16
#define LED2_BOTON 17
#define LED3_BOTON 18
#define LED4_BOTON 19
//4 leds para el timer 
#define LED1_TIMER 21
#define LED2_TIMER 22
#define LED3_TIMER 23
#define LED4_TIMER 25
//la alarma se define con otra led en vez de buzzer
#define INDICADOR 26
//volatil es para que inicie desde 0 cada uno
volatile int contadorBotones = 0;
volatile int contadorTemporizador = 0;
hw_timer_t * temporizador = NULL;

// Función para actualizar los LEDs de los botones
void actualizarLEDsBotones() {
  digitalWrite(LED1_BOTON, (contadorBotones & 0x01) ? HIGH : LOW);
  digitalWrite(LED2_BOTON, (contadorBotones & 0x02) ? HIGH : LOW);
  digitalWrite(LED3_BOTON, (contadorBotones & 0x04) ? HIGH : LOW);
  digitalWrite(LED4_BOTON, (contadorBotones & 0x08) ? HIGH : LOW);
}

// Función para actualizar los LEDs del temporizador
void actualizarLEDsTemporizador() {
  digitalWrite(LED1_TIMER, (contadorTemporizador & 0x01) ? HIGH : LOW);
  digitalWrite(LED2_TIMER, (contadorTemporizador & 0x02) ? HIGH : LOW);
  digitalWrite(LED3_TIMER, (contadorTemporizador & 0x04) ? HIGH : LOW);
  digitalWrite(LED4_TIMER, (contadorTemporizador & 0x08) ? HIGH : LOW);
}

// Interrupción para el botón de incremento
void IRAM_ATTR incrementarContador() {
  contadorBotones = (contadorBotones + 1) % 16; // Contador de 4 bits
  actualizarLEDsBotones();
}

// Interrupción para el botón de decremento
void IRAM_ATTR decrementarContador() {
  contadorBotones = (contadorBotones - 1 + 16) % 16; // Evitar valores negativos
  actualizarLEDsBotones();
}

// Interrupción del temporizador
void IRAM_ATTR enTemporizador() {
  contadorTemporizador = (contadorTemporizador + 1) % 16;
  actualizarLEDsTemporizador();
  if (contadorTemporizador == contadorBotones) {
    digitalWrite(INDICADOR, !digitalRead(INDICADOR)); // Cambiar el estado del pin
    contadorTemporizador = 0; // Reiniciar el contador del temporizador
  }
}

// Interrupción para el botón capacitivo
void IRAM_ATTR reiniciarTemporizador() {
  contadorTemporizador = 0;
  timerAlarmWrite(temporizador, 250000, true); // Reiniciar el temporizador
}

void setup() {
  pinMode(BOTON_INCREMENTO, INPUT_PULLUP);
  pinMode(BOTON_DECREMENTO, INPUT_PULLUP);
  pinMode(LED1_BOTON, OUTPUT);
  pinMode(LED2_BOTON, OUTPUT);
  pinMode(LED3_BOTON, OUTPUT);
  pinMode(LED4_BOTON, OUTPUT);
  pinMode(LED1_TIMER, OUTPUT);
  pinMode(LED2_TIMER, OUTPUT);
  pinMode(LED3_TIMER, OUTPUT);
  pinMode(LED4_TIMER, OUTPUT);
  pinMode(INDICADOR, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BOTON_INCREMENTO), incrementarContador, FALLING);
  attachInterrupt(digitalPinToInterrupt(BOTON_DECREMENTO), decrementarContador, FALLING);
  touchAttachInterrupt(BOTON_REINICIO, reiniciarTemporizador, 40); // El umbral se puede ajustar según sea necesario

  temporizador = timerBegin(0, 80, true); // 80 prescaler, incrementa cada 1us
  timerAttachInterrupt(temporizador, &enTemporizador, true);
  timerAlarmWrite(temporizador, 250000, true); // 250ms
  timerAlarmEnable(temporizador);

  actualizarLEDsBotones();
  actualizarLEDsTemporizador();
}

void loop() {
  // No se necesita hacer nada aquí
}
