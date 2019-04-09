# Sadelica

An ASCII rpg featuring full dialog trees, intricate quest lines, turn-based combat, and intriguing exploration that all runs inside of a terminal window.

### Screenshots

![Screenshot 1](https://i.imgur.com/o69t2tM.png)
![Screenshot 2](https://i.imgur.com/0A7YAvU.png)
![Screenshot 3](https://i.imgur.com/7IEt4L2.png)

### Installation

Requires g++ with -std=c++17, boost algorithm library, and ncurses (if linux)

```
make -j4
make run
```

### Valid Terminals

Terminals can be a bit funky here.  Don't try to execute this on the default windows command prompt because it will not be able to display any of the color codes correctly.  Your options are Git Bash, or ConEmu or something like that.

On Linux, the game uses ncurses, so most terminals should work.  However, there's a problem where ncurses clears the screen at the beginning, so you may have to input a keypress to get the first frame to render, after that it should work smoothly.

### Controls

```
<numpad> - movement (You can use arrow keys, but then you can't go diagonal.)
<tab> - select next party member
<space> - talk
<esc> - leave menu (when not talking)
<q> - quit game
<i> - inventory
<b> - backpack (shortcut to see all items)
<l> - look at
<m> - (combat only) cast magic
<s> - (combat only) shoot weapon
<shift + s> - save game
<shift + l> - load game
<pgup/pgdwn> - scroll game log
```
