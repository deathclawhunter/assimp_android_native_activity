#ifndef APP_TECHNIQUE_H
#define    APP_TECHNIQUE_H

#include "technique.h"
#include "lighting_technique.h"
#include "ogldev_math_3d.h"

class AppTechnique : public Technique {
public:

    static const uint MAX_POINT_LIGHTS = 2;
    static const uint MAX_SPOT_LIGHTS = 2;
    static const uint MAX_BONES = 100;

    AppTechnique();

    virtual bool Init();

    void SetWVP(const Matrix4f &WVP);

    void SetWorldMatrix(const Matrix4f &WVP);

    void SetColorTextureUnit(uint TextureUnit);

    void SetDirectionalLight(const DirectionalLight &Light);

    void SetPointLights(uint NumLights, const PointLight *pLights);

    void SetSpotLights(uint NumLights, const SpotLight *pLights);

    void SetEyeWorldPos(const Vector3f &EyeWorldPos);

    void SetMatSpecularIntensity(float Intensity);

    void SetMatSpecularPower(float Power);

    void SetBoneTransform(uint Index, const Matrix4f &Transform);

private:

    GLuint m_WVPLocation;
    GLuint m_WorldMatrixLocation;
    GLuint m_colorTextureLocation;
    GLuint m_eyeWorldPosLocation;
    GLuint m_matSpecularIntensityLocation;
    GLuint m_matSpecularPowerLocation;
    GLuint m_numPointLightsLocation;
    GLuint m_numSpotLightsLocation;

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Direction;
    } m_dirLightLocation;

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        struct {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } m_pointLightsLocation[MAX_POINT_LIGHTS];

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        GLuint Direction;
        GLuint Cutoff;
        struct {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } m_SpotLightsLocation[MAX_SPOT_LIGHTS];

    GLuint m_BoneLocation[MAX_BONES];

public:
    GLint m_AttrPositionLocation;
    GLint m_AttrTexcoordLocation;
    GLint m_AttrNormalLocation;
    GLint m_AttrBoneLocation;
    GLint m_AttrWeightLocation;
};


#endif	/* APP_TECHNIQUE_H */
