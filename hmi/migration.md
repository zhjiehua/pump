# weiduodianzi → hmi migration

## Policy

- **`hmi/`** is the only target for new features and structural changes.
- **`pump/weiduodianzi/`** is frozen as a behavioral reference for regression only.
- Fix blocking bugs in legacy only when they block side-by-side comparison.

## Regression checklist

Build and run both apps (F6/F5 in multi-root workspace):

| Check | weiduodianzi | hmi |
|-------|--------------|-----|
| Shell 320×240 | ✓ | ✓ |
| Run / Param / Setup nav | ✓ | ✓ |
| F1 pump, F5 start/pause, F8 purge | ✓ | ✓ |
| Password gate (Ctrl+Up) | ✓ | ✓ |
| MCU + PC connect | ✓ | ✓ auto-connect |
| Pressure alarm icons | ✓ | ✓ AlarmService |
| Gradient run-time flow | ✓ | ✓ GradientEngine |
| License activation | ✓ | ✓ AuthService |
| Config storage | wda.db (SQLite) | `data/deviceinfo.json`, `data/system.json`, `data/data.json` (+ `.bak`) |

## Import / backup JSON

Admin → **Import JSON** loads a JSON file (legacy monolithic or split) and saves to the active config paths.

| File | Contents |
|------|----------|
| `data/deviceinfo.json` | Serial, license, dates, usage counters |
| `data/system.json` | MCU/CDS config, gradients, calib tables, passwords |
| `data/data.json` | Flow, pressure limits, gradient selection |

Each file has a `.bak` twin; dual backup is handled by `AppSettings::save()`.  
Legacy monolithic `system.json` (with `glp` / `run` sections) is auto-migrated on first load.

## Embedded build (Qt 4.8.5)

```
export PATH=/home/jiehua/qt/Qt4.8.5/bin:$PATH
cd hmi
mkdir -p build_qt4 bin
cd build_qt4
qmake ../hmi.pro CONFIG+=embedded
make -j$(nproc)
```

Or in Cursor: **F6 → Build: HMI (Qt4)**, **F5 → Run HMI (Qt4)**.

See `utils/hmiconfig.h` (`CONFIG+=embedded` / `CONFIG+=touch`).  
Serial I/O uses `third_party/qextserialport` (not `QSerialPort`). JSON config uses
the cJSON-backed `qjsonshim` on Qt4.
