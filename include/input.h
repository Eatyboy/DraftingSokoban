#pragma once

#include <GLFW/glfw3.h>
#include <utils.h>

struct InputButtonState {
    bool down = false;
    bool pressed = false;
    bool released = false;
};

enum KeyInput {
    KEY_ESC, KEY_TAB, KEY_SPACE, KEY_LSHIFT, KEY_RSHIFT, KEY_ALT, KEY_LCTRL, KEY_RCTRL,
    KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,
    KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, 
    KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M,
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
};

enum MouseInput {
    MOUSE_LEFT,
    MOUSE_RIGHT,
    MOUSE_MIDDLE,
    MOUSE_4,
    MOUSE_5,
};

void InitializeInput(GLFWwindow* win);
void UpdateInputState();
InputButtonState GetKeyState(KeyInput key);
InputButtonState GetMouseButtonState(MouseInput button);
Vec2 GetMousePos();
