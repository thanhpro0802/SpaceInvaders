#ifndef GAMECANVAS_HPP
#define GAMECANVAS_HPP

#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/Color.hpp>

#include <images/BitmapDatabase.hpp>

class GameCanvas : public touchgfx::Widget
{
public:
    GameCanvas();
    virtual ~GameCanvas() {}

    virtual void draw(const touchgfx::Rect& invalidatedArea) const override;
    virtual touchgfx::Rect getSolidRect() const override;

    // Input handlers
    virtual void handleClickEvent(const touchgfx::ClickEvent& event) override;
    virtual void handleDragEvent(const touchgfx::DragEvent& event) override;
    void handleKeyEvent(uint8_t key);

    // Game loop tick update
    void update();

private:
    // Game state structures
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

    static const int MAX_BULLETS = 20;
    static const int MAX_ENEMIES = 10;
    static const int MAX_ASTEROIDS = 3;
    static const int MAX_EXPLOSIONS = 8;
    static const int MAX_ITEMS = 3;

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

    // Helper functions
    void drawBitmap(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, uint16_t bitmapId) const;
    void drawCircle(const touchgfx::Rect& invalidatedArea, int16_t cx, int16_t cy, int r, touchgfx::colortype col) const;
    void drawChar(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, char c, touchgfx::colortype col) const;
    void drawString(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, const char* str, touchgfx::colortype col) const;
    void drawScaledChar(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, char c, int scale, touchgfx::colortype col) const;
    void drawScaledString(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, const char* str, int scale, touchgfx::colortype col) const;
    void drawOutlinedScaledString(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, const char* str, int scale, touchgfx::colortype fill, touchgfx::colortype outline) const;
    void fillRoundedRect(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, int16_t w, int16_t h, int radius, touchgfx::colortype col) const;
    void drawButton(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, int16_t w, int16_t h, const char* label, bool selected) const;
    void drawStartOverlay(const touchgfx::Rect& invalidatedArea) const;
    void drawGameOverOverlay(const touchgfx::Rect& invalidatedArea) const;
    void drawStippleFlash(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, uint16_t bitmapId, touchgfx::colortype col) const;
    void spawnBullet();
    void spawnEnemy();
    void spawnAsteroid();
    bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
    void resetGame();
    void endGame();
    void recordLeaderboardScore();
};

#endif // GAMECANVAS_HPP
