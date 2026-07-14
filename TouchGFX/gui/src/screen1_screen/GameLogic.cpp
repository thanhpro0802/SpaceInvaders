#include <gui/screen1_screen/GameLogic.hpp>
#include <images/BitmapDatabase.hpp>
#include <touchgfx/Bitmap.hpp>
#include <stdlib.h>
#include <math.h>

GameLogic::GameLogic()
{
    playerWidth = touchgfx::Bitmap(BITMAP_PLAYER_SHIP_1_ID).getWidth();
    playerHeight = touchgfx::Bitmap(BITMAP_PLAYER_SHIP_1_ID).getHeight();
    playerSpeed = 5;
    highScore = 18000;
    for (int i = 0; i < 5; i++) leaderboard[i] = 0;
    leaderboard[0] = highScore;
    screenState = STATE_PLAYING;
    showLeaderboard = false;
    resetGame();
    screenState = STATE_START;
}

void GameLogic::resetGame()
{
    playerX = (240 - playerWidth) / 2;
    playerY = 320 - playerHeight - 30;

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].active = false;
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].active = false;
    }

    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        asteroids[i].active = false;
    }

    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        explosions[i].active = false;
    }

    for (int i = 0; i < MAX_ITEMS; i++)
    {
        items[i].active = false;
    }

    backgroundY = 0;
    enemySpawnTimer = 0;
    asteroidSpawnTimer = 0;
    shootCooldownTimer = 0;

    // HUD Init
    score = 0;
    lives = 3;
    health = 100;
    weaponLevel = 1;
    gameTicks = 0;
    hasShield = false;
    ultCount = 0;
    flashScreenTimer = 0;
    level = 1;
    levelTransitionTimer = 0;
    playerHitFlashTimer = 0;
    screenState = STATE_PLAYING;
    showLeaderboard = false;
    
    isBossFight = false;
    bossActive = false;
    bossHp = 0;
    bossMaxHp = 0;
    bossHitFlashTimer = 0;
    bossWidth = 48; // will be updated from bitmap once loaded
    bossHeight = 48;
    victoryTimer = 0;
}

