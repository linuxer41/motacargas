// /**
//  * \file main.cpp
//  * \brief Archivo principal para el control de un montacargas con 3 motores, 6 pulsadores pull-down,
//  *        finales de carrera, sensor de carga HX711 y display LCD.
//  * \author Francisco Ochoa. Ing. Electrónico - @franciscoochoa
//  */

// #include <Arduino.h>
// #include "drv8833.hpp"
// #include <HX711.h> // Biblioteca para el sensor de carga HX711
// #include <LiquidCrystal_I2C.h> // Biblioteca para el display LCD I2C

// // Definición de pines para los motores
// #define MOTOR_X_IN1 2
// #define MOTOR_X_IN2 4
// #define MOTOR_Y_IN1 5
// #define MOTOR_Y_IN2 18
// #define MOTOR_Z_IN1 19
// #define MOTOR_Z_IN2 23

// // Definición de pines para los pulsadores pull-down
// #define BTN_X_FORWARD 13
// #define BTN_X_BACKWARD 12
// #define BTN_Y_FORWARD 14
// #define BTN_Y_BACKWARD 27
// #define BTN_Z_FORWARD 26
// #define BTN_Z_BACKWARD 25

// // Definición de pines para los finales de carrera pull-down
// #define LIMIT_X_FORWARD 33
// #define LIMIT_X_BACKWARD 32
// #define LIMIT_Y_FORWARD 35
// #define LIMIT_Y_BACKWARD 34
// boolean limit_z_forward = false;
// boolean limit_z_backward = false;

// // Definición de pines para el sensor HX711
// #define HX711_DT 16
// #define HX711_SCK 17

// // Definición de pin para el potenciómetro (ajuste de velocidad)
// #define POTENTIOMETER_PIN 15 // Pin analógico para el potenciómetro

// // Canales PWM para cada motor
// #define CH_PWM_X 0
// #define CH_PWM_Y 1
// #define CH_PWM_Z 2

// // Frecuencia y resolución del PWM
// #define FREQ_PWM 20000
// #define RES_PWM 8

// // Instancias de los motores
// Motor motorX;
// Motor motorY;
// Motor motorZ;

// // Instancia del sensor HX711
// HX711 scale;

// // Instancia del display LCD I2C
// LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C del LCD y tamaño (16x2)

// // Variable para almacenar la velocidad máxima ajustada
// uint8_t maxSpeed = 0;

// // Variables de estado para los motores
// bool motorXMoving = false;
// bool motorYMoving = false;
// bool motorZMoving = false;

// // Variables para la lectura asíncrona del sensor de peso
// unsigned long lastWeightReadTime = 0;
// const unsigned long weightReadInterval = 100; // Intervalo de lectura del sensor de peso en milisegundos

// // Función para arrancar el motor con un impulso inicial y dirección
// void startMotor(Motor &motor, uint8_t targetSpeed, bool forward, bool &motorMoving) {
//     if (!motorMoving) { // Solo aplicar el impulso inicial si el motor estaba detenido
//         // Aumentar temporalmente el ciclo de trabajo para arrancar el motor
//         if (forward) {
//             motor.forward(100); // Arrancar hacia adelante a máxima velocidad
//         } else {
//             motor.reverse(100); // Arrancar hacia atrás a máxima velocidad
//         }
//         delay(10); // Esperar un momento para que el motor arranque
//         motorMoving = true; // Indicar que el motor está en movimiento
//     }

//     // Reducir a la velocidad deseada
//     if (forward) {
//         motor.forward(targetSpeed); // Ajustar velocidad hacia adelante
//     } else {
//         motor.reverse(targetSpeed); // Ajustar velocidad hacia atrás
//     }
// }

// // Función para detener el motor y actualizar el estado
// void stopMotor(Motor &motor, bool &motorMoving) {
//     motor.stop();
//     motorMoving = false; // Indicar que el motor está detenido
// }

// // Función para leer los pulsadores pull-down
// void readButtons() {
//     // Leer pulsadores del eje X
//     if (digitalRead(BTN_X_FORWARD) == LOW && digitalRead(LIMIT_X_FORWARD) == LOW) {
//         startMotor(motorX, maxSpeed, true, motorXMoving); // Arrancar hacia adelante con la velocidad ajustada
//     } else if (digitalRead(BTN_X_BACKWARD) == LOW && digitalRead(LIMIT_X_BACKWARD) == LOW) {
//         startMotor(motorX, maxSpeed, false, motorXMoving); // Arrancar hacia atrás con la velocidad ajustada
//     } else {
//         stopMotor(motorX, motorXMoving);
//     }

