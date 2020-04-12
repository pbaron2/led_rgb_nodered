#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

//#define DEBUG_ON

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>
#include <ArduinoJson.h>


#define LED_PIN     0
#define NUM_LEDS    180
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

#define WIFI_SSID   "Orange-BB54"
#define WIFI_PASS   "superpaulo"
#define DELAI_CONNEXION  30
#define DELAI_STATUS     60

#define MQTT_SERVER   "192.168.1.100"
#define MQTT_USER     "ledroom"
#define MQTT_PASS     "ledroompassword"
#define MQTT_TOPIC_STATUS    "led_room_status"
#define MQTT_TOPIC_DATA      "led_room_data"
 
#define MAX_HUES  16


extern CRGB leds[NUM_LEDS+1];
extern PubSubClient client;

extern int colorIndex;
extern byte hueForRainbow;
extern int effectIndex;

typedef struct ConfEffect ConfEffect;
struct ConfEffect
{
    const char* currentMode;
    byte luminosity;
    int speed;
    int eyeSize;
    int colorAmount;
    byte hues[MAX_HUES];
    bool isWhite;
    bool rainbow;
    bool direction;
};
extern ConfEffect effect;

void callback(char* topic, byte* payload, unsigned int length);
void reconnectMqtt();

/*
void rainbow(uint8_t index, uint8_t startHue, uint8_t endHue);
void chenillard(uint8_t index, uint8_t hue);
uint8_t gauss(uint8_t x, uint8_t mean, uint8_t sigma);
uint8_t sinNorm(uint8_t x, uint8_t mean);
*/

double gauss(int x, double mean, double sigma);

void showStrip();
void setPixel(int Pixel, byte red, byte green, byte blue);
void setAll(byte red, byte green, byte blue);

void rainbow(int SpeedDelay, byte luminosity);
void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void rainbowCycle(bool direction, int SpeedDelay, byte luminosity);
void cycle(byte hue1, byte hue2, byte hue3, bool direction, int SpeedDelay, byte luminosity);
void BounceToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void Twinkle(byte red, byte green, byte blue, int SpeedDelay);
void Sparkle(byte red, byte green, byte blue, int SpeedDelay);
void RunningLights(byte hue1, byte hue2, int WaveDelay, bool direction, byte luminosity);

byte * Wheel(byte WheelPos);

#endif
