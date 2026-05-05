/**
 * @file run_all.cpp
 * @brief Test runner. Invokes every test registered via the @c TEST macro
 *        in the @c tests/ subdirectory, prints a per-test PASS/FAIL line,
 *        and writes the same log to @c tests/unit_test_log.txt.
 *
 * Exit status is the number of failed tests (0 means all passed), so the
 * binary plays nicely with @c make and CI.
 */
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "RNG.h"
#include "test_harness.h"

namespace {

std::string nowIso8601() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
    localtime_r(&t, &tm);
    std::ostringstream os;
    os << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return os.str();
}

}  // namespace

int main() {
    constexpr std::uint64_t kSeed = 20260505;
    qsim::rng::seed(kSeed);

    std::ofstream log("tests/unit_test_log.txt");

    std::ostringstream banner;
    banner << "qsim unit test run\n"
           << "  timestamp : " << nowIso8601() << "\n"
           << "  rng seed  : " << kSeed << "\n\n";
    log      << banner.str();
    std::cout << banner.str();

    const int failures = qsim::testing::RunAllTests(log);

    log << std::flush;

    std::cout << "\nSee tests/unit_test_log.txt for details.\n";
    if (failures == 0) std::cout << "All tests passed.\n";
    else                std::cout << failures << " test(s) failed.\n";
    return failures;
}
