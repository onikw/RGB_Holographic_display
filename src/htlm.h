#ifndef _HTLM_H_
#define _HTLM_H_
// The String below "webpage" contains the complete HTML code that is sent to the client whenever someone connects to the webserver
String webpage = "<!DOCTYPE html>"
                 "<html>"
                 "<head><title>Page Title</title></head>"
                 "<body style='background-color: #EEEEEE;'>"
                 "<span style='color: #003366;'>"
                 "<h1>AIOTA</h1>"
                 "<form id=\"myForm\">"
                 "<p>Enter text:</p>"
                 "<input type='text' id='textID' name='userText' placeholder='Here enter text' required>"
                 "<button type=\"submit\">Submit</button>"
                 "</form>"
                 "</span>"
                 "</body>"
                 "<script>"

                 "var Socket;"
                 "document.getElementById('myForm').addEventListener('submit', function(event) {"
                 "event.preventDefault(); send_message();"
                 "});"

                 "var mess = document.getElementById('textID');"

                 "function init() {"
                 "Socket = new WebSocket('ws://' + window.location.hostname + ':81/');"
                 "Socket.onmessage = function(event) { processCommand(event); };"
                 "}"

                 "function send_message() {"
                 "var message_to_send = document.getElementById('textID').value;"
                 "console.log(message_to_send);"
                 "var msg = { type: 'message', value: message_to_send };"
                 "Socket.send(JSON.stringify(msg));"
                 "}"
                 "function processCommand(event) {"
                 "var obj = JSON.parse(event.data);"
                 "var type = obj.type;"
                 "document.getElementById('output').innerHTML = obj.value;"
                 "}"
                 "window.onload = function(event) { init(); };"
                 "</script>"
                 "</html>";

#endif