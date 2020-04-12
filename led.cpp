#include "main.h"

void rainbow(int SpeedDelay, byte luminosity)
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i+1] = CHSV(hueForRainbow, 255, luminosity);
  }
  showStrip();
  delay(SpeedDelay);
}

/*
void chenillard(uint8_t index, uint8_t hue)
{
  uint8_t mean = index / NUM_LEDS;
//  uint8_t sigma = 25/120 * NUM_LEDS
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i+1] = CHSV(sinNorm(i, mean), 255, ledMode.lumi);
  }
}
*/

double gauss(int x, double mean, double sigma)
{
  double i = (x-mean)/sigma;
  i = i * i;
  return exp(-0.5*i);
}

/*
float sinNorm(int x, int mean)
{
  return (sin(2*(x-mean)/(NUM_LEDS*3.14/10)) + 1) / 2;
}
*/


void showStrip() {
   FastLED.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue)
{
#ifdef DEBUG_ON
  Serial.print("LED ");
  Serial.print(Pixel);
  Serial.print(" : ");
  Serial.print(int(red));
  Serial.print(", ");
  Serial.print(int(green));
  Serial.print(", ");
  Serial.println(int(blue));
#endif
  leds[Pixel+1] = CRGB(red, green, blue);
}

void setAll(byte red, byte green, byte blue)
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    setPixel(i, red, green, blue);
  }
  showStrip();
}


void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
  int i;
  double level;
//  for(int j = 0; j < NUM_LEDS; j++)
//    setPixel(j, 0, 0, 0);
  if(effectIndex < NUM_LEDS-EyeSize)
  {
    i = effectIndex;
  }
  else
  {
    i = (NUM_LEDS-EyeSize) - (effectIndex - (NUM_LEDS-EyeSize));
  }
  for(int j = 0; j < NUM_LEDS; j++)
  {
    level = gauss(j, i + EyeSize/2, EyeSize/4);
    setPixel(j, int(red * level), int(green * level), int(blue * level));
  }
  showStrip();
  delay(SpeedDelay);
  effectIndex = (effectIndex + 1) % (2*(NUM_LEDS-EyeSize));
  if(effectIndex == 0)
    delay(ReturnDelay);
}

void rainbowCycle(bool direction, int SpeedDelay, byte luminosity)
{
  byte *c;
  for(int i=0; i < NUM_LEDS; i++)
  {
    c = Wheel(((i * 256 / NUM_LEDS) + effectIndex) & 255);
    if(direction)
      setPixel(i, *c * luminosity, *(c+1) * luminosity, *(c+2) * luminosity);
    else
      setPixel(NUM_LEDS-i-1, *c * luminosity, *(c+1) * luminosity, *(c+2) * luminosity);
  }
  showStrip();
  delay(SpeedDelay);
  effectIndex = (effectIndex + 1) % 256;
}

void cycle(byte hue1, byte hue2, byte hue3, bool direction, int SpeedDelay, byte luminosity)
{
  CRGB color;
  for(int j=0; j < NUM_LEDS; j++)
  {
    if((j < effectIndex && direction) || (j > effectIndex && !direction))
      hsv2rgb_rainbow(CHSV((hue3 - hue2)*effectIndex/(NUM_LEDS-1) + hue2, 255, luminosity), color);
    else
      hsv2rgb_rainbow(CHSV((hue2 - hue1)*effectIndex/(NUM_LEDS-1) + hue1, 255, luminosity), color);
    if(direction)
      setPixel(effectIndex, color.r, color.g, color.b);
    else
      setPixel(NUM_LEDS-effectIndex-1, color.r, color.g, color.b);
  }
  showStrip();
  delay(SpeedDelay);
  effectIndex = (effectIndex + 1) % NUM_LEDS;
}


void BounceToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
  int i;
  double level;
  for(int j = 0; j < NUM_LEDS; j++)
    setPixel(j, 0, 0, 0);
    
  if(effectIndex < ((NUM_LEDS-EyeSize)/2))
  {
    i = effectIndex;
  }
  else
  {
    i = (NUM_LEDS-EyeSize)/2 - (effectIndex - (NUM_LEDS-EyeSize)/2);
  }
  
  for(int j = 0; j < NUM_LEDS/2; j++)
  {
    level = gauss(j, i + EyeSize/2, EyeSize/4);
    setPixel(j, int(red * level), int(green * level), int(blue * level));
    setPixel(NUM_LEDS-j-1, int(red * level), int(green * level), int(blue * level));
  }
  
  showStrip();
  delay(SpeedDelay);
  effectIndex = (effectIndex + 1) % (2*(NUM_LEDS-EyeSize)/2);
  if(effectIndex == 0)
    delay(ReturnDelay);
}


int memTwinkle[NUM_LEDS];
void Twinkle(byte red, byte green, byte blue, int SpeedDelay)
{
  if(effectIndex == 0)
  {
    for(int j = 0; j < NUM_LEDS; j++)
      setPixel(j, 0, 0, 0);
  }
  if(effectIndex == 0 || effectIndex == NUM_LEDS)
  {
    for (int i = 0; i < NUM_LEDS; i++)
      memTwinkle[i] = i;
  }
 
  int randIdx;
  if(effectIndex < NUM_LEDS)
  {
    randIdx = random(NUM_LEDS-effectIndex-1);
    setPixel(memTwinkle[randIdx], red, green, blue);
    for (int j = randIdx; j < NUM_LEDS-effectIndex-1; j++)
      memTwinkle[j] = memTwinkle[j+1];
  }
  else
  {
    randIdx = random(NUM_LEDS-(effectIndex-NUM_LEDS)-1);
    setPixel(memTwinkle[randIdx], 0, 0, 0);
    for (int j = randIdx; j < NUM_LEDS-(effectIndex-NUM_LEDS)-1; j++)
      memTwinkle[j] = memTwinkle[j+1];
  }
    
  showStrip();
  delay(SpeedDelay);
  effectIndex = (effectIndex + 1) % (2*NUM_LEDS);
}


void Sparkle(byte red, byte green, byte blue, int SpeedDelay)
{
  if(effectIndex == 0)
  { 
    for(int j = 0; j < NUM_LEDS; j++)
      setPixel(j, 0, 0, 0);
    effectIndex = 1;
  }
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel,0,0,0);
}

void RunningLights(byte hue1, byte hue2, int WaveDelay, bool direction, byte luminosity)
{
  CRGB color;
  for(int i=0; i<NUM_LEDS; i++)
  {
    short level = luminosity * sin((i+effectIndex)/NUM_LEDS);
    hsv2rgb_rainbow(CHSV((hue2 - hue1)* effectIndex/(NUM_LEDS-1) + hue1, 255, abs(level)), color);
    if(direction)
      setPixel(i, color.r, color.g, color.b);
    else
      setPixel(NUM_LEDS-i-1, color.r, color.g, color.b);
  }
 
  showStrip();
  delay(WaveDelay);
  effectIndex = (effectIndex + 1) % NUM_LEDS;
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}
