// 2019-12-06

#pragma once

#include <map>

#define DEF_PROMPT ">"

class ArgList
{
  public:
  ArgList(String* p){args = p; idx = 0;}
  String getNextArg(char del = ' ');
  private:
  String* args;
  int idx;
};

using Handler  = void(*)(ArgList&, Stream&);
using HUnknown = void(*)(String&, Stream&);

class Console
{
  public:
  Console(Stream* s) {line.clear(); handlers.clear(); dev = s; prompt=DEF_PROMPT; unknown = nullptr;}
  Console(void): Console(&Serial){}
  void setPrompt(const String p = DEF_PROMPT) {prompt = p;}
  void start(void) {line.clear(); dev->println(); dev->print(prompt);}
  void run(void);
  bool busy(void) const {return !line.isEmpty();}
  Stream& stream(void) const {return *dev;};
  Stream& operator()(void) const {return stream();}
  void onCmd(const String& t, const Handler h){if(h != nullptr && !t.isEmpty()) handlers[t] = h;} // set handler for the command
  void onUnknown(HUnknown u) {unknown=u;}
  
  private:
  std::map<String,Handler> handlers; // dictionary command:handler
  Stream *dev;      // in/out device (stream)
  String line;      // command line: <command>[ <par1>[ <par2>[ ...]]]
  String prompt;    // current prompt
  HUnknown unknown; // handler for unrecognized commands
  void parse(void); // commands parsing
};

