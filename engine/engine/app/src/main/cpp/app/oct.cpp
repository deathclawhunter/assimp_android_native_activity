#include "oct.h"

Cube::Cube(Vector3i& start, int dim, Matrix4f& transform) {
    // initialize boundary
    m_CubeStartPoint = start;
    m_Dimension = dim;
    m_Transform = transform;
}

Cube::Cube(int x, int y, int z, int dim, Matrix4f& transform) {
    Vector3i start(x, y, z);

    Cube(start, dim, transform);
}

Cube::~Cube() {
    for (int i = 0; i < 8; i++) {
        if (m_Children[i] != NULL) {
            delete(m_Children[i]);
            m_Children[i] = NULL;
        }
    }
}

void Cube::GetMeshBound(AppMesh* mesh, Vector3i* result) {
    Vector4f bound[2];
    Vector4f tmp[8];
    // fetch original bound
    bound[0] = mesh->GetBoundingBox()[0];
    bound[1] = mesh->GetBoundingBox()[1];

    // expand to 8 points
    float dx = bound[1].x - bound[0].x;
    float dy = bound[1].y - bound[0].y;
    float dz = bound[1].z - bound[0].z;

    // top half
    tmp[0] = bound[0];
    tmp[1] = tmp[0];
    tmp[1].x += dx;
    tmp[2] = tmp[1];
    tmp[2].z += dz;
    tmp[3] = tmp[2];
    tmp[3].x -= dx;

    // bottom half
    tmp[4] = tmp[0];
    tmp[4].y += dy;
    tmp[5] = tmp[4];
    tmp[5].x += dx;
    tmp[6] = tmp[5];
    tmp[6].z += dz;
    tmp[7] = tmp[6];
    tmp[7].x -= dx;

    // apply transform and re-calculate the result
    for (int i = 0; i < 8; i++) {
        tmp[i] = m_Transform * tmp[i];
        if (i == 0) {
            result[0].x = (int) tmp[i].x;
            result[0].y = (int) tmp[i].y;
            result[0].z = (int) tmp[i].z;

            result[1].x = (int) tmp[i].x;
            result[1].y = (int) tmp[i].y;
            result[1].z = (int) tmp[i].z;
        } else {
            if ((int) tmp[i].x < result[0].x) {
                result[0].x = (int) tmp[i].x;
            }

            if ((int) tmp[i].y < result[0].y) {
                result[0].y = (int) tmp[i].y;
            }

            if ((int) tmp[i].z < result[0].z) {
                result[0].x = (int) tmp[i].x;
            }

            if ((int) tmp[i].x > result[1].x) {
                result[1].x = (int) tmp[i].x;
            }

            if ((int) tmp[i].y > result[1].y) {
                result[1].y = (int) tmp[i].y;
            }

            if ((int) tmp[i].z > result[1].z) {
                result[1].z = (int) tmp[i].z;
            }
        }
    }
}

/**
 * Assuming mesh is in this node boundary already
 */
void Cube::Insert(AppMesh *mesh) {

    if (m_Dimension <= MIN_DIM) {
        AddMesh(mesh);
        return;
    }

    Vector3i meshBounds[2];
    GetMeshBound(mesh, meshBounds);
    Insert(mesh, meshBounds);
}

