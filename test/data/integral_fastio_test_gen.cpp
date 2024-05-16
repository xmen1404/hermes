#include <iostream>
#include <fstream>
#include <glog/logging.h>
#include <random>
#include <ctime> 

int main() 
{
  std::srand(std::time(nullptr));
  std::ofstream out("integral_fastio_test.input");
  out << "15";
  out.flush();
  out.close();
}

