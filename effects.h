//   Graphical effects to run on the RGB Shades LED array
//   Each function should have the following components:
//    * Must be declared void with no parameters or will break function pointer array
//    * Check effectInit, if false then init any required settings and set effectInit true
//    * Set effectDelay (the time in milliseconds until the next run of this effect)
//    * All animation should be controlled with counters and effectDelay, no delay() or loops
//    * Pixel data should be written using leds[XY(x,y)] to map coordinates to the RGB Shades layout

// Triple Sine Waves
void threeSine() {

  static byte sineOffset = 0; // counter for current position of sine waves

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 20;
  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {

      // Calculate "sine" waves with varying periods
      // sin8 is used for speed; cos8, quadwave8, or triwave8 would also work here
      byte sinDistanceR = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 9 + x * 16)), 2);
      byte sinDistanceG = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 10 + x * 16)), 2);
      byte sinDistanceB = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 11 + x * 16)), 2);

      leds[XY(x, y)] = CRGB(255 - sinDistanceR, 255 - sinDistanceG, 255 - sinDistanceB);
    }
  }

  sineOffset++; // byte will wrap from 255 to 0, matching sin8 0-255 cycle

}


// RGB Plasma
void plasma() {

  static byte offset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = cos8(plasVector / 256);
  int yOffset = sin8(plasVector / 256);

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x - 7.5) * 10 + xOffset - 127) + sq(((float)y - 2) * 10 + yOffset - 127)) + offset);
      leds[XY(x, y)] = CHSV(color, 255, 255);
    }
  }

  offset++; // wraps at 255 for sin8
  plasVector += 16; // using an int for slower orbit (wraps at 65536)

}


// Scanning pattern left/right, uses global hue cycle
void rider() {

  static byte riderPos = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    riderPos = 0;
  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    int brightness = abs(x * (256 / kMatrixWidth) - triwave8(riderPos) * 2 + 127) * 3;
    if (brightness > 255) brightness = 255;
    brightness = 255 - brightness;
    CRGB riderColor = CHSV(cycleHue, 255, brightness);
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = riderColor;
    }
  }

  riderPos++; // byte wraps to 0 at 255, triwave8 is also 0-255 periodic

}


// Shimmering noise, uses global hue cycle
void glitter() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 15;
  }

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = CHSV(cycleHue, 255, random8(5) * 63);
    }
  }

}


// Fills saturated colors into the array from alternating directions
void colorFill() {

  static byte currentColor = 0;
  static byte currentRow = 0;
  static byte currentDirection = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 45;
    currentColor = 0;
    currentRow = 0;
    currentDirection = 0;
    currentPalette = RainbowColors_p;
  }

  // test a bitmask to fill up or down when currentDirection is 0 or 2 (0b00 or 0b10)
  if (!(currentDirection & 1)) {
    effectDelay = 45; // slower since vertical has fewer pixels
    for (byte x = 0; x < kMatrixWidth; x++) {
      byte y = currentRow;
      if (currentDirection == 2) y = kMatrixHeight - 1 - currentRow;
      leds[XY(x, y)] = currentPalette[currentColor];
    }
  }

  // test a bitmask to fill left or right when currentDirection is 1 or 3 (0b01 or 0b11)
  if (currentDirection & 1) {
    effectDelay = 20; // faster since horizontal has more pixels
    for (byte y = 0; y < kMatrixHeight; y++) {
      byte x = currentRow;
      if (currentDirection == 3) x = kMatrixWidth - 1 - currentRow;
      leds[XY(x, y)] = currentPalette[currentColor];
    }
  }

  currentRow++;

  // detect when a fill is complete, change color and direction
  if ((!(currentDirection & 1) && currentRow >= kMatrixHeight) || ((currentDirection & 1) && currentRow >= kMatrixWidth)) {
    currentRow = 0;
    currentColor += random8(3, 6);
    if (currentColor > 15) currentColor -= 16;
    currentDirection++;
    if (currentDirection > 3) currentDirection = 0;
    effectDelay = 300; // wait a little bit longer after completing a fill
  }


}

// Emulate 3D anaglyph glasses
void threeDee() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 50;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      if (x < 7) {
        leds[XY(x, y)] = CRGB::Blue;
      } else if (x > 8) {
        leds[XY(x, y)] = CRGB::Red;
      } else {
        leds[XY(x, y)] = CRGB::Black;
      }
    }
  }

  leds[XY(6, 0)] = CRGB::Black;
  leds[XY(9, 0)] = CRGB::Black;

}

// Random pixels scroll sideways, uses current hue
#define rainDir 0
void sideRain() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 30;
  }

  scrollArray(rainDir);
  byte randPixel = random8(kMatrixHeight);
  for (byte y = 0; y < kMatrixHeight; y++) leds[XY((kMatrixWidth - 1) * rainDir, y)] = CRGB::Black;
  leds[XY((kMatrixWidth - 1)*rainDir, randPixel)] = CHSV(cycleHue, 255, 255);

}

