// begin-snippet: show_deprecation_warnings
#define APPROVAL_TESTS_SHOW_DEPRECATION_WARNINGS 1
// end-snippet

// begin-snippet: hide_deprecated_code
#define APPROVAL_TESTS_HIDE_DEPRECATED_CODE 1
// end-snippet

#include "doctest/doctest.h"
#include "ApprovalTests/Approvals.h"

using namespace ApprovalTests;

TEST_CASE("TestStreamableObject")
{
    Approvals::verify(42);
}

TEST_CASE("CLion demo")
{
    // Trivial example code for https://youtrack.jetbrains.com/issue/CPP-1420
    int x = 2;
    int xsquared = x * x;
    CHECK(xsquared == 4);
}
