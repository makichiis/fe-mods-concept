#include <engine/logger.h>

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

typedef struct module {
    void* handle;
    void (*cb_on_start)();
} module;

void module_from_path(module* mod, const char* path) {
    mod->handle = dlopen(path, RTLD_LAZY);
    if (!mod->handle) {
        FE_ERROR("Failed to load module %s", path);
        return;
    }

    mod->cb_on_start = (void(*)())dlsym(mod->handle, "on_start");
}

#define MODS_PATH "./mods"

int main() {
    FE_INFO("Starting kernel...");

    FE_INFO("Loading modules...");

    DIR* moddir = opendir("./mods");
    if (!moddir) {
        FE_FATAL("Could not read modules directory: %s", strerror(errno));
        return 1;
    }

    struct dirent* dir_entry;
    while ((dir_entry = readdir(moddir)) != NULL) {
        char* dot = strrchr(dir_entry->d_name, '.');
        if (dot && strcmp(dot, ".so") == 0) {
            FE_INFO("Loading module %s...", dir_entry->d_name);
            
            char pathname[1024];
            snprintf(pathname, 1024, "%s/%s", MODS_PATH, dir_entry->d_name);
            
            module mod;
            module_from_path(&mod, pathname);

            if (mod.cb_on_start) mod.cb_on_start();
            FE_INFO("Done.");
        }
    }

    closedir(moddir);
}
