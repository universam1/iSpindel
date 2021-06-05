# iSpindle 文档

***翻译工作正在进行中，欢迎大家贡献力量！此翻译基于英语文档进行翻译，可能与德文原文不一致，请悉知***


iSpindle (ispindel) 文档
===================

**DIY 无线比重计**
***请考虑支持这个项目***  

[![Donate](https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif)](https://www.paypal.me/universam)


![iSpindle in clean water](/pics/floating.jpg)
![Dashboard](/pics/Dashboard.jpg)


## 目录


- [常见问题](FAQ-zh.md)
- [组件](Parts_zh.md)
- [校准](Calibration_zh.md)
- [电路图](circuit_diagram_zh.md)
- [FHEM 上传](upload-FHEM_zh.md)
- [Ubidots 脚本](ubidotsscripting_zh.md)


- [iSpindle 文档](#ispindle-文档)

  - [目录](#目录)
  - [声明许可](#声明许可)
  - [原理](#原理)
    - [*稳心高度*](#稳心高度)
  - [构造](#构造)
    - [详见 组件](#详见-组件)
    - [详见 零部件](#详见-零部件)
    - [详见 电路图](#详见-电路图)
    - [详见 iSpindel 面包板接入](#详见-ispindel-面包板接入)
    - [草图](#草图)
  - [配置](#配置)
    - [Ubidots](#ubidots)
    - [设置模式](#设置模式)
    - [Blynk](#blynk)
      - [演示案例](#演示案例)
      - [使用你自己的程序](#使用你自己的程序)
  - [图形用户界面](#图形用户界面)
    - [校准](#校准)
    - [Ubidots 曲线图](#ubidots-曲线图)
    - [CraftBeerPi](#craftbeerpi)
  - [软件](#软件)
    - [固件刷入](#固件刷入)


***

## 声明许可


> 作者保留所有权利，特此禁止任何违反适用专利的商业用途

***

## 原理

受 [Alternative to the Spindle](http://hobbybrauer.de/forum/viewtopic.php?f=7&t=11157&view=unread#p170499) 帖子启发, 诞生了采用低成本组件来复刻商业电子倾斜测量仪器的想法。

该系统是基于倾斜圆柱体的应用，这是一个巧妙而简单的概念。你不需要任何其它外部的参考（重力除外），而且圆柱体也是非常容易保持清洁的。倾斜角度的变化和浮力有关，因此与糖含量直接相关。质量中心与浮力中心之间会形成一个角度公式，取决于所在液体的密度。


![Kränung](/pics/kraengung.jpg)

因此我们的想法是将一个带有加速度传感器以及温度传感器的无线物联网设备放进一个能浮起的圆柱体当中。该系统利用传感器进行测量，每隔x分钟连接到 Wifi 并将其倾斜角度、即时温度以及电池电量发送到云端服务器，比如 Ubidots.com ，以便存储数据。

在 30 分钟的更新间隔下，它可以实现 3 个月左右的电池寿命！

### *稳心高度*

实际上这就是 “稳心”，圆柱体会随着液体密度与其质量中心和浮力中心的变化而倾斜，倾斜角度可以被测量。

调整圆柱体可以通过在底部增加些少许克重，使圆柱体更加直立，或在盖子增加几克，使其更加倾斜。

软件会从 XYZ 加速器得到的 X 和 Y 的数值计算欧拉角度并形成绝对角度，我们通过柏拉图度、比重值或类似相关的测量参考值的校准参数来进行计算。


***

## 构造

### 详见 [组件](Parts_zh.md)

### 详见 [零部件](Parts_zh.md)

### 详见 [电路图](circuit_diagram_zh.md)

### 详见 [iSpindel 面包板接入](iSpindelbreadboard_zh.md)

***

### 草图
- 如下图所示，3D打印的板用来固定塑料外壳内部的电子元件和电池，该 3D 模型可以在代码仓库里找到
![Sled](/pics/Schlitten_cad.jpg)
![Assembled](/pics/assembled2.jpg)
![Assembled](/pics/assembled.jpg)

<a href="http://www.youtube.com/watch?feature=player_embedded&v=gpVarh8BxhQ" target="_blank"><img src="http://img.youtube.com/vi/gpVarh8BxhQ/0.jpg" 
alt="Druck" width="240" height="180" border="10" /></a>



***

## 配置

### Ubidots

- 首先，必须在 [Ubidots.com](https://ubidots.com) 创建一个免费的帐户
- 接下来, 你需要转至菜单  ```API Credentials``` 以获取 ```Token``` 用于 iSpindel 授权向 Ubidots 账户写入数据。
***把它记下来***  
![Token](/pics/UbiToken.jpg)  

### 设置模式

通过按下  ```复位按钮``` 创建一个接入点，将允许你进行必要的设置来配置设备。在 `操作模式` 下的接入口是不显示的而且也不可访问，因为设计的原则是基于尽可能地缩短活跃时间。基本上它从醒来到发送数据并再次深度睡眠，需要不到三秒的时间，这样能使系统长时间运行下去。

> ```iSpindel``` 通过以 1s 的间隔闪烁 LED 指示灯显示进入 `配置模式` 。 
通过保存您的设置或等待五分钟的超时，它将结束访问接入点并尝试回到 `操作模式` 。


   ![Setup](/pics/configuration.png)

   ![AccessPoint](/pics/AP.png)![Portal](/pics/Portal.png)

  ![Info](/pics/info.png)

> 在 Ubidots ```Sources``` 一栏你可以监控更新的数据，iSpindel 会自行创建一个新的设备.  
现在你可以在 ```Dashboard``` 中创建好看的图表。

### Blynk

#### 演示案例

* 以这个例子来说，你需要 1800 (4x200 + 100 + 900) 个能量值才可以使用免费的版本;
* 下载 Blynk 应用程序：[Android](http://j.mp/blynk_Android) [iOS](http://j.mp/blynk_iOS)
* 扫描点击二维码图标并将相机对准底下的代码。
<p><img src="https://image.ibb.co/gxZFDz/Untitled.png" height="95" /></p>

<img src="/pics/BlynkApp.jpg" width="35%" />  <img src="/pics/BlynkQR.jpg" width="35%" />

* 请享用！

#### 使用你自己的程序

* 如果你想使用自己的应用程序或者尝试进行修改，传输端口的定义如下：
1. V20 - 不含单位的温度;
2. V30 - 不含单位的电量;
3. V1 - 倾斜角度数值;
4. V2 - `摄氏/华氏/开尔文` 温度数值;
5. V3 - 电量数值 `伏特`;
6. V4 - 重力;

***
## 图形用户界面

### 校准

> 为了将测量的 iSpindle 角度转换为柏拉图度（°Plato）、比重值（SG）或其他单位，有必要首先对传感器进行校准，通过已知比重的糖溶液进行多次参考测量。然后可以将这些参考测量值转换为一个数学函数，存储起来以供后续的测量和显示。由于每个自行组装的 iSpindle 会有不一样的测量数值，所以每个设备在组装或重装后必须单独进行校准，以获得准确的测量结果，详细的过程如下所示。

[校正程序](Calibration_zh.md)

### Ubidots 曲线图

- [柏拉图度计算公式](Calibration_zh.md#forumla)

### CraftBeerPi

[CraftBeerPi](https://github.com/universam1/iSpindel/issues/3)
***

## 软件 

### 固件刷入

[刷入固件](Firmware_zh.md)

***如果你喜欢, 请多多支持***  :beers:

[![Donate](https://www.paypalobjects.com/de_DE/DE/i/btn/btn_donate_LG.gif)](https://www.paypal.me/universam)
