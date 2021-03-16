#include "agents.h"
#define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_associations) {
    // check associations have length n agents
    BOOST_CHECK(pop.associations.size() == pop.nAgents);
}