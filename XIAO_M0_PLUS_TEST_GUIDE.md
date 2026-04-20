# XIAO M0 Plus 测试指导

## 1. 文档目标

本文档用于指导你验证新增的 `Seeed XIAO M0 Plus (SAMD21)` 支持是否正确，覆盖两部分仓库：

- `ArduinoCore-samd`
- `uf2-samdx1`

验证分为三层：

1. `core` 层验证：板型是否出现、variant 是否正确、示例是否能编译、基础上传行为是否正常。
2. `bootloader` 源码层验证：`uf2-samdx1` 中新增的 `XIAO_m0_plus` 是否能正常编译出 bootloader。
3. 端到端验证：Arduino IDE 选择新板型后，是否能进入新 bootloader、识别新 PID，并成功上传。

## 2. 当前代码状态

当前本地代码已经完成这些修改：

- Arduino 应用态 PID：`0x803F`
- Bootloader 目标 PID：`0x003F`
- Arduino 新板型条目：`seeed_XIAO_m0_plus`
- Arduino 新 variant：`variants/XIAO_m0_plus`
- UF2 新板目录：`uf2-samdx1/boards/XIAO_m0_plus`

当前还要注意一件事：

- `boards.txt` 里的 `seeed_XIAO_m0_plus.bootloader.file` 目前仍然临时指向旧的 `XIAO_m0` bootloader 二进制。

这意味着：

1. 你现在就可以验证新板型是否出现、代码是否能编译、variant 是否生效。
2. 你现在就可以验证 `uf2-samdx1` 里的新 bootloader 配置是否能构建。
3. 但你现在还不能把 `Burn Bootloader` 的结果当成最终结论，因为 Arduino core 里还没有接入新编译出的 `XIAO_m0_plus` bootloader 产物。

## 3. 本地仓库路径

- Core 仓库：`/home/seeed/workspace/seeeduino/ArduinoCore-samd`
- Bootloader 仓库：`/home/seeed/workspace/seeeduino/uf2-samdx1`

## 4. 是否需要先推送远端

不需要。

本地测试时，Arduino IDE 不会读取你的远端仓库，只会读取本机安装的板包。也就是说：

1. 你不需要先 push。
2. 你不需要先合并到线上主仓库。
3. 你只需要让 Arduino IDE 读到你本地修改后的 `ArduinoCore-samd` 即可开始测试。

正式给别人使用时，才需要后续把修改推送到对应仓库，并更新 Boards Manager 发布包。

## 5. 测试环境准备

### 5.1 硬件

- 一块 `XIAO SAMD21 Plus`
- USB 数据线
- 如果要直刷 bootloader，准备 SWD / OpenOCD / J-Link
- 如果要验证 I2S 运行时功能，准备 I2S 外设或逻辑分析仪

### 5.2 软件

- Arduino IDE 2.x 或 Arduino CLI
- `git`
- `make`
- `node`
- `arm-none-eabi-gcc`
- `python2`

Linux 下建议额外安装：

- `lsusb`
- `udevadm`
- `screen` 或 `minicom`

### 5.3 初始化 bootloader 子模块

如果 `uf2-samdx1` 还没初始化子模块，先执行：

```bash
cd /home/seeed/workspace/seeeduino/uf2-samdx1
git submodule update --init --recursive
```

## 6. 推荐测试顺序

建议严格按下面顺序做：

1. 先把本地 `ArduinoCore-samd` 接入 Arduino IDE。
2. 确认新板型 `Seeed XIAO M0 Plus` 出现在 IDE 中。
3. 先做编译验证，包括 Blink 和 I2S 编译。
4. 如果板子上已经有兼容 bootloader，可以尝试一次基础上传。
5. 编译 `uf2-samdx1` 里的新 bootloader。
6. 把新 bootloader 烧进板子。
7. 验证 bootloader 态 PID 是否为 `0x003F`。
8. 把新 bootloader 产物回填到 `ArduinoCore-samd`，更新 `bootloader.file`。
9. 再做最终端到端验证。

