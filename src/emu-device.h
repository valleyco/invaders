#pragma once
#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct tagPortDevice PortDevice;

  typedef int (*PortRead)(PortDevice *);
  typedef void (*PortWrite)(PortDevice *, int value);
  typedef void (*IntReq)(int num);
  typedef void (*ClockTicks)(PortDevice *);
  typedef void (*Dispose)(PortDevice *);

  typedef struct tagPortDevice
  {
    int readPortCount;
    PortRead *read;
    int writePortCount;
    PortWrite *write;
    void *data;
    ClockTicks clock_ticks;
    Dispose dispose;
  } PortDevice;

  typedef int (*KeyEvent)(PortDevice *, int keyVal, int isPressed);

#ifdef __cplusplus
}
#endif