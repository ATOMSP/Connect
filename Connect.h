#ifndef __CONNECT_H__
#define __CONNECT_H__



#ifdef __cplusplus
extern "C"{
#endif

/* Macro */
#define BUFF_SIZE 128
#define BUFF_HEAD_INDEX 0
#define CON_OPEN 0
#define CON_CLOSE 1

typedef struct Connect_Info ConInfo_t;
typedef int(*FrameReCpCallback)(ConInfo_t*);


/* Connect Mode */
typedef enum Rec_Mode{
  Mode_Frame = 0,
  Mode_Stream
}Mode_t;

/* Frame Mode Status */
typedef enum Rec_Status{
  STATE_IDLE = (int)0,
  STATE_REC
}Status_t;

typedef struct User_Dat{
  void * usrdat;
  FrameReCpCallback hook;
}UserDat_t;

/* Frame mode */
typedef struct FrameFormat{
  unsigned char FrameBuff[BUFF_SIZE];
  volatile unsigned short index_head;
  volatile unsigned short index_tail;  
  unsigned char Head;
  unsigned char Tail;
  unsigned short Len;
  Status_t state;
}FrameFormat_t;

/* Stream mode */
typedef struct StreamFormat_t{
  unsigned char StreamBuff[BUFF_SIZE];
  volatile unsigned short index_head;
  volatile unsigned short index_tail;
  unsigned short Len;
}StreamFormat_t;

/* connect Info Block */
typedef struct Connect_Info{
  volatile unsigned char rec;
  FrameFormat_t FrameFormat;
  StreamFormat_t StreamFormat;
  Mode_t mode;
  unsigned char Isclose;
  unsigned char FrameFlag;
  unsigned char NormalFlag; 
  UserDat_t User;
}ConInfo_t;

/* Connect Module API */
ConInfo_t * GetConInfo(void);
void ConnectInit(Mode_t mode);
void StreamRunning(void);
void FrameRecProFuncRegister(FrameReCpCallback func,void * dat);
void FrameSetFormat(unsigned char head,unsigned char tail);
/* */
int  Favailable(void);
int  Savailable(void);
int  Fread(void);
int  Sread(void);
void Sflush(void);
void Fflush(void);
/* Put in Interrupt func use */
int RecProcess(ConInfo_t * port);

#ifdef __cplusplus
}
#endif






#endif



