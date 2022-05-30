## Duktape

[Duktape](https://duktape.org/) is geared around compile time switches to minimise code size and memory footprint. Because of this, we can not rely on shared libraries, as we have no control on what switches it may have been compiled with. It also can cause inconsistent behaviour of plugins across multiple platforms, depending on what version of Duktape is installed on the system.

[#14853](https://github.com/OpenRCT2/OpenRCT2/issues/14853) is the primary issue that was solved by embedding our own copy of duktape.

This directory contains a pre-configured copy of duktape v2.6.0 with the following switches enabled:
* `DUK_USE_CPP_EXCEPTIONS` for C++ exception handling.
* `DUK_USE_DATE_NOW_WINDOWS` for Windows Vista compatibility.
* `DUK_USE_INTERRUPT_COUNTER` for aborting long or non-terminating scripts.

duktape is configured with a command such as:
```
python2 duktape-2.6.0/tools/configure.py --output-directory src/thirdparty/duktape \
    -DUK_USE_CPP_EXCEPTIONS
    -DUK_USE_DATE_NOW_WINDOWS
    -DUK_USE_INTERRUPT_COUNTER
```

See [Configuring Duktape for build](https://wiki.duktape.org/configuring) for more information.
