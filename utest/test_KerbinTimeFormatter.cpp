#include <gtest/gtest.h>
#include "KerbinTimeFormatter.hpp"

// ── Constants ─────────────────────────────────────────────────────────────────

TEST(KerbinTimeFormatter, OneHourIs3600Seconds) {
    EXPECT_DOUBLE_EQ(KerbinTimeFormatter::HOUR, 3'600.0);
}

TEST(KerbinTimeFormatter, OneDayIsSixHours) {
    EXPECT_DOUBLE_EQ(KerbinTimeFormatter::DAY, 6.0 * KerbinTimeFormatter::HOUR);
}

TEST(KerbinTimeFormatter, OneYearIs426Days) {
    EXPECT_DOUBLE_EQ(KerbinTimeFormatter::YEAR, 426.0 * KerbinTimeFormatter::DAY);
}

// ── format() ──────────────────────────────────────────────────────────────────

TEST(KerbinTimeFormatter, ZeroSeconds) {
    KerbinTimeFormatter fmt;
    EXPECT_EQ(fmt.format(0.0), "0y 0d 0h 0m 0.0s");
}

TEST(KerbinTimeFormatter, ExactlyOneSecond) {
    KerbinTimeFormatter fmt;
    EXPECT_EQ(fmt.format(1.0), "0y 0d 0h 0m 1.0s");
}

TEST(KerbinTimeFormatter, ExactlyOneMinute) {
    KerbinTimeFormatter fmt;
    EXPECT_EQ(fmt.format(60.0), "0y 0d 0h 1m 0.0s");
}

TEST(KerbinTimeFormatter, ExactlyOneHour) {
    KerbinTimeFormatter fmt;
    EXPECT_EQ(fmt.format(KerbinTimeFormatter::HOUR), "0y 0d 1h 0m 0.0s");
}

TEST(KerbinTimeFormatter, ExactlyOneDay) {
    KerbinTimeFormatter fmt;
    EXPECT_EQ(fmt.format(KerbinTimeFormatter::DAY), "0y 1d 0h 0m 0.0s");
}

TEST(KerbinTimeFormatter, ExactlyOneYear) {
    KerbinTimeFormatter fmt;
    EXPECT_EQ(fmt.format(KerbinTimeFormatter::YEAR), "1y 0d 0h 0m 0.0s");
}

TEST(KerbinTimeFormatter, MixedValue) {
    // 2y 3d 1h 5m 7.5s
    KerbinTimeFormatter fmt;
    const double t = 2.0  * KerbinTimeFormatter::YEAR
                   + 3.0  * KerbinTimeFormatter::DAY
                   + 1.0  * KerbinTimeFormatter::HOUR
                   + 5.0  * 60.0
                   + 7.5;
    EXPECT_EQ(fmt.format(t), "2y 3d 1h 5m 7.5s");
}

TEST(KerbinTimeFormatter, KerbinRotPeriodFormatsCorrectly) {
    // Kerbin rotation: 21549.425 s = 0y 0d 5h 59m 9.4s
    KerbinTimeFormatter fmt;
    const double rot = 21'549.425;
    const std::string result = fmt.format(rot);
    EXPECT_EQ(result, "0y 0d 5h 59m 9.4s");
}

TEST(KerbinTimeFormatter, FractionalSecondsRoundedToOneDecimal) {
    KerbinTimeFormatter fmt;
    EXPECT_EQ(fmt.format(1.45), "0y 0d 0h 0m 1.5s");
    EXPECT_EQ(fmt.format(1.44), "0y 0d 0h 0m 1.4s");
}