void Cube::Insert(AppMesh *mesh, Vector3i *meshBounds) {

    int dim = (m_Dimension >> 1);

    // subdivide

    // top half
    if (Contains(m_CubeStartPoint.x, m_CubeStartPoint.y, m_CubeStartPoint.z, dim, meshBounds)) {
        if (m_Children[0] == NULL) {
            m_Children[0] = new Cube(m_CubeStartPoint, dim, m_Transform);
        }
        m_Children[0]->Insert(mesh, meshBounds);
    } else if (Contains(m_CubeStartPoint.x + dim, m_CubeStartPoint.y, m_CubeStartPoint.z, dim, meshBounds)) {
        if (m_Children[1] == NULL) {
            m_Children[1] = new Cube(m_CubeStartPoint.x + dim, m_CubeStartPoint.y, m_CubeStartPoint.z, dim, m_Transform);
        }
        m_Children[1]->Insert(mesh, meshBounds);
    } else if (Contains(m_CubeStartPoint.x + dim, m_CubeStartPoint.y, m_CubeStartPoint.z + dim, dim, meshBounds)) {
        if (m_Children[2] == NULL) {
            m_Children[2] = new Cube(m_CubeStartPoint.x + dim, m_CubeStartPoint.y, m_CubeStartPoint.z + dim, dim, m_Transform);
        }
        m_Children[2]->Insert(mesh, meshBounds);
    } else if (Contains(m_CubeStartPoint.x, m_CubeStartPoint.y, m_CubeStartPoint.z + dim, dim, meshBounds)) {
        if (m_Children[3] == NULL) {
            m_Children[3] = new Cube(m_CubeStartPoint.x, m_CubeStartPoint.y, m_CubeStartPoint.z + dim, dim, m_Transform);
        }
        m_Children[3]->Insert(mesh, meshBounds);
    }

        // bottom half
    else if (Contains(m_CubeStartPoint.x, m_CubeStartPoint.y + dim, m_CubeStartPoint.z, dim, meshBounds)) {
        if (m_Children[4] == NULL) {
            m_Children[4] = new Cube(m_CubeStartPoint.x, m_CubeStartPoint.y + dim, m_CubeStartPoint.z, dim, m_Transform);
        }
        m_Children[4]->Insert(mesh, meshBounds);
    } else if (Contains(m_CubeStartPoint.x + dim, m_CubeStartPoint.y + dim, m_CubeStartPoint.z, dim, meshBounds)) {
        if (m_Children[5] == NULL) {
            m_Children[5] = new Cube(m_CubeStartPoint.x + dim, m_CubeStartPoint.y + dim, m_CubeStartPoint.z, dim, m_Transform);
        }
        m_Children[5]->Insert(mesh, meshBounds);
    } else if (Contains(m_CubeStartPoint.x + dim, m_CubeStartPoint.y + dim, m_CubeStartPoint.z + dim, dim, meshBounds)) {
        if (m_Children[6] == NULL) {
            m_Children[6] = new Cube(m_CubeStartPoint.x + dim, m_CubeStartPoint.y + dim, m_CubeStartPoint.z + dim, dim, m_Transform);
        }
        m_Children[6]->Insert(mesh, meshBounds);
    } else if (Contains(m_CubeStartPoint.x, m_CubeStartPoint.y + dim, m_CubeStartPoint.z + dim, dim, meshBounds)) {
        if (m_Children[7] == NULL) {
            m_Children[7] = new Cube(m_CubeStartPoint.x, m_CubeStartPoint.y + dim, m_CubeStartPoint.z + dim, dim, m_Transform);
        }
        m_Children[7]->Insert(mesh, meshBounds);
    } else {

        AddMesh(mesh);
    }
}

bool Cube::Contains(int x, int y, int z, int dim, Vector3i* meshBounds) {

    return meshBounds[0].x >= x && meshBounds[0].y >= y && meshBounds[0].z >= z &&
           meshBounds[1].x < x + dim && meshBounds[1].y < y + dim && meshBounds[1].z < z + dim;
}


void Cube::AddMesh(AppMesh* mesh) {
    if (m_MeshCount > m_MeshPointer) {
        m_Meshes[m_MeshPointer++] = mesh;
        return;
    }

    AppMesh **tmp = (AppMesh **) malloc(sizeof(AppMesh*) * (m_MeshCount + ALLOC_STEP));
    if (m_Meshes != NULL) {
        memcpy(tmp, m_Meshes, sizeof(AppMesh*) * m_MeshPointer);
        free(m_Meshes);
    }
    m_Meshes = tmp;
    m_Meshes[m_MeshPointer++] = mesh;
}