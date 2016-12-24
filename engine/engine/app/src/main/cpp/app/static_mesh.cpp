#define LOG_TAG "STATIC_MESH"

#include "AppLog.h"

#include "static_mesh.h"
#include "gl3stub.h"
#include "technique.h"

StaticMesh::StaticMesh() {
    ZERO_MEM(m_Buffers);
    m_pScene = NULL;
}


StaticMesh::~StaticMesh() {
    Clear();
}


void StaticMesh::Clear() {
    for (uint i = 0; i < m_Textures.size(); i++) {
        SAFE_DELETE(m_Textures[i]);
    }

    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }
}


bool StaticMesh::LoadMesh(const string &Filename) {
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
    } else {
        LOGE("ASSIMP: import failed: %s\n", m_Importer.GetErrorString());
        LOGE("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
    }

    return Ret;
}


bool StaticMesh::InitFromScene(const aiScene *pScene, const string &Filename) {
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    vector<Vector3f> Positions;
    vector<Vector3f> Normals;
    vector<Vector2f> TexCoords;
    vector<uint> Indices;

    uint NumVertices = 0;
    uint NumIndices = 0;

    // Count the number of vertices and indices
    for (uint i = 0; i < m_Entries.size(); i++) {
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
    Indices.reserve(NumIndices);

    // Initialize the meshes in the scene one by one
    for (uint i = 0; i < m_Entries.size(); i++) {
        const aiMesh *paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh, Positions, Normals, TexCoords, Indices);

        // This is to fix glDrawElementsBaseVertex() not support issue by
        // pre adding BaseVertex to Indices values
        if (i >= 1) {
            for (int j = m_Entries[i].BaseIndex; j < Indices.size(); j++) {
                Indices[j] += m_Entries[i].BaseVertex;
            }
        }
    }

    if (!InitMaterials(pScene, Filename)) {
        return false;
    }

    // Generate and populate the buffers with vertex attributes and the indices
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0],
                 GL_STATIC_DRAW);

    return true;
}


void StaticMesh::InitMesh(uint MeshIndex,
                          const aiMesh *paiMesh,
                          vector<Vector3f> &Positions,
                          vector<Vector3f> &Normals,
                          vector<Vector2f> &TexCoords,
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

    // Populate the index buffer
    for (uint i = 0; i < paiMesh->mNumFaces; i++) {
        const aiFace &Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}


bool StaticMesh::InitMaterials(const aiScene *pScene, const string &Filename) {
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

                // c++ string constuctor problem
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


void StaticMesh::Render() {
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(POSITION_LOCATION);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);

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
