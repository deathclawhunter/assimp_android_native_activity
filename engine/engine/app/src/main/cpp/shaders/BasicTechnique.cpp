#include <GLES2/gl2.h>
#include "BasicTechnique.h"

BasicTechnique::BasicTechnique() {
}


bool BasicTechnique::Init(const char *pVertexShader, const char *pFragmentShader) {
    if (!BaseTechnique::Init(pVertexShader, pFragmentShader)) {
        return false;
    }

    m_WVPLocation = GetUniformLocation("gWVP");
    m_WorldMatrixLocation = GetUniformLocation("gWorld");
    m_ColorTextureLocation = GetUniformLocation("gColorMap");
    m_DirLightLocation.Color = GetUniformLocation("gDirectionalLight.Base.Color");
    m_DirLightLocation.AmbientIntensity = GetUniformLocation(
            "gDirectionalLight.Base.AmbientIntensity");
    m_DirLightLocation.Direction = GetUniformLocation("gDirectionalLight.Direction");
    m_DirLightLocation.DiffuseIntensity = GetUniformLocation(
            "gDirectionalLight.Base.DiffuseIntensity");

    if (m_DirLightLocation.AmbientIntensity == INVALID_UNIFORM_LOCATION ||
        m_WVPLocation == INVALID_UNIFORM_LOCATION ||
        m_WorldMatrixLocation == INVALID_UNIFORM_LOCATION ||
        m_ColorTextureLocation == INVALID_UNIFORM_LOCATION ||
        m_DirLightLocation.Color == INVALID_UNIFORM_LOCATION ||
        m_DirLightLocation.DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
        m_DirLightLocation.Direction == INVALID_UNIFORM_LOCATION) {
        return false;
    }

    return true;
}

void BasicTechnique::SetWVP(const Matrix4f &WVP) {
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat *) WVP);
}


void BasicTechnique::SetWorldMatrix(const Matrix4f &World) {
    glUniformMatrix4fv(m_WorldMatrixLocation, 1, GL_TRUE, (const GLfloat *) World);
}


void BasicTechnique::SetColorTextureUnit(unsigned int TextureUnit) {
    glUniform1i(m_ColorTextureLocation, TextureUnit);
}


void BasicTechnique::SetDirectionalLight(const DirectionalLight &Light) {
    glUniform3f(m_DirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
    glUniform1f(m_DirLightLocation.AmbientIntensity, Light.AmbientIntensity);
    Vector3f Direction = Light.Direction;
    Direction.Normalize();
    glUniform3f(m_DirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
    glUniform1f(m_DirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
}


