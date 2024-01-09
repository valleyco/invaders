#pragma once
#ifdef __cplusplus
extern "C"
{
#endif


  typedef struct tagPortDevice
  {
    int readPortCount;
    int (**read)(struct tagPortDevice *);
    int writePortCount;
    void (**write)(struct tagPortDevice *,int val);
    void *data;
    void (*clock_ticks)(struct tagPortDevice *, int ticks);
    void (*dispose)(struct tagPortDevice *);
  } PortDevice;


  typedef int (*PortRead)(PortDevice *);
  typedef void (*PortWrite)(PortDevice *, int value);
  typedef void (*IntReq)(int num);
  typedef void (*ClockTicks)(PortDevice *, int ticks);
  typedef void (*Dispose)(PortDevice *);

  typedef int (*KeyEvent)(PortDevice *, int keyVal, int isPressed);

#ifdef __cplusplus
}
#endif