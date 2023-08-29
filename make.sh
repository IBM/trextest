#!/bin/sh

# Perpetually forget how meson works?  Me too!

ORIG_DIR=$PWD
BASE_DIR=$(dirname "$(readlink -f "$0")")
BUILD_DIR="$BASE_DIR/build"

clean() {
    rm -r "$BUILD_DIR"
}

setup() {
    cd "$BASE_DIR" || exit
    if ! [ -d "$BUILD_DIR" ] || ! [ -f "$BUILD_DIR/build.ninja" ] || [ "$#" -ne 0 ]; then
        clean
        meson setup build "$@"
    fi
}

if [ "$1" = "clean" ]; then
    clean
elif [ "$1" = "fmt" ]; then
    shift
    setup "$@"
    ninja -C "$BUILD_DIR" clang-format
else
    setup "$@"
    OBJECT_MODE=64 ninja -C "$BUILD_DIR" -v
fi

cd "$ORIG_DIR" || exit
