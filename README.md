

Requirements
------------

- `CMake` >= 3.25
- `Qt` >= 6.7.x


Build
-----

Clone this repo and run:

```sh
git submodule update --init --recursive
```

> [!NOTE]
> From now on, you should run `bare-dev vendor sync` after updating `bare` git submodule.

Now install all the dependencies running `npm install` or your package manager of preference.

Configure and build with `bare-dev`:

```sh
bare-dev configure
bare-dev build
```

Run
---

Only tested on Mac so far...

`./x` is a symlink to `./build/demo.app/Contents/MacOS/demo`.
