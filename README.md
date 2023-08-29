trex
----

trex is a **tr**ansient **ex**ecution vulnerability test suite, designed to help ensure mitigations are working in similar-to-real-world scenarios.

trex is based on Google's [Safeside](https://github.com/google/safeside) project, which has sadly been inactive for some time.  Safeside was intended to be a literate codebase for educating people on how speculative mechanisms work, whereas trex has the primary goal of being a pass/fail test suite.  Much of trex's code is adapted to C from the Safeside source and we are very grateful for its existence.

trex isn't especially reliable yet, and thus is yet to meet its goal.  In the meantime, it may still be useful or interesting to you.

trex was (mostly) written by Russell Currey and Nicholas Miehlbradt, but the Safeside authors deserve the most credit.

Requirements
============

- meson
- gcc or clang

Supported architectures
=======================

- x86\_64
- ppc64 (primarily tested on LE, but BE should work too)
- arm64

Supported operating systems
===========================

- Linux
- AIX (experimental)

Building
========

Run `make`.

trex uses `meson` because it's the only build system I could ~tolerate~understand.

Running
=======

Run the binaries in `build/{demos,tests}`.

Cross Compilation
=================

To cross compile use the flag `--cross-file` and the path to the cross compiler definition file to meson (or `make.sh` which will then passe them on to meson). These are stored in the `cross` directory.
