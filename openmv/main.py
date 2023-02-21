import sensor, image, time, pyb, struct, math
import pid2, data_pack, t1_findblob_task1
from pyb import Pin, SPI

#cs  = Pin("P3", Pin.OUT_OD)

## The hardware SPI bus for your OpenMV Cam is always SPI bus 2.

## NOTE: The SPI clock frequency will not always be the requested frequency. The hardware only supports
## frequencies that are the bus frequency divided by a prescaler (which can be 2, 4, 8, 16, 32, 64, 128 or 256).
## ESP32 SPI:50Mhz 50000000,
#spi = SPI(2, SPI.MASTER, baudrate=39000000, polarity=1, phase=1)

#def write_command_byte(c,i_count):
    #cs.low()
    #for j in range(i_count):
        #spi.send(c)
        ##print(i)
    #cs.high()

#def write_com_byte():
    #for i in range(100):
        #write_command_byte(0x00,100)
        ##write_command_byte(0xFF,80)

#def write_image(img, size):
    #cs.low()
    #spi.send(0x85)
    #spi.send(0xff)
    #spi.send(0xff)
    #spi.send(0xff)
    #spi.send(0xff)
    #spi.send(int (size / 256))
    #spi.send(int (size % 256))
    #spi.send(img)
    #cs.high()

#sensor.reset() # Initialize the camera sensor.
#sensor.set_contrast(1)
#sensor.set_brightness(1)
#sensor.set_saturation(1)
#sensor.set_gainceiling(16)
#sensor.set_pixformat(sensor.GRAYSCALE) # must be this RGB565
#sensor.set_framesize(sensor.QVGA) #must be this
#sensor.skip_frames(10)                                           #前10帧数据舍去，避免刚刚启动，数据不稳定造成误判
#sensor.set_auto_whitebal(False)                                  #关闭自动白平衡
#sensor.skip_frames(time = 2000) # Let new settings take affect.

#clock = time.clock() # Track elapsed milliseconds between snapshots().

###print(cframe.size())
## connected to your computer. The FPS should increase once disconnected.

#while (True):
    #clock.tick() # Track elapsed milliseconds between snapshots().
    #img = sensor.snapshot() # Take a picture and return the image.
    #cframe = img.compressed(quality=80)
    #write_image(cframe.bytearray(),cframe.size())
    ##print(cframe)
    #print(clock.fps()) # Note: Your OpenMV Cam runs about half as fast while


sensor.reset() # Initialize the camera sensor.
sensor.set_contrast(1)
sensor.set_brightness(1)
sensor.set_saturation(1)
sensor.set_gainceiling(16)
sensor.set_pixformat(sensor.GRAYSCALE) # must be this RGB565
sensor.set_framesize(sensor.QVGA) #must be this
sensor.skip_frames(10)                                           #前10帧数据舍去，避免刚刚启动，数据不稳定造成误判
sensor.set_auto_whitebal(False)                                  #关闭自动白平衡
sensor.skip_frames(time = 2000) # Let new settings take affect.

clock = time.clock() # Tracks FPS.
uart = pyb.UART(3, 500000, timeout_char = 1000)                  #打开串口3
red_threshold_01 = (0,55)                                       #目标色块的灰度值参数范围
flag_x_y = 0                                                     #这是一个标志位，循环调整无人机的x方向和y方向
num_stop = 0                                                     #罚站次数，罚站即不对无人机进行位置调整
time_flag = 0
pid_x = pid2.PID(80,0.5,0,0,30)                                   #x轴方向的pid控制，
pid_y = pid2.PID(60,0.5,0,0,30)                                   #y轴方向的pid控制

##################各个字母所对应的ASCII值######################
R = ord('R')
L = ord('L')
S = ord('S')
B = ord('B')
G = ord('G')
E = ord('E')
H = ord('H')
###########################################################
red_led = pyb.LED(1)
green_led = pyb.LED(2)
blue_led = pyb.LED(3)

red_led.off()
green_led.off()
blue_led.off()


while(True):
    flag = uart.readchar()
    if 1:#flag == H:
        blue_led.off()
        print("start work")
        red_led.on()
        t1_findblob_task1.task_find_blob()
    else:
        blue_led.on()

