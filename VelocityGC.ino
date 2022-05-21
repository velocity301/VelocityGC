#include "VelocityGC.h"

VelocityGC Controller;

void setup()
{
  Controller.init();
}

void loop()
{
  Controller.readBtns();
  Controller.readCmd();
  Controller.write();
}