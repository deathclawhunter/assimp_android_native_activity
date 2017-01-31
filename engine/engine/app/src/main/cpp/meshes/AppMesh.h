#ifndef _APP_MESH_H_
#define _APP_MESH_H_

#include <map>
#include <vector>

#include "ogldev_util.h"
#include "ogldev_math_3d.h"
#include "ogldev_texture.h"
#include "AppTechnique.h"
#include "BaseMesh.h"

/**
 * TODO: Keep in scenen HUD mesh code for now. Maybe need to remove as a whole later
 */
#define ENABLE_IN_SCENE_HUD 0

/**
 * Universal mesh for rigged and static meshes
 */
class AppMesh : public BaseMesh {
public:
    AppMesh(AppTechnique *render);

    ~AppMesh();

#if ENABLE_IN_SCENE_HUD
    bool IsHudMesh();
    void SetHudMesh(bool HudMesh);
#endif

protected:

    bool InitMeshData(vector<VertexBoneData> Bones);
    bool DrawMeshData();

private:

    enum APP_VB_TYPES {
        BONE_VB,
        APP_NUM_VBs
    };

    GLuint m_Buffers[APP_NUM_VBs];

#if ENABLE_IN_SCENE_HUD
    bool m_isHudMesh = false; // by default is NOT HUD mesh
#endif
};


#endif	/* _APP_MESH_H_ */

