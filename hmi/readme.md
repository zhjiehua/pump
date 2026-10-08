# LC3000U HMI

Desktop (Qt5) and embedded (Qt 4.8.5) HPLC pump panel. Replaces Qt4
`weiduodianzi` as the implementation target; the old tree is reference only.

## Build

### Qt5 desktop (default)

```
cd hmi
mkdir -p build bin
cd build && qmake ../hmi.pro && make -j$(nproc)
../bin/pump
```

Requires Qt 5 Widgets, SerialPort, Network.

### Qt 4.8.5 embedded (board image)

```
export PATH=/home/jiehua/qt/Qt4.8.5/bin:$PATH
export LD_LIBRARY_PATH=/home/jiehua/qt/Qt4.8.5/lib:$LD_LIBRARY_PATH
cd hmi
mkdir -p build_qt4 bin
cd build_qt4 && qmake ../hmi.pro CONFIG+=embedded && make -j$(nproc)
```

Uses `qextserialport` (Polling + timer) instead of `QSerialPort`.  
Build feature flags are documented in `utils/hmiconfig.h` (`CONFIG+=embedded`,
`CONFIG+=touch`).

Typical serial ports on board: `/dev/ttySAC1` (MCU), `/dev/ttySAC2` (CDS).

## Cursor shortcuts

In the multi-root workspace (`application` + `pump`):

| F6 task | F5 launch |
|---------|-----------|
| `Build: HMI` | `Run HMI` (Qt5) |
| `Build: HMI (Qt4)` | `Run HMI (Qt4)` |

Both targets write to `hmi/bin/pump` — rebuild with the Qt version you intend to run.

## Config (JSON dual backup)

No SQLite. Settings live under `{appDir}/data` (or AppConfigLocation/data):

| File | Contents |
|------|----------|
| `data/deviceinfo.json` | Serial, license, manufacture/install dates, usage counters |
| `data/system.json` | MCU/CDS config, gradients, pressure/flow/pulse calib, passwords |
| `data/data.json` | Flow setpoint, pressure limits, gradient selection |

Operator records live under `{appDir}/records`:

| File | Contents |
|------|----------|
| `records/event.json` | Event log |
| `records/alarm.json` | Alarm log |
| `records/maint.json` | Maintenance log |

Each file has a `.bak` twin; loaded if primary is missing/corrupt.  
Each file ends with `#checksum:<md5>` over the JSON body (see `components/dualbackup/`).  
Legacy monolithic `system.json` (with `glp` / `run` sections) is auto-migrated on first load.

Edit via **Setup → Internal** or toolbar **Cfg**.

## MCU protocol selection

On the Internal page, **MCU protocol** combo (default **Legacy**):

| Protocol | Frame | Typical baud | Notes |
|----------|-------|--------------|-------|
| Legacy MCU | 5-byte `0x80` control word | 9600 | Same as weiduodianzi `Protocol_mcu` |
| QinFine | `:` … `!` | 115200 | HPLC_PUMP_Mini / YuanRui |

Legacy maps ml/min → control word with **pump type / word factor**; pressure uses HMI-side `pressRawV0` + scale (51 MPa ≈ 0.0128). QinFine tables / calib require QinFine.

## CDS (PC) side

Unchanged: CXTH legacy or Clarity on RS232/UDP (9600). Configure on the same Internal page.

## PC simulation

Window shows a **320×240** product panel only (same footprint as weiduodianzi).  
MCU/CDS ports, protocol, and UI scale (1x/2x/3x) are under **Setup → Internal**  
(not on the run screen). Keyboard: arrow keys for nav/focus (same as panel F1/F3
left/right, F2/F5 up/down on device), **S** pump/stop, **R** start/hold, **P**
purge (stop only), Backspace back, **Esc** leaves edit / exits to bottom-nav mode on
Run/Param/Setup or **goBack** on sub-pages, **Ctrl+Up** admin. **Enter** starts
`EditCtrl` edit or selects text in other fields / clicks buttons. **↑↓←→** move
focus by on-screen position (not list order). Digits type into an active `EditCtrl`;
touch builds (`CONFIG+=touch`) use the on-screen numeric keyboard for `EditCtrl`
and table cells; default builds keep panel/desktop digit keys.

## UI parity note

Shell now matches weiduodianzi: blue **TopBar**, mid page stack, **BottomBar**
(Run/Param/Setup + link icons) with assets from `hmi.qrc` (`res/ui/*.png`).
Run page layout follows old Flow/Press/State/Time. Setup uses icon grid.
Remaining pages (Lang/Time/GLP/Gradient tables, EditCtrl keyboard) still thin
placeholders — port next as needed. Desktop ports/scale stay under **Setup → Internal**.

## Migration from weiduodianzi

See [migration.md](migration.md) for regression checklist and embedded build.

## Core services (refactored from MachineStat)

| Service | Role |
|---------|------|
| `MachineController` | Orchestration |
| `RunStateMachine` | Stop/Pause/Run/Pump/Purge/PcCtrl |
| `GradientEngine` | Runtime gradient flow |
| `AlarmService` | Comm / over-pressure alarms |
| `AuthService` | License activation |
| `UsageTracker` | GLP sys/pump time counters |
| `CommWorker` | Poll scheduler (500 ms) |

