#ifndef GAMELOGIC_HPP
#define GAMELOGIC_HPP

#include "GameTypes.hpp"
#include <touchgfx/hal/Types.hpp>

class GameLogic
{
public:
    static const int MAX_BULLETS = 20;
    static const int MAX_ENEMIES = 10;
    static const int MAX_ASTEROIDS = 3;
    static const int MAX_EXPLOSIONS = 8;
    static const int MAX_ITEMS = 3;

    GameLogic();

    void resetGame();
    void update();
    void handleKeyEvent(uint8_t key);

    // Entities
    int playerX, playerY;
    int playerWidth, playerHeight;
    int playerSpeed;

    // Background scroll position
    int backgroundY;

    Bullet bullets[MAX_BULLETS];
    Enemy enemies[MAX_ENEMIES];
    Asteroid asteroids[MAX_ASTEROIDS];
    Explosion explosions[MAX_EXPLOSIONS];
    Item items[MAX_ITEMS];

    // Spawning timers
    int enemySpawnTimer;
    int asteroidSpawnTimer;
    int shootCooldownTimer;

    // HUD State Variables
    int score;
    int highScore;
    int lives;
    int health;
    int weaponLevel;
    int gameTicks;
    bool hasShield;
    int ultCount;
    int flashScreenTimer;
    int level;
    int levelTransitionTimer;
    int playerHitFlashTimer;
    ScreenState screenState;
    bool showLeaderboard;
    int leaderboard[5];

    // Boss State Variables
    bool isBossFight;
    bool bossActive;
    float bossX, bossY;
    int bossHp;
    int bossMaxHp;
    float bossVx;
    int bossShootTimer;
    int bossHitFlashTimer;
    int bossWidth, bossHeight;
    int victoryTimer;
    int bossTargetPoint;
    int bossPauseTimer;
    bool bossIsPaused;

private:
    void spawnBullet();
    void spawnEnemy();
    void spawnAsteroid();
    bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
    void recordLeaderboardScore();
    void endGame();
};

#endif // GAMELOGIC_HPP
