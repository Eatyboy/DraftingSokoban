#include <input.h>
#include <map>
#include <Debug.h>

Vec2 mousePos;

std::map<KeyInput, InputButtonState> keyState;
std::map<MouseInput, InputButtonState> mouseState;

GLFWwindow* window;

void mousePosCallback(GLFWwindow* window, double mouseXIn, double mouseYIn) {
    float mouseX = static_cast<float>(mouseXIn);
    float mouseY = static_cast<float>(mouseYIn);
    mousePos = Vec2{ mouseX, mouseY };
}

Vec2 GetMousePos() {
    return mousePos;
}

int getGLFWKeyCode(KeyInput code) {
    switch (code) {
        case KEY_ESC: return GLFW_KEY_ESCAPE;
        case KEY_TAB: return GLFW_KEY_TAB;
        case KEY_SPACE: return GLFW_KEY_SPACE;
        case KEY_LSHIFT: return GLFW_KEY_LEFT_SHIFT;
        case KEY_RSHIFT: return GLFW_KEY_RIGHT_SHIFT;
        case KEY_ALT: return GLFW_KEY_LEFT_ALT;
        case KEY_LCTRL: return GLFW_KEY_LEFT_CONTROL;
        case KEY_RCTRL: return GLFW_KEY_RIGHT_CONTROL;
        case KEY_Q: return GLFW_KEY_Q;
        case KEY_W: return GLFW_KEY_W;
        case KEY_E: return GLFW_KEY_E;
        case KEY_R: return GLFW_KEY_R;
        case KEY_T: return GLFW_KEY_T;
        case KEY_Y: return GLFW_KEY_Y;
        case KEY_U: return GLFW_KEY_U;
        case KEY_I: return GLFW_KEY_I;
        case KEY_O: return GLFW_KEY_O;
        case KEY_P: return GLFW_KEY_P;
        case KEY_A: return GLFW_KEY_A;
        case KEY_S: return GLFW_KEY_S;
        case KEY_D: return GLFW_KEY_D;
        case KEY_F: return GLFW_KEY_F;
        case KEY_G: return GLFW_KEY_G;
        case KEY_H: return GLFW_KEY_H;
        case KEY_J: return GLFW_KEY_J;
        case KEY_K: return GLFW_KEY_K;
        case KEY_L: return GLFW_KEY_L;
        case KEY_Z: return GLFW_KEY_Z;
        case KEY_X: return GLFW_KEY_X;
        case KEY_C: return GLFW_KEY_C;
        case KEY_V: return GLFW_KEY_V;
        case KEY_B: return GLFW_KEY_B;
        case KEY_N: return GLFW_KEY_N;
        case KEY_M: return GLFW_KEY_M;
        case KEY_0: return GLFW_KEY_0;
        case KEY_1: return GLFW_KEY_1;
        case KEY_2: return GLFW_KEY_2;
        case KEY_3: return GLFW_KEY_3;
        case KEY_4: return GLFW_KEY_4;
        case KEY_5: return GLFW_KEY_5;
        case KEY_6: return GLFW_KEY_6;
        case KEY_7: return GLFW_KEY_7;
        case KEY_8: return GLFW_KEY_8;
        case KEY_9: return GLFW_KEY_9;
        case KEY_UP: return GLFW_KEY_UP;
        case KEY_DOWN: return GLFW_KEY_DOWN;
        case KEY_LEFT: return GLFW_KEY_LEFT;
        case KEY_RIGHT: return GLFW_KEY_RIGHT;
    }
}

int getGLFWMouseCode(MouseInput mouse) {
    switch (mouse) {
    case MOUSE_LEFT: return GLFW_MOUSE_BUTTON_LEFT;
    case MOUSE_RIGHT: return GLFW_MOUSE_BUTTON_RIGHT;
    case MOUSE_MIDDLE: return GLFW_MOUSE_BUTTON_MIDDLE;
    case MOUSE_4: return GLFW_MOUSE_BUTTON_4;
    case MOUSE_5: return GLFW_MOUSE_BUTTON_5;
    }
}

