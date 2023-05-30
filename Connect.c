#define USE_CONNECT
#include "Connect.h"
#include "Con_port.h"
#include <string.h>

/* connect Info object */
static ConInfo_t ConInfo;


/**
 * @brief Get the ConInfo
 * @param  
*/
ConInfo_t * GetConInfo(void)
{
  return &ConInfo;
}

/**
 * @brief flush the frame buff
 * @param  
*/
void Fflush(void)
{
    ConInfo.FrameFormat.index_head = ConInfo.FrameFormat.index_tail = BUFF_HEAD_INDEX;
}
/**
 * @brief flush the stream buff
 * @param  
*/
void Sflush(void)
{
    ConInfo.StreamFormat.index_head = ConInfo.StreamFormat.index_tail = BUFF_HEAD_INDEX;
}

/**
 * @brief get available byte number in frame buff
 * @param  
 * @return 
*/
int Favailable(void)
{
    return ((unsigned int)(BUFF_SIZE + ConInfo.FrameFormat.index_head - ConInfo.FrameFormat.index_tail)) % BUFF_SIZE;
}

/**
 * @brief get available byte number in stream buff
 * @param  
 * @return 
*/
int Savailable(void)
{
    return ((unsigned int)(BUFF_SIZE + ConInfo.StreamFormat.index_head - ConInfo.StreamFormat.index_tail)) % BUFF_SIZE;
}
/**
 * @brief read a byte in frame buff and len--
 * @param  
 * @return read byte Failed if -1
*/
int Fread(void)
{
    if (ConInfo.FrameFormat.index_head == ConInfo.FrameFormat.index_tail) {
        return -1;
    }
    else {
        unsigned char c = ConInfo.FrameFormat.FrameBuff[ConInfo.FrameFormat.index_tail];
        ConInfo.FrameFormat.index_tail = (unsigned short)(ConInfo.FrameFormat.index_tail + 1) % BUFF_SIZE;
        return c;
    }
}

/**
 * @brief read a byte in stream buff and len--
 * @param  
 * @return read byte Failed if -1
*/
int Sread(void)
{
    if (ConInfo.StreamFormat.index_head == ConInfo.StreamFormat.index_tail) {
        return -1;
    }
    else {
        unsigned char c = ConInfo.StreamFormat.StreamBuff[ConInfo.StreamFormat.index_tail];
        ConInfo.StreamFormat.index_tail = (unsigned short)(ConInfo.StreamFormat.index_tail + 1) % BUFF_SIZE;
        return c;
    }
}

/**
 * @brief Interrupt process byte so put it in Interrupt function 
 * @param port 
 * @return succeed if return 0
*/
int RecProcess(ConInfo_t* port)
{
    unsigned short i;
    switch ((int)port->mode) {
    case Mode_Frame: {
        i = (unsigned short)(port->FrameFormat.index_head + 1) % BUFF_SIZE;
        if (i != port->FrameFormat.index_tail) {
            switch ((int)port->FrameFormat.state) {
              case STATE_IDLE: {
                  if (port->rec == port->FrameFormat.Head) {
                      port->FrameFormat.state = STATE_REC;
                  }
                  else { port->FrameFormat.state = STATE_IDLE; }
                  break;
              }
              case STATE_REC: {
                  if (port->rec == port->FrameFormat.Tail) {
                      port->FrameFormat.state = STATE_IDLE;
                      if (port->User.hook && port->FrameFormat.Len == Favailable()) { port->User.hook(port); }
                      port->FrameFormat.Len = 0;
                      Fflush();
                  }
                  else if (port->rec != port->FrameFormat.Head) {
                      port->FrameFormat.FrameBuff[port->FrameFormat.index_head] = port->rec;
                      port->FrameFormat.index_head = i;
                      port->FrameFormat.Len++;
                      port->FrameFormat.state = STATE_REC;
                  }
                  break;
              }
            }
        }
        break;
    }
    case Mode_Stream: {
        i = (unsigned short)(port->StreamFormat.index_head + 1) % BUFF_SIZE;
        if (i != port->StreamFormat.index_tail) {
            port->StreamFormat.StreamBuff[port->StreamFormat.index_head] = port->rec;
            port->StreamFormat.index_head = i;
        }
        break;
    }
    default: return 1;
    }
    return 0;
}


/**
 * @brief Connect module Init 
* @param mode : frame mode and stream mode 
*/
void ConnectInit(Mode_t mode)
{
    memset(&ConInfo, 0, sizeof(ConInfo));
    ConInfo.mode = mode;
    ConInfo.FrameFormat.state = STATE_IDLE;
    Fflush();
    Sflush();
#ifdef HAVEN_HAL_CONNECT_INIT
    HAL_Connect_Enable();
#else
    #error "Can't find HAL_Connect_Init(void) "
#endif
    ConInfo.Isclose = CON_OPEN;
}


/**
 * @brief close the connect
 * @param  
*/
void ConnectClose(void)
{
    ConInfo.Isclose = CON_CLOSE;
}

/**
 * @brief open the connect
 * @param  
*/
void ConnectStart(void)
{
    ConInfo.Isclose = CON_OPEN;
#ifdef HAVEN_HAL_CONNECT_INIT
    HAL_Connect_Enable();
#else
    #error "Can't find HAL_Connect_Init(void) "
#endif
}


/**
 * @brief stream mode test func so put it in the loop : example while(1) {StreamRunning();}
 * @param  
*/
void StreamRunning(void)
{
    ConInfo.StreamFormat.Len = Savailable();
    for (int i = 0; i < ConInfo.StreamFormat.Len; i++) {
        ConnectLog("%c", (char)Sread());
    }
}


/**
 * @brief Frame mode Callback register
 * @param func hook 
 * @param dat user dat
*/
void FrameRecProFuncRegister(FrameReCpCallback func, void* dat)
{
    ConInfo.User.hook = func;
    ConInfo.User.usrdat = dat;
}

/**
 * @brief Frame mode set frame format after connect Init
 * @param head frame head 
 * @param tail frame tail
*/
void FrameSetFormat(unsigned char head, unsigned char tail)
{
    ConInfo.FrameFormat.Head = head;
    ConInfo.FrameFormat.Tail = tail;
}
