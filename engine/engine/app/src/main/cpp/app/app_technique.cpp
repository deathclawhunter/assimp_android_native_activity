#include <limits.h>
#include <string>
#include <GLES2/gl2.h>


#include "app_technique.h"

using namespace std;

AppTechnique::AppTechnique() {
}


bool AppTechnique::Init() {
    if (!Technique::Init()) {
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, "main_shader.vs")) {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, "main_shader.fs")) {
        return false;
    }

    if (!Finalize()) {
        return false;
    }

    m_WVPLocation = GetUniformLocation("gWVP");
    m_WorldMatrixLocation = GetUniformLocation("gWorld");
    m_colorTextureLocation = GetUniformLocation("gColorMap");
    m_eyeWorldPosLocation = GetUniformLocation("gEyeWorldPos");
    m_dirLightLocation.Color = GetUniformLocation("gDirectionalLight.Base.Color");
    m_dirLightLocation.AmbientIntensity = GetUniformLocation(
            "gDirectionalLight.Base.AmbientIntensity");
    m_dirLightLocation.Direction = GetUniformLocation("gDirectionalLight.Direction");
    m_dirLightLocation.DiffuseIntensity = GetUniformLocation(
            "gDirectionalLight.Base.DiffuseIntensity");
    m_matSpecularIntensityLocation = GetUniformLocation("gMatSpecularIntensity");
    m_matSpecularPowerLocation = GetUniformLocation("gSpecularPower");
    m_numPointLightsLocation = GetUniformLocation("gNumPointLights");
    m_numSpotLightsLocation = GetUniformLocation("gNumSpotLights");

    if (m_dirLightLocation.AmbientIntensity == INVALID_UNIFORM_LOCATION ||
        m_WVPLocation == INVALID_UNIFORM_LOCATION ||
        m_WorldMatrixLocation == INVALID_UNIFORM_LOCATION ||
        m_colorTextureLocation == INVALID_UNIFORM_LOCATION ||
        m_eyeWorldPosLocation == INVALID_UNIFORM_LOCATION ||
        m_dirLightLocation.Color == INVALID_UNIFORM_LOCATION ||
        m_dirLightLocation.DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
        m_dirLightLocation.Direction == INVALID_UNIFORM_LOCATION ||
        m_matSpecularIntensityLocation == INVALID_UNIFORM_LOCATION ||
        m_matSpecularPowerLocation == INVALID_UNIFORM_LOCATION ||
        m_numPointLightsLocation == INVALID_UNIFORM_LOCATION ||
        m_numSpotLightsLocation == INVALID_UNIFORM_LOCATION) {
        return false;
    }

    for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_pointLightsLocation); i++) {
        char Name[128];
        memset(Name, 0, sizeof(Name));
        SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Base.Color", i);
        m_pointLightsLocation[i].Color = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Base.AmbientIntensity", i);
        m_pointLightsLocation[i].AmbientIntensity = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Position", i);
        m_pointLightsLocation[i].Position = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Base.DiffuseIntensity", i);
        m_pointLightsLocation[i].DiffuseIntensity = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Atten.Constant", i);
        m_pointLightsLocation[i].Atten.Constant = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Atten.Linear", i);
        m_pointLightsLocation[i].Atten.Linear = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Atten.Exp", i);
        m_pointLightsLocation[i].Atten.Exp = GetUniformLocation(Name);

        if (m_pointLightsLocation[i].Color == INVALID_UNIFORM_LOCATION ||
            m_pointLightsLocation[i].AmbientIntensity == INVALID_UNIFORM_LOCATION ||
            m_pointLightsLocation[i].Position == INVALID_UNIFORM_LOCATION ||
            m_pointLightsLocation[i].DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
            m_pointLightsLocation[i].Atten.Constant == INVALID_UNIFORM_LOCATION ||
            m_pointLightsLocation[i].Atten.Linear == INVALID_UNIFORM_LOCATION ||
            m_pointLightsLocation[i].Atten.Exp == INVALID_UNIFORM_LOCATION) {
            return false;
        }
    }

    for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_SpotLightsLocation); i++) {
        char Name[128];
        memset(Name, 0, sizeof(Name));
        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.Color", i);
        m_SpotLightsLocation[i].Color = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.AmbientIntensity", i);
        m_SpotLightsLocation[i].AmbientIntensity = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Position", i);
        m_SpotLightsLocation[i].Position = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Direction", i);
        m_SpotLightsLocation[i].Direction = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Cutoff", i);
        m_SpotLightsLocation[i].Cutoff = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.DiffuseIntensity", i);
        m_SpotLightsLocation[i].DiffuseIntensity = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Constant", i);
        m_SpotLightsLocation[i].Atten.Constant = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Linear", i);
        m_SpotLightsLocation[i].Atten.Linear = GetUniformLocation(Name);

        SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Exp", i);
        m_SpotLightsLocation[i].Atten.Exp = GetUniformLocation(Name);

        if (m_SpotLightsLocation[i].Color == INVALID_UNIFORM_LOCATION ||
            m_SpotLightsLocation[i].AmbientIntensity == INVALID_UNIFORM_LOCATION ||
            m_SpotLightsLocation[i].Position == INVALID_UNIFORM_LOCATION ||
            m_SpotLightsLocation[i].Direction == INVALID_UNIFORM_LOCATION ||
            m_SpotLightsLocation[i].Cutoff == INVALID_UNIFORM_LOCATION ||
            m_SpotLightsLocation[i].DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
            m_SpotLightsLocation[i].Atten.Constant == INVALID_UNIFORM_LOCATION ||
            m_SpotLightsLocation[i].Atten.Linear == INVALID_UNIFORM_LOCATION ||
            m_SpotLightsLocation[i].Atten.Exp == INVALID_UNIFORM_LOCATION) {
            return false;
        }
    }

    for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_BoneLocation); i++) {
        char Name[128];
        memset(Name, 0, sizeof(Name));
        SNPRINTF(Name, sizeof(Name), "gBones[%d]", i);
        m_BoneLocation[i] = GetUniformLocation(Name);
    }

    return true;
}

