#include "Arduino.h"
#include "iostream"

typedef void (*http_handler)();

struct node
{
    char *path;
    http_handler handler;
    struct node *next;
};

class http_handler_linked_list
{
private:
    node *head, *tail;
public:
    http_handler_linked_list()
    {
      head = NULL;
      tail = NULL;
    }

    void add_node(char* path, http_handler handler)
    {
      node *tmp = new node;
      tmp->path = path;
      tmp->handler = handler;
      tmp->next = NULL;
      
      if(head == NULL)
      {
          head = tmp;
          tail = tmp;
      }
      else
      {
          tail->next = tmp;
          tail = tail->next;
      }
    }

    http_handler get_handler(String path)
    {
      node *tmp = head;
      while(tmp != NULL)
      {
        if(String(tmp->path).equals(path))
        {
          return tmp->handler;
        }
        tmp = tmp->next;
      }
      return NULL;
    }
};
