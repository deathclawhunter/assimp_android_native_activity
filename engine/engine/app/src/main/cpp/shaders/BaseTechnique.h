#ifndef _BASE_TECHNIQUE_H_
#define _BASE_TECHNIQUE_H_

#include "Technique.h"
#include "lighting_technique.h"
#include "ogldev_math_3d.h"

class BaseTechnique : public Technique {

public:
    BaseTechnique();
    bool Init(const char *pVertexShaderFile, const char *pFragmentShaderFile);

public:
    GLint m_AttrPositionLocation;
    GLint m_AttrTexcoordLocation;
    GLint m_AttrNormalLocation;
};


#endif	/* _BASE_TECHNIQUE_H_ */
