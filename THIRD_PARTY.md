# Third-party code, fonts and assets

Project ORKA itself is licensed under CC BY-NC-SA 4.0 (see `LICENSE`). The items below were written or made by others and keep their own licenses. When you add something you did not make yourself, add it here in the same change.

## Code in this repository

| What | Where | Author / source | License |
|---|---|---|---|
| Optick profiler | `Project ORKA/optick/` | Vadim Slyusarev, https://github.com/bombomby/optick | MIT (notice in each file) |
| nanoflann KD-tree | `Project ORKA/KDTreeImplementation.hpp` | Marius Muja, David G. Lowe, Jose Luis Blanco, https://github.com/jlblancoc/nanoflann | BSD (notice in file) |
| OpenSimplex noise | `Project ORKA/SimplexNoise.cpp` | Port of KdotJPG's original (2014) OpenSimplex noise, https://github.com/KdotJPG | Public domain |
| Integer hash (`lowbias32`) | `Data/Shaders/sdfPointcloudFill.comp` | Chris Wellons, https://github.com/skeeto/hash-prospector | Public domain |
| Integer hash | `Data/Shaders/debugID.frag` | Hugo Elias | Freely published tutorial code |

## Shaders

**Erosion demo** (`Data/Shaders/erosionBufferA.frag`, `erosionBufferB.frag`, `erosionFinal.frag`, `erosionTonemap.frag`). This is a port of Shadertoy code, and **these files as a whole are under CC BY-NC-SA 3.0** (https://creativecommons.org/licenses/by-nc-sa/3.0/), because one part requires it:

- "Terrain Erosion Noise" by Fewes, https://www.shadertoy.com/view/7ljcRW, which forks
- "Eroded Terrain Noise" by clayjohn, https://www.shadertoy.com/view/MtGcWh: MIT License, Copyright 2020 Clay John.
- `Ridges()` / `Erosion()`: adapted from "Gavoronoise" by guil, https://www.shadertoy.com/view/llsGWl, CC BY-NC-SA 3.0.
- `noised()`: "Noise - Gradient - 2D - Deriv" by Inigo Quilez, https://www.shadertoy.com/view/XdXBRH, MIT License.
- `boxIntersection()`: technique from Inigo Quilez, https://iquilezles.org/articles/intersectors
- `RNM()`: Reoriented Normal Mapping, Colin Barré-Brisebois and Stephen Hill (2012).
- `Tonemap_ACES()`: ACES filmic curve fit by Krzysztof Narkowicz (2016).

**Not yet traced:** `Data/Shaders/milkyWaySky.glsl` describes itself as a "Shadertoy-style galaxy" but names no source. If it was adapted from someone else's shader, add the credit and license here and in the file.

## Fonts

| File | Font | License |
|---|---|---|
| `Data/Fonts/text.ttf` | Assistant (Copyright 2020 The Assistant Project Authors; based on Source Sans Pro, Copyright 2010 The Source Sans Pro Authors) | SIL Open Font License 1.1, `licenses/OFL-Assistant.txt` |
| `Data/Fonts/minimal.ttf` | DejaVu Sans ExtraLight (Bitstream Vera derivative) | Bitstream Vera license + public domain changes, `licenses/DejaVu-Fonts-License.txt` |

## Textures and other assets

| File | Source | License |
|---|---|---|
| `Data/Textures/milky-way.jpg` | Most likely the "Stars + Milky Way" texture from Solar System Scope, https://www.solarsystemscope.com/textures/ (unconfirmed: the 8192x4096 size matches, but the image has no embedded credit) | CC BY 4.0, https://creativecommons.org/licenses/by/4.0/ |

All other meshes, textures, HDRs and sounds in `Project ORKA/Data/` are believed to be made by Project ORKA contributors. If you know otherwise, add the source here.

## Libraries installed at build time

The libraries listed in `libraries.txt` (GLFW, GLEW, GLM, Assimp, FreeType, OpenAL Soft, ASIO, FFmpeg, OpenSSL, curl, and others) are installed through vcpkg and are not stored in this repository. Each comes with its own license in the vcpkg install folder (`share/<library>/copyright`). The AI Chat demo downloads llama.cpp release binaries (MIT, https://github.com/ggml-org/llama.cpp) into `Project ORKA/Cache/` at runtime.

## MIT License text

Applies to the MIT-licensed items above, each under its own copyright line:

```
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
