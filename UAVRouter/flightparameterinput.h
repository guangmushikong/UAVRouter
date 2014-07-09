#ifndef FLIGHTPARAMETERINPUT_H
#define FLIGHTPARAMETERINPUT_H

#include <string>
using namespace std;

struct CmpByKeyDictionaryValue {
  bool operator()(const string& k1, const string& k2) {
    return k1 < k2;
  }
};

class FlightParameterInput
{
public:
    FlightParameterInput();
};

#endif // FLIGHTPARAMETERINPUT_H
