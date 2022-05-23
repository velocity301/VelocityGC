#pragma once
#include "VelocityGC.h"
#include "VelocityGC.cpp"

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