#include "Global.h"
#include <regex>
using namespace std;

string Global::gameSessionId = "";
string Global::scoreStr = "0";
int Global::scoreInt = 0;
Global::Global() {}
Global::~Global() {}

string Global::getSessionIdFromHeader(string head) {
  regex nlp("\\r\\n");
  string header = regex_replace(head, nlp, " ");
  regex pattern(".*Set-Cookie: (.*) Content-Type.*");
  cmatch result;
  bool valid = regex_match(header.c_str(), result, pattern);
  if (!valid)
    return "";
  else 
    return result[1];
}