// Pixels with random locations and random colors selected from a palette
// Use with the fadeAll function to allow old pixels to decay
void confetti() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomPalette();
    fadeActive = 2;
    fadeBaseColor = CRGB::Black;
  }

  // scatter random colored pixels at several random coordinates
  for (byte i = 0; i < 4; i++) {
    leds[XY(random16(kMatrixWidth), random16(kMatrixHeight))] = ColorFromPalette(currentPalette, random16(255), 255); //CHSV(random16(255), 255, 255);
    random16_add_entropy(1);
  }

}


// Draw slanting bars scrolling across the array, uses current hue
void slantBars() {

  static byte slantPos = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = CHSV(cycleHue, 255, triwave8(x * 32 + y * 32 + slantPos));
    }
  }

  slantPos -= 4;

}




#define charSpacing 2
// Scroll a text string
void scrollText(byte message, byte style, CRGB fgColor, CRGB bgColor, byte repeats) {
  static byte currentMessageChar = 0;
  static byte currentCharColumn = 0;
  static byte paletteCycle = 0;
  static CRGB currentColor;
  static byte currentWordCount = 0;
  static byte currentChar;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 35;
    currentMessageChar = 0;
    currentCharColumn = 0;
    selectFlashString(message);
    repCount = repeats;
    currentChar = loadStringChar(message, currentMessageChar);
    loadCharBuffer(currentChar);
    if (style == RAINBOW) {
      currentPalette = RainbowColors_p;
    } else if (style == PALETTEWORDS) {
      currentPalette = RainbowColors_p;
    }
    paletteCycle = 0;
    if (style == NORMAL) {
      currentColor = fgColor;
    } else if (style == PALETTEWORDS) {
      currentColor = ColorFromPalette(currentPalette, paletteCycle, 255);
    } else if (style == CANDYCANE || style == HOLLY || style == HOLLY2) {
      currentColor = colorCycle(style);
    }

    fillAll(CRGB::Black);
  }

  CRGB pixelColor;

  scrollArray(1);
  if (style == RAINBOW) paletteCycle +=10;
  
    for (byte y = 0; y < 5; y++) { // characters are 5 pixels tall
      if ((bitRead(charBuffer[currentCharColumn], y) == 1) && currentCharColumn < 5) {
        if (style == RAINBOW) {
          pixelColor = ColorFromPalette(currentPalette, paletteCycle+y*16, 255);
        } else {
          pixelColor = currentColor;
        }
      } else {
        pixelColor = bgColor;
      }
      leds[XY(kMatrixWidth-1, y)] = pixelColor;
    }

  currentCharColumn++;
  if (currentCharColumn > (4 + charSpacing)) {
    currentCharColumn = 0;
    currentMessageChar++;
    char nextChar = loadStringChar(message, currentMessageChar);
    if (nextChar == 0) { // null character at end of string
      currentMessageChar = 0;
      if (repCount > 0) repCount--;
      if (repCount == 0) cyclePattern();
      nextChar = loadStringChar(message, currentMessageChar);
    }
    
    
    if (currentChar == ' ' && nextChar != ' ') {
      if (style == PALETTEWORDS) {
        paletteCycle += 15;
        currentColor = ColorFromPalette(currentPalette, paletteCycle*15, 255);
      } else if (style == CANDYCANE || style == HOLLY) {
        currentColor = colorCycle(style);
      }
    }

    if (currentChar != ' ') {
      if (style == HOLLY2) currentColor = colorCycle(HOLLY);
    }
    
    
    loadCharBuffer(nextChar);
    currentChar = nextChar;
  }


}

//leds run around the periphery of the shades, changing color every go 'round
void shadesOutline() {
  
  static uint8_t x = 0;
  
  //startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 25;
    FastLED.clear();
    currentPalette = RainbowColors_p;
    fadeActive = 0;
  }

  CRGB pixelColor = CHSV(cycleHue, 255, 255);
  leds[OutlineMap(x)] = pixelColor;

  x++;
  if (x > (OUTLINESIZE-1)) x = 0;
  
}



// RotatingPlasma
void spinPlasma() {

  static byte offset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomPalette();
    fadeActive = 0;
  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = (cos8(plasVector)-127)/2;
  int yOffset = (sin8(plasVector)-127)/2;

  //int xOffset = 0;
  //int yOffset = 0;


  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x - 7.5) * 12 + xOffset) + sq(((float)y - 2) * 12 + yOffset)) + offset);
      leds[XY(x, y)] = ColorFromPalette(currentPalette, color, 255);
    }
  }

  offset++; // wraps at 255 for sin8
  plasVector += 1; // using an int for slower orbit (wraps at 65536)

}

