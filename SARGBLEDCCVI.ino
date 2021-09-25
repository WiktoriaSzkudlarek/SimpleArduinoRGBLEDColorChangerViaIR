#include <IRremote.h>

const int IR_RECEIVER_PIN = A5;

const int RED_LIGHT_PIN = 5;
const int GREEN_LIGHT_PIN = 6;
const int BLUE_LIGHT_PIN = 9;

const int BRIGHTNESS = 30;

int color = 0;
int red_value, green_value, blue_value;
int ir_code = -1;

decode_results results;
IRrecv irrecv(IR_RECEIVER_PIN);

void setup() {
  Serial.begin(9600);

  randomSeed(analogRead(A5));

  irrecv.enableIRIn(); // enable ir receiver module

  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(GREEN_LIGHT_PIN, OUTPUT);
  pinMode(BLUE_LIGHT_PIN, OUTPUT);
  change_to_basic_colors();
}

void loop() {
  ir_code = check_IR();
  if (ir_code != -1) {
    Serial.print("||");
    Serial.println(ir_code, HEX);
    switch (ir_code) {
      case 0xFF906F: // BRIGHTNESS UP - It increases all colors value by BRIGHTNESS
        brightness_up();
        break;

      case 0xFFB847: // BRIGHTNESS DOWN - It decreases all colors value by BRIGHTNESS
        brightness_down();
        break;

      case 0xFFF807:  // OFF - It turn off all LEDs
        RGB_color(0, 0, 0);
        break;

      case 0xBFF04F7: case 0xB04F: case 0x1643: // ON - It changes the "basic" color by pressing its button
        color++;
        change_to_basic_colors();
        break;

      case 0xFF9867: case 0x3BFB: // RED
        RGB_color(255, 0, 0);
        break;

      case 0xFFD827: case 0xE697: // GREEN
        RGB_color(0, 255, 0);
        break;

      case 0xFF8877: case 0x941F: // BLUE
        RGB_color(0, 0, 255);
        break;

      case 0xFFA857: case 0xFEDDB: // WHITE
        RGB_color(255, 255, 255);
        break;

      case 0xFFE817: case 0x5E7F: // rgb(255, 127, 0)
        RGB_color(255, 127, 0);
        break;

      case 0xFF48B7: case 0xC5B7: // rgb(0, 170, 255)
        RGB_color(0, 170, 255);
        break;

      case 0xFF6897: case 0xE57B: // rgb(0, 0, 128)
        RGB_color(0, 0, 128);
        break;

      case 0xFFB24D: case 0x1EF7:  // FLASH - It generates random values of color and plays endless faded animation
        flash();
        break;

      case 0xFF02FD: case 0x4B1B:  // rgb(255, 170, 0)
        RGB_color(255, 170, 0);
        break;

      case 0xFF32CD: case 0xCCFB:  // rgb(0, 255, 170)
        RGB_color(0, 255, 170);
        break;

      case 0xFF20DF: case 0x3D1B: // rgb(63, 0, 128)
        RGB_color(63, 0, 128);
        break;

      case 0xFF00FF: case 0x159F: // STROBE - The "basic" color changes after few seconds (doesn't impact on the "color" variable)
        strobe();
        break;

      case 0xFF50AF: case 0x195F: // rgb(255, 212, 0)
        RGB_color(255, 212, 0);
        break;

      case 0xFF7887: case 0x8657: // rgb(0, 255, 255)
        RGB_color(0, 255, 255);
        break;

      case 0xFF708F: case 0x7DB:  // rgb(122, 0, 191)
        RGB_color(122, 0, 191);
        break;

      case 0xFF58A7: case 0x97DB: // FADE - In this case it works like FLASH, but new values of colors are generated after end of the animation
        fade();
        break;

      case 0xFF38C7: case 0x3CBB: // rgb(255, 255, 0)
        RGB_color(255, 255, 0);
        break;

      case 0xFF28D7: case 0x9A18: // rgb(0, 85, 255)
        RGB_color(0, 85, 255);
        break;

      case 0xFFF00F: case 0x425F: // rgb(255, 0, 255)
        RGB_color(255, 0, 255);
        break;

      case 0xFF30CF: case 0xBE3F:  //SMOOTH - It works like STROBE and FADE combined together
        smooth();
        break;
    }
  }
  irrecv.enableIRIn();

  delay(300);
}

int check_IR() {
  if (irrecv.decode(&results)) {
    int ir_value = results.value;
    irrecv.resume();   
    return ir_value;
  }
  else
    return -1;
}


