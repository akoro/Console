# Console
*Small console class for embedded systems*

Based on c++11 for Arduino framework
***
### Command line: `<command>[ <arg1>[ <arg2>[ ...]]]`

## Usage example:
```cpp
#include "console.h"
//...
Console con(&Serial); // Serial is descendant of Stream.
                      // Serial is default, you can just write: Console con;
//...
void handler1(ArgList& L, Stream& S) // command line is "command1 45:32 start"
{
  String a1 = L.getNextArg(':'); // delimiter is semicolon, the first arg is "45"
  String a2 = L.getNextArg(); // delimiter is space, the second arg is "32"
  String a3 = L.getNextArg(); // the third arg is "start"
  //...
  String an = L.getNextArg();
  // if L.getNextArg() returns an empty string then no more arguments in the command line
  //...
  S.print("hello!");
}

void handler2(ArgList& L, Stream& S)
{
  //...
}

void test(ArgList& L, Stream& S)
{  // you can enumerate all argumenth by the next code
  String p;
  int i=0;
  while(!(p = L.getNextArg()).isEmpty())
    S.printf("arg%d = \"%s\"\n", i++, p.c_str());
}

void wrong_command(String& L, Stream& S)
{
  S.print("what is the commamd? ");
  S.println(L);
}

setup()
{
  //...
  con.onCmd("command1", handler1);
  con.onCmd("command2", handler2);
  con.onCmd("test", test);
  con.onUnknown(wrong_command);
  con.start(); // prompt will appear here
}

loop()
{
  //...
  con.run();
  //...
}
```
## Class method description
1. Set a new prompt
```cpp 
void setPrompt(const String p = '>')
``` 
2. Start console. It makes CR, LF and put the prompt
```cpp
void start(void)
```
