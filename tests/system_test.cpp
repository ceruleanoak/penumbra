#include <gtest/gtest.h>
#include "systems/RoomSystem.h"
#include "systems/ObjectFactory.h"
#include "core/Math.h"

using namespace Penumbra::Systems;
using namespace Penumbra::Game;
using namespace Penumbra::Math;

class RoomSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        roomSystem.initialize();
    }

    RoomSystem roomSystem;
};

TEST_F(RoomSystemTest, RoomCreation) {
    roomSystem.createRoom("test_room", 20, 15);

    EXPECT_TRUE(roomSystem.hasRoom("test_room"));

    const Room* room = roomSystem.getRoom("test_room");
    ASSERT_NE(room, nullptr);
    EXPECT_EQ(room->tileGrid.getWidth(), 20);
    EXPECT_EQ(room->tileGrid.getHeight(), 15);
}

TEST_F(RoomSystemTest, RoomLinking) {
    roomSystem.createRoom("room1", 10, 10);
    roomSystem.createRoom("room2", 10, 10);

    roomSystem.linkRooms("room1", "room2", TransitionDirection::North);

    const Room* room1 = roomSystem.getRoom("room1");
    ASSERT_NE(room1, nullptr);
    EXPECT_EQ(room1->northRoom, "room2");
}

TEST_F(RoomSystemTest, CurrentRoomSetting) {
    roomSystem.createRoom("start_room", 10, 10);

    bool success = roomSystem.setCurrentRoom("start_room");
    EXPECT_TRUE(success);
    EXPECT_EQ(roomSystem.getCurrentRoomID(), "start_room");
}

TEST_F(RoomSystemTest, RoomDiscovery) {
    roomSystem.createRoom("hidden_room", 10, 10);

    EXPECT_FALSE(roomSystem.isDiscovered("hidden_room"));

    roomSystem.markDiscovered("hidden_room");
    EXPECT_TRUE(roomSystem.isDiscovered("hidden_room"));
}

class ObjectFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(ObjectFactoryTest, EnemyCreation) {
    auto enemy = ObjectFactory::createEnemy("patrol", 100.0f, 200.0f);

    ASSERT_NE(enemy, nullptr);

    Vec2 pos = enemy->getPosition();
    EXPECT_FLOAT_EQ(pos.x, 100.0f);
    EXPECT_FLOAT_EQ(pos.y, 200.0f);
}

TEST_F(ObjectFactoryTest, StaticPlatformCreation) {
    auto platform = ObjectFactory::createStaticPlatform(50.0f, 50.0f, 64.0f, 16.0f);

    ASSERT_NE(platform, nullptr);

    AABB bounds = platform->getBounds();
    EXPECT_FLOAT_EQ(bounds.width(), 64.0f);
    EXPECT_FLOAT_EQ(bounds.height(), 16.0f);
}

TEST_F(ObjectFactoryTest, MovingPlatformCreation) {
    auto platform = ObjectFactory::createMovingPlatform(
        0.0f, 0.0f, 64.0f, 16.0f,
        100.0f, 0.0f, 50.0f
    );

    ASSERT_NE(platform, nullptr);
    EXPECT_TRUE(platform->isActive());
}

TEST_F(ObjectFactoryTest, BehaviorParsing) {
    EXPECT_EQ(ObjectFactory::parseEnemyBehavior("patrol"), EnemyBehavior::Patrol);
    EXPECT_EQ(ObjectFactory::parseEnemyBehavior("chase"), EnemyBehavior::Chase);
    EXPECT_EQ(ObjectFactory::parseEnemyBehavior("guard"), EnemyBehavior::Guard);
    EXPECT_EQ(ObjectFactory::parseEnemyBehavior("fly"), EnemyBehavior::Fly);
}

TEST_F(ObjectFactoryTest, PatternParsing) {
    EXPECT_EQ(ObjectFactory::parsePlatformPattern("static"), PlatformPattern::Static);
    EXPECT_EQ(ObjectFactory::parsePlatformPattern("linear"), PlatformPattern::LinearLoop);
    EXPECT_EQ(ObjectFactory::parsePlatformPattern("pingpong"), PlatformPattern::PingPong);
    EXPECT_EQ(ObjectFactory::parsePlatformPattern("circular"), PlatformPattern::Circular);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
