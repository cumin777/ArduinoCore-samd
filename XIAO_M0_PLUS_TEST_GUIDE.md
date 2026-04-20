# Seeed XIAO M0 Plus Test Guide

## Scope

This document describes how to validate the newly added `Seeed XIAO M0 Plus (SAMD21)` support across:

- `ArduinoCore-samd`
- `uf2-samdx1`

It separates validation into three levels:

1. Core-only validation: board menu, variant selection, compile, and basic upload behavior.
2. Bootloader-source validation: confirm the new UF2 board definition builds and exposes the intended bootloader PID.
3. End-to-end validation: confirm Arduino IDE can compile, reset into bootloader, detect the new bootloader PID, and upload successfully.

## Current Repository State

The current local changes are:

- Application/core PID: `0x803F`
- Bootloader PID target: `0x003F`
- Arduino board entry: `seeed_XIAO_m0_plus`
- Arduino variant directory: `variants/XIAO_m0_plus`
- UF2 board directory: `uf2-samdx1/boards/XIAO_m0_plus`

Important:

- The new board definition already exists in `boards.txt`.
- The new variant already exists and contains the new I2S pin definitions.
- The new UF2 board source already exists.
- `boards.txt` still points `bootloader.file` to the existing `XIAO_m0` bootloader binary for now.

That means:

- You can validate board visibility and sketch compilation immediately.
- You can validate the new bootloader source by building `uf2-samdx1`.
- You cannot fully validate `Burn Bootloader` or the final bootloader binary selection in Arduino until the newly built bootloader artifact is copied back into `ArduinoCore-samd/bootloaders` and `bootloader.file` is updated to the new artifact path.

## Repositories

- Core repo: `/home/seeed/workspace/seeeduino/ArduinoCore-samd`
- Bootloader repo: `/home/seeed/workspace/seeeduino/uf2-samdx1`

## What Does Not Require a Push

You do not need to push to a remote repository before local testing.

Arduino IDE does not read your remote repository. It reads a local board package from one of these places:

1. A locally installed custom hardware package under your sketchbook `hardware/` directory.
2. A Boards Manager installed package under your local Arduino package cache.

For local validation, the recommended method is to expose your modified `ArduinoCore-samd` as a local custom hardware package.

## Prerequisites

### Hardware

- One `XIAO SAMD21 Plus` board
- USB cable
- Optional SWD/OpenOCD/J-Link setup if you want to flash bootloader directly
- Optional I2S peripheral or logic analyzer if you want runtime I2S validation

### Software

- Arduino IDE 2.x or Arduino CLI
- `git`
- `make`
- `node`
- `arm-none-eabi-gcc` for bootloader build
- `python2` for UF2 conversion in `uf2-samdx1`

Optional but useful on Linux:

- `lsusb`
- `udevadm`
- `screen` or `minicom`

### Submodules

If the bootloader repo has not initialized submodules yet:

```bash
cd /home/seeed/workspace/seeeduino/uf2-samdx1
git submodule update --init --recursive
```

## Test Plan Summary

Run the tests in this order:

1. Install the modified `ArduinoCore-samd` locally into Arduino.
2. Confirm `Seeed XIAO M0 Plus` appears in the board list.
3. Compile smoke tests, including an I2S compile test.
4. If the board already has a compatible bootloader, try a basic upload.
5. Build the new bootloader from `uf2-samdx1`.
6. Flash the new bootloader to the board.
7. Verify bootloader PID `0x003F`.
8. Copy the new bootloader artifact into `ArduinoCore-samd`, update `bootloader.file`, and rerun end-to-end upload tests.

## Part 1: Install the Modified Core Locally

### Recommended Method: Sketchbook Hardware Symlink

On Linux, create a local hardware package link:

```bash
mkdir -p ~/Arduino/hardware/Seeeduino
ln -sfn /home/seeed/workspace/seeeduino/ArduinoCore-samd ~/Arduino/hardware/Seeeduino/samd
```

Then restart Arduino IDE.

Expected result:

- Arduino IDE should load this local core instead of requiring a packaged Boards Manager release.

### Alternative Method: Replace an Installed Package

If you prefer, you can overwrite the locally installed package under the Arduino package directory, but the symlink method is safer for development because it keeps the repo live.

## Part 2: Confirm the New Board Appears

Open Arduino IDE and check:

1. `Tools -> Board`
2. Search for `XIAO M0 Plus`

Expected result:

- `Seeed XIAO M0 Plus` appears as a selectable board.

If it does not appear:

- Confirm the symlink target is correct.
- Restart Arduino IDE completely.
- Confirm the board entry exists in `boards.txt`.

Relevant file:

- `/home/seeed/workspace/seeeduino/ArduinoCore-samd/boards.txt`

