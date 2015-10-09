/*
 * Copyright 2015 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE comm_tests
#include <boost/test/unit_test.hpp>

#include "comm/mutator.hpp"

BOOST_AUTO_TEST_CASE(mutator_test)
{
  using std::vector;
  using comm::block;
  using comm::mutator;
  using comm::signal;

  class test final : public mutator<float,float>
  {
  public:
    int operator()(const vector<signal<float>> & input,
                   vector<signal<float>> & output) override
    { return 0; }
  };

  {
    test obj{};
    block & ref = obj;
    ref();
  }

  block * ptr = new test{};
  (*ptr)();
  delete ptr;
}

