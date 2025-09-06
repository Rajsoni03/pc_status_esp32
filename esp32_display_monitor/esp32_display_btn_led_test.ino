#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#define LED 6
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // No reset pin

Adafruit_NeoPixel pixels(1, LED, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint8_t btn[4] = {3, 4, 2, 5};
uint8_t color[3] = {0, 0, 0};

void setup() {
  pixels.begin();
  pixels.clear();
  for (int i = 0; i < 4; i++) pinMode(btn[i], INPUT);

  Wire.begin();  // SDA, SCL default: GPIO8, GPIO9 for ESP32-C3
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  
  xTaskCreatePinnedToCore(
    updateTask,    // Task function
    "updateTask",  // Name of the task
    10000,         // Stack size in words
    NULL,          // Parameter to pass to the task
    1,             // Task priority (0 is lowest)
    NULL,          // Task handle
    0              // Core to pin the task to (0 or 1)
  );
}

void drawBar(const char* label, int y, int value) {
  display.setCursor(0, y);
  display.print(label);

  int barWidth = map(value, 0, 255, 0, 90);
  display.fillRect(10, y, barWidth, 8, SSD1306_WHITE);

  // Draw percentage value
  display.setCursor(128 - 24, y);
  display.print(value);
  display.print("%");
}

void updateData() {
  display.clearDisplay();
  drawBar("R", 0, color[0]);
  drawBar("G", 16, color[1]);
  drawBar("B", 32, color[2]);
  display.display();

  pixels.setPixelColor(0, pixels.Color(color[0], color[1], color[2]));
  pixels.show();
}


void updateTask(void *pvParameters) {
  for (;;) {
    updateData();
    delay(10);
  }
}

void loop() {
  for (int i = 0; i < 3; i++) {
    if (digitalRead(btn[i]) && color[i] < 255) {
      color[i]++;
    }
  }
  if (digitalRead(btn[3])) {
    for (int i = 0; i < 3; i++) {
      if (color[i] > 0) {
        color[i]--;
      }
    }
  }
  delay(10);
}
