#pragma once
// Tiny test harness. Tests are registered at static-init time via TEST(...).
// Each assertion records a pass/fail and a message but never throws, so a
// single test can report multiple failures. Header is single-file, header-only.

#include <cmath>
#include <complex>
#include <exception>
#include <functional>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace qsim::testing {

struct Test { std::string suite; std::string name; std::function<void()> fn; };

inline std::vector<Test>& registry()  { static std::vector<Test> v; return v; }
inline int& failures()                { static int f = 0; return f; }
inline std::ostringstream& details()  { static std::ostringstream s; return s; }

struct Registrar {
    Registrar(std::string suite, std::string name, std::function<void()> fn) {
        registry().push_back({std::move(suite), std::move(name), std::move(fn)});
    }
};

#define TEST(SUITE, NAME)                                                                       \
    static void SUITE##_##NAME##_body();                                                        \
    static qsim::testing::Registrar reg_##SUITE##_##NAME(#SUITE, #NAME, &SUITE##_##NAME##_body);\
    static void SUITE##_##NAME##_body()

#define QSIM_FAIL_(MSG) do { ++qsim::testing::failures(); qsim::testing::details() \
    << "    at " << __FILE__ << ":" << __LINE__ << "  " << MSG << "\n"; } while (0)

#define EXPECT_TRUE(c)            do { if (!(c)) QSIM_FAIL_("EXPECT_TRUE("  #c ")"); } while (0)
#define EXPECT_FALSE(c)           do { if  (c)  QSIM_FAIL_("EXPECT_FALSE(" #c ")"); } while (0)
#define EXPECT_EQ(a, b)           do { auto _x=(a); auto _y=(b); if (!(_x==_y))    \
    QSIM_FAIL_("EXPECT_EQ("       #a ", " #b ")"); } while (0)
#define EXPECT_NEAR(a, b, tol)    do { double _d = std::abs((double)(a)-(double)(b)); \
    if (_d > (tol)) QSIM_FAIL_("EXPECT_NEAR diff=" << _d); } while (0)
#define EXPECT_NEAR_COMPLEX(a, b, tol) do {                                                  \
    auto _d = std::abs(std::complex<double>(a) - std::complex<double>(b));                   \
    if (_d > (tol)) QSIM_FAIL_("EXPECT_NEAR_COMPLEX diff=" << _d); } while (0)
#define EXPECT_STATE_EQ(reg, expected, tol) do {                                             \
    const auto& _R = (reg); const auto& _E = (expected);                                     \
    if (_R.val.size() != _E.size()) QSIM_FAIL_("size mismatch");                             \
    else for (std::size_t _i = 0; _i < _R.val.size(); ++_i)                                  \
        if (std::abs(_R.val[_i] - _E[_i]) > (tol))                                           \
        { QSIM_FAIL_("amplitude " << _i << " differs by "                                    \
                     << std::abs(_R.val[_i] - _E[_i])); break; }                             \
    } while (0)
#define EXPECT_THROWS(stmt) do { bool _t=false; try { stmt; } catch (...) { _t=true; }       \
    if (!_t) QSIM_FAIL_("EXPECT_THROWS did not throw"); } while (0)

inline int RunAllTests(std::ostream& log) {
    int total = 0, failed = 0;
    for (const auto& t : registry()) {
        ++total;
        const int before = failures();
        details().str("");
        try { t.fn(); }
        catch (const std::exception& e) { QSIM_FAIL_("threw: " << e.what()); }
        catch (...)                     { QSIM_FAIL_("threw unknown exception"); }
        const int delta = failures() - before;
        log << (delta ? "  FAIL  " : "  PASS  ") << t.suite << "." << t.name << "\n";
        if (delta) { ++failed; log << details().str(); }
    }
    log << "\n[" << (total - failed) << "/" << total << " passed, " << failed << " failed]\n";
    return failed;
}

}  // namespace qsim::testing