## 7. 将本地 ArduinoCore-samd 接入 Arduino IDE

### 7.1 推荐方法：通过 sketchbook 的 hardware 目录软链接

Linux 下建议这样做：

```bash
mkdir -p ~/Arduino/hardware/Seeeduino
ln -sfn /home/seeed/workspace/seeeduino/ArduinoCore-samd ~/Arduino/hardware/Seeeduino/samd
```

然后彻底重启 Arduino IDE。

这样做的好处：

1. IDE 会直接读取你当前工作区里的代码。
2. 你每次修改后不用手工复制整个 core。
3. 最适合板卡开发阶段联调。

### 7.2 另一种方法：替换本地已安装板包

也可以直接覆盖 Arduino 本地安装的 Seeed `samd` 板包目录，但这个方式更容易污染现有环境，不如软链接适合开发。

## 8. 验证新板型是否出现

打开 Arduino IDE，检查：

1. `Tools -> Board`
2. 搜索 `XIAO M0 Plus`

预期结果：

- 可以看到 `Seeed XIAO M0 Plus`

如果看不到：

1. 检查软链接路径是否正确。
2. 检查 `~/Arduino/hardware/Seeeduino/samd` 是否真的指向当前仓库。
3. 完整退出 Arduino IDE 后重新打开。
4. 再确认 `boards.txt` 中确实存在 `seeed_XIAO_m0_plus`。

## 9. Core 层编译验证

这一阶段不依赖新 bootloader 二进制，可以先做。

### 9.1 Blink 编译测试

在 Arduino IDE 中选择：

- Board: `Seeed XIAO M0 Plus`

编译下面的 Blink：

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

预期结果：

- 编译通过

这一步主要验证：

1. `boards.txt` 新板型条目可用。
2. `build.variant=XIAO_m0_plus` 生效。
3. 链接脚本和 core 选择正确。

### 9.2 I2S 编译测试

编译下面的测试代码：

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

预期结果：

- 编译通过

这一步主要验证：

1. `variants/XIAO_m0_plus/variant.h` 中新增的 I2S 宏已被正确使用。
2. 新 variant 没有破坏原有 core 编译流程。

### 9.3 I2S 运行时验证，可选

如果你手头有 I2S 外设或逻辑分析仪，可以继续做运行时验证。

当前 pin 定义是：

- `PIN_I2S_SD = PIN_A8`
- `PIN_I2S_SCK = D2`
- `PIN_I2S_FS = D3`

建议验证方式：

1. 连接兼容的 I2S 外设，验证数据传输。
2. 或者使用逻辑分析仪，在 `I2S.begin()` 后观察 D2、D3 是否有时钟/帧同步活动。

## 10. 应用态 USB PID 验证

这一阶段验证的是应用态 PID，也就是 Arduino 程序跑起来后的 USB 身份。

目标值：

- `VID:PID = 2886:803F`

在 Linux 下，可以运行：

```bash
lsusb -d 2886:
```

或者持续观察：

```bash
watch -n 0.5 'lsusb -d 2886:'
```

预期结果：

- 板子运行用户程序时，枚举为 `2886:803f`

说明：

1. 这一步要求你已经把基于新板型编译的固件成功烧录进板子。
2. 如果板子里还是旧固件，可能还会看到旧 PID。

## 11. 在未接入新 bootloader 产物前的上传冒烟测试

这一步只是冒烟测试，不是最终结论。

原因是：

- 当前 `boards.txt` 仍然临时引用旧的 `XIAO_m0` bootloader 二进制。

你可以尝试在 Arduino IDE 中上传 Blink。

可能出现两种结果：

1. 上传成功：说明当前 bootloader 行为和现有上传流程仍兼容。
2. 上传失败：很可能不是 core 写错，而是新 bootloader 还没有真正编译、烧录并接回 Arduino core。

