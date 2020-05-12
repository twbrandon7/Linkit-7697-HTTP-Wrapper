#include "vector"
#include "string.h"
#include "Arduino.h"

class utils
{
  private:
  public:
    static std::vector<char*> split(char *str, const char *del) {
      std::vector<char*> arr;
      char *s = strtok(str, del);
      
      while(s != NULL) {
        arr.push_back(s);
        s = strtok(NULL, del);
      }
      return arr;
    }

    static String parseRequestPath(String line)
    {
      int len = line.length();
      char str[len+1];
      line.toCharArray(str, len+1);
      const char s[2] = " ";
      char *token;
      
      /* get the first token */
      token = strtok(str, s);
  
      int ctn = 0;
      /* walk through other tokens */
      while( token != NULL ) 
      {
         if(ctn == 1)
         {
           return String(token);
         }
         token = strtok(NULL, s);
         ctn += 1;
      }
      return "";
    }
};
