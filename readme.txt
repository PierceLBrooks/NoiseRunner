!!! EPILEPSY WARNING !!!

Hi there. I'm Pierce Brooks.
I made this game Noise Runner for Ludum Dare 34.
It is a game whose art and enemies are created from the bytes which compose any file that is given to it.
By default, Noise Runner uses its own executable file, NoiseRunner.exe.
You can change what file Noise Runner uses, along with various other settings, in the NoiseRunnerData.txt file.
In Noise Runner, you are a single byte trying to make it all the way to the end of the file you live inside.
The objective is to dodge the technicolor triangles that fly towards you and get as much score as possible.
If you are hit by a triangle, you will lose 1/4 of your score.
You gain score according to how close you are to the center of the world.
Good luck!

NoiseRunnerData.txt Settings:

Input File Name
Font File Name
Tile Scale
Target Tile Size
World Scanline Size Factor
World Scanline Speed Factor
Tile Size Factor
Number of Color Channels
Red Channel Mapping
Green Channel Mapping
Blue Channel Mapping
Alpha Channel Mapping

The last 4 settings allow you to map the values which are allocated to each color channel onto other channels.
0 = Red Channel
1 = Green Channel
2 = Blue Channel
3 = Alpha Channel

For the most part, the only setting that really matters is the Input File Name.
That is what is responsible for generating the game world.