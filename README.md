# Snake Maker

## Installing

You need to have support for OpenGL 4.5

### Linux

```bash
git clone https://github.com/RyanDKennedy/snake_maker.git
cd snake_maker
git submodule init
git submodule update
mkdir build; cd build;
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```
### Windows

```bash
git clone https://github.com/RyanDKennedy/snake_maker.git
cd snake_maker
git submodule init
git submodule update
mkdir build; cd build;
cmake ..
cmake --build .
```
1. Open the snake_maker.sln file in Visual Studio
2. Right click on the snake maker sln
3. Click properties
4. Change the C++ standard to the C++17 ones (2 options)
5. Build the project inside Visual Studio

## About

Snake Maker is the classic game snake, except you can build your own maps.
Snake Maker runs in an 800x800 window, you can't resize it.
Inspired by nsnake the terminal game.

## Controls

These are all the keys supported, so if you need to do something but can't find the key, just try these keys.

| button | action |
|--------|--------|
| w | up |
| s | down |
| a | left |
| d | right |
| backspace | go back |
| space | pause |
| mousewheel | scrolling on menus |

## Custom Maps
Please just use the built in map maker.

## Custom Tiles
Tile files are just a 2D array of RGB values, see the tiles folder.

