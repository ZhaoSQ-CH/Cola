import time, pyb, struct

C = ord('C')
F = ord('F')
G = ord('G')
B = ord('B')
R = ord('R')
L = ord('L')
D = ord('D')
U = ord('U')
E = ord('E')
S = ord('S')
H = ord('H')
uart3 = pyb.UART(3, 500000, timeout_char = 1000)
green_led = pyb.LED(2)

def send_89(direct,velcity):
    '''
    功能字为0x89，控制前进(后退)、左(右)、上升(下降)速度大小
    speed字段必须正
    '''
    s = 0xAA+0x8C+direct+(int(velcity/256))+(int(velcity%256))
    s = int(s % 256)
    temp_flow = struct.pack("<BBBBBhB",
                   0x00,
                   0xAA,
                   0x89,
                   03,
                   direct,
                   int(velcity),
                   s)
    uart3.write(temp_flow)

def send_98(direct,velcity):
    '''
    功能字为0x98，控制顺(逆)时针转动速度大小
    speed字段正负一样
    '''
    s = 0xAA+0x9B+direct+(int(velcity/256))+(int(velcity%256))
    s = int(s % 256)
    temp_flow = struct.pack("<BBBBBhB",
                   0x00,
                   0xAA,
                   0x98,
                   03,
                   direct,
                   velcity,
                   s)
    uart3.write(temp_flow)

def send_cmd(direct,velcity):
    if direct == C or direct == F:
        send_98(direct,velcity)
        time.sleep(3)
    else:
        send_89(direct,velcity)
        if direct == S:
            time.sleep(3)


