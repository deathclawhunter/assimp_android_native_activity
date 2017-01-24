#ifndef APP_MESH_H
#define    APP_MESH_H

#include <map>
#include <vector>

#include <Importer.hpp>      // C++ importer interface
#include <scene.h>       // Output data structure
#include <postprocess.h> // Post processing flags

#include "ogldev_util.h"
#include "ogldev_math_3d.h"
#include "ogldev_texture.h"
#include "technique.h"
#include "app_technique.h"

using namespace std;

#define DEBUG_POSITION 0

/**
 * TODO: Keep in scenen HUD mesh code for now. Maybe need to remove as a whole later
 */
#define ENABLE_IN_SCENE_HUD 0

/**
 * Universal mesh for rigged and static meshes
 */
class AppMesh {
public:
    AppMesh(AppTechnique* render);

    ~AppMesh();

    bool LoadMesh(const string &Filename);

    void Render();

    uint NumBones() const {
        return m_NumBones;
    }

    void BoneTransform(float TimeInSeconds, vector<Matrix4f> &Transforms);

#if ENABLE_IN_SCENE_HUD
    bool IsHudMesh();
    void SetHudMesh(bool HudMesh);
#endif

    Vector4f* GetBoundingBox();

    /**
     * Used for optimize calculation of boundaries, allowing define different
     * frequency based on mesh types
     */
    bool NeedCalcBoundary();

private:
    #define NUM_BONES_PER_VEREX 4

    AppTechnique* m_Render = NULL;

    const int SIMULATION_FREQUENCY = 100; // Calculate the boundary of animated object every 100 frames
    int m_SimulationCount = 0;

    struct BoneInfo {
        Matrix4f BoneOffset;
        Matrix4f FinalTransformation;

        BoneInfo() {
            BoneOffset.SetZero();
            FinalTransformation.SetZero();
        }
    };

    struct VertexBoneData {
        float IDs[NUM_BONES_PER_VEREX];
        float Weights[NUM_BONES_PER_VEREX];

        VertexBoneData() {
            Reset();
        };

        void Reset() {
            ZERO_MEM(IDs);
            ZERO_MEM(Weights);
        }

        void AddBoneData(uint BoneID, float Weight);
    };

    void CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);

    void CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime,
                                  const aiNodeAnim *pNodeAnim);

    void CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime,
                                  const aiNodeAnim *pNodeAnim);

    uint FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim);

    uint FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim);

    uint FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim);

    const aiNodeAnim *FindNodeAnim(const aiAnimation *pAnimation, const string NodeName);

    void ReadNodeHeirarchy(float AnimationTime, const aiNode *pNode,
                           const Matrix4f &ParentTransform);

    bool InitFromScene(const aiScene *pScene, const string &Filename);

    void InitMesh(uint MeshIndex,
                  const aiMesh *paiMesh,
                  vector<Vector3f> &Positions,
                  vector<Vector3f> &Normals,
                  vector<Vector2f> &TexCoords,
                  vector<VertexBoneData> &Bones,
                  vector<unsigned int> &Indices);

    void LoadBones(uint MeshIndex, const aiMesh *paiMesh, vector<VertexBoneData> &Bones);

    bool InitMaterials(const aiScene *pScene, const string &Filename);

    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

    enum VB_TYPES {
        INDEX_BUFFER,
        POS_VB,
        NORMAL_VB,
        TEXCOORD_VB,
        BONE_VB,
        DUMMP_VB,
        TEST_VB,
        TEST_INDEX,
        NUM_VBs
    };

    GLuint m_Buffers[NUM_VBs];

    struct MeshEntry {
        MeshEntry() {
            NumIndices = 0;
            BaseVertex = 0;
            BaseIndex = 0;
            MaterialIndex = INVALID_MATERIAL;
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };

    vector<MeshEntry> m_Entries;
    vector<Texture *> m_Textures;

    struct cmp_str {
        bool operator()(char const *a, char const *b) {
            return strcmp(a, b) < 0;
        }
    };

    // Add by Davis : it seems std::map does not work for STL string on Android, char* will do the work.
    map<char *, uint, cmp_str> m_BoneMapping; // maps a bone name to its index
    uint m_NumBones;
    vector<BoneInfo> m_BoneInfo;
    Matrix4f m_GlobalInverseTransform;

    const aiScene *m_pScene;
    Assimp::Importer m_Importer;

#if ENABLE_IN_SCENE_HUD
    bool m_isHudMesh = false; // by default is NOT HUD mesh
#endif
    // simple box bound
    Vector4f m_BoundingBox[2];

#if DEBUG_POSITION
    vector<Vector3f> Positions;
    vector<VertexBoneData> Bones;
    vector<uint> Indices;

    vector<Vector3f> EndPositions;
public:
    void Simulate(vector<Matrix4f> BoneTransforms, Matrix4f& WVP);
    vector<Vector3f> GetEndPositions();
#endif
};


#endif	/* APP_MESH_H */