//     // Leer pulsadores del eje Y
//     if (digitalRead(BTN_Y_FORWARD) == LOW && digitalRead(LIMIT_Y_FORWARD) == LOW) {
//         startMotor(motorY, maxSpeed, true, motorYMoving); // Arrancar hacia adelante con la velocidad ajustada
//     } else if (digitalRead(BTN_Y_BACKWARD) == LOW && digitalRead(LIMIT_Y_BACKWARD) == LOW) {
//         startMotor(motorY, maxSpeed, false, motorYMoving); // Arrancar hacia atrás con la velocidad ajustada
//     } else {
//         stopMotor(motorY, motorYMoving);
//     }

//     // Leer pulsadores del eje Z
//     if (digitalRead(BTN_Z_FORWARD) == LOW && limit_z_forward == false) {
//         startMotor(motorZ, maxSpeed, true, motorZMoving); // Arrancar hacia adelante con la velocidad ajustada
//     } else if (digitalRead(BTN_Z_BACKWARD) == LOW && limit_z_backward == false) {
//         startMotor(motorZ, maxSpeed, false, motorZMoving); // Arrancar hacia atrás con la velocidad ajustada
//     } else {
//         stopMotor(motorZ, motorZMoving);
//     }
// }

// // Función para leer el peso del sensor HX711 de manera asíncrona
// void readWeightAsync() {
//     static float weight = 0.0f; // Variable estática para almacenar el peso
//     if (millis() - lastWeightReadTime >= weightReadInterval) {
//         lastWeightReadTime = millis(); // Actualizar el tiempo de la última lectura
//         weight = scale.get_units(10); // Promedio de 10 lecturas
//     }
//     lcd.setCursor(6, 0);
//     lcd.print(weight / 1000, 2); // Mostrar peso con 2 decimales
//     lcd.print(" kg  "); // Limpiar residuos de texto anterior

//     // Mostrar el peso en la consola serial
//     Serial.print("Peso: ");
//     Serial.print(weight / 1000, 2);
//     Serial.print(" kg | Velocidad: ");
//     Serial.print(maxSpeed);
//     Serial.println(" %");
// }

// // Función para ajustar la velocidad máxima desde el potenciómetro
// void adjustMaxSpeed() {
//     int potValue = analogRead(POTENTIOMETER_PIN); // Leer el valor del potenciómetro (0-4095)
//     maxSpeed = map(potValue, 0, 4095, 0, 100);    // Mapear el valor a un rango de 0-100
// }

// void setup() {
//     // Inicialización de la comunicación serial
//     Serial.begin(115200);

//     // Configuración de los motores
//     motorX.setup(MOTOR_X_IN1, MOTOR_X_IN2, CH_PWM_X, FREQ_PWM, RES_PWM);
//     motorY.setup(MOTOR_Y_IN1, MOTOR_Y_IN2, CH_PWM_Y, FREQ_PWM, RES_PWM);
//     motorZ.setup(MOTOR_Z_IN1, MOTOR_Z_IN2, CH_PWM_Z, FREQ_PWM, RES_PWM);

//     // Configuración de los pulsadores pull-down
//     pinMode(BTN_X_FORWARD, INPUT_PULLUP);
//     pinMode(BTN_X_BACKWARD, INPUT_PULLUP);
//     pinMode(BTN_Y_FORWARD, INPUT_PULLUP);
//     pinMode(BTN_Y_BACKWARD, INPUT_PULLUP);
//     pinMode(BTN_Z_FORWARD, INPUT_PULLUP);
//     pinMode(BTN_Z_BACKWARD, INPUT_PULLUP);

//     // Configuración de los finales de carrera pull-down
//     pinMode(LIMIT_X_FORWARD, INPUT_PULLDOWN);
//     pinMode(LIMIT_X_BACKWARD, INPUT_PULLDOWN);
//     pinMode(LIMIT_Y_FORWARD, INPUT_PULLDOWN);
//     pinMode(LIMIT_Y_BACKWARD, INPUT_PULLDOWN);

//     // Configuración del potenciómetro
//     pinMode(POTENTIOMETER_PIN, INPUT);

//     // Configuración del sensor HX711
//     scale.begin(HX711_DT, HX711_SCK);
//     scale.set_scale(43371.f / 105.f); // Factor de calibración del sensor (ajustar según tu sensor)
//     scale.tare(); // Resetear la balanza a 0

//     // Configuración del display LCD
//     lcd.init();
//     lcd.backlight();
//     lcd.setCursor(0, 0);
//     lcd.print("Peso: ");
// }

// void loop() {
//     // Ajustar la velocidad máxima desde el potenciómetro
//     adjustMaxSpeed();

//     // Leer los pulsadores y actualizar los motores
//     readButtons();

//     // Leer el peso del sensor HX711 de manera asíncrona
//     readWeightAsync();

//     // Mostrar la velocidad máxima ajustada en el display LCD
//     lcd.setCursor(0, 1);
//     lcd.print("Vel: ");
//     lcd.print(maxSpeed);
//     lcd.print(" %   ");

//     // Pequeña pausa para evitar sobrecarga del procesador
//     delay(10);
// }