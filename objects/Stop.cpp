#include <Ice/Ice.h>
#include "SIPI.h"

using namespace std;

string
StopI::getName(const Ice::Current& current)
{
    return name_;
}