inline InputButtonState getCurrentKeyState(KeyInput key) {
    int glfwKey = getGLFWKeyCode(key);
    bool isDown = glfwGetKey(window, glfwKey);
    bool stateChange = keyState[key].down != isDown;

    return InputButtonState{
        isDown,
        isDown && stateChange,
        !isDown && stateChange,
    };
}

inline InputButtonState getCurrentMouseButtonState(MouseInput button) {
    int glfwMouseButtton = getGLFWMouseCode(button);
    bool isDown = glfwGetMouseButton(window, glfwMouseButtton);
    bool stateChange = mouseState[button].down != isDown;

    return InputButtonState{
        isDown,
        isDown && stateChange,
        !isDown && stateChange,
    };
}

void InitializeInput(GLFWwindow* win) {
    window = win;
    glfwSetCursorPosCallback(window, mousePosCallback);

    keyState[KEY_ESC] = InputButtonState{};
    keyState[KEY_TAB] = InputButtonState{};
    keyState[KEY_SPACE] = InputButtonState{}; 
    keyState[KEY_LSHIFT] = InputButtonState{}; 
    keyState[KEY_RSHIFT] = InputButtonState{}; 
    keyState[KEY_ALT] = InputButtonState{}; 
    keyState[KEY_LCTRL] = InputButtonState{}; 
    keyState[KEY_RCTRL] = InputButtonState{};
    keyState[KEY_Q] = InputButtonState{}; 
    keyState[KEY_W] = InputButtonState{}; 
    keyState[KEY_E] = InputButtonState{}; 
    keyState[KEY_R] = InputButtonState{}; 
    keyState[KEY_T] = InputButtonState{}; 
    keyState[KEY_Y] = InputButtonState{}; 
    keyState[KEY_U] = InputButtonState{}; 
    keyState[KEY_I] = InputButtonState{}; 
    keyState[KEY_O] = InputButtonState{}; 
    keyState[KEY_P] = InputButtonState{};
    keyState[KEY_A] = InputButtonState{}; 
    keyState[KEY_S] = InputButtonState{}; 
    keyState[KEY_D] = InputButtonState{}; 
    keyState[KEY_F] = InputButtonState{}; 
    keyState[KEY_G] = InputButtonState{}; 
    keyState[KEY_H] = InputButtonState{}; 
    keyState[KEY_J] = InputButtonState{}; 
    keyState[KEY_K] = InputButtonState{}; 
    keyState[KEY_L] = InputButtonState{}; 
    keyState[KEY_Z] = InputButtonState{}; 
    keyState[KEY_X] = InputButtonState{}; 
    keyState[KEY_C] = InputButtonState{}; 
    keyState[KEY_V] = InputButtonState{}; 
    keyState[KEY_B] = InputButtonState{}; 
    keyState[KEY_N] = InputButtonState{}; 
    keyState[KEY_M] = InputButtonState{};
    keyState[KEY_0] = InputButtonState{}; 
    keyState[KEY_1] = InputButtonState{}; 
    keyState[KEY_2] = InputButtonState{}; 
    keyState[KEY_3] = InputButtonState{}; 
    keyState[KEY_4] = InputButtonState{}; 
    keyState[KEY_5] = InputButtonState{}; 
    keyState[KEY_6] = InputButtonState{}; 
    keyState[KEY_7] = InputButtonState{}; 
    keyState[KEY_8] = InputButtonState{}; 
    keyState[KEY_9] = InputButtonState{};
    keyState[KEY_UP] = InputButtonState{}; 
    keyState[KEY_DOWN] = InputButtonState{}; 
    keyState[KEY_LEFT] = InputButtonState{}; 
    keyState[KEY_RIGHT] = InputButtonState{};

    mouseState[MOUSE_LEFT] = InputButtonState{};
    mouseState[MOUSE_RIGHT] = InputButtonState{};
    mouseState[MOUSE_MIDDLE] = InputButtonState{};
    mouseState[MOUSE_4] = InputButtonState{};
    mouseState[MOUSE_5] = InputButtonState{};
}

