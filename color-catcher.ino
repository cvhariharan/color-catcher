#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN 2

#define NUMPIXELS 1

#define PIN_S2 0
#define PIN_S3 1

#define PIN_OUT 3
#define SWITCH_PIN 4

#define RED_MIN 4
#define GREEN_MIN 4
#define BLUE_MIN 3

#define SAMPLES 10

unsigned int RED_MAX = 40;
unsigned int GREEN_MAX = 45;
unsigned int BLUE_MAX = 35;

int definedColors[][3] = {{255, 0, 0}, {255, 128, 0}, {255, 255, 0}, {128, 255, 0}, {0, 255, 0}, {0, 255,128}, 
                          {0, 255, 255}, {0, 128, 255}, {0, 0, 255}, {128, 0, 255}, {255, 0, 255}, {255, 0, 128}, 
                          {255, 255, 255}};

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

struct ColorValue {
  int red;
  int green;
  int blue;
};

ColorValue col;

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pinMode(PIN_S2, OUTPUT);
  pinMode(PIN_S3, OUTPUT);
  pinMode(PIN_OUT, INPUT);
  pinMode(SWITCH_PIN, INPUT);

  col = ColorValue {red: 255, green: 255, blue: 255};

  pixels.begin();
  Serial.begin(9600);

  calibrate();
}

void loop() {
  pixels.clear();
  int r, g, b;

  if(digitalRead(SWITCH_PIN) == HIGH) {
    r = map(read_red(), RED_MAX, RED_MIN, 0, 255);
    g = map(read_green(), GREEN_MAX, GREEN_MIN, 0, 255);
    b = map(read_blue(), BLUE_MAX, BLUE_MIN, 0, 255);
    delay(100);
    col = find_closest_color(r, g, b);
  }

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(col.red, col.green, col.blue));
    pixels.show();
  }
}


int read_red() {
  digitalWrite(PIN_S2, LOW);
  digitalWrite(PIN_S3, LOW);
  int pulse_width = pulseIn(PIN_OUT, LOW);
  return pulse_width;
}

int read_green() {
  digitalWrite(PIN_S2, HIGH);
  digitalWrite(PIN_S3, HIGH);
  int pulse_width = pulseIn(PIN_OUT, LOW);
  return pulse_width;
}

int read_blue() {
  digitalWrite(PIN_S2, LOW);
  digitalWrite(PIN_S3, HIGH);
  int pulse_width = pulseIn(PIN_OUT, LOW);
  return pulse_width;
}

ColorValue find_closest_color(int r, int g, int b) {
  int minDiff = 765;
  ColorValue col;
  size_t arrLen = sizeof(definedColors)/(3 * sizeof(int));
  for(int i = 0; i < arrLen; i++) {
    int dr = abs(r - definedColors[i][0]);
    int dg = abs(g - definedColors[i][1]);
    int db = abs(b - definedColors[i][2]);

    int totalDiff = dr + dg + db;
    if(totalDiff < minDiff) {
      minDiff = totalDiff;
      col.red = definedColors[i][0];
      col.green = definedColors[i][1];
      col.blue = definedColors[i][2];
    }
  }

  return col;
}

void calibrate() {
  int r = 0, g = 0, b = 0;
  for(int i = 0; i < SAMPLES; i++) {
    r = read_red();
    g = read_green();
    b = read_blue();
    
    RED_MAX = max(RED_MAX, r);
    GREEN_MAX = max(GREEN_MAX, g);
    BLUE_MAX = max(BLUE_MAX, b);
    
    pixels.setPixelColor(i, pixels.Color(252, 140, 42));
    pixels.show();
    delay(100);
  }
}
