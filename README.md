wmutlils' opt
=============

This project is for programs meant to accompany
[wmutils' core](https://github.com/wmutils/core).

utilities
---------

opt has less utilities than core, here is a little overview:

* chwb2 - control two different window borders
* wew   - print window events
* wname - print a window's name
* xmmv  - move a window with the mouse

For more information, refer to the programs manpages.

dependencies
------------

Like core, opt depends only on the XCB library.

license
-------

This project and all it's code is licensed under the [ISC](http://www.openbsd.org/policy.html)
license. See the LICENSE file.

build & install
---------------

    $ make
    # make install

In the file config.mk you can override build options,
such as the compiler flags or the installation path.
