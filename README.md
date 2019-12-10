# Console
*Small console class for embedded systems*
***
## Command line:

`<command> [ <arg1>[ <arg2>[ ...]]]`

## Usage:
```c++
Console con(&Serial); // Serial is descendant of Stream.
                      // Serial is default, you can just write: Console con;
...
void handler1(ArgList& L, Stream& S) // command line is "command1 45:32 start"
{
  String a1 = L.getNextArg(':'); // delimiter is semicolon, the first arg is "45"
  String a2 = L.getNextArg(); // delimiter is space, the second arg is "32"
  String a3 = L.getNextArg(); // the third arg is "start"
  ...
  String an = L.getNextArg();
  // if L.getNextArg() returns empty string then no more arguments in the command line
  ...
  S.print("hello!");
}

    void handler2(ArgList& L, Stream& S)
    {
      ...
    }

    void wrong_command(String& L, Stream& S)
    {
      S.print("what is the commamd? ");
      S.println(L);
    }

    setup()
    {
      ...
      con.onCmd("command1", handler1);
      con.onCmd("command2", handler2);
      con.onUnknown(wrong_command);
      con.start();
    }

    loop()
    {
      ...
      con.run();
      ...
    }
```
