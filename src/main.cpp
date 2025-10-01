#include <vector>
#include <utils.h>
//#include <tilemap.h>
//#include <gameObjects.h>
//#include <levels.h>
//#include <ui.h>
#include <string>
#include <format>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Debug.h>
#include <Windows.h>
#include <shader.h>

Int2 screenSize = Int2(1280, 720);
Vec2 mousePos = Vec2::zero;
float lastFrameTime = 0.0f;
float dt = 0.0f;

constexpr float moveDuration = 0.2f;

//Tilemap world;
//Int2 playerPos;
//void Move(Int2 movement);
//
//Int2 lastMoveDir = Int2::zero;
//
//void DrawPlayer();
//
//float tick_t = 0.0f;
//bool tickInProgress = false;
//
//void LevelSelect(UIContext& ui, Level* level, Int2 position, Texture2D* image) {
//	ui.Panel(UI::PanelStyle{ .image = image}, UI::Callbacks{ .label = "select", 
//        .onHover = [](UI::Element& e) {e.style.backgroundColor = LIGHTGRAY; },
//        .onActive = [](UI::Element& e) {e.style.backgroundColor = GRAY; },
//        .onClick = [level, position](UI::Element& e) {world.AddLevel(*level, position); 
//        }});
//}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    screenSize = { width, height };
}

void mousePosCallback(GLFWwindow* window, double mouseXIn, double mouseYIn) {
    float mouseX = static_cast<float>(mouseXIn);
    float mouseY = static_cast<float>(mouseYIn);
	mousePos = Vec2{ mouseX, mouseY };
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

    Shader shader("vertex.vert", "fragment.frag");

    //playerPos = { 0, 0 };

    ////Camera2D cam = { 0 };
    ////cam.offset = VEC2_ZERO;
    ////cam.rotation = 0.0f;
    ////cam.zoom = 0.25f;

    //UIContext ui = UIContext();

    //const char* tilemapFile = "res/tilemap.tmx";
    //world = Tilemap(tilemapFile);
    //std::vector<Level> levels;
    //Level* testLevel = LoadLevel("res/testLevel.tmx");
    //if (testLevel != nullptr) levels.push_back(*testLevel);

    //Texture2D testLevelImg = LoadTexture("res/testlevel.png");

    bool levelPickUIOpen = true;

    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        dt = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

   //     if (!tickInProgress) {

   //         if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) Move(Int2::up);
   //         if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) Move(Int2::down);
   //         if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) Move(Int2::left);
   //         if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) Move(Int2::right);
   //     }

   //     if (IsKeyReleased(KEY_SPACE)) levelPickUIOpen = !levelPickUIOpen;

   //     if (tickInProgress) {
   //         tick_t += dt / moveDuration;
   //         if (tick_t >= 1.0f) {
   //             tick_t = 0.0f;
   //             tickInProgress = false;
   //             playerPos += lastMoveDir;
   //         }
   //     }
   //     for (auto& box : world.gameObjects.boxes) {
   //         UpdatePushable(box.pushData, world.GetObject(box.ID), dt / moveDuration);
   //     }

   //     cam.offset = screenSize.toVector2() * 0.5f;
   //     cam.target = world.TilemapToWorldPos(playerPos) + lastMoveDir.toVector2() * (Smoothstep(tick_t) * world.tileSize.x);

   //     UI::MouseState currMouseState = UI::MouseState{
   //         .mousePos = mousePos,
			//.leftDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT),
			//.leftPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT),
			//.leftUp = IsMouseButtonUp(MOUSE_BUTTON_LEFT),
			//.leftReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT),
			//.rightDown = IsMouseButtonDown(MOUSE_BUTTON_RIGHT),
			//.rightPressed = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT),
			//.rightUp = IsMouseButtonUp(MOUSE_BUTTON_RIGHT),
			//.rightReleased = IsMouseButtonReleased(MOUSE_BUTTON_RIGHT),
   //     };
   //     ui.BeginUI(screenSize.toVector2(), currMouseState); {
   //         using namespace UI;

   //         if (levelPickUIOpen) {
			//	ui.Panel(PanelStyle{
			//		}, [&] {
   //                     LevelSelect(ui, testLevel, {16, 16}, &testLevelImg);
			//		});
   //         }
   //         ui.Text(std::format("Player position: ({}, {})", playerPos.x, playerPos.y), {
   //             .positioning = Absolute({20, 20}),
   //             });
	  //  } ui.EndUI();

   //     BeginDrawing(); {
   //         ClearBackground(SKYBLUE);

   //         BeginMode2D(cam); {
   //             world.Render(0);
   //             DrawPlayer();
   //         } EndMode2D();

   //         ui.Render();
   //     } EndDrawing();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

//void Move(Int2 movement) {
//    Int2 targetPos = playerPos + movement;
//
//    if (world.IsSolid(targetPos)) return;
//    for (auto& box : world.gameObjects.boxes) {
//        if (targetPos == world.GetObject(box.ID).position) Push(box.pushData, movement);
//    }
//
//    tickInProgress = true;
//    tick_t = 0.0f;
//    lastMoveDir = movement;
//}

//void DrawPlayer() {
//    Vec2 worldPos = world.TilemapToWorldPos(playerPos);
//    if (tickInProgress) {
//        worldPos = worldPos + lastMoveDir.toVector2() * (Smoothstep(tick_t) * world.tileSize.x);
//    }
//    DrawCircleV(worldPos, world.tileSize.x / 2.0f, YELLOW);
//}
