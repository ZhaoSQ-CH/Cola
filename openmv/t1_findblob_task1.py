import sensor, image, time, pyb, struct, math
import pid2, data_pack

from pyb import Pin, SPI

cs  = Pin("P3", Pin.OUT_OD)

# The hardware SPI bus for your OpenMV Cam is always SPI bus 2.

# NOTE: The SPI clock frequency will not always be the requested frequency. The hardware only supports
# frequencies that are the bus frequency divided by a prescaler (which can be 2, 4, 8, 16, 32, 64, 128 or 256).
# ESP32 SPI:50Mhz 50000000,
spi = SPI(2, SPI.MASTER, baudrate=39000000, polarity=1, phase=1)

def write_command_byte(c,i_count):
    cs.low()
    for j in range(i_count):
        spi.send(c)
        #print(i)
    cs.high()

def write_com_byte():
    for i in range(100):
        write_command_byte(0x00,100)
        #write_command_byte(0xFF,80)

def write_image(img, size):
    cs.low()
    spi.send(0x85)
    spi.send(0xff)
    spi.send(0xff)
    spi.send(0xff)
    spi.send(0xff)
    spi.send(int (size / 256))
    spi.send(int (size % 256))
    spi.send(img)
    cs.high()

#clock.tick() # Track elapsed milliseconds between snapshots().
#img = sensor.snapshot() # Take a picture and return the image.
#cframe = img.compressed(quality=80)
#write_image(cframe.bytearray(),cframe.size())
##print(cframe)
#print(clock.fps()) # Note: Your OpenMV Cam runs about half as fast while
#print(cframe.size())
# connected to your computer. The FPS should increase once disconnected.
clock = time.clock()

def compareBlob(blob1, blob2):                                    #比较两个色块大小的函数
    tmp = blob1.pixels() - blob2.pixels()
    if tmp == 0:
        return 0
    elif tmp > 0:
        return 1
    else:
        return -1

def task_find_blob():
    red_threshold_01 = (0,55)                                       #目标色块的灰度值参数范围
    time_flag = 0
    num_stop = 0
    pid_x = pid2.PID(80,0.5,0,0,30)                                   #x轴方向的pid控制，pid参数要自己调
    pid_y = pid2.PID(60,0.5,0,0,30)                                   #y轴方向的pid控制
    flag_x_y = 0

    ##################各个字母所对应的ASCII值######################
    R = ord('R')
    L = ord('L')
    S = ord('S')
    B = ord('B')
    G = ord('G')
    E = ord('E')
    ###########################################################

    while(True):
        clock.tick() # Track elapsed milliseconds between snapshots().
        img = sensor.snapshot().lens_corr(1.8)
        blobs = img.find_blobs([red_threshold_01],
            pixels_threshold=100, merge=True)                         #寻找目标色块，低于150像素的视为噪声
        img.binary([red_threshold_01], invert = True)                 #二值化处理

        #print(cframe)
        print(clock.fps()) # Note: Your OpenMV Cam runs about half as fast while
        #print(cframe.size())
        if blobs:                                                     #如果识别到目标色块
            #print(blobs)                                             #在终端打印出blobs的信息
            bigBlob = blobs[0]                                        #将第一个色块赋值给最大色块
            for blob_temp in blobs:                                   #此循环找出最大色块，进一步滤除噪声
                if compareBlob(bigBlob, blob_temp) == -1:
                    bigBlob = blob_temp
            img.draw_rectangle(bigBlob[0:4])                          #画个矩形框标出色块所在区域
            img.draw_cross(bigBlob[5], bigBlob[6])                    #画个十字架标出色块所在区域的中心点
            cframe = img.compressed(quality=100)
            write_image(cframe.bytearray(),cframe.size())
            if (abs(bigBlob.cx() - 80) < 5) and \
                (abs(bigBlob.cy() - 60) < 5):
                time_flag = 1
            if flag_x_y == 0 :                                        #调节x与y方向的切换标志
                speed_x = pid_x.IncPIDCalc(bigBlob.cx())
               # print("speed_x: %f" %speed_x)
                if speed_x > 0:                                       #说明目标当前x值偏小，无人机偏右，需要向左调整
                    data_pack.send_cmd(L,speed_x)
                else:
                    data_pack.send_cmd(R,abs(speed_x))
                    print("RIGHT")
                flag_x_y = 1                                          #标志位置1，表示下次循环调整y轴方向
            else :
                speed_y = pid_y.IncPIDCalc(bigBlob.cy())
                #print("speed_y: %f" %speed_y)
                if speed_y >= 0:                                      #说明目标当前y值偏小，无人机偏后，需要向前调整
                    data_pack.send_cmd(G,speed_y)
                else:
                    data_pack.send_cmd(B,abs(speed_y))
                flag_x_y = 0                                          #标志位置0，表示下次循环调整x轴方向
            if time_flag == 1:
                num_stop += 1
                if num_stop >= 200 :                                  #200次差不多5秒钟，降落
                    num_stop = 0
                    data_pack.send_cmd(E,0)
        else:
            data_pack.send_cmd(S,0)                                #没有检测到目标色块，这里让无人机原地罚站
