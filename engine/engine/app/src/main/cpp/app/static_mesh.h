#ifndef STATIC_MESH_H
#define	STATIC_MESH_H

#include <map>
#include <vector>

#include <Importer.hpp>      // C++ importer interface
#include <scene.h>       // Output data structure
#include <postprocess.h> // Post processing flags

#include "ogldev_util.h"
#include "ogldev_math_3d.h"
#include "ogldev_texture.h"

using namespace std;

class StaticMesh
{
public:
    StaticMesh();

    ~StaticMesh();

    bool LoadMesh(const string& Filename);

    void Render();
    
private:

    bool InitFromScene(const aiScene* pScene, const string& Filename);
    void InitMesh(uint MeshIndex,
                  const aiMesh* paiMesh,
                  vector<Vector3f>& Positions,
                  vector<Vector3f>& Normals,
                  vector<Vector2f>& TexCoords,
                  vector<unsigned int>& Indices);
    bool InitMaterials(const aiScene* pScene, const string& Filename);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF
  
enum STATIC_VB_TYPES {
    INDEX_BUFFER,
    POS_VB,
    NORMAL_VB,
    TEXCOORD_VB,
    NUM_VBs            
};

    GLuint m_Buffers[NUM_VBs];

    struct MeshEntry {
        MeshEntry()
        {
            NumIndices    = 0;
            BaseVertex    = 0;
            BaseIndex     = 0;
            MaterialIndex = INVALID_MATERIAL;
        }
        
        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };
    
    vector<MeshEntry> m_Entries;
    vector<Texture*> m_Textures;

    struct cmp_str
    {
        bool operator()(char const *a, char const *b)
        {
            return strcmp(a, b) < 0;
        }
    };

    Matrix4f m_GlobalInverseTransform;
    
    const aiScene* m_pScene;
    Assimp::Importer m_Importer;
};


#endif	/* STATIC_MESH_H */

