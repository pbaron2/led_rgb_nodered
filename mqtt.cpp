#include "main.h"

// StaticJsonDocument<1000> doc;
//DynamicJsonDocument doc(512);

void callback(char* topic, byte* payload, unsigned int length)
{
  StaticJsonDocument<512> doc;
  
  DeserializationError error = deserializeJson(doc, payload, length);

  if(error)
  {
    for(char i = 0; i < 10; i++)
    {
      leds[0] = CRGB(63, 0, 0);
      delay(200);
      leds[0] = CRGB(31, 31, 0);
      delay(200);
    }
  }
  else
  {
    colorIndex = 0;
    effectIndex = 0;
    hueForRainbow = 0;

    effect.currentMode = doc["mode"];
    JsonObject docData = doc["data"];
    effect.luminosity = ((byte) docData["Luminosity"]) * 16 - 1;
    effect.speed = docData["Speed"];
    effect.eyeSize = docData["EyeSize"];
    effect.isWhite = docData["isWhite"];
    effect.rainbow = docData["Rainbow"];
    effect.direction = docData["Direction"];
    
    effect.colorAmount = docData["colorAmount"];
    if(effect.colorAmount > MAX_HUES)
    {
      leds[0] = CRGB(31, 0, 31);
      FastLED.show();
      delay(5000);
    }
    String colorName = "Color-";
    for(int i = 0; i < min(MAX_HUES, effect.colorAmount); i++)
    {
      colorName.concat(i+1);
      effect.hues[i] = doc["data"][colorName];
    }
  }
}

void reconnectMqtt()
{
  while(!client.connected())
  {
    leds[0] = CRGB(31, 0, 31);
    FastLED.show();
    if(client.connect("LED_Room", MQTT_USER, MQTT_PASS))
    {
//      client.publish(MQTT_TOPIC_STATUS, "1");
      client.subscribe(MQTT_TOPIC_DATA);
      
      leds[0] = CRGB(0, 0, 63);
    }
    else
    {
      leds[0] = CRGB(63, 0, 0);
    }
      FastLED.show();
      delay(500);
  }
}
