# WheelFlip

只反转**鼠标滚轮**的滚动方向，触控板（和 Magic Mouse）保持自然滚动。

macOS 的“自然滚动”是鼠标和触控板共用的一个开关。WheelFlip 让触控板继续自然滚动，鼠标滚轮改回传统方向（往下滚，内容往下走），在所有应用里都生效，包括 Moonlight 这类串流客户端。

## 原理

一个约 50 行的 C 程序，用 `CGEventTap` 只拦截滚轮事件：

- 普通鼠标滚轮发出的是**离散**滚动事件（`kCGScrollWheelEventIsContinuous == 0`），把它的三组位移值取反；
- 触控板和 Magic Mouse 发出的是**连续**滚动事件，原样放行。

鼠标移动事件不经过它，对指针延迟没有任何影响。没有界面，常驻内存约 1MB。

一个容易踩的坑：设置 `DeltaAxis` 时，CoreGraphics 会自动重算 `FixedPtDeltaAxis` 和 `PointDeltaAxis`。如果逐个字段取反，后两组会被翻回原样，应用读到的方向就没变。所以要先把三组值全部读出来，再一起写回。

## 下载安装（推荐）

从 [Releases](https://github.com/cheneyveron/wheelflip/releases) 下载 `WheelFlip-x.y.zip`，解压后在仓库目录里运行：

```sh
./install.sh ~/Downloads/WheelFlip.app
```

发布版用 Developer ID 签名并经过苹果公证，双击可以直接打开，不会被 Gatekeeper 拦截。安装脚本会把它放到 `~/.local/share/wheelflip/` 并设为开机自动启动。

## 从源码安装

```sh
SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" ./install.sh
```

然后在 系统设置 → 隐私与安全性 → 辅助功能 里打开 **WheelFlip**。第一次启动时它会主动弹窗申请权限；打开开关后约 10 秒自动生效。

- 装到 `~/.local/share/wheelflip/WheelFlip.app`，由 LaunchAgent `local.wheelflip` 开机自动启动。
- `SIGN_IDENTITY` 可以省略，这时用临时签名（ad-hoc）。也能正常运行，但辅助功能权限绑定的是程序的指纹（CDHash），每次重新编译都要重新授权。用开发者证书签名的话，权限绑定的是“应用标识 + 证书”，重新编译后不用再授权。可用的证书可以用 `security find-identity -v -p codesigning` 查看。

## 卸载

```sh
./uninstall.sh
```

## 只编译

```sh
SIGN_IDENTITY="..." ./build.sh   # 输出 build/WheelFlip.app（arm64 + x86_64 通用版）
```

## 已知行为

- 没拿到辅助功能权限时，程序会每隔几秒退出一次，由 launchd 重新拉起。这是有意的：已经在运行的进程读不到新授予的权限，只有新进程才能读到。
- 首次弹窗只出现一次（标记文件 `~/.local/share/wheelflip/.prompted`）。之后如果权限被撤销，需要手动在辅助功能里重新打开。
- 有些高精度滚轮的鼠标（例如开启了平滑滚动的罗技 MX 系列）可能发出连续滚动事件，会被当成触控板放行。
