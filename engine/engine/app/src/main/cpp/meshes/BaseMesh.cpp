#define LOG_TAG "BaseMesh"

#include "AppLog.h"

#include "BaseMesh.h"
#include "GL3Stub.h"


void BaseMesh::VertexBoneData::AddBoneData(uint BoneID, float Weight) {
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

BaseMesh::BaseMesh(BaseTechnique* render) {
    m_NumBones = 0;
    m_pScene = NULL;
    m_Render = render;
}


BaseMesh::~BaseMesh() {
    Clear();
}


void BaseMesh::Clear() {
    for (uint i = 0; i < m_Textures.size(); i++) {
        SAFE_DELETE(m_Textures[i]);
    }
}


bool BaseMesh::LoadMesh(const string &Filename) {
    // Release the previously loaded mesh (if it exists)
    Clear();

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


bool BaseMesh::InitFromScene(const aiScene *pScene, const string &Filename) {
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    vector<Vector3f> Normals;
    vector<Vector2f> TexCoords;

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
    m_Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    m_Bones.resize(NumVertices);
    if (!hasBone) {
        for (uint i = 0; i < m_Bones.size(); i++) {
            m_Bones[i].Reset();
            m_Bones[i].AddBoneData(0, 1.0f);
        }
    }
    m_Indices.reserve(NumIndices);


    // Initialize the meshes in the scene one by one
    for (uint i = 0; i < m_Entries.size(); i++) {
        const aiMesh *paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh, m_Positions, Normals, TexCoords, m_Bones, m_Indices);

        // This is to fix glDrawElementsBaseVertex() not support issue by
        // pre adding BaseVertex to Indices values
        if (i >= 1) {
            for (int j = m_Entries[i].BaseIndex; j < m_Indices.size(); j++) {
                m_Indices[j] += m_Entries[i].BaseVertex;
            }
        }
    }

    Vector3f minVertex, maxVertex;
    for (uint i = 0; i < m_Positions.size(); i++) {
        Vector3f vertex = m_Positions[i];

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
    m_EndPositions.resize(m_Positions.size());

    if (!InitMaterials(pScene, Filename)) {
        return false;
    }

    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_BaseBuffers), m_BaseBuffers);

    // Generate and populate the buffers with vertex attributes and the indices
    glBindBuffer(GL_ARRAY_BUFFER, m_BaseBuffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0]) * m_Positions.size(), &m_Positions[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(m_Render->m_AttrPositionLocation);
    glVertexAttribPointer(m_Render->m_AttrPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_BaseBuffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(m_Render->m_AttrTexcoordLocation);
    glVertexAttribPointer(m_Render->m_AttrTexcoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_BaseBuffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(m_Render->m_AttrNormalLocation);
    glVertexAttribPointer(m_Render->m_AttrNormalLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (!InitMeshData(m_Bones)) {
        return false;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BaseBuffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0],
                 GL_STATIC_DRAW);

    return true;
}


void BaseMesh::InitMesh(uint MeshIndex,
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

    m_Start = 0.0f;
    m_End = AnimationInSeconds(); // initialize
}


void BaseMesh::LoadBones(uint MeshIndex, const aiMesh *pMesh, vector<VertexBoneData> &Bones) {
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


bool BaseMesh::InitMaterials(const aiScene *pScene, const string &Filename) {
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


void BaseMesh::Simulate(vector<Matrix4f> BoneTransforms, Matrix4f& WVP) {

    for (int i = 0; i < m_Indices.size(); i++) {
        Vector3f pos = m_Positions[m_Indices[i]];
        Matrix4f BoneTransform = BoneTransforms[int(m_Bones[m_Indices[i]].IDs[0])] * m_Bones[m_Indices[i]].Weights[0];
        BoneTransform = BoneTransform + BoneTransforms[int(m_Bones[m_Indices[i]].IDs[1])] * m_Bones[m_Indices[i]].Weights[1];
        BoneTransform = BoneTransform + BoneTransforms[int(m_Bones[m_Indices[i]].IDs[2])] * m_Bones[m_Indices[i]].Weights[2];
        BoneTransform = BoneTransform + BoneTransforms[int(m_Bones[m_Indices[i]].IDs[3])] * m_Bones[m_Indices[i]].Weights[3];
        Vector4f _pos = BoneTransform * Vector4f(pos.x, pos.y, pos.z, 1.0);
        _pos = WVP * _pos;

        m_EndPositions[m_Indices[i]] = Vector3f(_pos.x, _pos.y, _pos.z);
    }
}

vector<Vector3f> BaseMesh::GetEndPositions() {
    return m_EndPositions;
}


void BaseMesh::Render() {

    // LOGI("Entry size = %d\n", m_Entries.size());

    glBindBuffer(GL_ARRAY_BUFFER, m_BaseBuffers[POS_VB]);
    glEnableVertexAttribArray(m_Render->m_AttrPositionLocation);
    glVertexAttribPointer(m_Render->m_AttrPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_BaseBuffers[TEXCOORD_VB]);
    glEnableVertexAttribArray(m_Render->m_AttrTexcoordLocation);
    glVertexAttribPointer(m_Render->m_AttrTexcoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_BaseBuffers[NORMAL_VB]);
    glEnableVertexAttribArray(m_Render->m_AttrNormalLocation);
    glVertexAttribPointer(m_Render->m_AttrNormalLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    DrawMeshData();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BaseBuffers[INDEX_BUFFER]);

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


uint BaseMesh::FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim) {
    for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


uint BaseMesh::FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim) {
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


uint BaseMesh::FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim) {
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


void BaseMesh::CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime,
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
    // FIXME: this factor sometimes is negative
    // assert(Factor >= 0.0f && Factor <= 1.0f);
    if (Factor < 0.0f) {
        LOGW("Position factor becomes negative: %f", Factor);
        LOGW("Animation Time = %f, pNodeAnim->mPositionKeys[PositionIndex].mTime = %f, DeltaTime = %f",
             AnimationTime, pNodeAnim->mPositionKeys[PositionIndex].mTime, DeltaTime);
    }
    Factor = abs(Factor);
    const aiVector3D &Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D &End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void BaseMesh::CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime,
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
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    Factor = abs(Factor);
    const aiQuaternion &StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion &EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}


void BaseMesh::CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime,
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
    // assert(Factor >= 0.0f && Factor <= 1.0f);
    Factor = abs(Factor);
    const aiVector3D &Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D &End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void BaseMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode *pNode,
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

float BaseMesh::AnimationInSeconds() {
    if (m_pScene->mAnimations == NULL) {
        return 0; // no animation
    }

    float TicksPerSecond = (float) (m_pScene->mAnimations[0]->mTicksPerSecond != 0
                                    ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float Duration = (float) m_pScene->mAnimations[0]->mDuration;

    return Duration / TicksPerSecond;
}

void BaseMesh::SetAnimationStartInSeconds(float start) {
    m_Start = start;
}

void BaseMesh::SetAnimationEndInSeconds(float end) {
    m_End = end;
}

void BaseMesh::BoneTransform(float TimeInSeconds, vector<Matrix4f> &Transforms) {
    if (m_pScene->mAnimations == NULL) {
        return; // no animation
    }

    // TimeInSeconds += m_Start;

    Matrix4f Identity;
    Identity.InitIdentity();

    float TicksPerSecond = (float) (m_pScene->mAnimations[0]->mTicksPerSecond != 0
                                    ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    // float Duration = (float) m_pScene->mAnimations[0]->mDuration + m_End * TicksPerSecond;
    // float Duration = (float) m_pScene->mAnimations[0]->mDuration;
    float Duration = (float) m_End * TicksPerSecond;
    // float AnimationTime = fmod(TimeInTicks, Duration);
    float AnimationTime = fmod(TimeInTicks, Duration);

    float start = m_Start * TicksPerSecond;
    if (AnimationTime < start) {
        AnimationTime += start;
    }

    LOGI("TimeInSeconds = %f, TicksPerSecond = %f, TimeInTicks = %f, Duration = %f, AnimationTime = %f",
         TimeInSeconds, TicksPerSecond, TimeInTicks, Duration, AnimationTime);

    ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

    Transforms.resize(m_NumBones);

    for (uint i = 0; i < m_NumBones; i++) {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}


const aiNodeAnim *BaseMesh::FindNodeAnim(const aiAnimation *pAnimation, const string NodeName) {
    for (uint i = 0; i < pAnimation->mNumChannels; i++) {
        const aiNodeAnim *pNodeAnim = pAnimation->mChannels[i];

        if (string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return NULL;
}

Vector4f* BaseMesh::GetBoundingBox() {
    return m_BoundingBox;
}

bool BaseMesh::NeedCalcBoundary() {
    bool Need = false;
    if (m_SimulationCount == SIMULATION_FREQUENCY) {
        Need = true;
    }
    if (m_SimulationCount == SIMULATION_FREQUENCY) {
        m_SimulationCount = 0;
    } else {
        m_SimulationCount++;
    }

    return Need;
}

void BaseMesh::GetBound(vector<Vector3f> ary, Vector3f* ret) {
    ret[0] = ary[0];
    ret[1] = ary[0];

    for (int i = 1; i < ary.size(); i++) {
        if (ary[i].x < ret[0].x) {
            ret[0].x = ary[i].x;
        }

        if (ary[i].y < ret[0].y) {
            ret[0].y = ary[i].y;
        }

        if (ary[i].z < ret[0].z) {
            ret[0].z = ary[i].z;
        }

        if (ary[i].x > ret[1].x) {
            ret[1].x = ary[i].x;
        }

        if (ary[i].y > ret[1].y) {
            ret[1].y = ary[i].y;
        }

        if (ary[i].z > ret[1].z) {
            ret[1].z = ary[i].z;
        }
    }
}