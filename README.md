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
## Class methods description
### 1. Set a new prompt
```cpp 
void setPrompt(const String p = '>')
``` 
### 2. Start console. It makes CR, LF and put the prompt
```cpp
void start(void)
```
### 3. Run console in ethernal cycle
```cpp
void run(void)
```
### 4. Input is busy
```cpp
bool busy(void)
```
returns **true** if you start input a command.
It is useful if your program continuously outputs some info and when you need input a command the program will stop output the info.
### 5. Get console stream referense
```cpp
Stream& stream(void)
```
and
```cpp
Stream& operator()(void)
```
Returns a reference to console stream. Usage: `con().println();` or `con.stream.println();`
### 6. Set command handler
```cpp
void onCmd(const String&, const Handler)
```
here **Handler** is function address: `typedef void (*Handler)(ArgList&, Stream&)`. **ArgList** - command arguments list, **Stream** - console stream referense.
### 7. Set unknown command handle
```cpp
void onUnknown(HUnknown u)
```
here **UHandler** is function address: `typedef void (*UHandler)(String&, Stream&)`. **String** - whole command line
### 8. Getting separate arguments from command line
Class **ArgList** has single method
```cpp
String getNextArg(const char del = ' ')
```
returns the first argument delimited by the **del** symbol from the next one. Each next call of the method allows you get the hext argument. If the function returns an empty string it means no more arguments in the command line. The arguments list can be accessed in command handlers only. Default delimiter is space.
