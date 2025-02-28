

#include <Arduino.h>
#include <FastLED.h>
#include "font5x7.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "htlm.h"

// SSID and password of Wifi connection:
const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";

void webSocketEvent(byte, WStype_t, uint8_t *, size_t);

volatile bool flagaodp = 0;
char slowoodp[200];
bool odw = 0;
int opcja = 0;

// Initialization of webserver and websocket
WebServer server(80);                              // the server uses port 80 (standard port for websites
WebSocketsServer webSocket = WebSocketsServer(81); // the websocket uses port 81 (standard port for websockets

#define NUM_LEDS 18
#define DATA_PIN1 5
#define DATA_PIN2 7
#define IRSENSOR 3
#define SZER 50

CRGB led1[NUM_LEDS];
CRGB led2[NUM_LEDS];

uint8_t count = 0;

bool state = 0;

unsigned long czasstart = 0;
unsigned long czasstop = 0;
unsigned long czas = 0;
unsigned long pixe;
bool lite = 0;

bool tab1[8][5];
bool tab2[16][719];

int przes = 0;

int ladslow(char *slow)
{
    int ide = 0;

    // Przechodzimy przez każde "i"-te" miejsce w stringu aż do końca (kiedy znajdziemy '\0')

    for (int i = 0; i < 16; i++)
    {
        tab2[i][0] = 0;
    }
    while (slow[ide] != '\0')
    {

        int licz = slow[ide];
        for (int i = 0; i < 5; i++)
        {
            for (int j = 7; j >= 0; --j)
            {
                tab1[j][i] = ((font5x7[5 * licz + i] >> j) & 1);
            }
        }
        for (int i = 0; i < 7; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                tab2[i * 2][1 + j + 6 * ide] = tab1[i][j];
                tab2[i * 2 + 1][1 + j + 6 * ide] = tab1[i][j];
            }
            tab2[i * 2][1 + 6 + 6 * ide] = 0;
            tab2[i * 2 + 1][1 + 6 + 6 * ide] = 0;
        }

        ide++;
    }

    return ide;
}

int slow;

void setup()
{
    Serial.begin(9600);

    WiFi.begin(ssid, password);
    Serial.println("Establishing connection to WiFi with SSID: " + String(ssid));

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.print("Connected to network with IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", []() {                       // define here wat the webserver needs to do
        server.send(200, "text/html", webpage); //    -> it needs to send out the HTML string "webpage" to the client
    });
    server.begin(); // start server

    webSocket.begin();                 // start websocket
    webSocket.onEvent(webSocketEvent); // define a callback function -> what does the ESP32 need to do when an event from the websocket is received? -> run function "webSocketEvent()"

    pinMode(IRSENSOR, INPUT); // pin for interrupt from infrared sensorX
    FastLED.addLeds<WS2812Controller800Khz, DATA_PIN1, GRB>(led1, NUM_LEDS);
    FastLED.addLeds<WS2812Controller800Khz, DATA_PIN2, GRB>(led2, NUM_LEDS);
    FastLED.setBrightness(55);
    pinMode(15, OUTPUT);
    digitalWrite(15, HIGH);
    odw = 1;
}

void loop()
{

    server.handleClient(); // Needed for the webserver to handle all clients
    webSocket.loop();      // Update function for the webSockets

    if (flagaodp)
    {
        flagaodp = 0;
        slow = ladslow(slowoodp);
        Serial.println(slowoodp);
        Serial.println(slow);
    }

    EVERY_N_MILLISECONDS(150)
    {
        if (przes >= (((slow - 5) * 6 + 1)))
            przes = 0;
        else
            przes++;
    }
    if (state == 0 && analogRead(IRSENSOR) < 3000) // każde pół obrotu wchodzi
    {
        state = 1;
    }
    else if (state == 1 && analogRead(IRSENSOR) > 3000) // wychodzi z obrotu
    {
        state = 0;

        FastLED.clear();

        for (int i = 0; i < 31; i++)
        {

            FastLED.clear();

            for (int j = 0; j < 16; j++)

            {
                if (odw)
                {
                    if (!tab2[j][30 - (i % 31) + (przes % ((slow - 5) * 6 + 1))])
                    {

                        led1[15 - j] = CRGB::Red;
                        led2[15 - j] = CRGB::Red;
                    }
                }
                else
                {
                    if (tab2[j][30 - (i % 31) + (przes % ((slow - 5) * 6 + 1))])
                    {

                        led1[15 - j] = CRGB::Red;
                        led2[15 - j] = CRGB::Red;
                    }
                }
            }
            led1[0] = CRGB::Blue;
            led2[0] = CRGB::Blue;
            led1[17] = CRGB::Blue;
            led2[17] = CRGB::Blue;
            FastLED.show();

            if (analogRead(IRSENSOR) < 3000)
            {
                state = 1;
                i = 99999;
                break;
            }
        }
        FastLED.clear();
    }
}

// Every_n_millis
// map

// Ustawienie kolorów dla diod LED
// 8200 jak nie świci
// 850 jak świeci

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.println("Client " + String(num) + " disconnected");
        break;
    case WStype_CONNECTED:
        Serial.println("Client " + String(num) + " connected");
        // Optionally send a welcome message or perform additional logic
        break;
    case WStype_TEXT:
        // Use DynamicJsonDocument for potentially larger JSON messages
        DynamicJsonDocument doc(512); // Adjust size as necessary
        DeserializationError error = deserializeJson(doc, payload);
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }
        else
        {
            const char *message = doc["value"];
            int x;
            while (message[x] != '\0')
            {
                x++;
            }

            flagaodp = 1;
            strncpy(slowoodp, message, x + 1); // Copy the message to slowoodp with a limit to prevent overflow
        }
        Serial.println("");
        break;
    }
}