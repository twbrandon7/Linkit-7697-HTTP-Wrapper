#include "ServerWrapper.cpp"

char ssid[] = "";      // your network SSID (name)
char pass[] = "";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

ServerWrapper server(80);

void handleRoot(int retry){
  String body = "<!DOCTYPE html>";
  body += "<html>";
  body += "<head>";
  body += "<title>Page Title</title>";
  body += "</head>";
  body += "<body>";
  
  body += "<h1>This is a Heading</h1>";
  body += "<p>This is a paragraph.</p>";

  for(int i = 0; i < server.argLength(); i++)
  {
    body += "<p>" + server.arg(i) + "</p>";
  }
  
  body += "</body>";
  body += "</html>";
  server.send( 200, "text/html", body );
}

void setup() {
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
  }
  
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();

  server.on("/", [](){handleRoot(0);});
}

void loop()
{
  server.accept();
}


void printWifiStatus() {
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
