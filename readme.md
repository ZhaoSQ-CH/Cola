# 基于OpenMV与ESP32图像传输模块

## 介绍

# 研发进度

## 【任务规划-下一步做的内容】

- UART控制飞行器。
- web界面美化。
- LED灯指示。

## 【任务规划-完成】

- 实现WIFI图传。
- OpenMV 图像传输代码。
- 功率测试 ：最大消耗电流420mA，动态变化，测试电压4.98V。
- 使用openflying V2.1 板子测试代码。
- 测试黑色圆追踪

## 【任务规划-进行中】

2021年12月14日15:54:53 openmv 与 ESP32 进行SPI通信实现。

2021年12月29日21:50:06

实现WIFI图传，目前功能

- 图传最高配置，只支持JPG图像质量：quality=15，格式：GRAYSCALE，大小：QQVGA。

- 实际演示图、

  ![image-20211229215848313](photo/image-20211229215848313.png)

- 支持ESP32按钮

**目前遇到问题**

- 使用条件，需要多进行几次尝试，重连才能成功复现结果，因此需要进一步优化稳定性。
- 图像只支持黑白，需要实现目标支持 RGB565，quality=100，图像大小：QVGA。

2021年12月31日

进行优化WIFI图传，目前优化进度

- 图传最高配置，只支持JPG图像质量：quality=50，格式：GRAYSCALE、RGB5656，大小：QVGA。

- 实际演示图。

  ![image-20211231135308941](photo/image-20211231135308941.png)

- 支持ESP32按钮。

- openMV 测试帧率。

  |   格式    | quality | framesize | FPS  |
  | :-------: | :-----: | :-------: | :--: |
  | GRAYSCALE |   80    |   QVGA    |  50  |
  |  RGB5656  |   50    |   QVGA    |  75  |

- openm spi 频率：39MHz。

**目前遇到问题**

- RGB565（quality=50 ，QVGA ）图像传输会web端会闪烁，需要进一步优化闪烁问题。
- RGB565（quality=80 ，QVGA ）无法传输图像。
- GRAYSCALE（quality=80 ，QVGA ）图像传输会web端会闪烁，需要进一步优化闪烁问题。

# 软件架构



# 硬件配置

## 硬件连接

|   名称   |       openmv        |      ESP32      |
| :------: | :-----------------: | :-------------: |
| SPI_MOSI | P0(PB15, SPI2_MOSI) |  GPIO13(HSPID)  |
| SPI_MISO | P1(PB14, SPI2_MISO) |  GPIO12(HSPIQ)  |
| SPI_CLK  | P2(PB13, SPI2_CLK)  | GPIO14(HSPICLK) |
|  SPI_CS  |  P3(PB12, SPI2_SS)  | GPIO15(HSPICS0) |
|   GND    |         GND         |       GND       |

## 电路图

![image-20211214155322440](photo/image-20211214155322440.png)

![image-20211214155358413](photo/image-20211214155358413.png)

## 参数配置

|       名称       |        OPENMV配置         |      ESP32配置      | ESP32最高配置 |
| :--------------: | :-----------------------: | :-----------------: | :-----------: |
|     SPI速率      |           39MHz           |        未知         |     50MHz     |
|    UART波特率    |          115200           |       115200        |     5MHz      |
| 相机传感器分辨率 | GRAYSCALE、RNG565（QVGA） |       不需要        |      无       |
|     SPI 极性     |    polarity=1, phase=1    | polarity=1, phase=1 |      无       |
|       DMA        |    （暂时了解）未使用     |        使用         |               |

## SPI通信协议

