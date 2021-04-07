#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 32, &Wire, 4);
int idle = 0;

void setup() {
	Serial.begin(9600);
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
		Serial.println(F("Init failed"));
		for(;;);
	}

	loop();
}

void loop() {
	int tick;
	String data;
	if (Serial.available()) {
		idle = 0;
		tick = Serial.read();
		if (Serial.available()) {
			data = Serial.readStringUntil('\r');
			disp_up(data);
		}
		line_up(tick);
	} else {
		idle++;
		if (idle > 10) {
			display.clearDisplay();
			display.display();
			delay(5000);
		}
	}
	delay(400);
}

void line_up(int tick) {
	int i, col = tick / 15, row = (tick % 15) * 2;

	for (i = 0; i < col; i++) display.fillRect(115 + i * 3, 0, 2, 30, SSD1306_WHITE);
	display.fillRect(115 + col * 3, 0, 2, row, SSD1306_WHITE);
	display.display();
}

void disp_up(String data) {
	display.clearDisplay();

	display.setTextSize(2);
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(0, 0);
	display.cp437(true);

	display.println(data);
}

