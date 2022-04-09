## i3-termdialogs

This was created as a test of FTXUI library in an attempt to create terminal-based dialogs for i3
use. It may be developed further in the future, it may not. :)

### Building

Building should be as simple as:

    mkdir build
    cd build
    cmake ..
    make

### Dialogs

To allow small windows on i3, You have to configure minimum floating window size.
You also need a rule that will make the terminal float. I use matching with `floatme` title.

For example:

    for_window [title="floatme"] floating enable    # make windows with "floatme" title float
    floating_minimum_size 50 x 20                   # lower the minimum floating window size
                                                    # to 50 width and 20 height

#### volume

Dialog for increasing and decreasing volume.

![volume screenshot](docs/volume.png)

Example usage:

    urxvtc --geometry 60x3 --title floatme -e volume    # urxvt has a bug where new window height
                                                        # is 1 row lower than specified
    st -g 60x2 -t floatme -e volume
    xterm -T floatme -g 60x2 -e .build/volume
