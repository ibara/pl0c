pl0c
====
`pl0c` is a compiler for the
[PL/0](https://en.wikipedia.org/wiki/PL/0)
language. It reads in PL/0 source code and outputs
equivalent C source code.

It was written to be the subject of a series of
[blog posts](https://briancallahan.net/blog/20210814.html)
on writing a compiler from a hands-on practical perspective
for the beginner.

Building
--------
Just run `make`. Then (optionally) `make install`.

If you are on a platform that does not include
[`strtonum(3)`](https://man.openbsd.org/strtonum.3)
in its libc, remove `-DHAVE_STRTONUM` from `CFLAGS`.
In this case, you'll have to put `strtonum.c` in the
same directory as `pl0c`. This will be improved.

Version
-------
The current version is `1.0.1`.

You can find a source tarball in the Releases tab.

Issues and Pull Requests
------------------------
Issues and Pull Requests are accepted at any time.

If your Pull Request changes the compiler in any way, I ask
that you remember to update the bootstrap compiler using
`make genbootstrap` and including the updated bootstrap
compiler as part of the Pull Request.
