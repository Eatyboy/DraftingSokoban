#include <vector>
#include <utils.h>
#include <tilemap.h>
#include <gameObjects.h>
#include <levels.h>
#include <ui.h>
#include <format>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <Debug.h>
#include <shader.h>
#include <renderer.h>
#include <text.h>
#include <chrono>
#include <thread>
#include <input.h>
#include <glm/ext.hpp>
#include <array>

Int2 screenSize = Int2(1920, 1080);
glm::mat4 projection;
float lastFrameTime = 0.0f;
float dt = 0.0f;

constexpr float moveDuration = 0.25f;
constexpr float targetFrameTime = 1.0f / 165.0f;

Tilemap world;
Int2 playerPos;
void Move(Int2 movement);

Int2 lastMoveDir = Int2::zero;

void DrawPlayer();

float tick_t = 0.0f;
bool tickInProgress = false;

void LevelSelect(UIContext& ui, Level* level, Int2 position, Texture image) {
    ui.Panel(UI::PanelStyle{ 
            .image = image, 
            .sizing = {UI::Fixed(400), UI::Fixed(400)},
            .alignX = UI::AlignX::CENTER, 
            .alignY = UI::AlignY::CENTER
        }, UI::Callbacks{.label = "select",
            .onHover = [](UI::Element& e) {e.style.backgroundColor = LIGHTGRAY; },
            .onActive = [](UI::Element& e) {e.style.backgroundColor = GRAY; },
            .onClick = [level, position](UI::Element& e) {world.AddLevel(*level, position); }
        });
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    screenSize = { width, height };
}

float zoom = 1.0f;
int zoomLevel = 0;

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    zoomLevel = glm::max(glm::min((int)zoomLevel + (int)yoffset, 8), -8);
    zoom = powf(2.0f, 0.5f * (float)zoomLevel);
}

int fps = 0;