void fade() {
  int rand_val = random(255);

  while (1) {
    rand_val = random(255);
    if ( smoothfade(1, 1, 1 , rand_val) ) break;
  }
  irrecv.enableIRIn();
}

void smooth() {
  while (1) {
    // r
    if ( smoothfade(255, 0, 0, -1) ) break;

    // g
    if ( smoothfade(0, 255, 0, -1) ) break;

    // b
    if ( smoothfade(0, 0, 255, -1) ) break;

    // rg
    if ( smoothfade(255, 255, 0, -1) ) break;

    // rb
    if ( smoothfade(255, 0, 255, -1) ) break;

    // gb
    if ( smoothfade(0, 255, 255, -1) ) break;

    // rgb
    if ( smoothfade(255, 255, 255, -1) ) break;
  }
  irrecv.enableIRIn();
}

void strobe() {
  while (1) {
    // r
    if ( smoothfade(255, 0, 0, -2) ) break;

    // g
    if ( smoothfade(0, 255, 0, -2) ) break;

    // b
    if ( smoothfade(0, 0, 255, -2) ) break;

    // rg
    if ( smoothfade(255, 255, 0, -2) ) break;

    // rb
    if ( smoothfade(255, 0, 255, -2) ) break;

    // gb
    if ( smoothfade(0, 255, 255, -2) ) break;

    // rgb
    if ( smoothfade(255, 255, 255, -2) ) break;
  }
  irrecv.enableIRIn();
}

void flash() {
  int rand_val = random(255);

  while (1) {
    if ( smoothfade(1, 1, 1 , rand_val) ) break;
  }
  irrecv.enableIRIn();
}

int smoothfade(int r, int g, int b, int mode) {
  int e = 0;

  for (e = 0; e < 255; e++) {
    if (check_IR() != -1) {
      irrecv.enableIRIn();
      return 1;
    } else {
      if (mode < 0) {
        (mode == -1) ? RGB_color(r ? e : 0, g ? e : 0, b ? e : 0) : RGB_color(r, g, b);
      } else
        RGB_color(mode, e, 255 - e);
      delay(10);
    }
  }

  for (e = 255; e > 0; e--) {
    if (check_IR() != -1) {
      irrecv.enableIRIn();
      return 1;
    } else {
      if (mode < 0) {
        (mode == -1) ? RGB_color(r?e:0, g?e:0, b?e:0) : RGB_color(r, g, b);
      } else
        RGB_color(mode, e, 255 - e);
      delay(10);
    }
  }
  return 0;
}

void brightness_up() {
  (red_value > -1) ? (red_value += BRIGHTNESS) : 0;
  (green_value > -1) ? (green_value += BRIGHTNESS) : 0;
  (blue_value > -1) ? (blue_value += BRIGHTNESS) : 0;

  if (red_value > 255) red_value = 255;
  if (green_value > 255) green_value = 255;
  if (blue_value > 255) blue_value = 255;

  RGB_color(red_value, green_value, blue_value);
}

void brightness_down() {
  (red_value < 256) ? (red_value -= BRIGHTNESS) : 255;
  (green_value < 256) ? (green_value -= BRIGHTNESS) : 255;
  (blue_value < 256) ? (blue_value -= BRIGHTNESS) : 255;

  if (red_value < 0) red_value = 0;
  if (green_value < 0) green_value = 0;
  if (blue_value < 0) blue_value = 0;
  RGB_color(red_value, green_value, blue_value);
}

void change_to_basic_colors() {
  if (color > 7)
    color = 0;
  switch (color) {
    case 0:
      RGB_color(255, 0, 0); // Red
      break;

    case 1:
      RGB_color(0, 255, 0); // Green
      break;

    case 2:
      RGB_color(0, 0, 255); // Blue
      break;

    case 3:
      RGB_color(255, 255, 128); // Raspberry
      break;

    case 4:
      RGB_color(0, 255, 255); // Cyan
      break;

    case 5:
      RGB_color(255, 0, 255); // Magenta
      break;

    case 6:
      RGB_color(255, 255, 0); // Yellow
      break;

    case 7:
      RGB_color(255, 255, 255); // White
      break;
  }
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value) {
  red_value = red_light_value;
  green_value = green_light_value;
  blue_value = blue_light_value;
  analogWrite(RED_LIGHT_PIN, red_light_value);
  analogWrite(GREEN_LIGHT_PIN, green_light_value);
  analogWrite(BLUE_LIGHT_PIN, blue_light_value);
}
