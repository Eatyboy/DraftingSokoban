#include <vector>
#include <raylib.h>
#include <raymath.h>
#include <int2.h>
#include <tilemap.h>
#include <stdio.h>
#include <gameObjects.h>
#include <utils.h>
#include <levels.h>
#include <ui.h>

Int2 screenSize = Int2(1920, 1080);

constexpr float moveDuration = 0.2f;

constexpr Vector2 VEC2_ZERO = Vector2{ 0.0f, 0.0f };

Tilemap world;
Int2 playerPos;
void Move(Int2 movement);

Int2 lastMoveDir = Int2::zero;

void DrawPlayer();

float tick_t = 0.0f;
bool tickInProgress = false;

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(screenSize.x, screenSize.y, "Basic Raylib");

    playerPos = { 0, 0 };

    Camera2D cam = { 0 };
    cam.offset = VEC2_ZERO;
    cam.rotation = 0.0f;
    cam.zoom = 2.0f;

    UIContext ui = UIContext();

    const char* tilemapFile = "res/tilemap.tmx";
    world = Tilemap(tilemapFile);
    std::vector<Level> levels;
    std::unique_ptr<Level> testLevel = LoadLevel("res/testLevel.tmx");
    if (testLevel != nullptr) levels.push_back(*testLevel.get());

    world.AddLevel(*testLevel.get(), Int2{ 0, 10 });
    world.AddLevel(*testLevel.get(), Int2{ 16, 10 });

    while (!WindowShouldClose()) {
        screenSize.x = GetScreenWidth();
        screenSize.y = GetScreenHeight();
        float dt = GetFrameTime();
        Vector2 mousePos = GetMousePosition();

        if (!tickInProgress) {

            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) Move(Int2::up);
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) Move(Int2::down);
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) Move(Int2::left);
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) Move(Int2::right);
        }

        if (tickInProgress) {
            tick_t += dt / moveDuration;
            if (tick_t >= 1.0f) {
                tick_t = 0.0f;
                tickInProgress = false;
                playerPos += lastMoveDir;
            }
        }

        for (auto& box : world.gameObjects.boxes) {
            UpdatePushable(box.pushData, world.GetObject(box.ID), dt / moveDuration);
        }

        cam.offset = screenSize.toVector2() * 0.5f;
        cam.target = world.TilemapToWorldPos(playerPos) + lastMoveDir.toVector2() * (Smoothstep(tick_t) * world.tileSize.x);

        ui.BeginUI({ mousePos, IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsMouseButtonUp(MOUSE_BUTTON_LEFT) }); {
            using namespace UI;
            ui.Panel({ 0,0 }, Style{ .sizing = Grow(), .backgroundColor = BLUE, .padding = Spacing(16)}, [&](){
                ui.Panel({ 0,0 }, Style{.sizing = Fixed(300, 200), .backgroundColor = YELLOW}, [&](){});
            }); 
        } ui.EndUI();

        BeginDrawing(); {
            ClearBackground(SKYBLUE);

            BeginMode2D(cam); {
                world.Render(0);
                DrawPlayer();
            } EndMode2D();

            ui.Render();

            DrawText(TextFormat("Player pos: (%d, %d)", playerPos.x, playerPos.y), 5, 10, 24, WHITE);
        } EndDrawing();
    }

    CloseWindow();

    return 0;
}

void Move(Int2 movement) {
    Int2 targetPos = playerPos + movement;

    if (world.IsSolid(targetPos)) return;
    for (auto& box : world.gameObjects.boxes) {
        if (targetPos == world.GetObject(box.ID).position) Push(box.pushData, movement);
    }

    tickInProgress = true;
    tick_t = 0.0f;
    lastMoveDir = movement;
}

void DrawPlayer() {
    Vector2 worldPos = world.TilemapToWorldPos(playerPos);
    if (tickInProgress) {
        worldPos = worldPos + lastMoveDir.toVector2() * (Smoothstep(tick_t) * world.tileSize.x);
    }
    DrawCircleV(worldPos, world.tileSize.x / 2.0f, YELLOW);
}
