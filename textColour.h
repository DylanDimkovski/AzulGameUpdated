#ifndef TEXTCOLOUR_H
#define TEXTCOLOUR_H

#include <string>

#define TC_RED  std::string("\u001b[31m")
#define TC_YELLOW  std::string("\u001b[33m")
#define TC_DARKBLUE  std::string("\u001b[34m")
#define TC_LIGHTBLUE  std::string("\u001b[36m")
#define TC_BLACK  std::string("\u001b[30m")
#define TC_RESET  std::string("\u001b[0m")
#define TC_WHITE  std::string("\u001b[37m")
#define TC_MAGENTA std::string("\u001b[35m")
#define TC_BOLD std::string("\u001b[1m")
#define TC_BG_WHITE std::string("\u001b[1m\u001b[30m\u001b[47m")
#define TC_BG_LIGHTBLUE std::string("\u001b[1m\u001b[37m\u001b[46m")
#define TC_BG_DARKBLUE std::string("\u001b[1m\u001b[37m\u001b[44m")
#define TC_BG_RED std::string("\u001b[1m\u001b[37m\u001b[41m")
#define TC_BG_BLACK std::string("\u001b[1m\u001b[37m\u001b[40m")
#define TC_BG_YELLOW std::string("\u001b[1m\u001b[37m\u001b[43m")

#endif // !TEXTCOLOUR_H