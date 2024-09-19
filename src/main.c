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
    void (*cb_on_stop)();
} module;

typedef struct vec_module {
    module* elems;
    size_t nmemb;
    size_t capacity;
} vec_module;

#define for_vec(v, i) for (size_t i = 0; i < v.nmemb; ++i) 
#define vec_at(v, i) v.elems[i]

void vec_module_new(vec_module* vm) {
    vm->capacity = 32;
    vm->nmemb = 0;
    vm->elems = calloc(vm->capacity, sizeof *vm->elems);
}

void vec_module_resize(vec_module* vm, size_t capacity) {
    vm->capacity = capacity;
    vm->elems = realloc(vm->elems, vm->capacity * sizeof *vm->elems);
}

void vec_module_push(vec_module* vm, module mod) {
    if (vm->nmemb == vm->capacity)
        vec_module_resize(vm, vm->capacity * 2);

    vm->elems[vm->nmemb++] = mod;
}

void vec_module_unload_all_mods(vec_module* vm) {
    for (size_t i = 0; i < vm->nmemb; ++i) {
        dlclose(vm->elems[i].handle);
        vm->elems[i].handle = NULL;
    }
}

void vec_module_destroy(vec_module* vm) {
    for (size_t i = 0; i < vm->nmemb; ++i)
        if (vm->elems[i].handle != NULL)
            // TODO: use dlinfo to get mod name
            FE_ERROR("Unfreed module at %s at addr 0x%.8X", "todo", vm->elems[i].handle);
    free(vm->elems);
    vm->capacity = 0;
    vm->nmemb = 0;
}

void cb_no_callback() {}

#define mod_register_event(mod, name) {\
    mod->cb_##name = (void(*)())dlsym(mod->handle, #name);\
    if (!mod->cb_##name) mod->cb_##name = cb_no_callback;\
}

void module_from_path(module* mod, const char* path) {
    mod->handle = dlopen(path, RTLD_LAZY);
    if (!mod->handle) {
        FE_ERROR("Failed to load module %s", path);
    }

    mod_register_event(mod, on_start);
    mod_register_event(mod, on_stop);
}

#define MODS_PATH "./mods"

int main() {
    FE_INFO("Starting kernel...");
    FE_INFO("Loading modules...");
    
    vec_module modules;
    vec_module_new(&modules);

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
            vec_module_push(&modules, mod);

            FE_INFO("Done.");
        }
    }

    FE_INFO("Calling OnStart event...");
    for_vec(modules, i) {
        vec_at(modules, i).cb_on_start();
    }
    FE_INFO("OnStart fired for all loaded modules.");

    FE_INFO("Calling OnStop event...");
    for_vec(modules, i) {
        vec_at(modules, i).cb_on_stop();
    }
    FE_INFO("OnStop fired for all loaded modules.");

    vec_module_unload_all_mods(&modules);
    vec_module_destroy(&modules);

    closedir(moddir);
}
