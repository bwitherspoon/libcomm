/*
 * Copyright 2015 C. Brett Witherspoon
 */

#define BOOST_TEST_MODULE comm_tests
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <functional>
#include <random>

#include "comm/buffer.hpp"

BOOST_AUTO_TEST_CASE(buffer_test)
{
  const std::size_t size = 2048;

  auto buf = comm::make_buffer_pair<float>(size);
  auto in = std::move(buf.first);
  auto out = std::move(buf.second);

  // Test construction invariants
  BOOST_REQUIRE_GE(in->size(), size);
  BOOST_REQUIRE_EQUAL(out->size(), 0);

  std::vector<float> ref(in->size());

  std::default_random_engine eng{};
  std::uniform_real_distribution<> dist{0.0, 1.0};
  auto rand = std::bind(dist, eng);

  // Copy data into buffer and compare it with the output
  std::generate(ref.begin(), ref.end(), rand);
  std::copy(ref.begin(), ref.end(), in->data());
  in->advance(ref.size());

  BOOST_REQUIRE_EQUAL(in->size(), 0);
  BOOST_REQUIRE_EQUAL(out->size(), ref.size());
  BOOST_CHECK_EQUAL_COLLECTIONS(
      ref.begin(), ref.end(), out->data(), out->data() + out->size());

  out->advance(ref.size());

  BOOST_REQUIRE_GE(in->size(), size);
  BOOST_REQUIRE_EQUAL(out->size(), 0);

  // Repeat to test circular buffering
  ref.resize(in->size() / 2);
  std::generate(ref.begin(), ref.end(), rand);
  std::copy(ref.begin(), ref.end(), in->data());
  in->advance(ref.size());

  BOOST_REQUIRE_EQUAL(out->size(), ref.size());
  BOOST_CHECK_EQUAL_COLLECTIONS(
      ref.begin(), ref.end(), out->data(), out->data() + out->size());
}
