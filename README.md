# zlrclib 🎵

<a href="https://zephyrproject-rtos.github.io/zlrclib">
  <img alt="Documentation" src="https://img.shields.io/badge/documentation-3D578C?logo=sphinx&logoColor=white">
</a>
<a href="https://zephyrproject-rtos.github.io/zlrclib/doxygen">
  <img alt="API Documentation" src="https://img.shields.io/badge/API-documentation-3D578C?logo=c&logoColor=white">
</a>

zlrclib is an open source firmware for requesting lyrics from [lrclib.net](https://lrclib.net) 
and displaying them on OLED displays.

The firmware implements a lightweight client for the lrclib.net API, making it possible to fetch time-synced or unsynced lyrics directly from the internet. Lyrics can then be rendered on small displays such as SSD1306 or SH1106-based OLEDs, commonly used in embedded projects.

Built on top of the Zephyr RTOS, zlrclib demonstrates how to integrate networking, HTTP APIs, and text rendering in resource-constrained environments.

zlrclib is licensed under the Apache-2.0 license. The documentation is licensed under the CC BY 4.0 license.

[app_dev]: https://docs.zephyrproject.org/latest/develop/application/index.html
[workspace_app]: https://docs.zephyrproject.org/latest/develop/application/index.html#zephyr-workspace-app
[modules]: https://docs.zephyrproject.org/latest/develop/modules.html
[west_t2]: https://docs.zephyrproject.org/latest/develop/west/workspaces.html#west-t2
[board_porting]: https://docs.zephyrproject.org/latest/guides/porting/board_porting.html
[bindings]: https://docs.zephyrproject.org/latest/guides/dts/bindings.html
[drivers]: https://docs.zephyrproject.org/latest/reference/drivers/index.html
[zephyr]: https://github.com/zephyrproject-rtos/zephyr
[west_ext]: https://docs.zephyrproject.org/latest/develop/west/extensions.html
[runner_ext]: https://docs.zephyrproject.org/latest/develop/modules.html#external-runners

## Getting Started

Before getting started, make sure you have a proper Zephyr development
environment. Follow the official
[Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html).

### Initialization

The first step is to initialize the workspace folder (``workspace``) where
the ``zlrclib`` and all Zephyr modules will be cloned. Run the following
command:

```shell
# initialize workspace for the zlrclib (main branch)
west init -m https://github.com/walidbadar/zlrclib --mr main workspace
# update Zephyr modules
cd workspace
west update
```

### Building and running

To build the application, run the following command:

```shell
cd zlrclib
west build -b $BOARD app
```

where `$BOARD` is the target board.

You can use the `zlrclibdk/esp32/procpu` board found in this
repository. Note that Zephyr sample boards may be used if an
appropriate overlay is provided (see `app/boards`).

A sample debug configuration is also provided. To apply it, run the following
command:

```shell
west build -b $BOARD app -- -DEXTRA_CONF_FILE=debug.conf
```

Once you have built the application, run the following command to flash it:

```shell
west flash
```
