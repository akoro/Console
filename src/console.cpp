/*
Simple user console
Can be used on any Arduino compatible board
V1.0.1 2019-12-19
*/
#include <Arduino.h>
#include "console.h"

void Console::run(void)
{
  if(dev->available())
  {
    char c = dev->read();
    if(c >= ' ' && c < 127) // command line input
    {
      line += c;
      dev->write(c);
    }
    else if(c == '\r')  // command accepted,
    {
      dev->write("\r\n");
      line.trim();
      if(!line.isEmpty())
        parse();        // command parsing
      line.clear();
      dev->write(prompt.c_str());
    }
    else if(!line.isEmpty() && (c == '\b' || c == 127)) // backspace
    {
      line.remove(line.length()-1);
      dev->write("\b \b");
    }
  }
}

void Console::parse(void)
{
  int i = line.indexOf(' ');
  String cmd = (i != -1) ? line.substring(0,i) : line;
  cmd.trim();
  if(handlers.find(cmd) != handlers.end())
  {
    String args = (i != -1) ? line.substring(i) : "";
    args.trim();
    ArgList pl(&args);
    (*handlers[cmd])(pl, *dev);
  }
  else
    if(unknown != nullptr)
      (*unknown)(line, *dev);
    else
      dev->println("?");
}

void Console::cmdList(void)
{
    for(auto i: handlers)
      dev->println(i.first);
}


String ArgList::getNextArg(char del)
{
  String p = "";
  if(idx < 0)
    return p;
  int j = args->indexOf(del, idx);
  if(j > -1)
  {
    p = args->substring(idx,j);
    while((*args)[++j] == ' ');
  }
  else
    p = args->substring(idx);
  idx = j;
  return p;
}
