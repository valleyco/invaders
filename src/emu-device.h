#pragma once
#ifdef __cplusplus
extern "C"
{
#endif
  typedef int (*PortRead)(PortDevice *);
  typedef void (*PortWrite)(PortDevice *, int value);
  typedef void (*IntReq)(int num);
  typedef void (*ClockTicks)(PortDevice *, int ticks);
  typedef void (*Dispose)(PortDevice *);

  typedef int (*KeyEvent)(PortDevice *, int keyVal, int isPressed);

  typedef struct
  {
    int readPortCount;
    PortRead *read;
    int writePortCount;
    PortWrite *write;
    void *data;
    ClockTicks clock_ticks;
    Dispose dispose;
  } PortDevice;

#ifdef __cplusplus
}
#endif