所以这一阶段：

- 成功是加分项
- 失败不能直接判定代码方案错误

## 12. 编译新的 bootloader

### 12.1 编译命令

执行：

```bash
cd /home/seeed/workspace/seeeduino/uf2-samdx1
make BOARD=XIAO_m0_plus
```

这条命令会：

1. 读取 `boards/XIAO_m0_plus/board.mk`
2. 读取 `boards/XIAO_m0_plus/board_config.h`
3. 生成新的 bootloader `.bin`
4. 生成自更新 `.uf2`
5. 生成自更新 `.ino`

输出目录预期为：

- `/home/seeed/workspace/seeeduino/uf2-samdx1/build/XIAO_m0_plus/`

输出文件名通常类似：

- `bootloader-XIAO_m0_plus-<git描述>.bin`
- `update-bootloader-XIAO_m0_plus-<git描述>.uf2`
- `update-bootloader-XIAO_m0_plus-<git描述>.ino`

### 12.2 编译通过标准

满足以下几点即可判定通过：

1. `make` 无报错结束
2. `.bin` 成功生成
3. `.uf2` 成功生成
4. `.ino` 成功生成

如果失败，优先检查：

1. `arm-none-eabi-gcc` 是否在 `PATH`
2. `python2` 是否在 `PATH`
3. 子模块是否初始化

## 13. 将新 bootloader 烧进板子

有两种常用方式。

### 13.1 方式 A：使用 update UF2 自更新

适用条件：

- 板子当前已经有可用的 UF2 bootloader
- 板子能以 U 盘方式挂载

步骤：

1. 让板子进入当前 bootloader 模式
2. 把 `update-bootloader-XIAO_m0_plus-<git描述>.uf2` 拷进挂载出来的磁盘
3. 等待板子自动重启

预期结果：

- 板子重启后，运行的是新 bootloader

### 13.2 方式 B：使用 SWD / OpenOCD / J-Link 直刷

适用条件：

- 板子是空片
- 旧 bootloader 不可用
- 或者不想走 UF2 自更新路径

步骤原则：

1. 使用你的正常 SWD 工具链
2. 把生成出的 `.bin` 烧到 bootloader 起始地址 `0x00000000`

通过标准：

1. 板子可以正常进入 bootloader
2. 双击 reset 后可以停留在 bootloader

## 14. 验证 bootloader 态 PID

新的 bootloader 目标 PID 为：

- `VID:PID = 2886:003F`

让板子进入 bootloader 模式后执行：

```bash
lsusb -d 2886:
```

预期结果：

- 可以看到 `2886:003f`

如果仍然看到旧值 `002f`，通常说明：

1. 实际烧进去的还是旧 bootloader
2. 或者你烧录的并不是 `XIAO_m0_plus` 对应产物

## 15. 将新 bootloader 产物回填到 ArduinoCore-samd

这是完成最终 Arduino 验证前必须补上的一步。

### 15.1 建议做法

建议在 `ArduinoCore-samd/bootloaders` 下新增一个目录，例如：

```text
ArduinoCore-samd/bootloaders/XIAOM0_PLUS/
```

然后把新编译产物复制进去：

1. `bootloader-*.bin`
2. 如有需要，也把 `update-bootloader-*.uf2`
3. 如有需要，也把 `update-bootloader-*.ino`
4. 如有需要，也把 `update-bootloader-*.bin`

### 15.2 更新 boards.txt

把当前 `seeed_XIAO_m0_plus.bootloader.file` 从临时旧路径改成新路径。

目标格式示例：

```text
seeed_XIAO_m0_plus.bootloader.file=XIAOM0_PLUS/bootloader-XIAO_m0_plus-<git描述>.bin
```

### 15.3 为什么这一步必须做

如果不做这一步：

1. Arduino IDE 的 `Burn Bootloader` 不会用到新 bootloader 文件。
2. `ArduinoCore-samd` 和 `uf2-samdx1` 两边配置是脱节的。
3. 你做不到完整的最终回归。

