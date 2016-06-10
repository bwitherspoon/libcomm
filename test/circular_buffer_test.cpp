/*
 * Copyright 2015, 2016 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE signum_tests
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <functional>
#include <random>

#include "signum/circular_buffer.hpp"

namespace cb = signum::circular_buffer;

BOOST_AUTO_TEST_CASE(default_buffer_test)
{
    auto wr = cb::writer<float>();
    BOOST_REQUIRE_NE(wr.size(), 0);
    BOOST_REQUIRE_NE(wr.max_size(), 0);

    auto rd = wr.make_reader();
    BOOST_REQUIRE_EQUAL(rd.size(), 0);
}

BOOST_AUTO_TEST_CASE(buffer_test)
{
  const size_t sz = 2047;
  auto wr = cb::writer<float>(sz);
  auto rd = wr.make_reader();

  // Test equality operators
  {
    cb::reader<float> & rd_ = rd;
    BOOST_REQUIRE(rd == rd_);
  }
  BOOST_REQUIRE(wr == rd);

  // Test construction invariants
  BOOST_REQUIRE_EQUAL(wr.size(), sz);
  BOOST_REQUIRE_EQUAL(rd.size(), 0);

  // Generate random test data
  std::vector<float> ref(wr.size());

  std::default_random_engine eng{};
  std::uniform_real_distribution<> dist{0.0, 1.0};
  auto rand = std::bind(dist, eng);

  // Copy data into buffer and compare it with the output
  std::generate(ref.begin(), ref.end(), rand);
  std::copy(ref.begin(), ref.end(), wr.begin());
  wr.consume(ref.size());

  BOOST_REQUIRE_EQUAL(wr.size(), 0);
  BOOST_REQUIRE_EQUAL(rd.size(), ref.size());
  BOOST_CHECK_EQUAL_COLLECTIONS(
      ref.begin(), ref.end(), rd.begin(), rd.end());

  rd.consume(ref.size());

  BOOST_REQUIRE_GE(wr.size(), ref.size());
  BOOST_REQUIRE_EQUAL(rd.size(), 0);

  // Repeat to test circular buffering
  ref.resize(wr.size() / 2);
  std::generate(ref.begin(), ref.end(), rand);
  std::copy(ref.begin(), ref.end(), wr.begin());
  wr.consume(ref.size());

  BOOST_REQUIRE_EQUAL(rd.size(), ref.size());
  BOOST_CHECK_EQUAL_COLLECTIONS(
      ref.begin(), ref.end(), rd.begin(), rd.end());
}
