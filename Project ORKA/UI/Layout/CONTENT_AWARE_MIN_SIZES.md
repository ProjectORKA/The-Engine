# Content-Aware Min Sizes — Implementation Plan

Plan for implementing CSS-like **content-aware minimum sizes** in the ORKA retained UI layout engine. Intended for an implementing agent; read this file end-to-end before coding.

---

## 1. Problem statement

Today, flex items can shrink below their content, causing clipped text and broken interaction surfaces (game launcher headers were a real example). Browsers prevent this by default via **`min-width: auto` / `min-height: auto`** on flex items. ORKA currently treats unspecified mins as **hard zero**, so `flex-shrink: 1` can compress headers, rows, and labels.

### Observed gaps (current code)

| Location | Current behavior | Browser-like expectation |
|---|---|---|
| `UiLayoutEngine::measureNode` | `minimumWidth/Height: Auto` → content minimum **0** | Auto → min-content floor |
| `defaultFlexMinimumMain` | Returns **0** when `measured.minimum` on main axis is 0 | Flex item auto-min → min-content on main axis |
| `measureFlex` | Returns `{ Vec2(0), preferred, baseline }` | Minimum should reflect aggregated min-content |
| Scroll flex children | Special-case `minimumMain = 0` when overflow clips main axis | Correct — keep this (CSS `min-height: 0` behavior) |

Relevant files:

- `Project ORKA/UI/Layout/UiLayoutEngine.cpp` — `measureNode`, `measureFlex`, `arrangeFlex`, `defaultFlexMinimumMain`
- `Project ORKA/UI/Layout/UiLayoutTypes.hpp` — `UiSizeSpec`, `UiLayoutStyle`, `UiIntrinsicMeasure`
- `Project ORKA/UI/Authoring/UiStyleApplicationAdapter.cpp` — SCSS `min-width` / `min-height` parsing
- `Project ORKA/UI/Theme/UiAppFlex.cpp` — `min-w-0` class
- `Project ORKA/UI/Text/UiFontService.cpp` — text intrinsic mins (already good)
- `Project ORKA/UI/Testing/UiValidation.cpp` — layout regression tests

---

## 2. Goals

1. **Flex items with default (unspecified) min sizes must not shrink below min-content** on their flex main axis.
2. **Flex containers** (rows/columns) must publish a non-zero intrinsic minimum derived from children so nested headers survive column shrink.
3. **Explicit opt-out remains available:** `min-width: 0`, `min-height: 0`, and `.min-w-0` keep today’s shrink-friendly behavior for scroll panels.
4. **Scroll/clip containers** continue to shrink inside flex parents without forcing content mins (existing overflow exception stays).
5. **No breaking changes** to surfaces that already set explicit `min-height: 0` on scroll areas (e.g. `game-loader.ui.scss` `#game-list-panel`, `#game-list`).

## 3. Non-goals (this pass)

- Full CSS sizing level 3 (`max-content`, `fit-content`, `stretch`, indefinite percentage cycles).
- Grid min-content track sizing beyond what falls out naturally from shared helpers.
- Changing default `flex-shrink` (stay `1`).
- Rewriting all authored UI to remove `.shell-block { flex-shrink: 0 }` — that becomes optional cleanup after landing.

---

## 4. Target behavior (CSS subset to match)

Implement the practical subset used by authored UI:

### 4.1 Flex item default minimum

For a flex item whose `min-width` / `min-height` is **unspecified** (`UiSizeSpec` default / CSS `auto`):

- **Column flex main axis (height):** minimum main size = **min-content height** of the item.
- **Row flex main axis (width):** minimum main size = **min-content width** of the item.

### 4.2 Explicit overrides

| Author input | Meaning |
|---|---|
| *(unset)* | Content-aware auto minimum (new) |
| `min-width: 0` / `min-height: 0` | Minimum **0** — allow shrink (current behavior) |
| `min-width: 120dp` | Fixed floor (already supported) |
| `.min-w-0` class | Fixed `minimumWidth = 0` (already supported) |

