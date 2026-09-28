# pump

HPLC pump machine HMI for liquid-phase analysis.

## Projects

| Path | Role |
|------|------|
| [`hmi/`](hmi/) | New Qt5 desktop/embedded rewrite (`lc3000u_hmi`) |
| [`pump/weiduodianzi/`](pump/weiduodianzi/) | Legacy LC3000U UI (Qt4-era sources; desktop via Qt5 or Qt 4.8.5) |

## Cursor shortcuts (multi-root with `application`)

Global keybindings (already set):

- **F6** → pick a build task
- **F5** → pick a launch configuration

Pump tasks / launches:

| F6 task | F5 launch |
|---------|-----------|
| `Build: HMI` | `Run HMI` (Qt5 desktop) |
| `Build: HMI (Qt4)` | `Run HMI (Qt4)` (embedded / Qt 4.8.5) |
| `Build: weiduodianzi` | `Run weiduodianzi` (Qt5) |
| `Build: weiduodianzi (Qt4)` | `Run weiduodianzi (Qt4)` |
| `QMake: …` / `Clean: …` | |

## Legacy weiduodianzi (desktop)

**Frozen:** new features and structural changes go to [`hmi/`](hmi/) only. See [hmi/MIGRATION.md](hmi/MIGRATION.md) for the migration checklist. Config is JSON-only in `hmi/` (no SQLite).

Qt5 (default in Cursor tasks):

```bash
cd pump/weiduodianzi
mkdir -p build_desktop bin
cd build_desktop
/home/jiehua/qt/Qt5.14.2/5.14.2/gcc_64/bin/qmake ../weiduodianzi_desktop.pro
make -j$(nproc)
../bin/weiduodianzi
```

Qt 4.8.5 (original toolkit; install at `/home/jiehua/qt/Qt4.8.5`):

```bash
export PATH=/home/jiehua/qt/Qt4.8.5/bin:$PATH
export LD_LIBRARY_PATH=/home/jiehua/qt/Qt4.8.5/lib:$LD_LIBRARY_PATH
export QT_PLUGIN_PATH=/home/jiehua/qt/Qt4.8.5/plugins
cd pump/weiduodianzi
mkdir -p build_qt4_desktop bin
cd build_qt4_desktop
qmake ../weiduodianzi_qt4_desktop.pro
make -j$(nproc)
../bin/weiduodianzi -graphicssystem raster
```

In Cursor/VS Code, **F5 → Run weiduodianzi (Qt4)** sets the same `PATH` / `LD_LIBRARY_PATH` / `QT_PLUGIN_PATH` via `.vscode/launch.json` (no manual `export` needed).

Both targets write to `pump/weiduodianzi/bin/weiduodianzi` — rebuild with the Qt version you intend to run.

Panel size is **320×240** (3.5″); **640×480** is the 5″ config in `Common.h`. Qt 4.8 on a Wayland desktop (XWayland) often looks softer than on the embedded framebuffer or than the Qt5 desktop build — use Qt5 for day-to-day UI work, or log in with **Ubuntu on Xorg** when you need a sharper Qt4 preview.

`DEFINES += DESKTOP_HMI` skips `/dev/pwm`, fullscreen blank cursor, and `ifconfig`/`hwclock`.
Serial defaults to `/dev/ttyUSB0` (PC) and `/dev/ttyUSB1` (MCU); missing ports are OK for UI-only runs.

Sources under `pump/weiduodianzi/` are **UTF-8** (converted from GB18030/GBK; see `.editorconfig`).
