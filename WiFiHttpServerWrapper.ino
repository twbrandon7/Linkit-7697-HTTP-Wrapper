#include "server/ServerWrapper.cpp"
#include "client/ClientWrapper.cpp"

char ssid[] = "Xperia Z5_fb41"; // your network SSID (name)
char pass[] = "790951405";      // your network password
int keyIndex = 0;               // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

ClientWrapper client;
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

  delay(500);
  Serial.println("READY");

  testHttpClient();
  delay(1000);
  testHttpServer();

  Serial.println("Testing finished.");
}

void loop()
{

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

void testHttpClient()
{
  Serial.println("Start testing HTTP Client.");
  int statusCode;

  Serial.println("===== HTTP GET =====");
  client.begin("http://postman-echo.com/get?foo=bar");
  statusCode = client.GET();
  Serial.println(client.getString());
  Serial.println("Response Status Code: " + String(statusCode));
  Serial.println("----- response body START -----");
  Serial.println(client.getString());
  Serial.println("----- response body END -----");
  client.end();
  Serial.println();

  delay(1000);

  Serial.println("===== HTTP POST (Form Data) =====");
  client.begin("http://postman-echo.com/post");
  client.addHeader("Content-Type", "application/x-www-form-urlencoded");
  statusCode = client.POST("abc=efg&hij=klm");
  Serial.println(client.getString());
  Serial.println("Response Status Code: " + String(statusCode));
  Serial.println("----- response body START -----");
  Serial.println(client.getString());
  Serial.println("----- response body END -----");
  client.end();
  Serial.println();

  delay(1000);

  Serial.println("===== HTTP POST (JSON) =====");
  client.begin("http://postman-echo.com/post");
  client.addHeader("Content-Type", "application/json");
  statusCode = client.POST("{\"abc\": \"def\", \"ghi\": \"jkl\"}");
  Serial.println(client.getString());
  Serial.println("Response Status Code: " + String(statusCode));
  Serial.println("----- response body START -----");
  Serial.println(client.getString());
  Serial.println("----- response body END -----");
  client.end();
  Serial.println();

  delay(1000);

  Serial.println("===== HTTP PUT =====");
  client.begin("http://postman-echo.com/put");
  client.addHeader("Content-Type", "application/json");
  statusCode = client.PUT("{\"abc\": \"def\", \"ghi\": \"jkl\"}");
  Serial.println("Response Status Code: " + String(statusCode));
  Serial.println("----- response body START -----");
  Serial.println(client.getString());
  Serial.println("----- response body END -----");
  client.end();
  Serial.println();
}

void testHttpServer()
{
  Serial.println("Start testing server.");

  server.begin();
  server.on("/", []() { handleRoot(0); });
  server.on("/health", handleHealth);
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
