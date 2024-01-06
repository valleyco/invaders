#pragma once
#ifdef __cplusplus
extern "C"
{
#endif
typedef int (*PORT_READ)(void *, int);
typedef void (*PORT_WRITE)(void *, int, int);

typedef struct
{
  int portCount;
  int portOffset;
  PORT_READ *read;
  PORT_WRITE *write;
} PortDevice;

#ifdef __cplusplus
}
#endif