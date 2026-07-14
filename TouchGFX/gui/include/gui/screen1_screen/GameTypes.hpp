#ifndef GAMETYPES_HPP
#define GAMETYPES_HPP

#include <touchgfx/hal/Types.hpp>

enum EnemyType {
    ENEMY_TARGETED = 0,
    ENEMY_ZIGZAG,
    ENEMY_HORIZONTAL,
    ENEMY_BURST_SHOOTER,
    ENEMY_BALL_SHOOTER,
    ENEMY_DIAGONAL_DASH
};

struct Bullet {
    bool active;
    float x, y;
    int width, height;
    float vx, vy;
    bool isEnemyBullet; // true if fired by enemy
    bool isBallBullet;  // true if targeted yellow ball bullet
    int animFrame;
};

struct Enemy {
    bool active;
    EnemyType type;
    float x, y;
    float spawnX; // base X for zigzag
    float spawnY; // base Y for horizontal
    int width, height;
    float vx, vy;
    int hp;
    int hitFlashTimer;
    float time;
    int shootTimer;
};

struct Asteroid {
    bool active;
    float x, y;
    int width, height;
    float vy;
    uint16_t bitmapId;
};

struct Explosion {
    bool active;
    float x, y;
    int frame;
};

enum ItemType {
    ITEM_HEAL = 0,
    ITEM_SHIELD,
    ITEM_ULT,
    ITEM_WEAPON
};

enum ScreenState {
    STATE_START = 0,
    STATE_PLAYING,
    STATE_GAME_OVER
};

struct Item {
    bool active;
    ItemType type;
    float x, y;
    int width, height;
    float vy;
};

#endif // GAMETYPES_HPP
