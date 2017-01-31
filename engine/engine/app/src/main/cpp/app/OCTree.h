#ifndef _OCT_H_
#define _OCT_H_

#include "ogldev_math_3d.h"
#include "AppMesh.h"
#include "AppTechnique.h"

/**
 * Octree
 */
class Vector3i {
public:
    int x;
    int y;
    int z;

    Vector3i() { }

    Vector3i(Vector3i &other) {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
    }

    Vector3i(int x, int y, int z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

class Cube {
public:
    void Init(Vector3i &start, int dim, Matrix4f &transform);

    Cube(Vector3i &start, int dim, Matrix4f &transform);

    Cube(int x, int y, int z, int dim, Matrix4f &transform);

    ~Cube();

    void Insert(AppMesh *mesh);

    void Insert(AppMesh *mesh, Vector3i *meshBounds);

    void AddMesh(AppMesh *mesh);

    Cube *m_Children[8] = {0};
    Vector3i m_CubeStartPoint;

    /**
     * Indicate when to release this node if it is empty
     */
    int m_CacheCount = 0;
    int m_MaxCacheCount = 3;

    AppMesh **m_Meshes = NULL;
    int m_MeshCount = 0;
    int m_MeshPointer = 0;

    Cube *m_Parent;

private:
    // minimal dimension of the cubes
    const int MIN_DIM = 2;
    const int ALLOC_STEP = 5;

    bool Contains(int x, int y, int z, int dim, Vector3i *meshBounds);

    void GetMeshBound(AppMesh *mesh, Vector3i *meshBounds);

    int m_Dimension;
    Matrix4f m_Transform; // transformation for current frame
};

class Octree {
public:
    ~Octree();

    Octree(int dim);

    void SetTransform(Matrix4f &transform);

    void AddMesh(AppMesh *mesh);

    void Purge();

private:
    Cube *m_Tree;
    int m_Dimension;
    Matrix4f m_Transform;
};


#endif	/* _OCT_H_ */

