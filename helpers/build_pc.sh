# this is a helper script to build the project for PC
# across various platforms

# Usage: ./build_pc.sh <name> <platform> <sdl_version>

# <platform> can be one of the following: ubuntu, macos, windows
# <sdl_version> can be one of the following: sdl2, sdl1

if [ $# -ne 3 ]; then
    echo "Usage: ./build_pc.sh <name> <platform> <sdl_version>"
    exit 1
fi

NAME=$1
PLATFORM=$2
SDL_VERSION=$3

# install deps for the current platform
if [ "$PLATFORM" = "ubuntu" ]; then
    sudo apt-get update
    sudo apt-get install -y libsdl2-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev zlib1g-dev gcc g++ libcurl4-openssl-dev wget git libsdl1.2-dev libsdl-ttf2.0-dev libsdl-image1.2-dev libsdl-gfx1.2-dev libfreetype-dev libsdl-mixer1.2-dev libmpg123-dev
elif [ "$PLATFORM" = "macos" ]; then
    brew install sdl2 sdl2_mixer sdl2_ttf sdl2_image sdl2_gfx wget git sdl sdl_ttf sdl_image sdl_gfx freetype sdl_mixer mpg123
elif [ "$PLATFORM" = "windows" ]; then
    choco install -y make wget git sdl2 sdl
    pacman -S git mingw-w64-x86_64-gcc mingw64/mingw-w64-x86_64-SDL2 mingw64/mingw-w64-x86_64-SDL
    export PATH="${PATH}:/mingw64/bin"
else
    echo "Unknown platform: $PLATFORM"
    exit 1
fi

# call the right make command, depending on SDL version
COMMAND="make pc"
EXT="bin"
if [ "$SDL_VERSION" = "sdl1" ]; then
    COMMAND="make pc_sdl1"
    EXT="bin-sdl1"
fi

# call the right make command, (the makefile should take care of platform-dependent stuff)
$COMMAND

# package the binary into a zip, alongside assets
SYSTEM_SPECIFIC=""
if [ "$PLATFORM" = "ubuntu" ]; then
    echo "cd \$(dirname \$0) && ./${NAME}.${EXT}" > run.sh
    chmod +x run.sh
    SYSTEM_SPECIFIC="run.sh"
elif [ "$PLATFORM" = "macos" ]; then
    echo "cd \$(dirname \$0) && ./${NAME}.${EXT}" > run.command
    chmod +x run.command
    SYSTEM_SPECIFIC="run.command"
elif [ "$PLATFORM" = "windows" ]; then
    cp $NAME.${EXT} $NAME.exe
    EXT="exe"
fi

chmod +x ${NAME}.${EXT}
zip -r -9 "${NAME}_${PLATFORM}_${SDL_VERSION}.zip" ${NAME}.${EXT} resin ${SYSTEM_SPECIFIC}


# depending on the OS, package the resulting binary in a zip file
# if [ "$PLATFORM" = "ubuntu" ]; then
# elif [ "$PLATFORM" = "macos" ]; then
#     zip -r -j -9 "${NAME}_${PLATFORM}_${SDL_VERSION}.zip" ${NAME}.${EXT}
# elif [ "$PLATFORM" = "windows" ]; then
#     zip -r -j -9 "${NAME}_${PLATFORM}_${SDL_VERSION}.zip" ${NAME}.${EXT}
# fi

# TODO: on macos, bundle into a .app folder and add an info.plist