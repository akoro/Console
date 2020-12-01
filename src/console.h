// https://github.com/akoro/Console
// 2019-12-12
// 2020-09-20 virtual functions, added begin()

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
  bool echo;
  Console(Stream* s = &Serial, bool e = true) {line=""; handlers.clear(); dev = s; echo = e; prompt=DEF_PROMPT; unknown = nullptr;}
  virtual bool begin(void){return true;}
  virtual void run(void);
  void setPrompt(const String p = DEF_PROMPT) {prompt = p;}
  void start(void) {line.clear(); dev->println(); dev->print(prompt);}
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
