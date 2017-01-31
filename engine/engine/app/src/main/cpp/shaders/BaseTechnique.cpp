#include <GLES2/gl2.h>
#include "BaseTechnique.h"

BaseTechnique::BaseTechnique() {
}


bool BaseTechnique::Init(const char *pVertexShaderFile, const char *pFragmentShaderFile) {
    if (!Technique::Init()) {
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, pVertexShaderFile)) {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, pFragmentShaderFile)) {
        return false;
    }

    if (!Finalize()) {
        return false;
    }

    m_AttrPositionLocation = GetAttributeLocation("Position");
    m_AttrTexcoordLocation = GetAttributeLocation("TexCoord");
    m_AttrNormalLocation = GetAttributeLocation("Normal");

    return true;
}