## Part 3: Core-Only Compile Validation

This phase does not require a newly built bootloader.

### 3.1 Blink Compile Test

Select:

- Board: `Seeed XIAO M0 Plus`

Compile a basic Blink sketch:

```cpp
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
```

Expected result:

- Compilation succeeds.

This validates:

- `boards.txt` entry is usable
- `build.variant=XIAO_m0_plus` resolves correctly
- linker script and core selection are valid

### 3.2 I2S Compile Test

Compile this sketch:

```cpp
#include <I2S.h>

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {}

  if (I2S.begin(I2S_PHILIPS_MODE, 16000, 16)) {
    Serial.println("I2S begin OK");
  } else {
    Serial.println("I2S begin FAIL");
  }
}

void loop() {
}
```

Expected result:

- Compilation succeeds.

This validates the new variant I2S definitions are visible to the core.

Relevant file:

- `/home/seeed/workspace/seeeduino/ArduinoCore-samd/variants/XIAO_m0_plus/variant.h`

### 3.3 Optional Runtime I2S Pin Check

If you have hardware attached, verify the new pin mapping:

- `PIN_I2S_SD = PIN_A8`
- `PIN_I2S_SCK = D2`
- `PIN_I2S_FS = D3`

Runtime validation options:

1. Connect to a compatible I2S device and verify audio transfer.
2. Use a logic analyzer to check activity on D2 and D3 after `I2S.begin()`.

## Part 4: USB PID Validation in Application Mode

This phase validates the application-side USB PID from the core.

Expected application PID:

- `VID:PID = 2886:803F`

On Linux, connect the board in normal application mode and run:

```bash
lsusb -d 2886:
```

Or watch for reconnect events:

```bash
watch -n 0.5 'lsusb -d 2886:'
```

Expected result:

- When the user application is running, the board enumerates as `2886:803f`.

Notes:

- This result depends on the firmware built from the new board definition being flashed successfully.
- If the board still runs old firmware or old descriptors, you may still see the old PID.

## Part 5: Basic Upload Test Before New Bootloader Integration

This is only a partial test.

Because `boards.txt` currently still points to the existing `XIAO_m0` bootloader binary, you should treat this step as a smoke test, not final signoff.

Try uploading Blink from Arduino IDE.

Expected possibilities:

1. Upload succeeds if the existing board bootloader behavior is compatible enough.
2. Upload fails when Arduino expects the new bootloader PID flow but the board still exposes the old bootloader PID.

If upload fails here, that does not automatically mean the core changes are wrong. It usually means the new bootloader binary has not yet been built, flashed, and integrated.

## Part 6: Build the New Bootloader

### 6.1 Build Command

```bash
cd /home/seeed/workspace/seeeduino/uf2-samdx1
make BOARD=XIAO_m0_plus
```

What this does:

- Includes `boards/XIAO_m0_plus/board.mk`
- Includes `boards/XIAO_m0_plus/board_config.h`
- Builds a new bootloader binary, update UF2, and update INO

Relevant build logic:

- `Makefile`
- `boards/XIAO_m0_plus/board.mk`
- `boards/XIAO_m0_plus/board_config.h`

Expected output directory:

- `/home/seeed/workspace/seeeduino/uf2-samdx1/build/XIAO_m0_plus/`

Expected artifact patterns:

- `bootloader-XIAO_m0_plus-<git-describe>.bin`
- `update-bootloader-XIAO_m0_plus-<git-describe>.uf2`
- `update-bootloader-XIAO_m0_plus-<git-describe>.ino`

### 6.2 Build Pass Criteria

The build passes if:

1. `make` completes without errors.
2. The `.bin` artifact is generated.
3. The `.uf2` update file is generated.
4. The `.ino` self-update sketch is generated.

If build fails:

- Check `arm-none-eabi-gcc` exists in `PATH`.
- Check `python2` exists in `PATH`.
- Check submodules are initialized.

## Part 7: Flash the New Bootloader

There are two practical ways to test the new bootloader.

### Option A: Flash the Update UF2

Use this if the board already has a working UF2 bootloader and can mount as a mass-storage device.

Steps:

1. Enter the current bootloader mode.
2. Copy `update-bootloader-XIAO_m0_plus-<git-describe>.uf2` to the mounted drive.
3. Wait for reboot.

Expected result:

- The board reboots with the new bootloader image.

### Option B: Flash by SWD/OpenOCD/J-Link

Use this if:

- the board is blank
- the old bootloader is unusable
- the UF2 self-update path is not available

In that case, flash the generated `.bin` directly to bootloader region `0x00000000` using your normal SWD flow.

Pass criteria:

- The new bootloader starts.
- Double-tap reset enters bootloader mode.

