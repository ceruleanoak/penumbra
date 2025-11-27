#include <gtest/gtest.h>
#include "game/TileGrid.h"
#include "game/Player.h"
#include "core/Math.h"

using namespace Penumbra::Game;
using namespace Penumbra::Math;

class TileGridTest : public ::testing::Test {
protected:
    void SetUp() override {
        grid.initialize(10, 10);
    }

    TileGrid grid;
};

TEST_F(TileGridTest, Initialization) {
    EXPECT_EQ(grid.getWidth(), 10);
    EXPECT_EQ(grid.getHeight(), 10);
    EXPECT_EQ(grid.getTileSize(), 16);
}

TEST_F(TileGridTest, TileSetting) {
    Tile solidTile(TileType::Solid, 0);
    grid.setTile(5, 5, solidTile);

    const Tile& retrieved = grid.getTile(5, 5);
    EXPECT_EQ(retrieved.type, TileType::Solid);
}

TEST_F(TileGridTest, ValidPosition) {
    EXPECT_TRUE(grid.isValidPosition(0, 0));
    EXPECT_TRUE(grid.isValidPosition(9, 9));
    EXPECT_FALSE(grid.isValidPosition(-1, 0));
    EXPECT_FALSE(grid.isValidPosition(10, 0));
    EXPECT_FALSE(grid.isValidPosition(0, 10));
}

TEST_F(TileGridTest, WorldToGrid) {
    int gridX, gridY;
    grid.worldToGrid(32.0f, 48.0f, gridX, gridY);

    EXPECT_EQ(gridX, 2);
    EXPECT_EQ(gridY, 3);
}

TEST_F(TileGridTest, GridToWorld) {
    float worldX, worldY;
    grid.gridToWorld(2, 3, worldX, worldY);

    EXPECT_FLOAT_EQ(worldX, 32.0f);
    EXPECT_FLOAT_EQ(worldY, 48.0f);
}

TEST_F(TileGridTest, CollisionDetection) {
    Tile solidTile(TileType::Solid, 0);
    grid.setTile(5, 5, solidTile);

    AABB bounds(80.0f, 80.0f, 16.0f, 16.0f);

    EXPECT_TRUE(grid.checkCollision(bounds));

    AABB noBounds(0.0f, 0.0f, 16.0f, 16.0f);
    EXPECT_FALSE(grid.checkCollision(noBounds));
}

class PlayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        player.initialize(100.0f, 100.0f);
        grid.initialize(20, 20);
    }

    Player player;
    TileGrid grid;
};

TEST_F(PlayerTest, Initialization) {
    Vec2 pos = player.getPosition();
    EXPECT_FLOAT_EQ(pos.x, 100.0f);
    EXPECT_FLOAT_EQ(pos.y, 100.0f);
    EXPECT_TRUE(player.isAlive());
}

TEST_F(PlayerTest, HealthSystem) {
    int initialHealth = player.getHealth();

    player.takeDamage(10);
    EXPECT_EQ(player.getHealth(), initialHealth - 10);

    player.heal(5);
    EXPECT_EQ(player.getHealth(), initialHealth - 5);
}

TEST_F(PlayerTest, DeathState) {
    player.takeDamage(player.getMaxHealth());
    EXPECT_FALSE(player.isAlive());
}

TEST_F(PlayerTest, Respawn) {
    player.takeDamage(player.getMaxHealth());
    EXPECT_FALSE(player.isAlive());

    player.respawn(50.0f, 50.0f);
    EXPECT_TRUE(player.isAlive());

    Vec2 pos = player.getPosition();
    EXPECT_FLOAT_EQ(pos.x, 50.0f);
    EXPECT_FLOAT_EQ(pos.y, 50.0f);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
