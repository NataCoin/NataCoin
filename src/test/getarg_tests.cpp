#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-Nata");
    BOOST_CHECK(GetBoolArg("-Nata"));
    BOOST_CHECK(GetBoolArg("-Nata", false));
    BOOST_CHECK(GetBoolArg("-Nata", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-Natao"));
    BOOST_CHECK(!GetBoolArg("-Natao", false));
    BOOST_CHECK(GetBoolArg("-Natao", true));

    ResetArgs("-Nata=0");
    BOOST_CHECK(!GetBoolArg("-Nata"));
    BOOST_CHECK(!GetBoolArg("-Nata", false));
    BOOST_CHECK(!GetBoolArg("-Nata", true));

    ResetArgs("-Nata=1");
    BOOST_CHECK(GetBoolArg("-Nata"));
    BOOST_CHECK(GetBoolArg("-Nata", false));
    BOOST_CHECK(GetBoolArg("-Nata", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noNata");
    BOOST_CHECK(!GetBoolArg("-Nata"));
    BOOST_CHECK(!GetBoolArg("-Nata", false));
    BOOST_CHECK(!GetBoolArg("-Nata", true));

    ResetArgs("-noNata=1");
    BOOST_CHECK(!GetBoolArg("-Nata"));
    BOOST_CHECK(!GetBoolArg("-Nata", false));
    BOOST_CHECK(!GetBoolArg("-Nata", true));

    ResetArgs("-Nata -noNata");  // -Nata should win
    BOOST_CHECK(GetBoolArg("-Nata"));
    BOOST_CHECK(GetBoolArg("-Nata", false));
    BOOST_CHECK(GetBoolArg("-Nata", true));

    ResetArgs("-Nata=1 -noNata=1");  // -Nata should win
    BOOST_CHECK(GetBoolArg("-Nata"));
    BOOST_CHECK(GetBoolArg("-Nata", false));
    BOOST_CHECK(GetBoolArg("-Nata", true));

    ResetArgs("-Nata=0 -noNata=0");  // -Nata should win
    BOOST_CHECK(!GetBoolArg("-Nata"));
    BOOST_CHECK(!GetBoolArg("-Nata", false));
    BOOST_CHECK(!GetBoolArg("-Nata", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--Nata=1");
    BOOST_CHECK(GetBoolArg("-Nata"));
    BOOST_CHECK(GetBoolArg("-Nata", false));
    BOOST_CHECK(GetBoolArg("-Nata", true));

    ResetArgs("--noNata=1");
    BOOST_CHECK(!GetBoolArg("-Nata"));
    BOOST_CHECK(!GetBoolArg("-Nata", false));
    BOOST_CHECK(!GetBoolArg("-Nata", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-Nata", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-Nata", "eleven"), "eleven");

    ResetArgs("-Nata -bar");
    BOOST_CHECK_EQUAL(GetArg("-Nata", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-Nata", "eleven"), "");

    ResetArgs("-Nata=");
    BOOST_CHECK_EQUAL(GetArg("-Nata", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-Nata", "eleven"), "");

    ResetArgs("-Nata=11");
    BOOST_CHECK_EQUAL(GetArg("-Nata", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-Nata", "eleven"), "11");

    ResetArgs("-Nata=eleven");
    BOOST_CHECK_EQUAL(GetArg("-Nata", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-Nata", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-Nata", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-Nata", 0), 0);

    ResetArgs("-Nata -bar");
    BOOST_CHECK_EQUAL(GetArg("-Nata", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-Nata=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-Nata", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-Nata=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-Nata", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--Nata");
    BOOST_CHECK_EQUAL(GetBoolArg("-Nata"), true);

    ResetArgs("--Nata=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-Nata", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noNata");
    BOOST_CHECK(!GetBoolArg("-Nata"));
    BOOST_CHECK(!GetBoolArg("-Nata", true));
    BOOST_CHECK(!GetBoolArg("-Nata", false));

    ResetArgs("-noNata=1");
    BOOST_CHECK(!GetBoolArg("-Nata"));
    BOOST_CHECK(!GetBoolArg("-Nata", true));
    BOOST_CHECK(!GetBoolArg("-Nata", false));

    ResetArgs("-noNata=0");
    BOOST_CHECK(GetBoolArg("-Nata"));
    BOOST_CHECK(GetBoolArg("-Nata", true));
    BOOST_CHECK(GetBoolArg("-Nata", false));

    ResetArgs("-Nata --noNata");
    BOOST_CHECK(GetBoolArg("-Nata"));

    ResetArgs("-noNata -Nata"); // Nata always wins:
    BOOST_CHECK(GetBoolArg("-Nata"));
}

BOOST_AUTO_TEST_SUITE_END()