void AppTechnique::SetWVP(const Matrix4f &WVP) {
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat *) WVP);
}


void AppTechnique::SetWorldMatrix(const Matrix4f &World) {
    glUniformMatrix4fv(m_WorldMatrixLocation, 1, GL_TRUE, (const GLfloat *) World);
}


void AppTechnique::SetColorTextureUnit(unsigned int TextureUnit) {
    glUniform1i(m_colorTextureLocation, TextureUnit);
}


void AppTechnique::SetDirectionalLight(const DirectionalLight &Light) {
    glUniform3f(m_dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
    glUniform1f(m_dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
    Vector3f Direction = Light.Direction;
    Direction.Normalize();
    glUniform3f(m_dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
    glUniform1f(m_dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
}


void AppTechnique::SetEyeWorldPos(const Vector3f &EyeWorldPos) {
    glUniform3f(m_eyeWorldPosLocation, EyeWorldPos.x, EyeWorldPos.y, EyeWorldPos.z);
}


void AppTechnique::SetMatSpecularIntensity(float Intensity) {
    glUniform1f(m_matSpecularIntensityLocation, Intensity);
}


void AppTechnique::SetMatSpecularPower(float Power) {
    glUniform1f(m_matSpecularPowerLocation, Power);
}


void AppTechnique::SetPointLights(unsigned int NumLights, const PointLight *pLights) {
    glUniform1i(m_numPointLightsLocation, NumLights);

    for (unsigned int i = 0; i < NumLights; i++) {
        glUniform3f(m_pointLightsLocation[i].Color, pLights[i].Color.x, pLights[i].Color.y,
                    pLights[i].Color.z);
        glUniform1f(m_pointLightsLocation[i].AmbientIntensity, pLights[i].AmbientIntensity);
        glUniform1f(m_pointLightsLocation[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
        glUniform3f(m_pointLightsLocation[i].Position, pLights[i].Position.x, pLights[i].Position.y,
                    pLights[i].Position.z);
        glUniform1f(m_pointLightsLocation[i].Atten.Constant, pLights[i].Attenuation.Constant);
        glUniform1f(m_pointLightsLocation[i].Atten.Linear, pLights[i].Attenuation.Linear);
        glUniform1f(m_pointLightsLocation[i].Atten.Exp, pLights[i].Attenuation.Exp);
    }
}

void AppTechnique::SetSpotLights(unsigned int NumLights, const SpotLight *pLights) {
    glUniform1i(m_numSpotLightsLocation, NumLights);

    for (unsigned int i = 0; i < NumLights; i++) {
        glUniform3f(m_SpotLightsLocation[i].Color, pLights[i].Color.x, pLights[i].Color.y,
                    pLights[i].Color.z);
        glUniform1f(m_SpotLightsLocation[i].AmbientIntensity, pLights[i].AmbientIntensity);
        glUniform1f(m_SpotLightsLocation[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
        glUniform3f(m_SpotLightsLocation[i].Position, pLights[i].Position.x, pLights[i].Position.y,
                    pLights[i].Position.z);
        Vector3f Direction = pLights[i].Direction;
        Direction.Normalize();
        glUniform3f(m_SpotLightsLocation[i].Direction, Direction.x, Direction.y, Direction.z);
        glUniform1f(m_SpotLightsLocation[i].Cutoff, cosf(ToRadian(pLights[i].Cutoff)));
        glUniform1f(m_SpotLightsLocation[i].Atten.Constant, pLights[i].Attenuation.Constant);
        glUniform1f(m_SpotLightsLocation[i].Atten.Linear, pLights[i].Attenuation.Linear);
        glUniform1f(m_SpotLightsLocation[i].Atten.Exp, pLights[i].Attenuation.Exp);
    }
}


void AppTechnique::SetBoneTransform(uint Index, const Matrix4f &Transform) {
    assert(Index < MAX_BONES);
    //Transform.Print();
    glUniformMatrix4fv(m_BoneLocation[Index], 1, GL_TRUE, (const GLfloat *) Transform);
}
