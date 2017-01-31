#ifndef _BASIC_MESH_H_
#define _BASIC_MESH_H_

#include <map>
#include <vector>

#include "ogldev_util.h"
#include "ogldev_math_3d.h"
#include "ogldev_texture.h"
#include "AppTechnique.h"
#include "BaseMesh.h"

/**
 * Basic mesh only implements with basic shader class, BasicTechnique
 */
class BasicMesh : public BaseMesh {
public:
    BasicMesh(BasicTechnique *render);

    ~BasicMesh();

protected:

    bool InitMeshData(vector<VertexBoneData> Bones);
    bool DrawMeshData();

};


#endif	/* _BASIC_MESH_H_ */

