#include <iostream>
#include <fstream>

#include "hermes/fastio/fastio.h"
#include "hermes/random/random.h"

int main() {
  // data
  const int TEST_SIZE = 1000;  
  const char data_path[] = "input.txt";
 
  std::vector<int> data;
  for(int i = 0; i < TEST_SIZE; ++i) {
    data.push_back(hermes::random::IntegralRandom::RandInt32(0));
  }

  std::ofstream out(data_path);
  for(int i = 0; i < TEST_SIZE; ++i) {
    out << data[i];
    if (hermes::random::IntegralRandom::RandBool())
      out << ' ';
    else 
      out << '\n';
  }
  out.close();

  hermes::fastio::IntegralFastIO<int> input;
  input.Init(data_path);

  int idx = 0, value;
  while (input.ReadOne(value)) {
    CHECK(value == data[idx]) << "Expected value = " << data[idx] << ", received value = " << value;
    idx += 1;
  }
  
  CHECK(idx == data.size()) << "Did not parsed all data: " << idx << "/" << data.size(); 
  LOG(INFO) << "PASSED";
}
