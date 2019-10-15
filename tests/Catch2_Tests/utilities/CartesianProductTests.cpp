#include "Catch.v2.7.1.hpp"
#include "ApprovalTests/utilities/CartesianProduct.h"

#include <string>
#include <vector>
#include <set>

using namespace ApprovalTests;

using Results = std::vector<std::string>;

// ------------------------------------------------------------------
// Tests which use a non-template object for accumulating results
// from CartesianProduct::cartesian_product(), to show the simplest
// way of using that function.
// ------------------------------------------------------------------

namespace
{
// A hard-coded struct for accumulating results
    struct AccumulateResults2StringsCommaSeparated
    {
        Results results;
        void operator()(std::string&& s1, std::string&& s2)
        {
            results.push_back(s1 + "," + s2);
        }
    };
}

TEST_CASE("Cartesian product with hard-coded-converter")
{
    const std::vector<std::string> input1{"hello"};
    const std::vector<std::string> input2{"world"};
    AccumulateResults2StringsCommaSeparated results_store;
    CartesianProduct::cartesian_product(results_store, input1, input2);
    const Results expected{"hello,world"};
    REQUIRE(results_store.results == expected);
}
