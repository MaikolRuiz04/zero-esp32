# Legacy / Archived Code

This directory collects older or currently unused modules and assets that were removed from the active firmware build to keep `src/` lean.

Contents:
- `features/image_display.*` – Experimental JPEG (SPIFFS) rendering demo for centered scaled image display using TJpg_Decoder.
- `features/countdown_clock.*` – Initial blocking countdown implementation (used `delay(1000)` each loop) before optimized non-blocking timer + incremental redraw architecture.
- `data/pic.jpg` – Sample JPEG referenced by the image display demo.

Rationale for archiving instead of deleting:
- Provides a reference for future image/persistence work (SPIFFS usage + JPEG scaling pattern).
- Offers historical contrast with the non-blocking timer design.

Guidelines:
- Do not compile these directly; they are excluded from active builds because nothing includes them.
- If you resurrect functionality, move the files back under `src/` (and refactor to match current non-blocking patterns) rather than editing in-place here.
- Periodically prune this folder if it grows or once the history is well-documented in git.

To restore a file (example):
```
# Move back a feature
mv archive/legacy/features/image_display.* src/features/
```

If you never need these again, you can safely delete the entire `archive/legacy` folder.
