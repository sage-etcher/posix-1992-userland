# posix-1992-userland

Implementing POSIX.2-1992 userland for modern systems

see `man ls.1` for more details on usage.

Compile-time dependencies:

- posix `make` (`gnumake`, `bsdmake`, `make`)
- gettext

## Install 

```
$ make
# make install
```

## Devel

```
$ create-hook-symlinks
$ echo "ENABLE_TESTS=true" >>config.mk
$ make -B depend
$ make
$ make install
```
