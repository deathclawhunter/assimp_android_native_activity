#ifndef _BASIC_TECHNIQUE_H_
#define _BASIC_TECHNIQUE_H_

#include "ogldev_math_3d.h"
#include "BaseTechnique.h"

/**
 * Very basic shader including world transformations and environment lighting
 */
class BasicTechnique : public BaseTechnique {
public:

    BasicTechnique();

    virtual bool Init(const char *pVertexShader, const char *pFragmentShader);

    void SetWVP(const Matrix4f &WVP);

    void SetWorldMatrix(const Matrix4f &WVP);

    void SetColorTextureUnit(uint TextureUnit);

    void SetDirectionalLight(const DirectionalLight &Light);

    void SetEyeWorldPos(const Vector3f &EyeWorldPos);

    void SetMatSpecularIntensity(float Intensity);

    void SetMatSpecularPower(float Power);

private:

    GLuint m_WVPLocation;
    GLuint m_WorldMatrixLocation;
    GLuint m_ColorTextureLocation;
    GLuint m_EyeWorldPosLocation;
    GLuint m_MatSpecularIntensityLocation;
    GLuint m_MatSpecularPowerLocation;

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Direction;
    } m_DirLightLocation;
};


#endif	/* _BASIC_TECHNIQUE_H_ */
