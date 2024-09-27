// Written by mo thunderz (last update: 27.08.2022)
//
// ---------------------------------------------------------------------------------------

#include <WiFi.h>             // needed to connect to WiFi
#include <WebServer.h>        // needed to create a simple webserver (make sure tools -> board is set to ESP32, otherwise you will get a "WebServer.h: No such file or directory" error)
#include <WebSocketsServer.h> // needed for instant communication between client and server through Websockets
#include <ArduinoJson.h>      // needed for JSON encapsulation (send multiple variables with one string)
#include "htlm.h"
// SSID and password of Wifi connection:
const char *ssid = "ASUS";
const char *password = "Student2000";

void webSocketEvent(byte, WStype_t, uint8_t *, size_t);

bool flagaodp = 0;
char slowoodp[200];

// The JSON library uses static memory, so this will need to be allocated:
// -> in the video I used global variables for "doc_tx" and "doc_rx", however, I now changed this in the code to local variables instead "doc" -> Arduino documentation recomends to use local containers instead of global to prevent data corruption

// Initialization of webserver and websocket
WebServer server(80);                              // the server uses port 80 (standard port for websites
WebSocketsServer webSocket = WebSocketsServer(81); // the websocket uses port 81 (standard port for websockets

void setup()
{
    Serial.begin(115200);

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
}

void loop()
{
    server.handleClient(); // Needed for the webserver to handle all clients
    webSocket.loop();      // Update function for the webSockets

    if (flagaodp)
    {
        flagaodp = 0;
        Serial.println(slowoodp);
    }
}
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