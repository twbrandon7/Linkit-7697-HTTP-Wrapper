#pragma once

#include "vector"
#include "string.h"
#include "string"
#include "Arduino.h"

struct url_info
{
  char *host;
  char *protocol;
  char *path;
  int port;
};

class utils
{
private:
public:
  static std::vector<std::string> split(const std::string &str, const std::string &delim)
  {
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
      pos = str.find(delim, prev);
      if (pos == std::string::npos)
        pos = str.length();
      std::string token = str.substr(prev, pos - prev);
      if (!token.empty())
        tokens.push_back(token);
      prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
  }

  static String parseRequestPath(String line)
  {
    int len = line.length();
    char str[len + 1];
    line.toCharArray(str, len + 1);
    const char s[2] = " ";
    char *token;

    /* get the first token */
    token = strtok(str, s);

    int ctn = 0;
    /* walk through other tokens */
    while (token != NULL)
    {
      if (ctn == 1)
      {
        return String(token);
      }
      token = strtok(NULL, s);
      ctn += 1;
    }
    return "";
  }

  static url_info parseUrl(char *url)
  {
    url_info info;

    // seperate protocol
    std::vector<std::string> sep_protocol = split(url, "://");
    info.protocol = (char *)sep_protocol[0].c_str();

    if(sep_protocol.size() <= 1) return info;

    // seperate path
    char *remain = (char *)sep_protocol[1].c_str();
    char *pathPtr = strchr(remain, '/');

    int pathPtrIdx = pathPtr - remain + 1;

    char *hostAndPort = (char *)malloc((pathPtrIdx + 1) * sizeof(char));
    memcpy(hostAndPort, remain, pathPtrIdx - 1);
    hostAndPort[pathPtrIdx-1] = 0;

    int pathLen = strlen(pathPtr);
    char *path = (char *)malloc((pathLen + 1) * sizeof(char));
    memcpy(path, pathPtr, pathLen);
    path[pathLen] = 0;

    info.path = path;

    // seperate port
    std::vector<std::string> sep_port = split(hostAndPort, ":");
    if (sep_port.size() == 1)
    {
      if (strcmp(info.protocol, "http") == 0)
      {
        info.port = 80;
      }
      else if (strcmp(info.protocol, "https"))
      {
        info.port = 443;
      }
    }
    else
    {
      info.port = atoi(sep_port[1].c_str());
    }

    int hostLen = sep_port[0].length();
    info.host = (char *)malloc((hostLen + 1) * sizeof(char));
    memcpy(info.host, (char *) sep_port[0].c_str(), hostLen);
    info.host[hostLen] = 0;
    return info;
  }
};
