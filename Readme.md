### 串口接收模块
##  目前理论上支持多个平台，只在HAL库测试过，该模块通过环形缓冲  
##  区实现，支持接收如上位机不定长数据流或者是指定数据帧或用户自定义帧结构

-------------------------------------------------------------------------------------

# 实现功能
1. 支持两种模式：**Mode_stream**和**Mode_frame**，即数据流模式和帧结构接收模式，前者可不受约束接收
   来自上位机的数据，后者则是通过帧结构来接收数据，并在中断中触发用户回调，用户可自由对接收的帧数据进行解
   析，只需在初始化时提供帧头和帧尾即可
# API汇总
`ConInfo_t * GetConInfo(void);`
`void ConnectInit(Mode_t mode);`
`void StreamRunning(void);`
`void FrameRecProFuncRegister(FrameReCpCallback func,void * dat);`
`void FrameSetFormat(unsigned char head,unsigned char tail);`
`int  Favailable(void);`
`int  Savailable(void);`
`int  Fread(void);`
`int  Sread(void);`
`void Sflush(void);`
`void Fflush(void);`
# 说明：如采用帧格式，需先设置帧格式数据，后调用初始化
`void FrameSetFormat(0xAA,0x55);`
`void ConnectInit(Mode_t mode)`

