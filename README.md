﻿# Arduino HTTP Client and Server Wrapper for Linkit 7697

This project aim to provide an object oriented HTTP wrapper for [MediaTek Linkit™ 7697](https://labs.mediatek.com/en/platform/linkit-7697). This project utilize `LWiFi.h` to create a TCP client and server, and do some routine work for developers. Such as filling HTTP header, parse HTTP request and parse query string.

## Installation

The wrapper is not an Arduino Library. For installation, jutst clone this repository into your Arduino project directory. For example, here is an Arduino project:

```bash
.
└── HTTP_wrapper_test
    └── HTTP_wrapper_test.ino
```

Change directory to your arduino project.

```bash
cd HTTP_wrapper_test
```

Clone this repository:

```bash
git clone https://github.com/twbrandon7/Linkit-7697-HTTP-Wrapper.git wrapper
```

For version control, you can create `.gitmodules`. The content of `.gitmodules` are as following:

```.gitmodules
[submodule "wrapper"] 
path = wrapper 
url = https://github.com/twbrandon7/Linkit-7697-HTTP-Wrapper.git
```

Now, the structure of your project should look like this:

```bash
.
├── HTTP_wrapper_test
│   ├── HTTP_wrapper_test.ino
│   ├── .gitmodules
│   └── wrapper
│       ├── README.md
│       ├── WiFiHttpServerWrapper.ino
│       ├── client
│       │   └── ClientWrapper.cpp
│       ├── server
│       │   ├── ServerWrapper.cpp
│       │   └── http_handler_linked_list.cpp
│       └── utils
            └── utils.cpp
```

## Usage

### HTTP Server

#### Simple Example

```cpp
/* Include the server wrapper */
#include "wrapper/server/ServerWrapper.cpp"

/*
 * Establish a WiFi AP or connect to an existing WiFi AP.
 */
// ...... <skip>

/* Create a http server */
ServerWrapper server(80);

void setup()
{
    /* WiFi Configuration */
    // ...... <skip>

    server.begin();

    // Setting the handler when the specified path is requested.
    server.on("/", []() { handleRoot(0); });
    server.on("/close", handleClose);
}

void loop()
{
    // Add this line in a loop to receive HTTP request.
    server.accept();
}

/*
 * The `var` parameter is an example to pass value into the handler.
 * You can remove it if you don't need to pass any value into the handler.
 */
void handleRoot(int var)
{
  String body = "<!DOCTYPE html>";
  body += "<html>";
  body += "<head>";
  body += "<title>Page Title</title>";
  body += "</head>";
  body += "<body>";

  body += "<h1>This is a Heading</h1>";
  body += "<p>This is a paragraph.</p>";
  body += "<p>var is \"" + String(var) + "\"</p>";

  for (int i = 0; i < server.argLength(); i++)
  {
    body += "<p>" + server.argKey(i) + " = " + server.arg(i) + "</p>";
  }

  body += "</body>";
  body += "</html>";
  server.send(200, "text/html", body);
}

void handleClose()
{
  String body = "Closed";
  server.send(200, "text/html", body);
  server.stop();
}

```

#### Full example

```cpp
/* Include the server wrapper */
#include "wrapper/server/ServerWrapper.cpp"

/* WiFi setting */
char ssid[] = ""; // your network SSID (name)
char pass[] = ""; // your network password
int keyIndex = 0; // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

/* Create a http server */
ServerWrapper server(80);

void setup()
{

  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the status:
  printWifiStatus();

  testHttpServer();
}

void loop()
{
    // server.accept();
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

/*
 * Configure a http server.
 */
void testHttpServer()
{
  Serial.println("Start server.");

  server.begin();

  // Setting the handler when the specified path is requested.
  server.on("/", []() { handleRoot(0); });
  server.on("/close", handleClose);

  IPAddress ip = WiFi.localIP();

  Serial.print("Server is running. Visit \"http://");
  Serial.print(ip);
  Serial.println("\"");
  Serial.print("Visit \"http://");
  Serial.print(ip);
  Serial.println("/close\" to stop the server.");

  // The "server.accept();" is required when using ServerWrapper.
  // You can also put it into "loop()" function.
  while (server.isRunning())
  {
    server.accept();
  }  
}

void handleRoot(int var)
{
  String body = "<!DOCTYPE html>";
  body += "<html>";
  body += "<head>";
  body += "<title>Page Title</title>";
  body += "</head>";
  body += "<body>";

  body += "<h1>This is a Heading</h1>";
  body += "<p>This is a paragraph.</p>";
  body += "<p>var is \"" + String(var) + "\"</p>";

  for (int i = 0; i < server.argLength(); i++)
  {
    body += "<p>" + server.argKey(i) + " = " + server.arg(i) + "</p>";
  }

  body += "</body>";
  body += "</html>";
  server.send(200, "text/html", body);
}

void handleHealth()
{
  String body = "Health check OK!";
  server.send(200, "text/html", body);
}

void handleClose()
{
  String body = "Closed";
  server.send(200, "text/html", body);
  server.stop();
}
```

### HTTP Client

#### Simple Example

```cpp
/* Include the client wrapper */
#include "wrapper/client/ClientWrapper.cpp"

/*
 * Establish a WiFi AP or connect to an existing WiFi AP.
 */
// ...... <skip>

/* Create a http client */
ClientWrapper client;

void setup()
{
    /* WiFi Configuration */
    // ...... <skip>

    Serial.begin();

    int statusCode;

    // HTTP GET
    client.begin("http://postman-echo.com/get?foo=bar");
    statusCode = client.GET();
    // Print HTTP response body
    if(statusCode == 200)
        Serial.println(client.getString());
    // Close HTTP client
    client.end();

    // HTTP POST
    client.begin("http://postman-echo.com/post");
    client.addHeader("Content-Type", "application/x-www-form-urlencoded");
    statusCode = client.POST("abc=efg&hij=klm");
    if(statusCode == 200)
        Serial.println(client.getString());
    client.end();

    // HTTP POST (JSON)
    client.begin("http://postman-echo.com/post");
    client.addHeader("Content-Type", "application/json");
    statusCode = client.POST("{\"abc\": \"def\", \"ghi\": \"jkl\"}");
    if(statusCode == 200)
        Serial.println(client.getString());
    client.end();

    // HTTP PUT
    client.begin("http://postman-echo.com/put");
    client.addHeader("Content-Type", "application/json");
    statusCode = client.PUT("{\"abc\": \"def\", \"ghi\": \"jkl\"}");
    if(statusCode == 200)
        Serial.println(client.getString());
    client.end();
}

void loop()
{

}
```

## Note

There are several function are not fully implmented. Such as request methods in HTTP Client. However it is welcom to modified the code by yourself. And it is also welcom to create a pull request to improve this project.

The definition of the wrappers can be found in `./server/ServerWrapper.cpp` and `./client/ClientWrapper.cpp`.