void GameLogic::update()
{
    if (screenState == STATE_START)
    {
        backgroundY += 2;
            return;
    }

    if (screenState == STATE_GAME_OVER)
    {
        backgroundY += 1;
            return;
    }

    // Tick game time
    gameTicks++;

    // Trigger transitions every 2400px of background travel.
    if (!isBossFight && backgroundY >= level * 2400 && levelTransitionTimer == 0)
    {
        level++;
        levelTransitionTimer = 120; // 2 seconds (120 frames at 60fps)
        if ((level % 5) == 0)
        {
            isBossFight = true;
            bossActive = true;
            bossHp = (200 + (level / 5 - 1) * 60);
            bossMaxHp = bossHp;
            bossWidth = 128;
            bossHeight = 128;
            bossX = 120 - bossWidth / 2;
            bossY = -bossHeight;
            bossVx = 1.5f;
            bossShootTimer = 0;
            bossHitFlashTimer = 0;
            flashScreenTimer = 15;
            victoryTimer = 0;

            bossTargetPoint = 0;
            bossPauseTimer = 0;
            bossIsPaused = false;
        }
    }

    int scrollSpeed = 1;
    if (isBossFight)
    {
        scrollSpeed = 0; // Freeze background scrolling during boss battle!
    }
    else if (levelTransitionTimer > 0)
    {
        levelTransitionTimer--;
        if (levelTransitionTimer > 60)
        {
            scrollSpeed = 12; // Warp speed!
        }
        else
        {
            scrollSpeed = 1 + (levelTransitionTimer * 11 / 60); // Decelerating back
        }
    }

    backgroundY += scrollSpeed;

    if (flashScreenTimer > 0)
    {
        flashScreenTimer--;
    }
    if (playerHitFlashTimer > 0)
    {
        playerHitFlashTimer--;
    }

    // Auto-fire player bullets based on weapon level
    if (shootCooldownTimer > 0)
    {
        shootCooldownTimer--;
    }
    else
    {
        spawnBullet();
        shootCooldownTimer = 15;
    }

    // Update bullets (both player and enemy)
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            bullets[i].x += bullets[i].vx;
            bullets[i].y += bullets[i].vy;
            if (bullets[i].isBallBullet)
            {
                bullets[i].animFrame++;
            }
            if (bullets[i].y < 32 || bullets[i].y > 320 || bullets[i].x < -20 || bullets[i].x > 260)
            {
                bullets[i].active = false;
            }
        }
    }

    // Update dropped items
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (items[i].active)
        {
            items[i].y += items[i].vy;
            
            // Collision with Player
            if (checkCollision((int)items[i].x - 9, (int)items[i].y - 9, 18, 18,
                               playerX, playerY, playerWidth, playerHeight))
            {
                items[i].active = false;
                
                switch (items[i].type)
                {
                case ITEM_HEAL:
                    health = (health + 50 > 100) ? 100 : health + 50;
                    score += 200;
                    break;
                case ITEM_SHIELD:
                    hasShield = true;
                    score += 200;
                    break;
                case ITEM_ULT:
                    ultCount = (ultCount < 3) ? ultCount + 1 : 3;
                    score += 200;
                    break;
                case ITEM_WEAPON:
                    weaponLevel = (weaponLevel < 3) ? weaponLevel + 1 : 3;
                    score += 500;
                    break;
                }
                
                if (score > highScore) highScore = score;
            }
            else if (items[i].y > 320)
            {
                items[i].active = false;
            }
        }
    }

    // Spawn enemies (suppressed during hyperjump and boss fight)
    if (levelTransitionTimer > 60 || isBossFight)
    {
        // Don't spawn new enemies during warp or boss fight!
    }
    else
    {
        if (enemySpawnTimer > 0)
        {
            enemySpawnTimer--;
        }
        else
        {
            spawnEnemy();
            int spawnDelay = 45 - (level - 1) * 4;
            if (spawnDelay < 18) spawnDelay = 18;
            enemySpawnTimer = spawnDelay;
        }
    }

    // Update enemies behavior
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].active)
        {
            enemies[i].time += 1.0f / 60.0f;
            float dx, dy, dist;

            switch (enemies[i].type)
            {
            case ENEMY_TARGETED:
                dx = playerX - enemies[i].x;
                dy = playerY - enemies[i].y;
                dist = sqrtf(dx * dx + dy * dy);
                if (dist > 5.0f)
                {
                    enemies[i].x += (dx / dist) * (20.0f / 60.0f);
                    enemies[i].y += (dy / dist) * (20.0f / 60.0f);
                }
                else
                {
                    enemies[i].y += 20.0f / 60.0f;
                }
                break;

            case ENEMY_ZIGZAG:
                enemies[i].x = enemies[i].spawnX + sinf(enemies[i].time * 3.0f) * 60.0f;
                enemies[i].y += 30.0f / 60.0f;
                break;

            case ENEMY_HORIZONTAL:
                enemies[i].x += enemies[i].vx * (35.0f / 60.0f);
                enemies[i].y = enemies[i].spawnY - sinf(enemies[i].time * 1.5f) * 80.0f;
                
                if ((enemies[i].vx > 0 && enemies[i].x > 240) || (enemies[i].vx < 0 && enemies[i].x < -enemies[i].width) || enemies[i].y > 320)
                {
                    enemies[i].active = false;
                }
                break;

            case ENEMY_BURST_SHOOTER:
                enemies[i].y += 12.0f / 60.0f;
                enemies[i].shootTimer++;
                if (enemies[i].shootTimer >= 180)
                {
                    enemies[i].shootTimer = 0;
                    float centerX = enemies[i].x + enemies[i].width / 2.0f;
                    float baseY = enemies[i].y + enemies[i].height;
                    
                    for (int step = 0; step < 4; step++)
                    {
                        for (int b = 0; b < MAX_BULLETS; b++)
                        {
                            if (!bullets[b].active)
                            {
                                bullets[b].active = true;
                                bullets[b].isEnemyBullet = true;
                                bullets[b].isBallBullet = true;
                                bullets[b].animFrame = step * 3;
                                bullets[b].width = 10;
                                bullets[b].height = 10;
                                
                                if (step == 0)      { bullets[b].x = centerX;       bullets[b].y = baseY; }
                                else if (step == 1) { bullets[b].x = centerX - 4.0f;  bullets[b].y = baseY - 10.0f; }
                                else if (step == 2) { bullets[b].x = centerX + 4.0f;  bullets[b].y = baseY - 20.0f; }
                                else if (step == 3) { bullets[b].x = centerX;       bullets[b].y = baseY - 30.0f; }
                                
                                bullets[b].vx = 0.0f;
                                bullets[b].vy = 1.2f;
                                break;
                            }
                        }
                    }
                }
                break;

            case ENEMY_BALL_SHOOTER:
                enemies[i].y += 15.0f / 60.0f;
                enemies[i].shootTimer++;
                if (enemies[i].shootTimer >= 160)
                {
                    enemies[i].shootTimer = 0;
                    for (int b = 0; b < MAX_BULLETS; b++)
                    {
                        if (!bullets[b].active)
                        {
                            bullets[b].active = true;
                            bullets[b].isEnemyBullet = true;
                            bullets[b].isBallBullet = true;
                            bullets[b].animFrame = 0;
                            bullets[b].width = 10;
                            bullets[b].height = 10;
                            bullets[b].x = enemies[i].x + (enemies[i].width - bullets[b].width) / 2;
                            bullets[b].y = enemies[i].y + enemies[i].height;
                            
                            float bx = playerX + playerWidth / 2 - bullets[b].x;
                            float by = playerY + playerHeight / 2 - bullets[b].y;
                            float bdist = sqrtf(bx * bx + by * by);
                            if (bdist > 0)
                            {
                                bullets[b].vx = (bx / bdist) * 0.8f;
                                bullets[b].vy = (by / bdist) * 0.8f;
                            }
                            else
                            {
                                bullets[b].vx = 0.0f;
                                bullets[b].vy = 0.8f;
                            }
                            break;
                        }
                    }
                }
                break;

            case ENEMY_DIAGONAL_DASH:
            default:
                enemies[i].x += enemies[i].vx * (150.0f / 60.0f);
                enemies[i].y += enemies[i].vy * (150.0f / 60.0f);
                break;
            }

            if (enemies[i].hitFlashTimer > 0)
            {
                enemies[i].hitFlashTimer--;
            }

            if (enemies[i].type != ENEMY_HORIZONTAL && enemies[i].y > 320)
            {
                enemies[i].active = false;
            }
        }
    }

    // Spawn asteroids (suppressed during hyperjump and boss fight)
    if (levelTransitionTimer > 60 || isBossFight)
    {
        // Don't spawn new asteroids during warp or boss fight!
    }
    else
    {
        if (asteroidSpawnTimer > 0)
        {
            asteroidSpawnTimer--;
        }
        else
        {
            spawnAsteroid();
            asteroidSpawnTimer = 480;
        }
    }

    // Update asteroids
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroids[i].active)
        {
            asteroids[i].y += asteroids[i].vy;
            if (asteroids[i].y > 320)
            {
                asteroids[i].active = false;
            }
        }
    }

    // Update sprite explosions
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        if (explosions[i].active)
        {
            explosions[i].frame++;
            if (explosions[i].frame >= 10)
            {
                explosions[i].active = false;
            }
        }
    }

    // Bullet Collisions (Forgiving shrunken hitboxes)
    for (int b = 0; b < MAX_BULLETS; b++)
    {
        if (!bullets[b].active) continue;

        if (bullets[b].isEnemyBullet)
        {
            if (checkCollision((int)bullets[b].x + 2, (int)bullets[b].y + 2, bullets[b].width - 4, bullets[b].height - 4,
                               playerX + 6, playerY + 6, playerWidth - 12, playerHeight - 12))
            {
                bullets[b].active = false;
                
                if (hasShield)
                {
                    hasShield = false; // Shield absorbed!
                }
                else
                {
                    playerHitFlashTimer = 15; // Nhấp nháy xanh lá trong 15 khung hình
                    int dmg = isBossFight ? 10 : 20;
                    health -= dmg; // Giảm sát thương của đạn khi đấu Boss
                    if (health <= 0)
                    {
                        lives--;
                        health = 100;
                        weaponLevel = 1; // reset weapon level on death
                        if (lives < 0)
                        {
                            endGame();
                            return;
                        }
                    }
                }
            }
        }
        else
        {
            for (int e = 0; e < MAX_ENEMIES; e++)
            {
                if (!enemies[e].active) continue;
                if (checkCollision((int)bullets[b].x + 2, (int)bullets[b].y + 2, bullets[b].width - 4, bullets[b].height - 4,
                                   (int)enemies[e].x + 6, (int)enemies[e].y + 6, enemies[e].width - 12, enemies[e].height - 12))
                {
                    bullets[b].active = false;
                    enemies[e].hp--;
                    enemies[e].hitFlashTimer = 5; // Nhấp nháy đỏ trong 5 khung hình

                    if (enemies[e].hp <= 0)
                    {
                        enemies[e].active = false;

                        // Trigger Explosion
                        for (int ex = 0; ex < MAX_EXPLOSIONS; ex++)
                        {
                            if (!explosions[ex].active)
                            {
                                explosions[ex].active = true;
                                explosions[ex].x = enemies[e].x + enemies[e].width / 2;
                                explosions[ex].y = enemies[e].y + enemies[e].height / 2;
                                explosions[ex].frame = 0;
                                break;
                            }
                        }

                        // Reward Score
                        score += 100;
                        if (score > highScore) highScore = score;

                        // 15% Chance to drop bonus items
                        if (rand() % 100 < 15)
                        {
                            for (int it = 0; it < MAX_ITEMS; it++)
                            {
                                if (!items[it].active)
                                {
                                    items[it].active = true;
                                    items[it].x = enemies[e].x + enemies[e].width / 2;
                                    items[it].y = enemies[e].y + enemies[e].height / 2;
                                    items[it].vy = 1.0f;
                                    
                                    // Choose item type based on random weights
                                    int roll = rand() % 100;
                                    if (roll < 35)      items[it].type = ITEM_WEAPON; // 35% chance
                                    else if (roll < 60) items[it].type = ITEM_HEAL;   // 25% chance
                                    else if (roll < 80) items[it].type = ITEM_SHIELD; // 20% chance
                                    else                items[it].type = ITEM_ULT;    // 20% chance
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    // Player collision with Enemy or Asteroid (Forgiving shrunken hitboxes)
    for (int e = 0; e < MAX_ENEMIES; e++)
    {
        if (enemies[e].active && checkCollision(playerX + 6, playerY + 6, playerWidth - 12, playerHeight - 12,
                                                (int)enemies[e].x + 6, (int)enemies[e].y + 6, enemies[e].width - 12, enemies[e].height - 12))
        {
            enemies[e].active = false;
            // Spawn explosion for enemy
            for (int ex = 0; ex < MAX_EXPLOSIONS; ex++)
            {
                if (!explosions[ex].active)
                {
                    explosions[ex].active = true;
                    explosions[ex].x = enemies[e].x + enemies[e].width / 2;
                    explosions[ex].y = enemies[e].y + enemies[e].height / 2;
                    explosions[ex].frame = 0;
                    break;
                }
            }
            
            if (hasShield)
            {
                hasShield = false; // Shield absorbed!
            }
            else
            {
                playerHitFlashTimer = 15; // Nhấp nháy xanh lá trong 15 khung hình
                health -= 40; // Heavy collision damage
                if (health <= 0)
                {
                    lives--;
                    health = 100;
                    weaponLevel = 1; // reset weapon level
                    if (lives < 0)
                    {
                        endGame();
                        return;
                    }
                }
            }
        }
    }
    for (int a = 0; a < MAX_ASTEROIDS; a++)
    {
        if (asteroids[a].active && checkCollision(playerX + 6, playerY + 6, playerWidth - 12, playerHeight - 12,
                                                  (int)asteroids[a].x + 6, (int)asteroids[a].y + 6, asteroids[a].width - 12, asteroids[a].height - 12))
        {
            asteroids[a].active = false;
            
            if (hasShield)
            {
                hasShield = false; // Shield absorbed!
            }
            else
            {
                playerHitFlashTimer = 15; // Nhấp nháy xanh lá trong 15 khung hình
                health -= 30; // Asteroid damage
                if (health <= 0)
                {
                    lives--;
                    health = 100;
                    weaponLevel = 1; // reset weapon level
                    if (lives < 0)
                    {
                        endGame();
                        return;
                    }
                }
            }
        }
    }
    // --- Boss Fight update loop ---
    if (bossActive)
    {
        if (bossHitFlashTimer > 0)
        {
            bossHitFlashTimer--;
        }

        // 1. Move boss down to start position ty[0] (32) first
        if (bossY < 32.0f && !bossIsPaused && bossTargetPoint == 0)
        {
            bossY += 1.0f;
        }
        else
        {
            // Triangle movement points
            // Triangle movement points (Căn lại cho Boss 128x128 trên màn hình 240x320)
            float tx[3] = {10.0f, 102.0f, 56.0f};
            float ty[3] = {32.0f, 32.0f, 140.0f};
            
            if (bossIsPaused)
            {
                bossPauseTimer--;
                
                // 2. Boss Weapon: Shoot 2 vertical streams of yellow ball bullets
                bossShootTimer++;
                if (bossShootTimer >= 12) // Rapid fire during pause!
                {
                    bossShootTimer = 0;
                    
                    // Left launcher stream (Cánh trái boss 128x128)
                    for (int b = 0; b < MAX_BULLETS; b++)
                    {
                        if (!bullets[b].active)
                        {
                            bullets[b].active = true;
                            bullets[b].isEnemyBullet = true;
                            bullets[b].isBallBullet = true;
                            bullets[b].width = 16;
                            bullets[b].height = 16;
                            bullets[b].x = bossX + 20 - 8;
                            bullets[b].y = bossY + 96;
                            bullets[b].vx = 0.0f;
                            bullets[b].vy = 3.5f;
                            bullets[b].animFrame = 0;
                            break;
                        }
                    }
                    
                    // Right launcher stream (Cánh phải boss 128x128 - Cách xa 80 pixel)
                    for (int b = 0; b < MAX_BULLETS; b++)
                    {
                        if (!bullets[b].active)
                        {
                            bullets[b].active = true;
                            bullets[b].isEnemyBullet = true;
                            bullets[b].isBallBullet = true;
                            bullets[b].width = 16;
                            bullets[b].height = 16;
                            bullets[b].x = bossX + 100 - 8;
                            bullets[b].y = bossY + 96;
                            bullets[b].vx = 0.0f;
                            bullets[b].vy = 3.5f;
                            bullets[b].animFrame = 0;
                            break;
                        }
                    }
                }
                
                if (bossPauseTimer <= 0)
                {
                    bossIsPaused = false;
                    bossTargetPoint = (bossTargetPoint + 1) % 3;
                }
            }
            else
            {
                // Move towards current target point
                float dx = tx[bossTargetPoint] - bossX;
                float dy = ty[bossTargetPoint] - bossY;
                float dist = sqrtf(dx * dx + dy * dy);
                
                if (dist < 4.0f)
                {
                    bossIsPaused = true;
                    bossPauseTimer = 180; // 3 seconds pause
                    bossShootTimer = 0;
                }
                else
                {
                    // Move speed = 1.4 pixels per frame
                    bossX += (dx / dist) * 1.4f;
                    bossY += (dy / dist) * 1.4f;
                }
            }
        }

        // 3. Collision: Player Bullets hitting Boss
        for (int b = 0; b < MAX_BULLETS; b++)
        {
            if (bullets[b].active && !bullets[b].isEnemyBullet)
            {
                if (checkCollision((int)bullets[b].x, (int)bullets[b].y, bullets[b].width, bullets[b].height,
                                   (int)bossX + 4, (int)bossY + 4, bossWidth - 8, bossHeight - 8))
                {
                    bullets[b].active = false;
                    bossHp -= 8; // Buff damage cho người chơi để kết liễu nhanh hơn
                    bossHitFlashTimer = 5; // nháy đỏ

                    if (bossHp <= 0)
                    {
                        bossActive = false;
                        
                        // Spawn massive chain of explosions
                        for (int k = 0; k < 6; k++)
                        {
                            for (int ex = 0; ex < MAX_EXPLOSIONS; ex++)
                            {
                                if (!explosions[ex].active)
                                {
                                    explosions[ex].active = true;
                                    explosions[ex].x = bossX + (rand() % bossWidth);
                                    explosions[ex].y = bossY + (rand() % bossHeight);
                                    explosions[ex].frame = -k * 3; // staggered delay explosion!
                                    break;
                                }
                            }
                        }

                        score += 10000; // 10k points for boss!
                        if (score > highScore) highScore = score;

                        victoryTimer = 240; // 4 seconds victory screen
                    }
                }
            }
        }

        // 4. Collision: Player Ship colliding with Boss
        if (checkCollision(playerX + 6, playerY + 6, playerWidth - 12, playerHeight - 12,
                           (int)bossX + 4, (int)bossY + 4, bossWidth - 8, bossHeight - 8))
        {
            if (hasShield)
            {
                hasShield = false;
            }
            else
            {
                playerHitFlashTimer = 15;
                health -= 50; // Heavy collision damage
                if (health <= 0)
                {
                    lives--;
                    health = 100;
                    weaponLevel = 1;
                    if (lives < 0)
                    {
                        endGame();
                        return;
                    }
                }
            }
        }
    }

    // Victory screen countdown timer
    if (victoryTimer > 0)
    {
        victoryTimer--;
        if (victoryTimer == 1)
        {
            isBossFight = false;
            bossActive = false;
            victoryTimer = 0;
            level++;
            backgroundY = (level - 1) * 2400;
            levelTransitionTimer = 120;
        }
    }

    // Request screen repaint
}

void GameLogic::spawnBullet()
{
    touchgfx::Bitmap bulletBmp(BITMAP_SHOOT_1_ID);
    int bW = bulletBmp.getWidth();
    int bH = bulletBmp.getHeight();

    if (weaponLevel == 1)
    {
        // Level 1: Single straight bullet
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullets[i].active)
            {
                bullets[i].active = true;
                bullets[i].isEnemyBullet = false;
                bullets[i].isBallBullet = false;
                bullets[i].width = bW;
                bullets[i].height = bH;
                bullets[i].x = playerX + (playerWidth - bW) / 2;
                bullets[i].y = playerY - bH;
                bullets[i].vx = 0.0f;
                bullets[i].vy = -6.0f;
                break;
            }
        }
    }
    else if (weaponLevel == 2)
    {
        // Level 2: Double parallel bullets
        int spawned = 0;
        for (int i = 0; i < MAX_BULLETS && spawned < 2; i++)
        {
            if (!bullets[i].active)
            {
                bullets[i].active = true;
                bullets[i].isEnemyBullet = false;
                bullets[i].isBallBullet = false;
                bullets[i].width = bW;
                bullets[i].height = bH;
                bullets[i].y = playerY - bH;
                bullets[i].vx = 0.0f;
                bullets[i].vy = -6.0f;
                
                if (spawned == 0)
                    bullets[i].x = playerX + playerWidth / 2 - 8;
                else
                    bullets[i].x = playerX + playerWidth / 2 + 4;
                    
                spawned++;
            }
        }
    }
    else
    {
        // Level 3: Triple spread bullets
        int spawned = 0;
        for (int i = 0; i < MAX_BULLETS && spawned < 3; i++)
        {
            if (!bullets[i].active)
            {
                bullets[i].active = true;
                bullets[i].isEnemyBullet = false;
                bullets[i].isBallBullet = false;
                bullets[i].width = bW;
                bullets[i].height = bH;
                bullets[i].x = playerX + (playerWidth - bW) / 2;
                bullets[i].y = playerY - bH;
                
                if (spawned == 0)
                {
                    bullets[i].vx = -1.2f;
                    bullets[i].vy = -5.8f;
                }
                else if (spawned == 1)
                {
                    bullets[i].vx = 0.0f;
                    bullets[i].vy = -6.0f;
                }
                else
                {
                    bullets[i].vx = 1.2f;
                    bullets[i].vy = -5.8f;
                }
                
                spawned++;
            }
        }
    }
}

void GameLogic::spawnEnemy()
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
        {
            enemies[i].active = true;
            enemies[i].time = 0.0f;
            enemies[i].shootTimer = rand() % 40;
            enemies[i].hitFlashTimer = 0;
            int enemyHp = level;
            if (enemyHp > 8) enemyHp = 8;
            enemies[i].hp = enemyHp;

            int typeRand = rand() % 6;
            if (backgroundY < 2400)
            {
                while (typeRand == ENEMY_BURST_SHOOTER || typeRand == ENEMY_BALL_SHOOTER)
                {
                    typeRand = rand() % 6;
                }
            }
            else if (backgroundY < 4800)
            {
                if ((typeRand == ENEMY_BURST_SHOOTER || typeRand == ENEMY_BALL_SHOOTER) && (rand() % 100 < 70))
                {
                    while (typeRand == ENEMY_BURST_SHOOTER || typeRand == ENEMY_BALL_SHOOTER)
                    {
                        typeRand = rand() % 6;
                    }
                }
            }
            enemies[i].type = (EnemyType)typeRand;
            if (enemies[i].type == ENEMY_BURST_SHOOTER || enemies[i].type == ENEMY_BALL_SHOOTER)
            {
                enemies[i].hp = enemyHp + 1;
            }

            uint16_t enemyBmpId = BITMAP_ENEMY_SHIP_1_ID;
            switch (enemies[i].type)
            {
            case ENEMY_TARGETED: enemyBmpId = BITMAP_ENEMY_SHIP_TARGETED_ID; break;
            case ENEMY_ZIGZAG: enemyBmpId = BITMAP_ENEMY_SHIP_ZIGZAG_ID; break;
            case ENEMY_HORIZONTAL: enemyBmpId = BITMAP_ENEMY_SHIP_HORIZONTAL_ID; break;
            case ENEMY_BURST_SHOOTER: enemyBmpId = BITMAP_ENEMY_SHIP_STRAIGHT_ID; break;
            case ENEMY_BALL_SHOOTER: enemyBmpId = BITMAP_ENEMY_SHIP_SPIRAL_ID; break;
            case ENEMY_DIAGONAL_DASH: enemyBmpId = BITMAP_ENEMY_SHIP_SHOOTER_ID; break;
            }

            touchgfx::Bitmap enemyBmp(enemyBmpId);
            int eW = enemyBmp.getWidth();
            int eH = enemyBmp.getHeight();
            enemies[i].width = eW;
            enemies[i].height = eH;

            switch (enemies[i].type)
            {
            case ENEMY_TARGETED:
                enemies[i].x = rand() % (240 - eW);
                enemies[i].y = -eH;
                enemies[i].vy = 1.2f;
                break;
            case ENEMY_ZIGZAG:
                enemies[i].spawnX = 50 + rand() % (140 - eW);
                enemies[i].x = enemies[i].spawnX;
                enemies[i].y = -eH;
                enemies[i].vy = 1.5f;
                break;
            case ENEMY_HORIZONTAL:
                if (rand() % 2 == 0)
                {
                    enemies[i].x = -eW;
                    enemies[i].vx = 1.5f;
                }
                else
                {
                    enemies[i].x = 240;
                    enemies[i].vx = -1.5f;
                }
                enemies[i].y = 120.0f;
                enemies[i].spawnY = enemies[i].y;
                break;
            case ENEMY_BURST_SHOOTER:
                enemies[i].x = rand() % (240 - eW);
                enemies[i].y = -eH;
                enemies[i].vy = 0.8f;
                break;
            case ENEMY_BALL_SHOOTER:
                enemies[i].x = rand() % (240 - eW);
                enemies[i].y = -eH;
                enemies[i].vy = 0.6f;
                break;
            case ENEMY_DIAGONAL_DASH:
            default:
                if (rand() % 2 == 0)
                {
                    enemies[i].x = -eW;
                    enemies[i].vx = 0.707f;
                }
                else
                {
                    enemies[i].x = 240;
                    enemies[i].vx = -0.707f;
                }
                enemies[i].y = -eH;
                enemies[i].vy = 0.707f;
                break;
            }
            break;
        }
    }
}

void GameLogic::spawnAsteroid()
{
    uint16_t selectedBmp = BITMAP_ASTEROID_RED_ID;
    touchgfx::Bitmap astBmp(selectedBmp);
    int aW = astBmp.getWidth();
    int aH = astBmp.getHeight();

    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (!asteroids[i].active)
        {
            asteroids[i].active = true;
            asteroids[i].bitmapId = selectedBmp;
            asteroids[i].width = aW;
            asteroids[i].height = aH;
            asteroids[i].x = rand() % (240 - aW);
            asteroids[i].y = -aH;
            asteroids[i].vy = 0.5f + (float)(rand() % 10) / 10.0f;
            break;
        }
    }
}

bool GameLogic::checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2);
}