**Add:** `.min-h-0` class mirroring `.min-w-0` for column-flex scroll children.

### 4.3 Scroll/overflow exception (keep)

If a flex item clips its main axis (`overflow-y: scroll/auto/hidden` in column flex, or `overflow-x` in row flex), its flex minimum on that axis stays **0** so it can shrink and scroll. This matches CSS and existing comment at `UiLayoutEngine.cpp` ~1028.

### 4.4 Leaf vs container min-content

| Node kind | min-content source |
|---|---|
| Text (`UiFontService::measureIntrinsic`) | Already returns `minimum` — use it |
| Widget leaves (button, image, …) | `measureNode` preferred/minimum from widget metrics |
| Flex container (`div.row`, `div.col`) | Aggregate from `measureFlex` (must be fixed — today returns `minimum = 0`) |
| Plain box with children | Max/min union of child mins already partially done via `content.minimum = max(children.minimum)` in `measureNode` — verify and extend |

---

## 5. Proposed design

### 5.1 Represent “auto min” vs “zero min” explicitly

**Do not overload `UiSizeKind::Auto` for both “unset” and “0”.** Today, style adapter maps CSS `auto` to empty `UiSizeSpec{}`, but `measureNode` treats Auto as 0.

**Recommended approach (minimal API churn):**

1. Keep `UiSizeSpec::Auto` meaning **“automatic / content-aware”** in layout code.
2. Treat **`UiSizeSpec::fixed(0)`** as explicit zero minimum (already produced by `min-height: 0` in SCSS and `.min-w-0`).
3. Update `measureNode` so when `minimumWidth.kind == Auto`, use **`content.minimum.x`** (after child aggregation), not `0`. Same for height.

No new enum value required if `fixed(0)` and `Auto` are distinguished consistently everywhere mins are resolved.

**Audit all call sites** that compare `minimum*Spec.kind != UiSizeKind::Auto` to decide between “explicit min” vs “default flex min helper” — logic becomes:

```cpp
if (minimumMainSpec.kind == UiSizeKind::Fixed && minimumMainSpec.value == 0)
    minimumMain = 0;
else if (minimumMainSpec.kind != UiSizeKind::Auto)
    minimumMain = resolve(minimumMainSpec, ...);
else if (childClipsMainAxis)
    minimumMain = 0;
else
    minimumMain = contentAwareFlexMinimum(measured, axis);
```

### 5.2 Replace `defaultFlexMinimumMain`

Rename/refactor to something like `flexAutomaticMinimumMain(measured, axis)`:

```cpp
Float flexAutomaticMinimumMain(const UiIntrinsicMeasure& measured, UiAxis axis)
{
    const Float contentMin = mainValue(measured.minimum, axis);
    if (contentMin > 0) return contentMin;

    // Defensive fallback: if minimum wasn't propagated, use preferred on cross/simple leaves
    // ONLY when preferred is not clearly a stretched/definite percentage size.
    // Prefer tightening measureFlex/measureNode first so this fallback rarely triggers.
    return 0;
}
```

**Important:** delete or rewrite the old comment (“Preferred often reflects parent constraint…”) once containers propagate true mins — the fallback should not silently reintroduce zero mins for flex containers.

### 5.3 Fix `measureFlex` minimum propagation

Today:

```cpp
return { Vec2(0), preferred, baseline };
```

Change to compute **`minimum` cross/main sizes** from flex line simulation (similar to preferred):

- For each flex line, track **max child min-content on cross axis** and **sum/min of child min-content on main axis** (respect wrapping).
- For single-line column flex: `minimum.height = sum(childMinHeights) + gaps`, `minimum.width = max(childMinWidths)`.
- For single-line row flex: dual logic on axes.
- For wrap: union across lines (cross grows by line count).

