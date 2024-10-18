#include <catch2/catch_test_macros.hpp>

#include "hermes/container/spsc_queue.h"

using namespace hermes::container;

TEST_CASE("Init Test") { SpscQueue<int> queue; }