做完这一步后：

1. Arduino IDE 侧 bootloader 文件路径和源码产物一致
2. `Burn Bootloader` 才有意义
3. 最终发布包才完整

## 16. 最终端到端验证

当新 bootloader 产物已经回填到 `ArduinoCore-samd` 后，再做完整验证。

### 16.1 最终上传测试

步骤：

1. Arduino IDE 选择 `Seeed XIAO M0 Plus`
2. 上传 Blink
3. 观察端口切换
4. 等待上传完成

预期结果：

1. 板子先进入 bootloader
2. 主机识别到 bootloader 设备
3. 上传成功
4. 板子回到应用态运行

### 16.2 PID 切换验证

完整上传流程中，USB PID 应该按下面顺序变化：

1. 应用态：`2886:803f`
2. Bootloader 态：`2886:003f`
3. 上传完成后回到应用态：`2886:803f`

### 16.3 Burn Bootloader 验证

如果你的调试环境支持，在 Arduino IDE 中再验证：

1. `Tools -> Burn Bootloader`

预期结果：

1. 使用的是 `seeed_XIAO_m0_plus.bootloader.file` 指定的新 bootloader
2. 烧录成功
3. 板子进入 bootloader 时枚举为 `2886:003f`

## 17. 通过标准清单

### 17.1 Core 层通过

- IDE 中出现 `Seeed XIAO M0 Plus`
- Blink 编译通过
- I2S 示例编译通过
- variant 正确生效

### 17.2 Bootloader 源码层通过

- `make BOARD=XIAO_m0_plus` 成功
- 生成新的 `.bin/.uf2/.ino`
- 板子刷入后能进入 bootloader
- bootloader 枚举为 `2886:003f`

### 17.3 最终端到端通过

- 应用态枚举为 `2886:803f`
- 上传流程成功
- 上传期间 PID 正确在 `803f` 和 `003f` 间切换
- `Burn Bootloader` 使用的是新 bootloader 产物

## 18. 常见失败场景

### 18.1 IDE 里看不到新板型

优先检查：

1. `~/Arduino/hardware/Seeeduino/samd` 是否链接正确
2. Arduino IDE 是否完整重启
3. `boards.txt` 是否已包含 `seeed_XIAO_m0_plus`

### 18.2 Bootloader 编译失败

优先检查：

1. `arm-none-eabi-gcc` 是否存在
2. `python2` 是否存在
3. 子模块是否已初始化

### 18.3 编译能过，但上传失败

通常意味着：

1. Core 层已经正确
2. 但 bootloader 还没有真正换成新版本
3. 或者 host 还在等旧/新 PID 之外的设备切换

### 18.4 Bootloader PID 还是旧值

通常意味着：

1. 没有刷入新 bootloader
2. 刷入的是错误产物
3. 自更新 UF2 没有真正执行成功

## 19. 建议签核顺序

建议按这个顺序签核：

1. 先签核 core 编译行为
2. 再签核 bootloader 构建产物
3. 再签核 bootloader 刷写和 bootloader PID
4. 再签核最终上传链路
5. 最后签核 `Burn Bootloader`

## 20. 最快的实测路径

如果你想最快跑通一次完整验证，建议这样做：

1. 把本地 `ArduinoCore-samd` 通过软链接接入 Arduino IDE
2. 确认 `Seeed XIAO M0 Plus` 出现
3. 先编译 Blink 和 I2S 示例
4. 在 `uf2-samdx1` 中执行 `make BOARD=XIAO_m0_plus`
5. 将新 bootloader 烧进板子
6. 用 `lsusb` 确认 bootloader 态为 `2886:003f`
7. 把新 bootloader 产物复制回 `ArduinoCore-samd/bootloaders`
8. 更新 `boards.txt` 中的 `bootloader.file`
9. 最后在 Arduino IDE 中做一次完整上传验证
