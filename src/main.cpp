#include <raylib.h>
#include <stdio.h>

class Orang
{
private:
    int x;
    int y;
    Texture2D texture;

public:
    Orang(int posX, int posY, const char *imagePath)
    {
        x = posX;
        y = posY;
        texture = LoadTexture(imagePath);
    }

    void draw()
    {
        DrawTextureEx(texture, (Vector2){x, y}, 0, 2.0f, WHITE);
    }

    void update()
    {
        bool kanan = IsKeyDown(KEY_RIGHT);
        bool kiri = IsKeyDown(KEY_LEFT);
        bool atas = IsKeyDown(KEY_UP);
        bool bawah = IsKeyDown(KEY_DOWN);

        static int frame = 1;
        static int frameCounter = 0;
        const int frameSpeed = 8; // Mengatur kecepatan animasi (angka yang lebih tinggi akan memperlambat animasi)

        if (kanan)
        {
            x += 2;
            frameCounter++;
            if (frameCounter >= frameSpeed)
            {
                frameCounter = 0;
                frame = (frame % 4) + 1;
            }
            LoadAndSetTexture("src/assets/walk right1.png", "src/assets/walk right2.png", "src/assets/walk right3.png", "src/assets/walk right4.png", frame);
        }
        else if (kiri)
        {
            x -= 2;
            frameCounter++;
            if (frameCounter >= frameSpeed)
            {
                frameCounter = 0;
                frame = (frame % 4) + 1;
            }
            LoadAndSetTexture("src/assets/walk left1.png", "src/assets/walk left2.png", "src/assets/walk left3.png", "src/assets/walk left4.png", frame);
        }
        else if (atas)
        {
            y -= 2;
            frameCounter++;
            if (frameCounter >= frameSpeed)
            {
                frameCounter = 0;
                frame = (frame % 4) + 1;
            }
            LoadAndSetTexture("src/assets/walk up1.png", "src/assets/walk up2.png", "src/assets/walk up3.png", "src/assets/walk up4.png", frame);
        }
        else if (bawah)
        {
            y += 2;
            frameCounter++;
            if (frameCounter >= frameSpeed)
            {
                frameCounter = 0;
                frame = (frame % 4) + 1;
            }
            LoadAndSetTexture("src/assets/walk down1.png", "src/assets/walk down2.png", "src/assets/walk down3.png", "src/assets/walk down4.png", frame);
        }
        else
        {
            frameCounter = 0;
        }

        if (x > GetScreenWidth() - 64) // playerWidth adalah lebar dari sprite pemain
        {
            x = GetScreenWidth() - 64;
        }
        else if (x < 0 - 64)
        {
            x = 0 - 64;
        }
        if (y > GetScreenHeight() - 64) // playerHeight adalah tinggi dari sprite pemain
        {
            y = GetScreenHeight() - 64;
        }
        else if (y < 0 - 64)
        {
            y = 0 - 64;
        }
    }

    void LoadAndSetTexture(const char *frame1, const char *frame2, const char *frame3, const char *frame4, int frame)
    {
        if (frame == 1)
            texture = LoadTexture(frame1);
        else if (frame == 2)
            texture = LoadTexture(frame2);
        else if (frame == 3)
            texture = LoadTexture(frame3);
        else if (frame == 4)
            texture = LoadTexture(frame4);
    }

    Rectangle kotak()
    {
        float margin = 50.0f;
        return (Rectangle){(float)x + margin, (float)y + margin, texture.width * 2.0f - 2 * margin, texture.height * 2.0f - 2 * margin};
    }
};

class Meteor
{
public:
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
    Color color;

    Meteor() {}

    Meteor(Vector2 pos, Vector2 spd, float rad, bool act, Color col)
    {
        position = pos;
        speed = spd;
        radius = rad;
        active = act;
        color = col;
    }

    void update(int screenWidth, int screenHeight)
    {
        if (active)
        {
            position.x += speed.x;
            position.y += speed.y;
        }

        if (position.x > screenWidth + radius)
            position.x = -(radius);
        else if (position.x < 0 - radius)
            position.x = screenWidth + radius;
        if (position.y > screenHeight + radius)
            position.y = -(radius);
        else if (position.y < 0 - radius)
            position.y = screenHeight + radius;
    }

    void draw()
    {
        DrawCircleV(position, radius, color);
    }

    bool cekTabrakan(Rectangle rect)
    {
        return CheckCollisionCircleRec(position, radius, rect);
    }

    void resetPosition(int screenWidth, int screenHeight)
    {
        position = {GetRandomValue(100, screenWidth - 100), GetRandomValue(100, screenHeight - 100)};
        speed = {GetRandomValue(-2, 2), GetRandomValue(-2, 2)};
    }
};

enum MenuState
{
    MAIN_MENU,
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_WIN,
    GAME_LOSE
};

void SaveHighScore(int time)
{
    FILE *file = fopen("src/highscore.txt", "w");
    if (file != NULL)
    {
        fprintf(file, "%d\n", time);
        fclose(file);
    }
}

