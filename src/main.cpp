#include <vector>
#include <utils.h>
#include <tilemap.h>
#include <gameObjects.h>
#include <levels.h>
#include <ui.h>
#include <string>
#include <format>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Debug.h>
#include <Windows.h>
#include <shader.h>
#include <renderer.h>
#include <text.h>
#include <glm/ext.hpp>
#include <chrono>
#include <thread>

Int2 screenSize = Int2(1280, 720);
glm::mat4 projection;
Vec2 mousePos = Vec2::zero;
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
	ui.Panel(UI::PanelStyle{ .image = image}, UI::Callbacks{ .label = "select", 
        .onHover = [](UI::Element& e) {e.style.backgroundColor = LIGHTGRAY; },
        .onActive = [](UI::Element& e) {e.style.backgroundColor = GRAY; },
        .onClick = [level, position](UI::Element& e) {world.AddLevel(*level, position); 
        }});
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    screenSize = { width, height };
}

void mousePosCallback(GLFWwindow* window, double mouseXIn, double mouseYIn) {
    float mouseX = static_cast<float>(mouseXIn);
    float mouseY = static_cast<float>(mouseYIn);
    mousePos = Vec2{ mouseX, mouseY };
}

float zoom = 1.0f;
int zoomLevel = 0;

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    zoomLevel = glm::max(glm::min((int)zoomLevel + (int)yoffset, 8), -8);
    zoom = powf(2.0f, 0.5f * (float)zoomLevel);
}

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
    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetScrollCallback(window, scrollCallback);

    Shader shader("vertex.vert", "fragment.frag");

    InitRenderer();
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

    bool levelPickUIOpen = true;

    Vec2 camPos = Vec2::zero;

    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        dt = currentFrameTime - lastFrameTime;
        if (dt < targetFrameTime) {
            std::this_thread::sleep_for(std::chrono::duration<float>(targetFrameTime - dt));
        }
        lastFrameTime = currentFrameTime;


        if (!tickInProgress) {
            if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP)) Move(Int2::up);
            if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN)) Move(Int2::down);
            if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_LEFT)) Move(Int2::left);
            if (glfwGetKey(window, GLFW_KEY_D) || glfwGetKey(window, GLFW_KEY_RIGHT)) Move(Int2::right);
        }

        //if (IsKeyReleased(KEY_SPACE)) levelPickUIOpen = !levelPickUIOpen;

        if (tickInProgress) {
            tick_t += dt / moveDuration;
            for (auto& box : world.gameObjects.boxes) {
                UpdatePushable(box.pushData, world.GetObjectData(box.ID), tick_t);
            }
            if (tick_t >= 1.0f) {
                tick_t = 0.0f;
                tickInProgress = false;
                playerPos += lastMoveDir;
            }
        }

        Vec2 target = world.TilemapToWorldPos(playerPos) + (Vec2)lastMoveDir * (Smoothstep(tick_t) * world.tileSize.x) + (Vec2)world.tileSize * 0.5f;
        camPos = (Vec2)screenSize * 0.5f - Vec2(target.x, -target.y) * zoom;

        UI::MouseState currMouseState = UI::MouseState{
            .mousePos = mousePos,
          //.leftDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT),
          //.leftPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT),
          //.leftUp = IsMouseButtonUp(MOUSE_BUTTON_LEFT),
          //.leftReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT),
          //.rightDown = IsMouseButtonDown(MOUSE_BUTTON_RIGHT),
          //.rightPressed = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT),
          //.rightUp = IsMouseButtonUp(MOUSE_BUTTON_RIGHT),
          //.rightReleased = IsMouseButtonReleased(MOUSE_BUTTON_RIGHT),
        };
        ui.BeginUI(screenSize, currMouseState); {
            using namespace UI;
            ui.Panel(PanelStyle{
                .sizing = { Fit(), Fit() },
                .padding = Padding(24.0f),
                .margin = Margin(50.0f),
                .roundness = rounded_xl,
                .backgroundColor = GREEN,
                .borderWidth = 3.0f,
                .borderColor = DARKGREEN,
                }, [&] {
                    ui.Panel(PanelStyle{
                            .sizing = { Fixed(300), Fixed(200) },
                            .roundness = rounded_md,
                            .backgroundColor = BLUE,
                            .borderWidth = 3.0f,
                            .borderColor = DARKBLUE,
                        });
                });
            //if (levelPickUIOpen) {
            //    ui.Panel(PanelStyle{
            //        }, [&] {
            //              LevelSelect(ui, testLevel, {16, 16}, testLevelImg);
            //        });
            //}
            ui.Text(std::format("Player position: ({}, {})", playerPos.x, playerPos.y), {
                .font = font,
                .positioning = Absolute({0, 0}),
                });
        } ui.EndUI();

        glClearColor(SKYBLUE.r / 255.0f, SKYBLUE.g / 255.0f, SKYBLUE.b / 255.0f, SKYBLUE.a / 255.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        projection = glm::ortho(0.0f, (float)screenSize.x, (float)screenSize.y, 0.0f, -1.0f, 1.0f);
        projection = glm::translate(projection, glm::vec3(camPos.x, camPos.y, 0.0f));
        projection = glm::scale(projection, glm::vec3(zoom, zoom, 1.0f));

        world.Render(0);
        DrawPlayer();

        projection = glm::ortho(0.0f, (float)screenSize.x, (float)screenSize.y, 0.0f, -1.0f, 1.0f);

        ui.Render();

        RenderText("Hello, world!\nI am a piece of text.\nNice to meet you!", *font, 32.0f, { 400, 400 }, YELLOW);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void Move(Int2 movement) {
    Int2 targetPos = playerPos + movement;

    if (world.IsSolid(targetPos)) return;
    for (auto& box : world.gameObjects.boxes) {
        if (targetPos == world.GetObjectData(box.ID).position) Push(box.pushData, movement);
    }

    tickInProgress = true;
    tick_t = 0.0f;
    lastMoveDir = movement;
}

void DrawPlayer() {
    Vec2 worldPos = world.TilemapToWorldPos(playerPos);
    if (tickInProgress) {
        Vec2 offset = (Vec2)lastMoveDir * (Smoothstep(tick_t) * (Vec2)world.tileSize.x);
        worldPos = worldPos + offset;
    }
    DrawRect(worldPos, (Vec2)world.tileSize, YELLOW, 16.0f);
}
