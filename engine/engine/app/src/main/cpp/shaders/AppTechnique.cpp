#include <GLES2/gl2.h>
#include "AppTechnique.h"

AppTechnique::AppTechnique() {
}

bool AppTechnique::Init() {
    if (!BasicTechnique::Init("main_shader.vs", "main_shader.fs")) {
        return false;
    }

    m_numPointLightsLocation = GetUniformLocation("gNumPointLights");
    m_numSpotLightsLocation = GetUniformLocation("gNumSpotLights");

    if (m_numPointLightsLocation == INVALID_UNIFORM_LOCATION ||
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

    m_AttrBoneLocation = GetAttributeLocation("BoneIDs");
    m_AttrWeightLocation = GetAttributeLocation("Weights");

    return true;
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
    glUniformMatrix4fv(m_BoneLocation[Index], 1, GL_TRUE, (const GLfloat *) Transform);
}
