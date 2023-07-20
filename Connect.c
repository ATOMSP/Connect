#define USE_CONNECT
#include "Connect.h"
#include "Con_port.h"
#include <string.h>



/**
 * @brief flush the frame buff
 * @param  
*/
void Fflush(ConInfo_t *con)
{
    con->FrameFormat.index_head = con->FrameFormat.index_tail = BUFF_HEAD_INDEX;
}
/**
 * @brief flush the stream buff
 * @param  
*/
void Sflush(ConInfo_t *con)
{
    con->StreamFormat.index_head = con->StreamFormat.index_tail = BUFF_HEAD_INDEX;
}



/**
 * @brief get available byte number in frame buff
 * @param  
 * @return 
*/
int Favailable(ConInfo_t *con)
{
    return ((unsigned int)(BUFF_SIZE + con->FrameFormat.index_head - con->FrameFormat.index_tail)) % BUFF_SIZE;
}

/**
 * @brief get available byte number in stream buff
 * @param  
 * @return 
*/
int Savailable(ConInfo_t *con)
{
    return ((unsigned int)(BUFF_SIZE + con->StreamFormat.index_head - con->StreamFormat.index_tail)) % BUFF_SIZE;
}
/**
 * @brief read a byte in frame buff and len--
 * @param  
 * @return read byte Failed if -1
*/
int Fread(ConInfo_t *con)
{
    if (con->FrameFormat.index_head == con->FrameFormat.index_tail) {
        return -1;
    }
    else {
        unsigned char c = con->FrameFormat.FrameBuff[con->FrameFormat.index_tail];
        con->FrameFormat.index_tail = (unsigned short)(con->FrameFormat.index_tail + 1) % BUFF_SIZE;
        return c;
    }
}

/**
 * @brief read a byte in stream buff and len--
 * @param  
 * @return read byte Failed if -1
*/
int Sread(ConInfo_t *con)
{
    if (con->StreamFormat.index_head == con->StreamFormat.index_tail) {
        return -1;
    }
    else {
        unsigned char c = con->StreamFormat.StreamBuff[con->StreamFormat.index_tail];
        con->StreamFormat.index_tail = (unsigned short)(con->StreamFormat.index_tail + 1) % BUFF_SIZE;
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
                      if (port->User.hook && port->FrameFormat.Len == Favailable(port)) { port->User.hook(port); }
                      port->FrameFormat.Len = 0;
                      Fflush(port);
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
void ConnectInit(ConInfo_t *con,Mode_t mode)
{
    memset(con, 0, sizeof(ConInfo_t));
    con->mode = mode;
    con->FrameFormat.state = STATE_IDLE;
    Fflush(con);
    Sflush(con);
#ifdef HAVEN_HAL_CONNECT_INIT
    // HAL_Connect_Enable();
    // HAL_Connect_Enable_K210();
#else
    #error "Can't find HAL_Connect_Enable(void) "
#endif
    con->Isclose = CON_OPEN;
}


/**
 * @brief close the connect
 * @param  
*/
void ConnectClose(ConInfo_t *con)
{
    con->Isclose = CON_CLOSE;
}

/**
 * @brief open the connect
 * @param  
*/
void ConnectStart(ConInfo_t *con)
{
    con->Isclose = CON_OPEN;
#ifdef HAVEN_HAL_CONNECT_INIT
    // HAL_Connect_Enable();
    // HAL_Connect_Enable_K210();
#else
    #error "Can't find HAL_Connect_Enable(void) "
#endif
}


/**
 * @brief stream mode test func so put it in the loop : example while(1) {StreamRunning();}
 * @param  
*/
void StreamRunning(ConInfo_t *con)
{
    con->StreamFormat.Len = Savailable(con);
    for (int i = 0; i < con->StreamFormat.Len; i++) {
        ConnectLog("%c", (char)Sread(con));
    }
}


/**
 * @brief Frame mode Callback register
 * @param func hook 
 * @param dat user dat
*/
void FrameRecProFuncRegister(ConInfo_t *con,FrameReCpCallback func, void* dat)
{
    con->User.hook = func;
    con->User.usrdat = dat;
}

/**
 * @brief Frame mode set frame format after connect Init
 * @param head frame head 
 * @param tail frame tail
*/
void FrameSetFormat(ConInfo_t *con,unsigned char head, unsigned char tail)
{
    con->FrameFormat.Head = head;
    con->FrameFormat.Tail = tail;
}