## Part 8: Validate Bootloader PID

Expected bootloader PID:

- `VID:PID = 2886:003F`

Enter bootloader mode and run:

```bash
lsusb -d 2886:
```

Expected result:

- The board enumerates as `2886:003f`.

If it still shows `002f`:

- The new bootloader was not flashed.
- Or the flashed image was not built from `boards/XIAO_m0_plus`.

## Part 9: Integrate the New Bootloader Back into ArduinoCore-samd

This is the missing step required for complete Arduino-side validation.

### 9.1 Copy the New Artifact into the Core Repo

Recommended approach:

1. Create a dedicated bootloader folder inside `ArduinoCore-samd/bootloaders`, for example:

```text
ArduinoCore-samd/bootloaders/XIAOM0_PLUS/
```

2. Copy the generated bootloader `.bin` into that folder.

3. Optionally also copy the `update-bootloader-*.uf2`, `.ino`, and `.bin` files for release packaging consistency.

### 9.2 Update boards.txt

Update `seeed_XIAO_m0_plus.bootloader.file` from the current temporary value to the new artifact path.

Example target form:

```text
seeed_XIAO_m0_plus.bootloader.file=XIAOM0_PLUS/bootloader-XIAO_m0_plus-<git-describe>.bin
```

### 9.3 Why This Step Matters

Without this step:

- `Burn Bootloader` in Arduino will not use the new `XIAO_m0_plus` bootloader artifact.
- The source repo and package repo remain out of sync.

With this step:

- Arduino-side bootloader operations become aligned with the new board definition.

## Part 10: End-to-End Final Validation

After integrating the built bootloader back into `ArduinoCore-samd`, rerun the full flow.

### 10.1 Final Upload Test

1. Select `Seeed XIAO M0 Plus` in Arduino IDE.
2. Upload Blink.
3. Observe the port reset behavior.
4. Confirm upload completes.

Expected result:

- Board enters bootloader mode
- Host detects the bootloader port
- Upload succeeds
- Board reboots into application firmware

### 10.2 PID Transition Test

Observe USB enumeration during upload.

Expected sequence:

1. Normal firmware mode: `2886:803f`
2. Bootloader mode during upload: `2886:003f`
3. Return to normal firmware mode: `2886:803f`

### 10.3 Burn Bootloader Test

If your hardware setup supports it:

1. Use Arduino IDE `Tools -> Burn Bootloader`
2. Confirm the configured bootloader file is the new `XIAO_m0_plus` binary

Expected result:

- Bootloader flashing succeeds
- Board enumerates with bootloader PID `0x003F`

## Pass/Fail Checklist

### Core-Only Pass

- `Seeed XIAO M0 Plus` appears in Arduino IDE
- Blink compiles
- I2S sketch compiles
- Variant resolves correctly

### Bootloader-Source Pass

- `make BOARD=XIAO_m0_plus` succeeds
- New bootloader artifacts are generated
- Flashing the new bootloader succeeds
- Board enumerates as `2886:003f` in bootloader mode

### End-to-End Pass

- Application mode enumerates as `2886:803f`
- Arduino upload succeeds
- PID transitions between `803f` and `003f` correctly
- `Burn Bootloader` uses the new artifact path after integration

## Common Failure Modes

### Board Does Not Appear in Arduino IDE

- Local hardware symlink path is wrong
- Arduino IDE was not restarted
- The folder name under `hardware/Seeeduino/samd` is incorrect

### Bootloader Build Fails

- `arm-none-eabi-gcc` missing
- `python2` missing
- submodules not initialized

### Upload Fails but Compile Succeeds

- Core side is correct, but bootloader side is not yet integrated
- Board is still running an old bootloader PID
- Host is waiting for a port that never appears

### Bootloader PID Is Still Old

- Wrong bootloader image flashed
- Old bootloader remained on the device
- Update UF2 was not actually applied

## Suggested Signoff Order

Use this signoff sequence:

1. Sign off core compile behavior.
2. Sign off bootloader build output.
3. Sign off bootloader flashing and bootloader PID.
4. Sign off final Arduino upload behavior.
5. Sign off `Burn Bootloader` only after the new artifact is copied back into `ArduinoCore-samd` and `bootloader.file` is updated.

## Recommended Next Action

If you want the fastest practical validation path:

1. Install the modified `ArduinoCore-samd` locally.
2. Verify board menu and compile.
3. Build `uf2-samdx1` for `XIAO_m0_plus`.
4. Flash the new bootloader.
5. Verify `2886:003f` in bootloader mode.
6. Copy the new bootloader artifact into `ArduinoCore-samd`.
7. Update `bootloader.file`.
8. Do the final Arduino upload test.
