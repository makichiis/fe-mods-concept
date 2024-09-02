#!/bin/sh

run_b_configure() {
    cmake -S . -B build 
}

run_b_build() {
    cmake --build build -- -j 12
}

run_b_run() {
    if [ ! -d ./mods ]; then 
        run_b_buildmods
    fi 
    run_b_configure \
    && run_b_build \
    && ./build/engine-kernel 
}

run_b_compilemodule() {
    f=$1

    fo=${f#*\/} 
    echo "$0: Building ${fo/.cpp/.so}..."
    g++ -Iinc/ -fPIC -shared src/logger.c "$f" -o "./mods/${fo/.cpp/.so}" 
    echo "$0: Done building ${fo/.cpp/.so}"

}

run_b_buildmods() {
    mkdir -p ./mods 
    for f in mods-src/*.cpp; do
        [ -e "$f" ] || continue 
        run_b_compilemodule $f &
    done 
    wait 
}

run_b_clean() {
    if [ ! -d ./build/CMakeFiles ]; then 
        >&2 echo "$0: $(realpath .)/build/ is not configured by CMake."
        exit 1 
    fi 

    rm -rf ./build 
    rm -rf ./mods 
} 

subcmd=$1
run_b_${subcmd} 
