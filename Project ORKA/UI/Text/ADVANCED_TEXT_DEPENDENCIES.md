# Optional advanced text backend

`UiTextShaper.cpp` and `UiGlyphCache.cpp` have no external text dependency
unless `ORKA_UI_ENABLE_ADVANCED_TEXT` is defined. The checked-in project does
define it for the x64 Debug and Release configurations. Those configurations
consume a private vcpkg install under `text-deps\x64-windows`; the directory is
ignored by Git and must be populated locally.

The adjacent `vcpkg.json` is pinned by its
`builtin-baseline`. It requests FreeType, HarfBuzz with ICU, ICU itself, and
msdfgen with its extensions. msdfgen's default features are disabled so the
text backend does not pull in Skia.

ICU configures through autotools and cannot build when its install prefix
contains spaces. Keep the install root at the repository-level `text-deps`
path, not below `Project ORKA`. vcpkg also locates `vswhere.exe` through
`ProgramFiles(x86)`; restore that variable in shells where it is missing:

```powershell
[Environment]::SetEnvironmentVariable(
  'ProgramFiles(x86)', 'C:\Program Files (x86)')

& 'C:\vcpkg\vcpkg.exe' install `
  --x-manifest-root="$PWD\Project ORKA\UI\Text" `
  --x-install-root="$PWD\text-deps" `
  --triplet=x64-windows
```

The x64 project configurations use the following equivalent settings. The
global vcpkg tree already supplies ORKA's existing FreeType link dependency,
so the private list does not repeat `freetype[d].lib`.

```xml
<ClCompile>
  <PreprocessorDefinitions>ORKA_UI_ENABLE_ADVANCED_TEXT;%(PreprocessorDefinitions)</PreprocessorDefinitions>
  <AdditionalIncludeDirectories>$(SolutionDir)text-deps\x64-windows\include;$(SolutionDir)text-deps\x64-windows\include\harfbuzz;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
</ClCompile>
<Link>
  <AdditionalLibraryDirectories>$(SolutionDir)text-deps\x64-windows\debug\lib;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
  <AdditionalDependencies>harfbuzz.lib;icuucd.lib;icuind.lib;msdfgen-core.lib;msdfgen-ext.lib;%(AdditionalDependencies)</AdditionalDependencies>
</Link>
```

For Release, use `text-deps\x64-windows\lib` and
`harfbuzz.lib;icuuc.lib;icuin.lib;msdfgen-core.lib;msdfgen-ext.lib`.
The matching HarfBuzz, ICU, msdfgen, tinyxml2, and zlib DLLs must be copied
from the triplet's `bin` or `debug\bin` directory beside `Project ORKA.exe`;
the project post-build step performs these copies.
Custom triplets may change debug suffixes; use the exact names emitted into
that triplet's `lib` directories. All libraries must use ORKA's MSVC runtime
model. Defining the macro without every header and library is an intentional
compile/link error.

Limitations of the optional implementation:

- Font paths must already be resolved by the caller. The service does not
  search operating-system font collections or open packaged/virtual resources.
- One FreeType library and one loaded face per registration are owned by each
  service instance. Shaping is serialized by the service mutex.
- Font fallback is selected per Unicode scalar. It does not perform
  font-specific whole-cluster fallback, so a combining sequence split across
  fonts can render poorly.
- BiDi, grapheme, word, and line boundaries come from ICU. Script itemization
  uses ICU script properties; advanced language-specific font selection is not
  attempted.
- HarfBuzz returns visual glyph order and design-unit positions. The
  `prepareForLayout()` adapter preserves widths and logical break metadata, but
  `UiPreparedText` cannot retain Y advances, glyph offsets, face IDs, or BiDi
  levels. Rendering should consume `UiShapedText` directly.
- Color metadata reports available COLR, CBDT/sbix, and SVG font tables. This
  service does not decode color paints, bitmaps, or SVG and contains no GPU
  upload/rendering code.
- Variable axis values are passed to HarfBuzz and independently applied to the
  FreeType raster face. Unsupported fonts, unknown or out-of-range axes, and
  application failures return distinct `UiGlyphRasterFailure` values.
- Raster dimensions and variation-axis counts are bounded deterministically.
  Empty-outline glyphs should be represented by shaping advances and not
  submitted to the bitmap cache.

The deterministic fallback maps one decoded Unicode scalar to one glyph ID,
uses fixed design-unit advances, and emits basic whitespace/hyphen breaks. It
is suitable for tests and bootstrap UI, not complex-script production text.
