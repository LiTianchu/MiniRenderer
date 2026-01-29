# MiniRenderer 
A project built upon the famous Tinyrenderer project https://github.com/ssloy/tinyrenderer

![Showcase](./results/complete_diablo.png)

## Build (CMake)

### One-liner (scripts)

```bash
chmod +x scripts/*.sh
./scripts/build.sh -t Release
./scripts/run.sh --ssao --model head

# switch model (defaults to african head)
./scripts/run.sh ssao boggie
./scripts/run.sh normal diablo3_pose
```

### Manual CMake

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/minirenderer flat head
```

Note: the helper scripts always use a single build folder: `./build`.

## Runtime flags

The renderer is built as an “engine style” binary: all render modes are compiled in.
Pick the mode and model at runtime:

```bash
./scripts/run.sh --flat --model head
./scripts/run.sh --ssao --model boggie
./scripts/run.sh --mode normal --model diablo3_pose

# tweak lighting and rotate model (no rebuild needed)
./scripts/run.sh --normal --model head --intensity 3.0 --yaw 45
```

## CLI reference

### scripts/build.sh

Builds the engine into `./build`.

```bash
./scripts/build.sh -t Debug
./scripts/build.sh -t Release
```

Options:

- `-t, --type <Debug|Release>`: CMake build type (default: `Debug`)

### scripts/run.sh

Runs `./build/minirenderer` and passes runtime flags through.

Basic:

```bash
./scripts/run.sh --flat --model head
./scripts/run.sh --ssao --model boggie
./scripts/run.sh --mode normal --model diablo3_pose
```

Modes:

- `--mode <mode>` where `<mode>` is one of:
	- `wireframe`, `flat`, `smooth`, `texture`, `uv`, `normal`, `depth`, `shadowmap`, `ssao`, `complete`

Convenience mode flags (equivalent to `--mode <mode>`):

- `--wireframe`
- `--flat`
- `--smooth`
- `--texture`
- `--uv`
- `--normal`
- `--depth`
- `--shadowmap`
- `--ssao`
- `--complete`

Model selection:

- `--model <model>` where `<model>` is one of:
	- `head` (default)
	- `african_head` (alias for `head`)
	- `boggie`
	- `diablo3_pose` (aliases: `diablo`, `diablo3`)

Lighting / transform tuning:

- `--intensity <float>`: main light intensity (example: `1.2`, `2.0`, `3.5`)
- `--yaw <degrees>`: rotate model about vertical (Y) axis (example: `-30`, `45`, `90`)

Positional compatibility:

```bash
./scripts/run.sh <mode> [model]
```

### Output file

The renderer writes `output.tga` to the current working directory.
When using `scripts/run.sh`, that is the repo root, so you’ll find it at [output.tga](output.tga).

## Demo

**Demo 1 - Wireframe**: edge-only render of the mesh.

![Demo1](./results/wireframe_diablo.png)

**Demo 2 - Depth buffer**: visualized depth (z) output.

![Demo2](./results/depth_buffer_diablo.png)

**Demo 3 - Flat shading**: per-face lighting.

![Demo3](./results/flat_diablo.png)

**Demo 4 - Smooth (Gouraud) shading**: per-vertex lighting interpolation.

![Demo4](./results/smooth_diablo.png)

**Demo 5 - UV**: UV coordinates mapped to colors.

![Demo5](./results/uv_diablo.png)

**Demo 6 - Texture (diffuse map)**: textured render with the diffuse/albedo map.

![Demo6](./results/texture_diablo.png)

**Demo 7 - Normal + shadow mapping**: normal-mapped lighting with shadow test.

![Demo7](./results/texture_normal_shadowmap_diablo.png)

**Demo 8 - SSAO**: screen-space ambient occlusion mask (grayscale).

![Demo8](./results/ssao_diablo.png)

**Demo 9 - Complete**: normal + texture + shadowmap, then SSAO applied.

![Demo9](./results/complete_diablo.png)
