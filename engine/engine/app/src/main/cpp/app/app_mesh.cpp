#define LOG_TAG "APP_MESH"

#include "AppLog.h"

#include "app_mesh.h"
#include "gl3stub.h"
#include "technique.h"


void AppMesh::VertexBoneData::AddBoneData(uint BoneID, float Weight) {
    for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
        if (Weights[i] == 0.0) {
            IDs[i] = (GLfloat) BoneID;
            Weights[i] = Weight;
            return;
        }
    }

    // should never get here - more bones than we have space for
    assert(0);
    // return;
}

AppMesh::AppMesh() {
    ZERO_MEM(m_Buffers);
    m_NumBones = 0;
    m_pScene = NULL;
}


AppMesh::~AppMesh() {
    Clear();
}


void AppMesh::Clear() {
    for (uint i = 0; i < m_Textures.size(); i++) {
        SAFE_DELETE(m_Textures[i]);
    }

    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }

    // Commet by Davis
    /* if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    } */
}


bool AppMesh::LoadMesh(const string &Filename) {
    // Release the previously loaded mesh (if it exists)
    Clear();

    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    bool Ret = false;

    m_pScene = m_Importer.ReadFile(Filename.c_str(), ASSIMP_LOAD_FLAGS);

    if (m_pScene) {
        m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
        m_GlobalInverseTransform.Inverse();
        Ret = InitFromScene(m_pScene, Filename);
    }
    else {
        LOGE("ASSIMP: import failed: %s\n", m_Importer.GetErrorString());
        LOGE("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
    }

    return Ret;
}


bool AppMesh::InitFromScene(const aiScene *pScene, const string &Filename) {
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    vector<Vector3f> Positions;
    vector<Vector3f> Normals;
    vector<Vector2f> TexCoords;
    vector<VertexBoneData> Bones;
    vector<uint> Indices;

    uint NumVertices = 0;
    uint NumIndices = 0;

    bool hasBone = false;

    // Count the number of vertices and indices
    for (uint i = 0; i < m_Entries.size(); i++) {
        if (pScene->mMeshes[i]->mBones > 0) {
            hasBone = true;
        }
        m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
        m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex = NumVertices;
        m_Entries[i].BaseIndex = NumIndices;

        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices += m_Entries[i].NumIndices;
    }

    // Reserve space in the vectors for the vertex attributes and indices
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Bones.resize(NumVertices);
    if (!hasBone) {
        for (uint i = 0; i < Bones.size(); i++) {
            Bones[i].Reset();
            Bones[i].AddBoneData(0, 1.0f);
        }
    }
    Indices.reserve(NumIndices);


    // Initialize the meshes in the scene one by one
    for (uint i = 0; i < m_Entries.size(); i++) {
        const aiMesh *paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);

        // This is to fix glDrawElementsBaseVertex() not support issue by
        // pre adding BaseVertex to Indices values
        if (i >= 1) {
            for (int j = m_Entries[i].BaseIndex; j < Indices.size(); j++) {
                Indices[j] += m_Entries[i].BaseVertex;
            }
        }
    }

    Vector3f minVertex, maxVertex;
    for (uint i = 0; i < Positions.size(); i++) {
        Vector3f vertex = Positions[i];

        if (i == 0) {
            minVertex = vertex;
            maxVertex = vertex;
        } else {
            if (vertex.x < minVertex.x) {
                minVertex.x = vertex.x;
            } else if (vertex.x > maxVertex.x) {
                maxVertex.x = vertex.x;
            }

            if (vertex.y < minVertex.y) {
                minVertex.y = vertex.y;
            } else if (vertex.y > maxVertex.y) {
                maxVertex.y = vertex.y;
            }

            if (vertex.z < minVertex.z) {
                minVertex.z = vertex.z;
            } else if (vertex.z > maxVertex.z) {
                maxVertex.z = vertex.z;
            }
        }
    }

    m_BoundingBox[0] = Vector4f(minVertex.x, minVertex.y, minVertex.z, 1.0f);
    m_BoundingBox[1] = Vector4f(maxVertex.x, maxVertex.y, maxVertex.z, 1.0f);

    if (!InitMaterials(pScene, Filename)) {
        return false;
    }

    // Generate and populate the buffers with vertex attributes and the indices
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(Technique::POSITION_LOCATION);
    glVertexAttribPointer(Technique::POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(Technique::TEXCOORD_LOCATION);
    glVertexAttribPointer(Technique::TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(Technique::NORMAL_LOCATION);
    glVertexAttribPointer(Technique::NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(Technique::BONE_LOCATION);
    glVertexAttribPointer(Technique::BONE_LOCATION, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexBoneData), (const GLvoid *) 0);
    glEnableVertexAttribArray(Technique::WEIGHT_LOCATION);
    glVertexAttribPointer(Technique::WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexBoneData), (const GLvoid *) 16);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0],
                 GL_STATIC_DRAW);

    // return GLCheckError();
    return true;
}


