#include <gtest/gtest.h>
#include <algorithm>
#include "KspBodyDatabase.hpp"

// ── Helpers ───────────────────────────────────────────────────────────────────

static bool contains_name(const std::vector<CelestialBody>& v, std::string_view name) {
    return std::any_of(v.begin(), v.end(),
        [name](const CelestialBody& b){ return b.name == name; });
}

// ── Catalogue size ────────────────────────────────────────────────────────────

TEST(KspBodyDatabase, ContainsAllSeventeenBodies) {
    KspBodyDatabase db;
    EXPECT_EQ(db.all().size(), 17u);
}

// ── find() ────────────────────────────────────────────────────────────────────

TEST(KspBodyDatabase, FindKerbinReturnsBody) {
    KspBodyDatabase db;
    auto body = db.find("Kerbin");
    ASSERT_TRUE(body.has_value());
    EXPECT_EQ(body->name, "Kerbin");
}

TEST(KspBodyDatabase, FindKerbinRadius) {
    KspBodyDatabase db;
    auto body = db.find("Kerbin");
    ASSERT_TRUE(body.has_value());
    EXPECT_DOUBLE_EQ(body->radius, 600'000.0);
}

TEST(KspBodyDatabase, FindKerbinMu) {
    KspBodyDatabase db;
    auto body = db.find("Kerbin");
    ASSERT_TRUE(body.has_value());
    EXPECT_NEAR(body->mu, 3.5316e12, 1e8);
}

TEST(KspBodyDatabase, FindKerbinAtmosphere) {
    KspBodyDatabase db;
    auto body = db.find("Kerbin");
    ASSERT_TRUE(body.has_value());
    EXPECT_DOUBLE_EQ(body->atm, 70'000.0);
}

TEST(KspBodyDatabase, FindNonExistentBodyReturnsNullopt) {
    KspBodyDatabase db;
    EXPECT_FALSE(db.find("Mars").has_value());
    EXPECT_FALSE(db.find("").has_value());
}

TEST(KspBodyDatabase, FindKerbolHasNoSoi) {
    KspBodyDatabase db;
    auto body = db.find("Kerbol");
    ASSERT_TRUE(body.has_value());
    EXPECT_FALSE(body->soi.has_value());
}

TEST(KspBodyDatabase, FindKerbolHasNoParent) {
    KspBodyDatabase db;
    auto body = db.find("Kerbol");
    ASSERT_TRUE(body.has_value());
    EXPECT_FALSE(body->parent.has_value());
}

TEST(KspBodyDatabase, FindKerbinSoi) {
    KspBodyDatabase db;
    auto body = db.find("Kerbin");
    ASSERT_TRUE(body.has_value());
    ASSERT_TRUE(body->soi.has_value());
    EXPECT_NEAR(*body->soi, 84'159'286.0, 1.0);
}

TEST(KspBodyDatabase, FindMunParentIsKerbin) {
    KspBodyDatabase db;
    auto body = db.find("Mun");
    ASSERT_TRUE(body.has_value());
    ASSERT_TRUE(body->parent.has_value());
    EXPECT_EQ(*body->parent, "Kerbin");
}

// ── children_of() ─────────────────────────────────────────────────────────────

TEST(KspBodyDatabase, ChildrenOfKerbinHasTwoBodies) {
    KspBodyDatabase db;
    EXPECT_EQ(db.children_of("Kerbin").size(), 2u);
}

TEST(KspBodyDatabase, ChildrenOfKerbinContainsMun) {
    KspBodyDatabase db;
    EXPECT_TRUE(contains_name(db.children_of("Kerbin"), "Mun"));
}

TEST(KspBodyDatabase, ChildrenOfKerbinContainsMinmus) {
    KspBodyDatabase db;
    EXPECT_TRUE(contains_name(db.children_of("Kerbin"), "Minmus"));
}

TEST(KspBodyDatabase, ChildrenOfKerbinSortedByOrbitalRadius) {
    KspBodyDatabase db;
    auto kids = db.children_of("Kerbin");
    ASSERT_EQ(kids.size(), 2u);
    // Mun (12 000 km) closer than Minmus (47 000 km)
    EXPECT_EQ(kids[0].name, "Mun");
    EXPECT_EQ(kids[1].name, "Minmus");
    EXPECT_LT(*kids[0].orbital_radius, *kids[1].orbital_radius);
}

TEST(KspBodyDatabase, ChildrenOfKerbolHasSevenPlanets) {
    KspBodyDatabase db;
    EXPECT_EQ(db.children_of("Kerbol").size(), 7u);
}

TEST(KspBodyDatabase, ChildrenOfKerbolInCorrectOrder) {
    KspBodyDatabase db;
    auto kids = db.children_of("Kerbol");
    ASSERT_EQ(kids.size(), 7u);
    std::vector<std::string> expected = {
        "Moho", "Eve", "Kerbin", "Duna", "Dres", "Jool", "Eeloo"
    };
    for (std::size_t i = 0; i < expected.size(); ++i)
        EXPECT_EQ(kids[i].name, expected[i]) << "mismatch at index " << i;
}

TEST(KspBodyDatabase, ChildrenOfJoolHasFiveMoons) {
    KspBodyDatabase db;
    EXPECT_EQ(db.children_of("Jool").size(), 5u);
}

TEST(KspBodyDatabase, ChildrenOfDunaHasOnemoon) {
    KspBodyDatabase db;
    auto kids = db.children_of("Duna");
    ASSERT_EQ(kids.size(), 1u);
    EXPECT_EQ(kids[0].name, "Ike");
}

TEST(KspBodyDatabase, BodyWithNoChildrenReturnsEmpty) {
    KspBodyDatabase db;
    EXPECT_TRUE(db.children_of("Moho").empty());
    EXPECT_TRUE(db.children_of("Mun").empty());
    EXPECT_TRUE(db.children_of("Eeloo").empty());
}
