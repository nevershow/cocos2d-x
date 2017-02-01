#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
using namespace std;
class Global {
public:
  Global();
  ~Global();
  static string gameSessionId;
  static string scoreStr;
  static int scoreInt;
  static string getSessionIdFromHeader(string header);
};

#endif