// Display bursts of sparks
void fireworks() {

  byte sparksDone = 0;
  static int sparkLife = 50;
  static boolean boom = false;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    gSkyburst = 1;
    fadeActive = 0;
  }


  if (boom) {
    FastLED.clear();
    boom = false;
  } else {
    fadeAll(40);
  }
  
  if (sparkLife > 0) sparkLife--;

  
  for( byte b = 0; b < NUM_SPARKS; b++) {
    if (sparkLife <= 0) gSparks[b].show = 0;
    gSparks[b].Move();
    gSparks[b].Draw();
    sparksDone += gSparks[b].show;
  }

  if (sparksDone == 0) gSkyburst = 1;
  //Serial.println(sparksDone);

  if( gSkyburst) {
    effectDelay = 5;
    sparkLife = random(16,150);
    CRGB color;
    hsv2rgb_rainbow( CHSV( random8(), 255, 255), color);
    accum88 sx = random(127-64,127+64)<<8;
    accum88 sy = random(127-16,127+16)<<8;
    for( byte b = 0; b < NUM_SPARKS; b++) {
      gSparks[b].Skyburst(sx, sy, 0, color);
    }
    gSkyburst = 0;
    sparksDone = 0;
    fillAll(CRGB::Gray);
    boom = true;
  }
  
}
void scrollTextZero() {
  scrollText(0, NORMAL, CRGB::Crimson, CRGB::Black, 3);
}

void scrollTextOne() {
  scrollText(1, NORMAL, CRGB::CRGB(255,64,54), CRGB::Black, 3);
}

void scrollTextTwo() {
  scrollText(2, NORMAL, CRGB::Green, CRGB(0, 0, 8), 3);
}

CRGB heart_mask(uint16_t x, uint16_t y, CRGB fg, CRGB bg) {
  CRGB color = fg;
  if (x < 7) color = blend(bg,fg,heart[y][x]);
  else if (x > 8) color = blend(bg,fg,heart[y][x-9]);
  else color = bg;
  return color;
}

void plasma_heart(void) {
  static byte offset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center
    
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomPalette();
    offset = 0;
  }

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = inoise8(x*64,y*64,plasVector);
      color = qsub8(color,16);
      color = qadd8(color,scale8(color,39));
      leds[XY(x, y)] = heart_mask(x, y, ColorFromPalette(currentPalette, color, 255, 255), CRGB::Black);
    }
  }

  offset+=1; // wraps at 255 for sin8
  plasVector += 8; // using an int for slower orbit (wraps at 65536)

}

void confetti_heart(void) {
  
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomPalette();
  }

  // scatter random colored pixels at several random coordinates
  CRGB color;
  for (byte i = 0; i < 4; i++) {
    uint16_t x = random16(kMatrixWidth);
    uint16_t y = random16(kMatrixHeight);
    color = heart_mask(x, y, ColorFromPalette(currentPalette, random16(255), 255), CRGB::Black);
    leds[XY(x,y)] = color; 
    random16_add_entropy(1);
  }
}

void strobe_heart(void) {
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 30;
    selectRandomPalette();
  }

  byte color = random8();

  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = heart_mask(x, y, ColorFromPalette(currentPalette, color, 255, 255), ColorFromPalette(currentPalette, color+127, 255, 255));
    }
  }

}

class Slider {
  private:
    uint8_t length;
    int velocity;
    CRGB color;
    uint8_t y;
    int16_t x;
  public:
    Slider();
    init(void);
    render(void);
};


Slider::init(void) {
  length = random8(8) + 2;
  velocity = random8(2);
  if (velocity == 0) velocity = -1;
  else velocity = 1;
  if (velocity == -1) x = 16;
  else x = 0 - length;
  y = random8(5);
  color = ColorFromPalette(currentPalette, random8(255), 255); 
}

Slider::Slider(void) {
  init();
}

Slider::render(void) {
  x = x + velocity;
  int16_t start_x = x;
  int16_t end_x = x + length;

  if ((velocity == -1 && end_x < -5) || (velocity == 1 && start_x > 20)) {
    init();
    return;
  }
  
  if (start_x < 0) start_x = 0;
  if (end_x > (kMatrixWidth-1)) end_x = kMatrixWidth - 1;

  for (int i = start_x; i <= end_x; i++) {
    leds[XY(i,y)] = color;
  }
  
}

Slider sliders[16];

void slide() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 50;
    selectRandomPalette();
    for (int i = 0; i < 16; i++) {
      sliders[i].init();
    }
  }

  
  fillAll(0);
  for (int i = 0; i < 16; i++) {
    sliders[i].render();
  }


}

void filledWave() {

  static byte sineOffset = 0; // counter for current position of sine waves

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 20;
    selectRandomPalette();
  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      int16_t brightness = (y+1) * (255 / kMatrixHeight);
      brightness -= sin8(sineOffset * 3 + x * 7);
      brightness *=2;
      if (brightness > 255) brightness = 255;
      if (brightness < 0) brightness = 0;

      leds[XY(x, y)] = CRGB(brightness,0,0);
    }
  }

  sineOffset++; // byte will wrap from 255 to 0, matching sin8 0-255 cycle
}
