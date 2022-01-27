
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <clocale>
#include <filesystem>

#include "doctest.h"
//
#include "common/ExtensionManager.h"

TEST_CASE("Check ConversionTemplate") {  // 12/2/2020 -> 737761

    std::setlocale(LC_ALL, "en_US.UTF-8");

    ExtensionManager manager(true);


    /*
    CHECK(gregDate.getAbsoluteDate() == 737761);
    */
}

//#pragma clang diagnostic pop