Start with **single-line + wrap cases already covered by tests**; add helper `measureFlexMinimum(...)` beside existing preferred logic to keep `measureFlex` readable.

### 5.4 `measureNode` minimum resolution

In `measureNode` (~639–647), replace:

```cpp
const Float minimumContentWidth = nodeStyle.minimumWidth.kind == UiSizeKind::Auto ? 0 : resolve(...);
```

with Auto branch using `content.minimum.x` (and `.y` for height). Explicit `Fixed/Percent/Content` paths stay.

Ensure final:

```cpp
contentPreferred = max(contentPreferred, Vec2(minimumContentWidth, minimumContentHeight));
```

still holds so preferred never drops below min.

### 5.5 Percent / stretch width children

**Risk called out in old comment:** a child with `width: 100%` has preferred width = parent width, but min-content width may be smaller (text min width). For **minimum** calculations, always use **`measured.minimum`**, never preferred, when computing flex floors.

Add a regression test: column flex, shrinking parent, child label with long unbreakable text — child must not clip below one line height.

### 5.6 Optional: `min-h-0` flex class

In `UiAppFlex.hpp` / `UiAppFlex.cpp`:

- Add `minH0 = "min-h-0"` alongside `minW0`.
- Apply `style.minimumHeight = UiSizeSpec::fixed(0)`.

Document in `UI/DESIGN_RULES.md` overflow section: scroll flex children should use **`min-h-0`** (class) or **`min-height: 0`** (SCSS), not rely on overflow alone once auto-min lands.

---

## 6. Implementation phases

### Phase A — Core layout engine (required)

1. Implement `flexAutomaticMinimumMain` and wire into **`arrangeFlex`** and **`measureFlex`** child loops (two duplicated sites today ~693 and ~1022 — dedupe if trivial).
2. Fix **`measureFlex`** to return non-zero `minimum`.
3. Fix **`measureNode`** Auto min resolution to use aggregated `content.minimum`.
4. Verify scroll/clip exception still zeroes mins on clipping axis only.
5. Run/build `UiValidation` layout cases.

### Phase B — Authored UI ergonomics (small)

1. Add `.min-h-0` class.
2. Optionally add `min-height: 0` note to `flex.ui.scss` comment block.

### Phase C — Regression tests (required)

Extend `Project ORKA/UI/Testing/UiValidation.cpp`:

| Test name | Scenario | Assertion |
|---|---|---|
| `flex-auto-min-header-row` | Column shell height 120, header row with text measurer (24px line), list child with flex-grow | Header row height ≥ 24, text not assigned < line height |
| `flex-auto-min-container` | Column shrinks; nested `div.row` with label child | Row container min height propagates; row not shrunk to ~0 |
| `flex-explicit-min-zero-scroll` | Column, child with `overflow-y: auto`, `min-height: 0`, tall content | Child can shrink below content extent; scroll extent > viewport |
| `flex-min-w0-allows-shrink` | Row, item with `.min-w-0`, long text | Item width can shrink below text min width (text wraps or clips per overflow) |
| `game-launcher-header-survival` | Minimal authored bundle mirroring launcher shell structure | Without `shell-block`, headers survive window height 400→200 resize |

Update existing **`flexMinimumContentSize`** test expectations/docstring if behavior becomes stricter (second item may keep more height — document intended outcome).

Add test entry to validation registry (~line 1520).

### Phase D — Consumer cleanup (optional, separate commit)

After Phase A lands and tests pass:

1. **`game-loader.ui.scss`**: try removing `.shell-block { flex-shrink: 0 }` and C++ fixed heights on `#type-title` / `#type-kicker` in `GameLoader.cpp` `applyLauncherLayout`.
2. Keep `min-height: 0` on `#game-list-panel` and `#game-list`.
3. Manual smoke: launcher headers, tile grid wrap, click/hover still OK.

---

## 7. File-by-file checklist

