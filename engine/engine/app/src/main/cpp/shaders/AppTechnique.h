#ifndef APP_TECHNIQUE_H
#define APP_TECHNIQUE_H

#include "lighting_technique.h"
#include "ogldev_math_3d.h"
#include "BasicTechnique.h"

/**
 * More powerful shader that can handle both static or animated meshes
 * it includes point lights, spot lights and bone animation.
 * This one is used for objects in the scene.
 */
class AppTechnique : public BasicTechnique {
public:

    static const uint MAX_POINT_LIGHTS = 2;
    static const uint MAX_SPOT_LIGHTS = 2;
    static const uint MAX_BONES = 100;

    AppTechnique();

    virtual bool Init();

    void SetPointLights(uint NumLights, const PointLight *pLights);

    void SetSpotLights(uint NumLights, const SpotLight *pLights);

    void SetBoneTransform(uint Index, const Matrix4f &Transform);

    void SetEyeWorldPos(const Vector3f &EyeWorldPos);

    void SetMatSpecularIntensity(float Intensity);

    void SetMatSpecularPower(float Power);

private:

    GLuint m_numPointLightsLocation;
    GLuint m_numSpotLightsLocation;
    GLuint m_EyeWorldPosLocation;
    GLuint m_MatSpecularIntensityLocation;
    GLuint m_MatSpecularPowerLocation;

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
    GLint m_AttrBoneLocation;
    GLint m_AttrWeightLocation;
};


#endif	/* APP_TECHNIQUE_H */
