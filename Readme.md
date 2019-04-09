# Sadelica

An ASCII rpg featuring full dialog trees, intricate quest lines, turn-based combat, and intriguing exploration that all runs inside of a terminal window.

### Screenshots

![Screenshot 1](https://imgur.com/o69t2tM)
![Screenshot 2](https://imgur.com/0A7YAvU)
![Screenshot 3](https://imgur.com/7IEt4L2)

### Installation

Requires g++ with -std=c++17, boost algorithm library, and ncurses (if linux)

Use "make" to build, and "make run" to execute.

### Valid Terminals

Terminals can be a bit funky here.  Don't try to execute this on the default windows command prompt because it will not be able to display any of the color codes correctly.  Your options are Git Bash, or ConEmu or something like that.

On Linux, the game uses ncurses, so most terminals should work.  However, there's a problem where ncurses clears the screen at the beginning, so you may have to input a keypress to get the first frame to render, after that it should work smoothly.

