#include <gtest/gtest.h>
#include "rendering/Camera.h"
#include "core/Math.h"

using namespace Penumbra::Rendering;
using namespace Penumbra::Math;

class CameraTest : public ::testing::Test {
protected:
    void SetUp() override {
        camera.initialize(800.0f, 600.0f);
    }

    Camera camera;
};

TEST_F(CameraTest, Initialization) {
    EXPECT_FLOAT_EQ(camera.getViewportWidth(), 800.0f);
    EXPECT_FLOAT_EQ(camera.getViewportHeight(), 600.0f);
    EXPECT_FLOAT_EQ(camera.getZoom(), 1.0f);
}

TEST_F(CameraTest, PositionSetting) {
    camera.setPosition(100.0f, 200.0f);
    Vec2 pos = camera.getPosition();

    EXPECT_FLOAT_EQ(pos.x, 100.0f);
    EXPECT_FLOAT_EQ(pos.y, 200.0f);
}

TEST_F(CameraTest, ZoomSetting) {
    camera.setZoom(2.0f);
    EXPECT_FLOAT_EQ(camera.getZoom(), 2.0f);
}

TEST_F(CameraTest, ViewportResize) {
    camera.setViewportSize(1024.0f, 768.0f);

    EXPECT_FLOAT_EQ(camera.getViewportWidth(), 1024.0f);
    EXPECT_FLOAT_EQ(camera.getViewportHeight(), 768.0f);
}

TEST_F(CameraTest, BoundsConstraint) {
    camera.setBounds(0.0f, 0.0f, 100.0f, 100.0f);
    camera.setPosition(150.0f, 150.0f);

    Vec2 pos = camera.getPosition();

    EXPECT_LE(pos.x, 100.0f);
    EXPECT_LE(pos.y, 100.0f);
    EXPECT_GE(pos.x, 0.0f);
    EXPECT_GE(pos.y, 0.0f);
}

TEST_F(CameraTest, TargetFollowing) {
    camera.setMode(CameraMode::FollowPlayer);
    camera.setTarget(Vec2(100.0f, 100.0f));

    camera.update(0.016f);

    Vec2 pos = camera.getPosition();
    EXPECT_NE(pos.x, 0.0f);
    EXPECT_NE(pos.y, 0.0f);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
