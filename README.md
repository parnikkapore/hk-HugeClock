# Big Clock
### A nice and large clock application for Haiku

## What's this?

It's a large, Conky-inspired clock display intended to be a replicant! Although of course you can run it as an app as well.

[![Screenshot of the app in action](https://discuss.haiku-os.org/uploads/default/original/2X/8/81c3d7ff1dbede049c24c260ba0ebea6e448b217.png "Click to play video")](https://cdn.discordapp.com/attachments/415847550495490058/656865134731591725/2019-12-18_21-24-36.mp4)

## Compiling

We use pretty much the standard makefile engine setup. Simply run `make` at the top directory and you should be good to go!

Do not run `setarch` on 32-bit x86, otherwise the replicant may not work.

This project does not have any dependencies beyond the C++ development environment.

A .hpkg package may be created by running `make package`.
