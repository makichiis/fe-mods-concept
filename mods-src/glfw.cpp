#include <iostream>
#include <engine/logger.h>

#include <mod/template.h>

struct GLFWContext {
    GLFWContext() {
        FE_WARNING("Context created.");
    }

    ~GLFWContext() {
        FE_WARNING("Context destroyed.");
    }
};

EVENT on_start() {
    FE_INFO("Initializing GLFW...");

    GLFWContext ctx;

    FE_INFO("GLFW started.");
}

