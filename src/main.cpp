#include <Arduino.h>

// === 写法一：轮询 ===
#if EXAMPLE == 1

#define LED_PIN    5
#define BUTTON_PIN 0

bool ledState = false;
bool lastButtonState = HIGH;

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    digitalWrite(LED_PIN, LOW);
    Serial.println("=== Example 1: Polling ===");
}

void loop() {
    bool currentState = digitalRead(BUTTON_PIN);
    if (lastButtonState == HIGH && currentState == LOW) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        Serial.printf("LED: %s\n", ledState ? "ON" : "OFF");
    }
    lastButtonState = currentState;
    delay(10);
}

// === 写法二：中断 ===
#elif EXAMPLE == 2

#define LED_PIN    5
#define BUTTON_PIN 0

volatile bool ledState = false;
volatile unsigned long lastInterruptTime = 0;
volatile bool buttonPressed = false;

void IRAM_ATTR buttonISR() {
    unsigned long now = millis();
    if (now - lastInterruptTime > 150) {
        buttonPressed = true;
        lastInterruptTime = now;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
    Serial.println("=== Example 2: Interrupt ===");
}

void loop() {
    if (buttonPressed) {
        buttonPressed = false;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        Serial.printf("LED: %s\n", ledState ? "ON" : "OFF");
    }
}

// === 写法三：中断 + FreeRTOS Queue ===
#elif EXAMPLE == 3

#define LED_PIN    5
#define BUTTON_PIN 0

QueueHandle_t buttonQueue;
TaskHandle_t ledTaskHandle;

volatile unsigned long lastInterruptTime = 0;

void IRAM_ATTR buttonISR() {
    unsigned long now = millis();
    if (now - lastInterruptTime > 150) {
        int event = 1;
        xQueueSendFromISR(buttonQueue, &event, NULL);
        lastInterruptTime = now;
    }
}

void ledTask(void *pvParameters) {
    int event;
    bool ledState = false;
    while (1) {
        if (xQueueReceive(buttonQueue, &event, portMAX_DELAY) == pdTRUE) {
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState ? HIGH : LOW);
            Serial.printf("LED: %s\n", ledState ? "ON" : "OFF");
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    buttonQueue = xQueueCreate(5, sizeof(int));
    xTaskCreate(ledTask, "LED_Task", 2048, NULL, 1, &ledTaskHandle);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
    Serial.println("=== Example 3: Interrupt + FreeRTOS Queue ===");
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}

#endif
