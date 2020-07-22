# AcidJS
This is a fork of QuickJS that compatible with MSVC and using CMAKE to compile.
*All the modifications should only affect behaviors on Windows.*

## Build
```
cmake
```
and do what ever depending on the tool chain which you are using.

the compiler and the interpreter I've put it under "subdirectory" due to my poor level of writing cmake files.

## TODO
- [ ] support ATOMIC
- [ ] format define _MSC_VER which are flying around.

## License of QuickJS
QuickJS sources are copyright Fabrice Bellard and Charlie Gordon.
