/*
Simple user console
Can be used on any Arduino compatible board
V1.0.0 2019-12-12
V1.0.1 2019-12-19 added ArgList::isEmpty
*/

#pragma once
#include <map>

#define DEF_PROMPT    ">"  // Default prompt of console
#define DEF_DELIMITER ' '  // default args delimiter

// helper for exttracting separate arguments of command line
class ArgList
{
  public:
  ArgList(String* p): args(p), idx(0) {}
  String getNextArg(char del = DEF_DELIMITER);
  bool isEmpty(void) {return args->isEmpty();}
  private:
  String* args;
  int idx;
};

// Command handlers types
using Handler  = void (*)(ArgList&, Stream&);
using HUnknown = void (*)(String&, Stream&);

// Console class
class Console
{
  public:
  Console(Stream* s = &Serial): dev(s), prompt(DEF_PROMPT), unknown(nullptr)
    {line.clear(); handlers.clear();}
  void setPrompt(const String p = DEF_PROMPT) {prompt = p;}
  void start(void) {line.clear(); dev->println(); dev->print(prompt);}
  void run(void);
  bool busy(void) const {return !line.isEmpty();}
  Stream& stream(void) const {return *dev;};
  Stream& operator()(void) const {return stream();}
  void onCmd(const String& t, const Handler h)
    {if(h != nullptr && !t.isEmpty()) handlers[t] = h;} // set handler for the command
  void onUnknown(HUnknown u) {unknown = u;}
  void cmdList(void);

  private:
  std::map<String,Handler> handlers; // dictionary command:handler
  Stream *dev;      // in/out device (stream)
  String line;      // command line: <command>[ <par1>[ <par2>[ ...]]]
  String prompt;    // current prompt
  HUnknown unknown; // handler for unrecognized commands
  void parse(void); // commands parsing
};
