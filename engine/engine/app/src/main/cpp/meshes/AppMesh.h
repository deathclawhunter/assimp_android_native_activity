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
 * Universal mesh for rigged and static meshes
 */
class AppMesh : public BaseMesh {
public:
    AppMesh(AppTechnique *render);

    ~AppMesh();

protected:

    bool InitMeshData(vector<VertexBoneData> Bones);
    bool DrawMeshData();

private:

    enum APP_VB_TYPES {
        BONE_VB,
        APP_NUM_VBs
    };

    GLuint m_Buffers[APP_NUM_VBs];
};


#endif	/* _APP_MESH_H_ */

