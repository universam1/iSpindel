# 固件

## 通过 网页接口 刷入

- 从项目仓库里下载最新的 .bin 固件

    <https://github.com/universam1/iSpindel/releases>

- 在 `Maintenance` 一项里选择固件文件并按更新

    ![flash web](/pics/maintenance.png)

## 通过 USB 刷入 (Windows)

- 下载并运行：[NodeMCU-Flasher](https://github.com/nodemcu/nodemcu-flasher/raw/master/Win32 从 [/releases 路径文件夹](https://github.com/universam1/iSpindel/releases)

    > 重要提示：选择 “下载” 按钮以正确下载文件

- 选择 Wemos 的 COM 端口
- 使用 `Config` 选项 悬着第一个 `.bin` 固件
- 点击 `Flash` 选项显示进度条
- 刷入固件后, 点击 `RESET` 重置键

    > 当 LED 灯以 1 秒的间隔闪烁时，`配置模式` 会处于激活状态，可以通过退出设置模式或等待五分钟超时来退出该模式。

## 通过 USB 刷入 (Linux)

- 下载并解压最新版本的 [esptool](https://github.com/igrr/esptool-ck/releases)，然后使用终端命令将目录更改为新解压的目录，示例：

    ```bash
        wget https://github.com/igrr/esptool-ck/releases/download/0.4.12/esptool-0.4.12-linux64.tar.gz
        tar xf esptool-0.4.12-linux64.tar.gz
        cd esptool-0.4.12-linux64
    ```

- 从 [iSpindel Releases page](https://github.com/universam1/iSpindel/releases) 下载最新的固件，例如：

    ```bash
        wget https://github.com/universam1/iSpindel/releases/download/7.1.2/firmware.bin
    ```

- 确定设备 iSpindel 的路径，这可以通过列出插入 iSpindel 前后的所有串行 USB 设备找到，比如可能在 iSpindel 插入之前。

    ```bash
    ls -l /dev/ttyUSB*
    ls: cannot access '/dev/ttyUSB*': No such file or directory
    ```

    在 iSpindel 插入之后：

    ```bash
    ls -l /dev/ttyUSB*
    crw-rw-rw- 1 root dialout 188, 0 Dec 11 23:20 /dev/ttyUSB0
    ```

    上面显示 iSpindel 设备名称是 /dev/ttyUSB0

- 用找到的 iSpindel 设备名称 (在这个例子是 /dev/ttyUSB0) 通过运行 esptool 指令来上传固件：

    ```bash
    ./esptool -vv -cd nodemcu -cb 921600 -cp /dev/ttyUSB0 -ca 0x00000 -cf ./firmware.bin
    ```
