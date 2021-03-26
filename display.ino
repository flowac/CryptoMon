#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 32, &Wire, 4);

void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Init failed"));
    for(;;);
  }
  while(Serial.available()) Serial.read();

  //display.invertDisplay(true);
  loop();
}

void loop() {
  int tick;
  String data;
  if (Serial.available()) {
    tick = Serial.read();
    data = Serial.readStringUntil('\r');
    disp_up(data, tick);
  }

  delay(500);
}

void disp_up(String data, int tick) {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.cp437(true);

  display.println(data);
  if (tick < 31) {
    display.fillRect(120, 0, 3, tick, SSD1306_WHITE);
  } else{
    display.fillRect(120, 0, 3, 30, SSD1306_WHITE);
    display.fillRect(124, 0, 3, tick - 30, SSD1306_WHITE);
  }

  display.display();
}

