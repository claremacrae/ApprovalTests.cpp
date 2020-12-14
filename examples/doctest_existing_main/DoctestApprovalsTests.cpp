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

TEST_CASE("CLion demo") static int getInt(int x)
{
    int xsquared = x * x;
    return xsquared;}
{
    // Trivial example code for https://youtrack.jetbrains.com/issue/CPP-1420
    int x = 2;
    int xsquared = getInt(x);
    CHECK(xsquared == 4);
}

// Steps done:
// * Select line 23
// * Extract method
// * Select "Above"
// * Click "OK"
// * Click "Continue" to ignore warning about duplicate getInt() function - which is in a different program

// Output as generated by CLion 2020.3
// Build #CL-203.5981.166, built on December 1, 2020
