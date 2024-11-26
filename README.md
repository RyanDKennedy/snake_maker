# Snake Maker

## About

Snake Maker is the classic game snake, except you can build your own maps.
Snake Maker runs in an 800 pixel by 800 pixel resolution.
Inspired by nsnake the terminal game.

## Installing

You need to have support for OpenGL 3.3 or greater since all versions greater are backwards compatible to 3.3

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

If gcc doesn't work try to use clang.
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..
```

### Windows

#### Method 1 (Preferred method if working)

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
4. Change the C++ standard to the C++17, and the C standard to C17
5. Build the project inside Visual Studio

#### Method 2 (More reliable method if method 1 doesn't work)

1. Install winlibs, add both winlibs bin directories to your path
2. Install cmake
3. Use cmake to generate a mingw makefile
4. Run mingw-make in the build directory.

## Controls

These are all the keys supported, so if you need to do something but can't find the key, just try these keys.

| button | action |
|--------|--------|
| w | up |
| s | down |
| a | left |
| d | right |
| escape | go back |
| space | speed up snake |
| mousewheel | scrolling on menus |
| left click | select / primary function |
| right click | secondary function |

## Custom Maps
Please just use the built in map creator. The format for the maps are specified below in the Maps section.

## Custom Tiles
Please just use the built in tile creator. They are stored as a grid of RGB values, however the format is a little bit sensitive, make sure you have no trailing newlines. The format is specified below int he Tiles section.

## Share Directory Structure

The "share" folder contains all resources that are used by the application. Inside there will be a settings file which stores your settings, a maps folder which contains map files (.snake), a tiles folder which contains all of your tile files (.tile), and a skins directory that contains all of your skin files (.skin).

**You can edit where these are looked for by editing src/common.hpp then recompiling the project**

### Settings
| option | value type | acceptable values (n = value) | description |
| --- | --- | --- | --- |
| starting_speed | integer | 1 <= n | the amount of blocks the snake moves per second |
| starting_size | integer | 1 <= n | the size of the body (not including head) that the snake starts with |
| tile_create_width | integer | 16 <= n | the width of the tile in the create tile menu  NOTE: the reason why n >= 16 is because I experienced issues. opengl doesn't actually have a minimum texture dimensions but my driver didn't support it. to get around this make a texture that is a factor of your original one, and draw each pixel as a group of pixels. |
| tile_create_height | integer | 16 <= n | the height of the tile in the create tile menu  NOTE: the reason why n >= 16 is because I experienced issues. opengl doesn't actually have a minimum texture dimensions but my driver didn't support it. to get around this make a texture that is a factor of your original one, and draw each pixel as a group of pixels. |
| texture_filtering | integer | 0 <= n <= 1 | this enables texture filtering on the tiles, this just looks bad though in my experience. |

example:
```
starting_speed=10
starting_size=1
tile_create_width=16
tile_create_height=16
texture_filtering=0
```

### Maps (.snake)

Maps are split into 4 sections, they have to go in this order.
1. Settings
2. TileDefinitions
3. BoardMap
4. CollisionMap

The file must end with a "[END]" directly below the collision map. See the example if you are confused.

#### Settings - for map files

| option | value type | description |
| --- | --- | --- |
| width | int | the width of the board, unit is tiles |
| height | int | the height of the board, unit is tiles |
| tile_width | int | the width of each tile, the unit is pixels |
| tile_height | int | the height of each tile, the unit is pixels |
| starting_x | int | the x value of the position where the player will start, positions start at (0,0) |
| starting_y | int | the y value of the position where the player will start, positions start at (0,0) |
| starting_direction | int | the direction the player is facing when the first start the map. 0 = up, 1 = right, 2 = down, 3 = left |
| skin | int | the name of the skin that you want to use, the skins directory will be searched. should not contain .skin file extension in the name |

Make sure that the skin that you are using complies with the tile_width and tile_height properties.

example:
```
[Settings]
width=60
height=60
tile_width=10	
tile_height=10
starting_x=5
starting_y=9
starting_direction=0
skin=arrow
```

#### Tile Definitions

This is a list of all usable tiles inside of the BoardMap section. This will assign a number to each tile that will later be used in the BoardMap Section. All tiles must be listed in sequential order starting from 0.

Make sure that the tiles that you are using complies with the tile_width and tile_height properties in the settings section.

example:
```
[TileDefinitions]
first_tile=0
red=1
wall=2
```

#### Board Map

This section specifies how the board will look. It is stored as an array of integers, the integers correspond to the tile definitions section. The grid must be atleast the size specified inside the settings section with the "height" and "width" properties.

example for 5x5 map:
```
[BoardMap]
1,1,1,1,1
1,0,0,0,1
1,0,0,0,1
1,0,0,0,1
1,1,1,1,1
```

#### Collision Map
The collision map is an array of integers which correspond to if that place has collision, will it kill you if you run into it. The collision map is stored exactly like the board map except the only acceptable values are "1", and "0". "1" corresponds to that the block will kill you if you run into it, and "0" corresponds to you can run through it without dying.

example for a 5x5 map:
```
[CollisionMap]
1,1,1,1,1
1,0,0,0,1
1,0,0,0,1
1,0,0,0,1
1,1,1,1,1
```

#### Example

```
[Settings]
width=5
height=5
tile_width=10	
tile_height=10
starting_x=2
starting_y=0
starting_direction=0
skin=arrow
[TileDefinitions]
empty=0
solid=1
[BoardMap]
1,1,1,1,1
1,0,0,0,1
1,0,0,0,1
1,0,0,0,1
1,1,1,1,1
[CollisionMap]
1,1,1,1,1
1,0,0,0,1
1,0,0,0,1
1,0,0,0,1
1,1,1,1,1
[END]
```

### Tiles
Tiles are just a grid of rgb values. The format is really sensitive, so make sure you have no trailing newlines in the file. If the example.tile is 10 tall make sure that ```cat example.tile | wc -l``` returns 9.
NOTE: the rows end with a comma unlike how they did inside of the maps inside of the .snake files.

example of a 5x5 tile:
```
(255, 255, 255), (255, 255, 255), (255, 255, 255), (255, 255, 255), (255, 255, 255),
(255, 255, 255), (255, 255, 255), (255, 255, 255), (255, 255, 255), (255, 255, 255),
(255, 255, 255), (255, 255, 255), (255, 255, 255), (255, 255, 255), (255, 255, 255),
(255, 255, 255), (255, 255, 255), (255, 255, 255), (255, 255, 255), (255, 255, 255),
(255, 255, 255), (255, 255, 255), (255, 255, 255), (255, 255, 255), (255, 255, 255),
```
(255, 255, 255) corresponds to RGB values, so this would be the color white.

### Skins
Skin files are stored as key value pairs, where the value is a tile name.

| option | description |
| --- | --- |
| apple_tile | the tile that apples will be drawn as |
| snake_vertical_tile | the tile that will be displayed for the snake's body when traveling up or down |
| snake_horizontal_tile | the tile that will be displayed for the snake's body when traveling left or right |
| snake_left_up_tile | the tile that will be displayed for the snake's body when traveling from right to up, or from traveling from down to left. |
| snake_right_up_tile | the tile that will be displayed for the snake's body when traveling from left to up, or from traveling from down to right. |
| snake_left_down_tile | the tile that will be displayed for the snake's body when traveling from right to down, or from traveling from up to left. |
| snake_right_down_tile | the tile that will be displayed for the snake's body when traveling from left to down, or from traveling from up to right. |
| snake_tail_down_tile | the tile that will be displayed when the snake's tail is traveling down. |
| snake_tail_up_tile | the tile that will be displayed when the snake's tail is traveling up |
| snake_tail_right_tile | the tile that will be displayed when the snake's tail is traveling right |
| snake_tail_left_tile | the tile that will be displayed when the snake's tail is traveling left |
| snake_head_down_tile | the tile that will be displayed when the snake's head is traveling down |
| snake_head_up_tile | the tile that will be displayed when the snake's head is traveling up |
| snake_head_right_tile | the tile that will be displayed when the snake's head is traveling right |
| snake_head_left_tile | the tile that will be displayed when the snake's head is traveling left |

example:
```
apple_tile=apple
snake_vertical_tile=skin_arrow_vert
snake_horizontal_tile=skin_arrow_hor
snake_left_up_tile=skin_arrow_left_up
snake_right_up_tile=skin_arrow_right_up
snake_left_down_tile=skin_arrow_left_down
snake_right_down_tile=skin_arrow_right_down
snake_tail_down_tile=skin_arrow_tail_down
snake_tail_up_tile=skin_arrow_tail_up
snake_tail_right_tile=skin_arrow_tail_right
snake_tail_left_tile=skin_arrow_tail_left
snake_head_down_tile=skin_arrow_head_down
snake_head_up_tile=skin_arrow_head_up
snake_head_right_tile=skin_arrow_head_right
snake_head_left_tile=skin_arrow_head_left
```