| File | Action |
|---|---|
| `UI/Layout/UiLayoutEngine.cpp` | Main logic: `measureNode`, `measureFlex`, `defaultFlexMinimumMain` / arrange paths |
| `UI/Layout/UiLayoutEngine.hpp` | Only if extracting new helper declarations |
| `UI/Layout/UiLayoutTypes.hpp` | Document `Auto` min semantics in brief comment if helpful |
| `UI/Theme/UiAppFlex.hpp` | Add `minH0` constant |
| `UI/Theme/UiAppFlex.cpp` | Apply `min-h-0` → `minimumHeight = fixed(0)` |
| `UI/Testing/UiValidation.cpp` | New regression cases + registry entries |
| `UI/DESIGN_RULES.md` | Short note under overflow/flex about auto-min vs `min-h-0` |
| `Data/UI/flex.ui.scss` | Optional comment for `min-h-0` once class exists |
| `GameLoader.cpp` / `game-loader.ui.scss` | Optional cleanup in Phase D |

**Do not** change `UiStyleApplicationAdapter` parsing unless tests prove CSS `auto` keyword needs distinct handling — empty spec vs `fixed(0)` should suffice.

---

## 8. Validation commands

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build-orka.ps1
```

Headless UI validation (if wired in project):

```powershell
.\Project ORKA.exe --modern-ui-test
```

Focus failures on layout cases containing `flex-auto-min`, `flexMinimumContentSize`, `flex-basis-wrap-min-content`.

Manual:

1. Launch game loader, resize window shorter/taller — header/title/subtitle stay fully visible.
2. Hover tiles anywhere (including over text) — hover stable.
3. Click tile — game launches, no stuck active state.
4. Tile grid uses horizontal wrap at full window width.

---

## 9. Acceptance criteria

- [ ] Flex items with **unspecified** min sizes cannot be arranged with main size below their min-content on that axis (unless overflow clip exception applies).
- [ ] Flex **container** nodes report non-zero `UiIntrinsicMeasure.minimum` when children have intrinsic mins.
- [ ] Explicit **`min-height: 0` / `min-width: 0`** still allows scroll panels to shrink (`#game-list` behavior unchanged).
- [ ] Existing **`flex-basis-wrap-min-content`** and **`flexMinimumContentSize`** tests pass (update assertions only if strictly more correct).
- [ ] New regression tests listed in Phase C pass.
- [ ] Debug|x64 build succeeds with 0 errors.
- [ ] No new authored UI requires `.shell-block { flex-shrink: 0 }` purely to prevent header clipping (optional cleanup validates this).

---

## 10. Risks and mitigations

| Risk | Mitigation |
|---|---|
| Scroll areas stop shrinking | Keep clip-axis exception; require explicit `min-height: 0` on known scroll flex children; test `flex-explicit-min-zero-scroll` |
| Performance: extra measure passes | Reuse cached `measureNode` results; avoid third full layout pass |
| Percent-width children block shrink incorrectly | Use `measured.minimum`, not preferred, for flex floors; add test |
| Widget/composite internals | Verify buttons with label children: min height includes text line from widget measure or child text nodes |
| Authored reconcile resets interaction | Unrelated to this task — do not scope creep |

---

## 11. Mental model for future authors

After this lands:

- **Default:** flex items respect content min size — headers/labels stay readable.
- **Scroll flex child:** add `min-height: 0` (SCSS) or `min-h-0` (class).
- **Intentional shrink below content:** explicit `min-width: 0` / `min-height: 0`.
- **`flex-shrink: 0`:** still valid when you want to forbid shrinking entirely (badges, fixed chrome), but no longer required just to prevent text clipping.

---

## 12. Suggested commit sequence

1. `UI Layout: propagate flex container min-content minimums`
2. `UI Layout: content-aware auto min for flex items`
3. `UI Flex: add min-h-0 helper class`
4. `UI Tests: flex auto-min regressions`
5. *(optional)* `GameLoader: drop shell-block shrink workaround`

Keep commits reviewable; land tests with or immediately after core logic.