int main(void) {
    createDebugConsole();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(screenSize.x, screenSize.y, "Drafting Sokoban Game", NULL, NULL);
    if (window == nullptr) {
        debugError("Failed to create a GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        debugError("Failed to initialize GLAD\n");
        return -1;
    }

    Vec2 screenDPI;
    glfwGetWindowContentScale(window, &screenDPI.x, &screenDPI.y);
    screenDPI *= 96;

    glfwSetFramebufferSizeCallback(window, windowResizeCallback);
    glfwSetScrollCallback(window, scrollCallback);

    Shader shader("vertex.vert", "fragment.frag");

    InitRenderer();
    InitializeInput(window);
    InitTextRenderer(screenDPI);

    playerPos = { 0, 0 };

    //Camera2D cam = { 0 };
    //cam.offset = VEC2_ZERO;
    //cam.rotation = 0.0f;

    UIContext ui = UIContext();

    const char* tilemapFile = "res/tilemap.tmx";
    world = Tilemap();
    world.LoadTilemap(tilemapFile, &shader);
    std::vector<Level> levels;
    Level* testLevel = LoadLevel("res/testLevel.tmx");
    if (testLevel != nullptr) levels.push_back(*testLevel);

    Texture testLevelImg = LoadTexture("res/testlevel.png");

    Font* font = LoadFont("res/fonts/Merriweather_24pt-Regular.ttf");

    bool levelPickUIOpen = false;

    Vec2 camPos = Vec2::zero;

    std::array<int, 120> fpsNums;
    int totalFrameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        dt = currentFrameTime - lastFrameTime;
        fpsNums[totalFrameCount % fpsNums.size()] = (int)(1.0f / dt);
        totalFrameCount++;
        int fps = 0;
        for (int i = 0; i < std::min(totalFrameCount, (int)fpsNums.size()); ++i) {
            fps += fpsNums[i];
        }
        fps = fps / std::min(totalFrameCount, (int)fpsNums.size());
        if (dt < targetFrameTime) {
            std::this_thread::sleep_for(std::chrono::duration<float>(targetFrameTime - dt));
        }
        lastFrameTime = currentFrameTime;

        UpdateInputState();
        Vec2 mousePos = GetMousePos();

        if (!tickInProgress) {
            if (GetKeyState(KEY_W).down || GetKeyState(KEY_UP).down) Move(Int2::up);
            if (GetKeyState(KEY_S).down || GetKeyState(KEY_DOWN).down) Move(Int2::down);
            if (GetKeyState(KEY_A).down || GetKeyState(KEY_LEFT).down) Move(Int2::left);
            if (GetKeyState(KEY_D).down || GetKeyState(KEY_RIGHT).down) Move(Int2::right);
        }

        if (GetKeyState(KEY_SPACE).released) levelPickUIOpen = !levelPickUIOpen;

        if (tickInProgress) {
            tick_t += dt / moveDuration;

            std::vector<Int2> toUpdate;
            toUpdate.reserve(world.gameObjects.boxes.size());

            for (auto& pair : world.gameObjects.boxes) {
                if (pair.second.pushData.isMoving) toUpdate.push_back(pair.first);
            }
            std::sort(toUpdate.begin(), toUpdate.end(), [](Int2 a, Int2 b) {
                if (world.gameObjects.boxes[a].pushData.moveDelta == Int2::up) {
                    return a.y < b.y;
                } else if (world.gameObjects.boxes[a].pushData.moveDelta == Int2::down) {
                    return a.y > b.y;
                } else if (world.gameObjects.boxes[a].pushData.moveDelta == Int2::left) {
                    return a.x < b.x;
                } else if (world.gameObjects.boxes[a].pushData.moveDelta == Int2::right) {
                    return a.x > b.x;
                }
            });
            for (auto& pos : toUpdate) {
                UpdatePushable(world, world.gameObjects.boxes[pos].pushData, world.objects[pos], tick_t);
            }

            if (tick_t >= 1.0f) {
                tick_t = 0.0f;
                tickInProgress = false;
                playerPos += lastMoveDir;
            }
        }

        Vec2 target = world.TilemapToWorldPos(playerPos) + (Vec2)lastMoveDir * (Smoothstep(tick_t) * world.tileSize.x) + (Vec2)world.tileSize * 0.5f;
        camPos = (Vec2)screenSize * 0.5f - Vec2(target.x, target.y) * zoom;

        UI::MouseState currMouseState = UI::MouseState{
            .mousePos = mousePos,
            .left = GetMouseButtonState(MOUSE_LEFT),
            .right = GetMouseButtonState(MOUSE_RIGHT),
            .middle = GetMouseButtonState(MOUSE_MIDDLE),
        };
        ui.BeginUI(screenSize, currMouseState); {
            using namespace UI;
            if (levelPickUIOpen) {
                ui.Panel(PanelStyle{
                        .alignX = AlignX::CENTER,
                        .alignY = AlignY::CENTER,
                        .backgroundColor = BLANK,
                    }, [&] {
                          LevelSelect(ui, testLevel, {8, 8}, testLevelImg);
                    });
            }
            ui.Text(std::format("FPS: {}", fps), {
                .font = font,
                .positioning = Absolute({0, 0}),
                });
            ui.Text(std::format("Player position: ({}, {})", playerPos.x, playerPos.y), {
                .font = font,
                .positioning = Absolute({0, 40}),
                });
            ui.Text(std::format("Mouse position: ({}, {})", mousePos.x, mousePos.y), {
                .font = font,
                .positioning = Absolute({0, 80}),
                });
        } ui.EndUI();

        ClearColor(SKYBLUE);

        projection = glm::ortho(0.0f, (float)screenSize.x, (float)screenSize.y, 0.0f, -1.0f, 1.0f);
        projection = glm::translate(projection, glm::vec3(camPos.x, camPos.y, 0.0f));
        projection = glm::scale(projection, glm::vec3(zoom, zoom, 1.0f));

        world.Render(0);
        DrawPlayer();

        projection = glm::ortho(0.0f, (float)screenSize.x, (float)screenSize.y, 0.0f, -1.0f, 1.0f);

        ui.Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void Move(Int2 movement) {
    Int2 targetPos = playerPos + movement;

    if (world.IsSolid(targetPos)) return;
    if (world.gameObjects.boxes.count(targetPos) > 0) {
        std::vector<Int2> boxesToPushKeys;
        boxesToPushKeys.push_back(targetPos);
        Int2 currPos = targetPos;

        while (world.gameObjects.boxes.count(currPos + movement) > 0) {
            boxesToPushKeys.push_back(currPos + movement);
            currPos += movement;
        }

        if (world.IsSolid(currPos + movement)) return;

        for (auto it = boxesToPushKeys.rbegin(); it != boxesToPushKeys.rend(); ++it) {
            Push(world.gameObjects.boxes[*it].pushData, movement);
        }
    }

    tickInProgress = true;
    tick_t = 0.0f;
    lastMoveDir = movement;
}

void DrawPlayer() {
    Vec2 worldPos = world.TilemapToWorldPos(playerPos);
    if (tickInProgress) {
        Vec2 offset = (Vec2)lastMoveDir * (Smoothstep(tick_t) * (float)world.tileSize.x);
        worldPos = worldPos + offset;
    }
    DrawRect(worldPos, (Vec2)world.tileSize, YELLOW, 16.0f);
}