void UpdateInputState() {
    keyState[KEY_ESC] = getCurrentKeyState(KEY_ESC);
    keyState[KEY_TAB] = getCurrentKeyState(KEY_TAB);
    keyState[KEY_SPACE] = getCurrentKeyState(KEY_SPACE); 
    keyState[KEY_LSHIFT] = getCurrentKeyState(KEY_LSHIFT); 
    keyState[KEY_RSHIFT] = getCurrentKeyState(KEY_RSHIFT); 
    keyState[KEY_ALT] = getCurrentKeyState(KEY_ALT); 
    keyState[KEY_LCTRL] = getCurrentKeyState(KEY_LCTRL); 
    keyState[KEY_RCTRL] = getCurrentKeyState(KEY_RCTRL);
    keyState[KEY_Q] = getCurrentKeyState(KEY_Q); 
    keyState[KEY_W] = getCurrentKeyState(KEY_W); 
    keyState[KEY_E] = getCurrentKeyState(KEY_E); 
    keyState[KEY_R] = getCurrentKeyState(KEY_R); 
    keyState[KEY_T] = getCurrentKeyState(KEY_T); 
    keyState[KEY_Y] = getCurrentKeyState(KEY_Y); 
    keyState[KEY_U] = getCurrentKeyState(KEY_U); 
    keyState[KEY_I] = getCurrentKeyState(KEY_I); 
    keyState[KEY_O] = getCurrentKeyState(KEY_O); 
    keyState[KEY_P] = getCurrentKeyState(KEY_P);
    keyState[KEY_A] = getCurrentKeyState(KEY_A); 
    keyState[KEY_S] = getCurrentKeyState(KEY_S); 
    keyState[KEY_D] = getCurrentKeyState(KEY_D); 
    keyState[KEY_F] = getCurrentKeyState(KEY_F); 
    keyState[KEY_G] = getCurrentKeyState(KEY_G); 
    keyState[KEY_H] = getCurrentKeyState(KEY_H); 
    keyState[KEY_J] = getCurrentKeyState(KEY_J); 
    keyState[KEY_K] = getCurrentKeyState(KEY_K); 
    keyState[KEY_L] = getCurrentKeyState(KEY_L); 
    keyState[KEY_Z] = getCurrentKeyState(KEY_Z); 
    keyState[KEY_X] = getCurrentKeyState(KEY_X); 
    keyState[KEY_C] = getCurrentKeyState(KEY_C); 
    keyState[KEY_V] = getCurrentKeyState(KEY_V); 
    keyState[KEY_B] = getCurrentKeyState(KEY_B); 
    keyState[KEY_N] = getCurrentKeyState(KEY_N); 
    keyState[KEY_M] = getCurrentKeyState(KEY_M);
    keyState[KEY_0] = getCurrentKeyState(KEY_0); 
    keyState[KEY_1] = getCurrentKeyState(KEY_1); 
    keyState[KEY_2] = getCurrentKeyState(KEY_2); 
    keyState[KEY_3] = getCurrentKeyState(KEY_3); 
    keyState[KEY_4] = getCurrentKeyState(KEY_4); 
    keyState[KEY_5] = getCurrentKeyState(KEY_5); 
    keyState[KEY_6] = getCurrentKeyState(KEY_6); 
    keyState[KEY_7] = getCurrentKeyState(KEY_7); 
    keyState[KEY_8] = getCurrentKeyState(KEY_8); 
    keyState[KEY_9] = getCurrentKeyState(KEY_9);
    keyState[KEY_UP] = getCurrentKeyState(KEY_UP); 
    keyState[KEY_DOWN] = getCurrentKeyState(KEY_DOWN); 
    keyState[KEY_LEFT] = getCurrentKeyState(KEY_LEFT); 
    keyState[KEY_RIGHT] = getCurrentKeyState(KEY_RIGHT);

    mouseState[MOUSE_LEFT] = getCurrentMouseButtonState(MOUSE_LEFT);
    mouseState[MOUSE_RIGHT] = getCurrentMouseButtonState(MOUSE_RIGHT);
    mouseState[MOUSE_MIDDLE] = getCurrentMouseButtonState(MOUSE_MIDDLE);
    mouseState[MOUSE_4] = getCurrentMouseButtonState(MOUSE_4);
    mouseState[MOUSE_5] = getCurrentMouseButtonState(MOUSE_5);
}

InputButtonState GetKeyState(KeyInput key) {
    return keyState[key];
}

InputButtonState GetMouseButtonState(MouseInput button) {
    return mouseState[button];
}