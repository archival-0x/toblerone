# toblerone

a wallpaper management tool for X11-powered systems

intro
=====

`xsetroot` / `hsetroot` and `feh` are complex. I don't want to spend time figuring out how
they work, since I don't want to spend all my time ricing and setting my wallpaper.

`toblerone` was created with the sole purpose of fixing this issue.


install
=======

You shall need the following from your favorite distribution:

* libcurl
* libx11 / xlib
* Imlib2

Build and install with make:

```
$ make
$ sudo make install
```

manual
======

This should be self-explanatory:

    $ ./toblerone -h
    usage:
    ./toblerone -hbr [FILENAME]

    -h / --help  -- print help menu
    -b / --background [FILENAME] -- set background as image
    -r / --random  -- set random background image (ignored if -b is called)


license
=======

https://codemuch.tech/license.txt
