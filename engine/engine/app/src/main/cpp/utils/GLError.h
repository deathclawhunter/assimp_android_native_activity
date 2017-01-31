#ifndef _GLERROR_H_
#define _GLERROR_H_

#include "GL3Stub.h"
// #include <EGL/egl.h>

#ifdef __cplusplus
extern "C" {
#endif
    extern void printGLString(const char *name, GLenum s);
    extern void checkGlError(const char *op);
#ifdef __cplusplus
}
#endif

#endif /* _GLERROR_H_ */
