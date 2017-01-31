#define LOG_TAG "APP_MESH"

#include "AppLog.h"

#include "AppMesh.h"
#include "GL3Stub.h"


AppMesh::AppMesh(AppTechnique *render) : BaseMesh(render) {
    ZERO_MEM(m_Buffers);
}


AppMesh::~AppMesh() {
    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
    BaseMesh::~BaseMesh();
}

bool AppMesh::InitMeshData(vector<VertexBoneData> Bones) {

    AppTechnique *render = (AppTechnique *) m_Render;

    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(render->m_AttrBoneLocation);
    glVertexAttribPointer(render->m_AttrBoneLocation, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexBoneData), (const GLvoid *) 0);
    glEnableVertexAttribArray(render->m_AttrWeightLocation);
    glVertexAttribPointer(render->m_AttrWeightLocation, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexBoneData), (const GLvoid *) 16);

    return true;
}

bool AppMesh::DrawMeshData() {

    AppTechnique *render = (AppTechnique *) m_Render;

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
    glEnableVertexAttribArray(render->m_AttrBoneLocation);
    glVertexAttribPointer(render->m_AttrBoneLocation, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexBoneData), (const GLvoid *) 0);
    glEnableVertexAttribArray(render->m_AttrWeightLocation);
    glVertexAttribPointer(render->m_AttrWeightLocation, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexBoneData), (const GLvoid *) 16);

    return true;
}

#if ENABLE_IN_SCENE_HUD
bool AppMesh::IsHudMesh() {
    return m_isHudMesh;
}

void AppMesh::SetHudMesh(bool HudMesh) {
    m_isHudMesh = HudMesh;
}
#endif
