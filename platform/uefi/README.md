
### PainterEngine in UEFI(edk2)

#### Build

Default without `TimerLib`:

First, you need to put the "PainterEngine" folder into the edk2 root directory.

1. Without `TimerLib`:

> `PX_TimeGetTime()` and `PX_TimeGetTimeUs()` are not implemented, and `elapsed` in `px_main` is hardcode to `100`.

```
build -p PainterEngine\platform\uefi\PainterEngine.dsc -a X64 -t VS2022 -m PainterEngine\platform\uefi\PainterEngineTemplate.inf
```

2. With real `TimerLib`:

> You need use platform specified TimerLib replace `TimerLib|[PATH-TO-SPECIFIC-TIMER-LIB]/xxxTimerLib.inf` in `PainterEngine.dsc`

```
build -p PainterEngine\platform\uefi\PainterEngine.dsc -a X64 -t VS2022 -m PainterEngine\platform\uefi\PainterEngineTemplate.inf -DREAL_TIMER_LIB=TRUE
```

#### Some Info

1. `UEFI_APPLICATION` template: `PainterEngineTemplate.inf` and `UefiAppTemplate.c`
2. `ENTRY_POINT` in `.inf` must be `PainterEngineUefiMain`
3. Must have `main()` function in `UefiAppTemplate.c`

#### State

Feature support Lists:
1. Output(GOP)
2. Input(Keyboard, Mouse)
3. File
4. Time
