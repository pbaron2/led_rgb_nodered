#include "main.h"

WiFiClient espClient;
PubSubClient client(espClient);

CRGB leds[NUM_LEDS+1];


ConfEffect effect = {0};

CRGB color;

long lastMsg = 0;

int colorIndex = 0;
byte hueForRainbow = 0;
int effectIndex = 0;

byte hue1, hue2, hue3;


void setup()
{
  delay( 1000 ); // power-up safety delay (3000 usually)
  
#ifdef DEBUG_ON
  Serial.begin(115200);
  Serial.println("Go!");
#endif

  // LEDs
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS+1).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(255);

#ifndef DEBUG_ON 
  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int nbTent = 0;
  while (WiFi.status() != WL_CONNECTED && nbTent < DELAI_CONNEXION * 4)
  {
    if (nbTent % 2 == 0)
      leds[0] = CRGB::Yellow;
    else
      leds[0] = CRGB::Black;
    FastLED.show();
    nbTent++;
    delay(250);
  }
  Serial.println();

  if(nbTent >= DELAI_CONNEXION * 4) // Connexion echouee
  {
    while(1)
    {
      if (nbTent % 2 == 0)
        leds[0] = CRGB::Red;
      else
        leds[0] = CRGB::Black;
      FastLED.show();
      nbTent++;
      delay(500);
    }
  }
  else
  {
    leds[0] = CRGB::Green;
    FastLED.show();
    delay(500);
    leds[0] = CRGB::Black;
  }

  // MQTT
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
  reconnectMqtt();

  effect.currentMode = "none";
  
#else
  effect.currentMode = "cylon";
  effect.hues[0] = 0;
  effect.speed = 20;
  effect.eyeSize = 8;
  effect.luminosity = 255;
  effect.rainbow = 0;
#endif
}


void loop()
{
#ifdef DEBUG_ON
  Serial.print("Mode : ");
  Serial.println(effect.currentMode);
#else
  reconnectMqtt();
  client.loop();
#endif
  
  /*
  long nowMsg = millis();
  if (nowMsg - lastMsg > DELAI_STATUS*1000)
  {
    lastMsg = nowMsg;
    client.publish(MQTT_TOPIC_STATUS, "1");
  }
*/

  if(strcmp(effect.currentMode, "none") != 0)
  {
    leds[0] = CRGB::Green;
    if(strcmp(effect.currentMode, "off") == 0)
    {
        setAll(0, 0, 0);
    }
    else
    {
      if(strcmp(effect.currentMode, "static") == 0)
      {
        if (effect.isWhite)
        {
          color.r = effect.luminosity;
          color.g = effect.luminosity;
          color.b = effect.luminosity;
        }
        else
        {
          hsv2rgb_rainbow(CHSV(effect.hues[0], 255, effect.luminosity), color);
        }
        setAll(color.r, color.g, color.b);
      }
      else if (strcmp(effect.currentMode, "rainbow") == 0)
      {
        rainbow(500/effect.speed, effect.luminosity);
      }
      else if (strcmp(effect.currentMode, "fadeinout") == 0)
      {
  
      }
      else if (strcmp(effect.currentMode, "cycle") == 0)
      {
        if (effect.rainbow)
        {
            rainbowCycle(effect.direction, 500/effect.speed, effect.luminosity);
        }
        else
        {
          if(effectIndex == 0)
          {
            hue1 = effect.hues[colorIndex];
            colorIndex = (colorIndex + 1) % effect.colorAmount;
            hue2 = effect.hues[colorIndex];
            if(effect.colorAmount > 2)
            {
              hue3 = effect.hues[colorIndex];
              colorIndex = (colorIndex + 1) % effect.colorAmount;
            }
            else
            {
              hue3 = hue1;
            }
          }
          hsv2rgb_rainbow(CHSV(effect.hues[0], 255, effect.luminosity), color);
          cycle(hue1, hue2, hue3, effect.direction, 500/effect.speed, effect.luminosity);
        }
      }
      else if (strcmp(effect.currentMode, "cylon") == 0)
      {
        if (effect.rainbow)
        {
          hsv2rgb_rainbow(CHSV(hueForRainbow, 255, effect.luminosity), color);
        }
        else
        {
          hsv2rgb_rainbow(CHSV(effect.hues[0], 255, effect.luminosity), color);
        }
        CylonBounce(color.r, color.g, color.b, effect.eyeSize, 500/effect.speed, 500/effect.speed);
      }
      else if (strcmp(effect.currentMode, "bouncingCenter") == 0)
      {
        if (effect.rainbow)
        {
          hsv2rgb_rainbow(CHSV(hueForRainbow, 255, effect.luminosity), color);
        }
        else
        {
          hsv2rgb_rainbow(CHSV(effect.hues[0], 255, effect.luminosity), color);
        }
        BounceToCenter(color.r, color.g, color.b, effect.eyeSize, 500/effect.speed, 500/effect.speed);
      }
      else if (strcmp(effect.currentMode, "sparkle") == 0)
      {
        if (effect.rainbow)
        {
          hsv2rgb_rainbow(CHSV(random(256), random(256), effect.luminosity), color);
        }
        else
        {
          hsv2rgb_rainbow(CHSV(effect.hues[0], 255, effect.luminosity), color);
        }
        Sparkle(color.r, color.g, color.b, 500/effect.speed);
      }
      else if (strcmp(effect.currentMode, "twinkle") == 0)
      {
        if (effect.rainbow)
        {
          hsv2rgb_rainbow(CHSV(random(256), random(256), effect.luminosity), color);
        }
        else
        {
          hsv2rgb_rainbow(CHSV(effect.hues[0], 255, effect.luminosity), color);
        }
        Twinkle(color.r, color.g, color.b, 500/effect.speed);
      }
      else if (strcmp(effect.currentMode, "chase") == 0)
      {
        if (effect.rainbow)
        {
          hue1 = hueForRainbow;
          hue2 = hueForRainbow;
        }
        else
        {
          if(effectIndex == 0)
          {
            hue1 = effect.hues[colorIndex];
            if(effect.colorAmount > 1)
            {
              colorIndex = (colorIndex + 1) % effect.colorAmount;
              hue2 = effect.hues[colorIndex];
            }
            else
            {
              hue2 = hue1;
            }
          }
        }
        RunningLights(hue1, hue2, 5000/effect.speed, effect.direction, effect.luminosity);
      }
      else if (strcmp(effect.currentMode, "running") == 0)
      {
  
      }
      else if (strcmp(effect.currentMode, "fire") == 0)
      {
  
      }
      else if (strcmp(effect.currentMode, "bouncingBalls") == 0)
      {
  
      }
      else if (strcmp(effect.currentMode, "meteor") == 0)
      {
  
      }
      else
      {
        leds[0] = CRGB::Red;
        FastLED.show();
      }
      if(effectIndex == 0)
        hueForRainbow = hueForRainbow + 1;
    }
  }
  else
  {
    setAll(127, 127, 127);
    leds[0] = CRGB::Yellow;
    FastLED.show();
  }

}
