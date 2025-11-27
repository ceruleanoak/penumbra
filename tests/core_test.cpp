#include <gtest/gtest.h>
#include "core/Math.h"

using namespace Penumbra::Math;

class MathTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(MathTest, AABBConstruction) {
    AABB box(0.0f, 0.0f, 10.0f, 20.0f);

    EXPECT_FLOAT_EQ(box.width(), 10.0f);
    EXPECT_FLOAT_EQ(box.height(), 20.0f);
    EXPECT_FLOAT_EQ(box.center().x, 5.0f);
    EXPECT_FLOAT_EQ(box.center().y, 10.0f);
}

TEST_F(MathTest, AABBContainsPoint) {
    AABB box(0.0f, 0.0f, 10.0f, 10.0f);

    EXPECT_TRUE(box.contains(Vec2(5.0f, 5.0f)));
    EXPECT_TRUE(box.contains(Vec2(0.0f, 0.0f)));
    EXPECT_TRUE(box.contains(Vec2(10.0f, 10.0f)));
    EXPECT_FALSE(box.contains(Vec2(-1.0f, 5.0f)));
    EXPECT_FALSE(box.contains(Vec2(11.0f, 5.0f)));
}

TEST_F(MathTest, AABBIntersection) {
    AABB box1(0.0f, 0.0f, 10.0f, 10.0f);
    AABB box2(5.0f, 5.0f, 10.0f, 10.0f);
    AABB box3(20.0f, 20.0f, 5.0f, 5.0f);

    EXPECT_TRUE(box1.intersects(box2));
    EXPECT_TRUE(box2.intersects(box1));
    EXPECT_FALSE(box1.intersects(box3));
    EXPECT_FALSE(box3.intersects(box1));
}

TEST_F(MathTest, LerpFunction) {
    EXPECT_FLOAT_EQ(lerp(0.0f, 10.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ(lerp(0.0f, 10.0f, 1.0f), 10.0f);
    EXPECT_FLOAT_EQ(lerp(0.0f, 10.0f, 0.5f), 5.0f);
}

TEST_F(MathTest, ClampFunction) {
    EXPECT_FLOAT_EQ(clamp(5.0f, 0.0f, 10.0f), 5.0f);
    EXPECT_FLOAT_EQ(clamp(-5.0f, 0.0f, 10.0f), 0.0f);
    EXPECT_FLOAT_EQ(clamp(15.0f, 0.0f, 10.0f), 10.0f);
}

TEST_F(MathTest, ApproxEqual) {
    EXPECT_TRUE(approxEqual(1.0f, 1.0f));
    EXPECT_TRUE(approxEqual(1.0f, 1.000001f));
    EXPECT_FALSE(approxEqual(1.0f, 1.1f));
}

TEST_F(MathTest, AngleConversion) {
    float degrees = 180.0f;
    float radians = toRadians(degrees);
    float backToDegrees = toDegrees(radians);

    EXPECT_NEAR(radians, 3.14159265359f, 0.0001f);
    EXPECT_NEAR(backToDegrees, degrees, 0.0001f);
}

TEST_F(MathTest, ColorConstruction) {
    Color white(1.0f, 1.0f, 1.0f, 1.0f);
    Vec4 whiteVec = white.toVec4();

    EXPECT_FLOAT_EQ(whiteVec.r, 1.0f);
    EXPECT_FLOAT_EQ(whiteVec.g, 1.0f);
    EXPECT_FLOAT_EQ(whiteVec.b, 1.0f);
    EXPECT_FLOAT_EQ(whiteVec.a, 1.0f);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
