# Untitled - By: Administrator - 周一 五月 7 2018
import sensor, image, time, math, struct ,pyb,utime
class PID:
    def __init__(self, SetPoint,Proportion,Integral,Derivative,Limit):
        # WARNING: Don't use PA4-X5 or PA5-X6 as echo pin without a 1k resistor
        self.SetPoint   = SetPoint      #设定值
        self.Proportion = Proportion    #P
        self.Integral   = Integral      #I
        self.Derivative = Derivative    #D
        self.Limit      = Limit         #限幅
        self.LastError  = 0             #前1次误差值
        self.PrevError  = 0             #前2次误差值
        self.iError     = 0             #当前误差
        self.iIncpid    = 0             #增量误差
        self.Uk         = 0             #输出返回\

    def IncPIDCalc(self, NextPoint):
          # 当前误差
          self.iError = self.SetPoint - NextPoint
          # 增量误差
          self.iIncpid = (self.Proportion * (self.iError - self.LastError)+ self.Integral *
                         self.iError + self.Derivative * (self.iError - 2 * self.LastError +
                         self.PrevError))
          #存储误差，用于下次计算
          self.PrevError = self.LastError
          self.LastError = self.iError

          self.Uk += self.iIncpid
          self.Uk = self.Limit_Amplitude(self.Uk)
         # print("NextPoint : %f" % NextPoint)
         # print("Uk : %f" % self.Uk)
          return self.Uk

    def Limit_Amplitude(self,num):
        if num > self.Limit :
            return self.Limit
        elif num < -self.Limit:
            return -self.Limit
        else:
            return num



