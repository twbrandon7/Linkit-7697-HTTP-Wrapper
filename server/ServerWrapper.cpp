#include "Arduino.h"
#include "LWiFi.h"
#include "vector"
#include "string"

#include "http_handler_linked_list.cpp"
#include "../utils/utils.cpp"

class ServerWrapper
{
private:
  WiFiServer *server;
  WiFiClient *currentClient;
  http_handler_linked_list *handlerList;
  http_handler error_handler = NULL;
  http_handler not_found_handler = NULL;

  std::vector<String> queryKeys;
  std::vector<String> queryValues;
  String path = "";

  bool run = false;

  String getRequestLine(WiFiClient client)
  {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    int lineCount = 0;
    String line = "";
    String reqLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        line += String(c);
        //Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
        if(c == '\r' || c == '\n') {
          lineCount += 1;
          if(lineCount == 1) {
            reqLine = line;
          }
          line = "";
        }
      }
    }
    return reqLine;
  }

  void parseParameters(String url)
  {
    queryKeys.clear();
    queryValues.clear();
    path = "";

    int len = url.length();
    char *c_url = (char *)malloc((len + 1) * sizeof(char));
    memcpy(c_url, (char *) url.c_str(), len);
    c_url[len] = 0;

    std::vector<std::string> arr = utils::split(c_url, "?");
    path = String((char*) arr[0].c_str());

    if(arr.size() > 1)
    {
      std::vector<std::string> pars = utils::split(arr[1], "&");
      for(int i = 0; i < pars.size(); i++)
      {
        std::vector<std::string> sep = utils::split(pars[i], "=");
        if(sep.size() >= 2)
        {
          queryKeys.push_back(String((char*) sep[0].c_str()));
          queryValues.push_back(String((char*) sep[1].c_str()));
        }
      }
    }
  }

public:
  ServerWrapper(int port)
  {
    handlerList = new http_handler_linked_list();
    server = new WiFiServer(port);
  }
  
  ~ServerWrapper() { }

  void begin()
  {
    server->begin();
    run = true;
    Serial.println("[ServerWrapper] started");
  }

  void stop()
  {
    run = false;
    Serial.println("[ServerWrapper] closed");
  }
  
  void on(char* path, http_handler handler) {
    handlerList->add_node(path, handler);
  }

  void onError(http_handler handler)
  {
    this->error_handler = handler;
  }

  void onNotFound(http_handler handler)
  {
    this->not_found_handler = handler;
  }

  void send(int code, String mimeType, String body)
  {
    String text = "";
    switch(code)
    {
      case 200:
        text = "OK";
        break;
      case 404:
        text = "Not Found";
        break;
      case 400:
        text = "Bad Request";
        break;
      default:
        text = "NA";
    }
    currentClient->println(String("HTTP/1.1 ") + String(code) + String(" ") + text);
    currentClient->println("Connection: close");  // the connection will be closed after completion of the request
    currentClient->println("Cache-Control: no-cache");
    currentClient->println();
    currentClient->print(body); 
    currentClient->println(); 
  }

  String arg(int index)
  {
    return String(this->queryValues[index]);
  }

  String argKey(int index)
  {
    return String(this->queryKeys[index]);
  }

  int argLength()
  {
    return this->queryValues.size();
  }

  bool isRunning()
  {
    return run;
  }

  void accept()
  {
    if(!run) return;
    
    WiFiClient client = server->available();
    if (client)
    {
      Serial.println("[ServerWrapper] new client");
    
      String reqLine = this->getRequestLine(client);
      String reqPath = utils::parseRequestPath(reqLine);
      this->parseParameters(reqPath);
      
      if(reqLine.equals("") || reqPath.equals("") || path.equals(""))
      {
        if(this->error_handler == NULL)
        {
          this->send(400, "text/html", "400 Bad Request.");
        }
        else
        {
          error_handler();
        }
      }
      else
      {
        currentClient = &client;
        http_handler handler = handlerList->get_handler(path);
        if(handler != NULL)
        {
          handler();
        }
        else
        {
          if(this->not_found_handler == NULL)
          {
            this->send(404, "text/html", " 404 Not found. The request URL \"" + reqPath + "\" was not found on this server.");
          }
          else
          {
            not_found_handler();
          }
        }
      }
      
      delay(1);

      // close the connection:
      client.stop();
      Serial.println("[ServerWrapper] client disonnected");
    }
  }
};