void AppMesh::InitMesh(uint MeshIndex,
                           const aiMesh *paiMesh,
                           vector<Vector3f> &Positions,
                           vector<Vector3f> &Normals,
                           vector<Vector2f> &TexCoords,
                           vector<VertexBoneData> &Bones,
                           vector<uint> &Indices) {
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    // Populate the vertex attribute vectors
    for (uint i = 0; i < paiMesh->mNumVertices; i++) {
        const aiVector3D *pPos = &(paiMesh->mVertices[i]);
        const aiVector3D *pNormal = &(paiMesh->mNormals[i]);
        const aiVector3D *pTexCoord = paiMesh->HasTextureCoords(0)
                                      ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        Positions.push_back(Vector3f(pPos->x, pPos->y, pPos->z));
        Normals.push_back(Vector3f(pNormal->x, pNormal->y, pNormal->z));
        TexCoords.push_back(Vector2f(pTexCoord->x, pTexCoord->y));
    }

    if (paiMesh->mBones > 0) {
        LoadBones(MeshIndex, paiMesh, Bones);
    }

    // Populate the index buffer
    for (uint i = 0; i < paiMesh->mNumFaces; i++) {
        const aiFace &Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}


void AppMesh::LoadBones(uint MeshIndex, const aiMesh *pMesh, vector<VertexBoneData> &Bones) {
    for (uint i = 0; i < pMesh->mNumBones; i++) {

        uint BoneIndex = 0;
        // Add by Davis : it seems std::map does not work for STL string on Android, char* will do the work.
        char *BoneName = pMesh->mBones[i]->mName.data;

        if (m_BoneMapping.empty() || m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {

            // Allocate an index for a new bone
            BoneIndex = m_NumBones;
            m_NumBones++;
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
            m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;


            m_BoneMapping[BoneName] = BoneIndex;
        }
        else {
            BoneIndex = m_BoneMapping[BoneName];
        }

        // LOGI("pMesh->mBones[i]->mNumWeights = %d\n", pMesh->mBones[i]->mNumWeights);
        for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
            uint VertexID =
                    m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}


bool AppMesh::InitMaterials(const aiScene *pScene, const string &Filename) {
    // Extract the directory part from the file name
    string::size_type SlashIndex = Filename.find_last_of("/");
    string Dir;

    if (SlashIndex == string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    for (uint i = 0; i < pScene->mNumMaterials; i++) {
        const aiMaterial *pMaterial = pScene->mMaterials[i];

        m_Textures[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL,
                                      NULL) == AI_SUCCESS) {
                // change all path to current relative path
                char *ptr = Path.data;
                if (strchr(Path.data, '/') != 0) {
                    ptr = strrchr(Path.data, '/') + 1;
                } else if (strchr(Path.data, '\\') != 0) {
                    ptr = strrchr(Path.data, '\\') + 1;
                }
                string p;
                p.append(ptr);
                if (p.substr(0, 2) == ".\\") {
                    p = p.substr(2, p.size() - 2);
                }

                string FullPath;
                FullPath.append(p);

                m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath);

                if (!m_Textures[i]->Load()) {
                    LOGE("Error loading texture '%s'\n", FullPath.c_str());
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    Ret = false;
                }
                else {
                    LOGI("%d - loaded texture '%s'\n", i, FullPath.c_str());
                }
            }
        }
    }

    return Ret;
}


void AppMesh::Render() {

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glEnableVertexAttribArray(Technique::POSITION_LOCATION);
    glVertexAttribPointer(Technique::POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glEnableVertexAttribArray(Technique::TEXCOORD_LOCATION);
    glVertexAttribPointer(Technique::TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glEnableVertexAttribArray(Technique::NORMAL_LOCATION);
    glVertexAttribPointer(Technique::NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
    glEnableVertexAttribArray(Technique::BONE_LOCATION);
    glVertexAttribPointer(Technique::BONE_LOCATION, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexBoneData), (const GLvoid *) 0);
    glEnableVertexAttribArray(Technique::WEIGHT_LOCATION);
    glVertexAttribPointer(Technique::WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE,
                          sizeof(VertexBoneData), (const GLvoid *) 16);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);

    // LOGI("Entry size = %d\n", m_Entries.size());

    for (uint i = 0; i < m_Entries.size(); i++) {
        const uint MaterialIndex = m_Entries[i].MaterialIndex;

        assert(MaterialIndex < m_Textures.size());

        if (m_Textures[MaterialIndex]) {
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }

        glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT,
                       (void *) (sizeof(uint) * m_Entries[i].BaseIndex));

    }
}


uint AppMesh::FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim) {
    for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


uint AppMesh::FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim) {
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


uint AppMesh::FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim) {
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


void AppMesh::CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime,
                                           const aiNodeAnim *pNodeAnim) {
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float) (pNodeAnim->mPositionKeys[NextPositionIndex].mTime -
                               pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor =
            (AnimationTime - (float) pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D &Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D &End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void AppMesh::CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime,
                                           const aiNodeAnim *pNodeAnim) {
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float) (pNodeAnim->mRotationKeys[NextRotationIndex].mTime -
                               pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor =
            (AnimationTime - (float) pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion &StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion &EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}


void AppMesh::CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime,
                                          const aiNodeAnim *pNodeAnim) {
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    uint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float) (pNodeAnim->mScalingKeys[NextScalingIndex].mTime -
                               pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor =
            (AnimationTime - (float) pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D &Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D &End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void AppMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode *pNode,
                                    const Matrix4f &ParentTransform) {
    char *NodeName = (char *) pNode->mName.data;

    const aiAnimation *pAnimation = m_pScene->mAnimations[0];

    Matrix4f NodeTransformation(pNode->mTransformation);

    const aiNodeAnim *pNodeAnim = FindNodeAnim(pAnimation, NodeName);

    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        Matrix4f ScalingM;
        ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        Matrix4f TranslationM;
        TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation =
                m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }

    for (uint i = 0; i < pNode->mNumChildren; i++) {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}


void AppMesh::BoneTransform(float TimeInSeconds, vector<Matrix4f> &Transforms) {
    if (m_pScene->mAnimations == NULL) {
        return; // no animation
    }

    Matrix4f Identity;
    Identity.InitIdentity();

    float TicksPerSecond = (float) (m_pScene->mAnimations[0]->mTicksPerSecond != 0
                                    ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, (float) m_pScene->mAnimations[0]->mDuration);

    ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

    Transforms.resize(m_NumBones);

    for (uint i = 0; i < m_NumBones; i++) {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}


const aiNodeAnim *AppMesh::FindNodeAnim(const aiAnimation *pAnimation, const string NodeName) {
    for (uint i = 0; i < pAnimation->mNumChannels; i++) {
        const aiNodeAnim *pNodeAnim = pAnimation->mChannels[i];

        if (string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return NULL;
}

bool AppMesh::IsHudMesh() {
    return m_isHudMesh;
}

void AppMesh::SetHudMesh(bool HudMesh) {
    m_isHudMesh = HudMesh;
}

Vector4f* AppMesh::GetBoundingBox() {
    return m_BoundingBox;
}