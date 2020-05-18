#include "Arduino.h"
#include "LWiFi.h"
#include "../utils/utils.cpp"

class ClientWrapper
{
private:
    WiFiClient client;
    url_info info;
    std::vector<String> headers;
    String httpBody = "";

    int request(String method, String body)
    {
        if (client.connect(info.host, info.port))
        {
            // Make a HTTP request:
            client.println(method + " " + String(info.path) + " HTTP/1.0");
            client.println("Host: " + String(info.host));
            client.println("Accept: */*");
            client.println("Connection: close");
            for(int i = 0; i < headers.size(); i++)
            {
                client.println(headers[i]);
            }
            if(body.length() != 0)
            {
                client.println("Content-length: " + String(body.length()));
                client.println();
                client.println(body);
            }
            client.println();
            client.flush();
            delay(10);
        }
        else
        {
            Serial.println("[HTTP Client] Failed to connect to \"" + String(info.host) + ":" + String(info.port) + "\"");
        }

        int tryCtn = 30000; // Timeout in 30 seconds
        int lineCnt = 0;
        httpBody = "";
        String statusLine = "";
        boolean accepted = false;

        char pre;
        while (tryCtn--)
        {
            // read HTTP header
            boolean currentLineIsBlank = true;
            while (client.available())
            {
                accepted = true;
                char c = client.read();

                if (lineCnt == 0)
                {
                    statusLine += String(c);
                }

                if (c == '\n' && currentLineIsBlank)
                {
                    break;
                }
                if (c == '\n')
                {
                    // you're starting a new line
                    currentLineIsBlank = true;
                }
                else if (c != '\r')
                {
                    // you've gotten a character on the current line
                    currentLineIsBlank = false;
                }

                if (pre == '\r' && c == '\n')
                {
                    lineCnt++;
                }

                pre = c;
            }

            httpBody = "";
            while (client.available())
            {
                char c = client.read();
                httpBody += String(c);
            }

            // if the server's disconnected, stop the client:
            if (!client.connected())
            {
                Serial.println();
                Serial.println("[HTTP Client] disconnecting from server.");
                client.stop();
                break;
            }
            delay(1);
        }

        if(accepted) {
            std::vector<std::string> sep_status = utils::split(statusLine.c_str(), " ");
            if(sep_status.size() < 3)
            {
                Serial.println("[HTTP Client] Bad response format.");
                return 0;
            }
            int code = atoi((char*) sep_status[1].c_str());
            return code;
        } else {
            if(tryCtn == 0)
            {
                Serial.println("[HTTP Client] Timeout.");
            }
            else
            {
                Serial.println("[HTTP Client] Error occurred.");
            }
            return -1;
        }
    }

public:
    void begin(String url)
    {
        headers.clear();
        char c_url[url.length() + 1];
        url.toCharArray(c_url, url.length() + 1);
        utils::parseUrl(c_url, info);
    }

    void addHeader(String key, String value)
    {
        headers.push_back(key + ": " + value);
    }

    int PUT(String body)
    {
        return this->request("PUT", body);
    }

    int POST(String body)
    {
        return this->request("POST", body);
    }

    int GET()
    {
        return this->request("GET", "");
    }

    String getString()
    {
        return this->httpBody;
    }

    void end()
    {
        client.stop();
        free(info.host);
        free(info.path);
        free(info.protocol);
    }
};