void GameLogic::recordLeaderboardScore()
{
    for (int i = 0; i < 5; i++)
    {
        if (score > leaderboard[i])
        {
            for (int j = 4; j > i; j--)
            {
                leaderboard[j] = leaderboard[j - 1];
            }
            leaderboard[i] = score;
            break;
        }
    }
}

void GameLogic::endGame()
{
    if (score > highScore)
    {
        highScore = score;
    }
    recordLeaderboardScore();
    screenState = STATE_GAME_OVER;
    showLeaderboard = false;
}

void GameLogic::handleKeyEvent(uint8_t key)
{
    if (screenState == STATE_START)
    {
        if (key == '\r' || key == '\n' || key == ' ')
        {
            resetGame();
        }
        else if (key == 'l' || key == 'L' || key == 't' || key == 'T')
        {
            showLeaderboard = !showLeaderboard;
        }
            return;
    }

    if (screenState == STATE_GAME_OVER)
    {
        if (key == '\r' || key == '\n' || key == ' ')
        {
            resetGame();
        }
        else if (key == 'l' || key == 'L' || key == 't' || key == 'T')
        {
            showLeaderboard = !showLeaderboard;
        }
        else if (key == 'm' || key == 'M')
        {
            resetGame();
            screenState = STATE_START;
            showLeaderboard = false;
        }
            return;
    }

    int step = 15;
    if (key == 'w' || key == 'W')
    {
        playerY -= step;
    }
    else if (key == 's' || key == 'S')
    {
        playerY += step;
    }
    else if (key == 'a' || key == 'A')
    {
        playerX -= step;
    }
    else if (key == 'd' || key == 'D')
    {
        playerX += step;
    }
    else if (key == ' ' || key == 32)
    {
        // Ultimate Bomb: Clear all screen enemies and bullets!
        if (ultCount > 0)
        {
            ultCount--;
            flashScreenTimer = 8; // Kích hoạt 8 khung hình nhấp nháy toàn màn hình

            // Kích nổ hàng loạt vụ nổ ngẫu nhiên trên toàn màn hình
            for (int j = 0; j < 15; j++)
            {
                for (int ex = 0; ex < MAX_EXPLOSIONS; ex++)
                {
                    if (!explosions[ex].active)
                    {
                        explosions[ex].active = true;
                        explosions[ex].x = 10 + (rand() % 220);
                        explosions[ex].y = 40 + (rand() % 260);
                        explosions[ex].frame = rand() % 4; // Bắt đầu ở các frame ngẫu nhiên để tạo sự tự nhiên
                        break;
                    }
                }
            }

            for (int e = 0; e < MAX_ENEMIES; e++)
            {
                if (enemies[e].active)
                {
                    enemies[e].active = false;
                    score += 100;
                    if (score > highScore) highScore = score;
                }
            }
            // Clear enemy bullets
            for (int b = 0; b < MAX_BULLETS; b++)
            {
                if (bullets[b].active && bullets[b].isEnemyBullet)
                {
                    bullets[b].active = false;
                }
            }
        }
    }

    // Bounds checking (accounting for top HUD bar)
    if (playerX < 0) playerX = 0;
    if (playerX > 240 - playerWidth) playerX = 240 - playerWidth;
    if (playerY < 32) playerY = 32;
    if (playerY > 320 - playerHeight) playerY = 320 - playerHeight;

}