int LoadHighScore()
{
    int highscore = 0;
    FILE *file = fopen("src/highscore.txt", "r");
    if (file != NULL)
    {
        fscanf(file, "%d", &highscore);
        fclose(file);
    }
    return highscore;
}

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Game");
    InitAudioDevice();
    SetTargetFPS(60);

    Texture2D menu = LoadTexture("src/assets/BG/1.png");
    Texture2D pause = LoadTexture("src/assets/BG/3.png");
    Texture2D win = LoadTexture("src/assets/BG/5.png");
    Texture2D lose = LoadTexture("src/assets/BG/4.png");

    Orang orang1(200, 300, "src/assets/walk.png");

    Meteor meteors[30];

    for (int i = 0; i < 30; i++)
    {
        Vector2 position = {GetRandomValue(100, screenWidth - 100), GetRandomValue(100, screenHeight - 100)};
        Vector2 speed = {GetRandomValue(-2, 2), GetRandomValue(-2, 2)};
        float radius = GetRandomValue(10, 30);
        Color color = {GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255), 255};
        meteors[i] = Meteor(position, speed, radius, false, color);
    }

    MenuState currentState = MAIN_MENU;

    Music gameMusic = LoadMusicStream("src/assets/sakura.mp3");
    PlayMusicStream(gameMusic);
    int time = 0;
    int highscore = LoadHighScore();

    // Nyoba Gif
    int backgroundFrames = 0;
    Image backgroundImage = LoadImageAnim("src/assets/BG/glow.gif", &backgroundFrames);
    Texture2D backgroundTexture = LoadTextureFromImage(backgroundImage);
    unsigned int nextFrameDataOffset = 0;  // Offset byte saat ini ke frame berikutnya dalam image.data
    int currentBackgroundFrame = 0;       // Frame animasi saat ini untuk dimuat dan digambar
    int frameDelay = 8;                   // Penundaan frame untuk beralih antar frame animasi
    int frameCounter = 0;                 // Penghitung frame umum

    while (!WindowShouldClose())
    {
        UpdateMusicStream(gameMusic);

        // Nyoba Gif
        frameCounter++;
        if (frameCounter >= frameDelay && currentState == GAME_RUNNING)
        {
            currentBackgroundFrame++;
            if (currentBackgroundFrame >= backgroundFrames) currentBackgroundFrame = 0;

            nextFrameDataOffset = backgroundImage.width * backgroundImage.height * 4 * currentBackgroundFrame;
            UpdateTexture(backgroundTexture, ((unsigned char *)backgroundImage.data) + nextFrameDataOffset);

            frameCounter = 0;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentState)
        {
        case MAIN_MENU:
            DrawTexture(menu, 0, 0, WHITE);
            DrawText(TextFormat("HIGHSCORE: %d Detik", highscore), 10, 10, 20, BLACK);

            if (IsKeyPressed(KEY_ENTER))
            {
                currentState = GAME_RUNNING;
                time = 0;
                for (int i = 0; i < 30; i++)
                {
                    meteors[i].active = false;
                }
            }
            break;

        case GAME_RUNNING:
            DrawTexture(backgroundTexture, 0, 0, WHITE);

            time++;
            if (time > 180)
            {
                for (int i = 0; i < 30; i++)
                {
                    meteors[i].active = true;
                }
            }

            DrawText(TextFormat("TIME: %.02f ", (float)time / 60), 20, 20, 30, BLACK);
            

            orang1.draw();
            orang1.update();
            for (int i = 0; i < 30; i++)
            {
                meteors[i].update(screenWidth, screenHeight);
                meteors[i].draw();
                if (meteors[i].cekTabrakan(orang1.kotak()) && meteors[i].active)
                {
                    currentState = GAME_LOSE;
                    StopMusicStream(gameMusic);
                    if (time / 60 > highscore)
                    {
                        highscore = time / 60;
                        SaveHighScore(highscore);
                    }
                }
            }

            if (IsKeyPressed(KEY_P))
            {
                currentState = GAME_PAUSED;
                PauseMusicStream(gameMusic);
            }

            if (time / 60 == 60)
            {
                currentState = GAME_WIN;
                StopMusicStream(gameMusic);
            }

            if (time <= 180)
            {
                DrawText(TextFormat("%d", (3 - time / 60)), 370, 200, 150, BLACK);
            }

            break;

        case GAME_PAUSED:
            DrawTexture(pause, 0, 0, WHITE);
            DrawText("P For Resume", 15, 580, 15, WHITE);

            if (IsKeyPressed(KEY_P))
            {
                currentState = GAME_RUNNING;
                ResumeMusicStream(gameMusic);
            }
            break;

        case GAME_WIN:
            DrawTexture(win, 0, 0, WHITE);
            DrawText(TextFormat("TIME: %.02f", (float)time / 60), 10, 10, 20, BLACK);
            DrawText(TextFormat("HIGHSCORE: %d Detik", highscore), 10, 40, 20, BLACK);

            if (IsKeyPressed(KEY_ENTER))
            {
                currentState = GAME_RUNNING;
                time = 0;
                PlayMusicStream(gameMusic);
                for (int i = 0; i < 30; i++)
                {
                    meteors[i].resetPosition(screenWidth, screenHeight);
                    meteors[i].active = false;
                }
            }

            break;

        case GAME_LOSE:
            DrawTexture(lose, 0, 0, WHITE);
            DrawText(TextFormat("HIGHSCORE: %d", highscore), 10, 10, 20, BLACK);

            if (IsKeyPressed(KEY_ENTER))
            {
                currentState = GAME_RUNNING;
                time = 0;
                PlayMusicStream(gameMusic);
                for (int i = 0; i < 30; i++)
                {
                    meteors[i].resetPosition(screenWidth, screenHeight);
                    meteors[i].active = false;
                }
            }
            break;
        }

        EndDrawing();
    }

    UnloadTexture(backgroundTexture); 
    UnloadImage(backgroundImage);    
    UnloadMusicStream(gameMusic);
    StopMusicStream(gameMusic);

    CloseWindow();

    return 0;
}
