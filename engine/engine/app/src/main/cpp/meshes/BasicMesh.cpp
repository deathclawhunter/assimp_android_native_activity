#define LOG_TAG "BasicMesh"

// #include "AppLog.h"

#include "BasicMesh.h"

BasicMesh::BasicMesh(BasicTechnique *render) : BaseMesh(render) {
}

BasicMesh::~BasicMesh() {
}

bool BasicMesh::InitMeshData(vector<VertexBoneData> Bones) {

    // Do not expect any Bone data in basic mesh.

    return true;
}

bool BasicMesh::DrawMeshData() {

    // Do not have any customized mesh data to draw

    return true;
}

