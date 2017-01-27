/*

	Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "skybox.h"
#include "ogldev_pipeline.h"
#include "AppCamera.h"

SkyBox::SkyBox() {
    m_pSkyboxTechnique = NULL;
    m_pCubemapTex = NULL;
}


SkyBox::~SkyBox() {
    SAFE_DELETE(m_pSkyboxTechnique);
    SAFE_DELETE(m_pCubemapTex);
}


bool SkyBox::Init(const string &Directory,
                  const string &PosXFilename,
                  const string &NegXFilename,
                  const string &PosYFilename,
                  const string &NegYFilename,
                  const string &PosZFilename,
                  const string &NegZFilename) {
    m_pSkyboxTechnique = new SkyboxTechnique();

    if (!m_pSkyboxTechnique->Init()) {
        printf("Error initializing the skybox technique\n");
        return false;
    }

    m_pSkyboxTechnique->Enable();
    m_pSkyboxTechnique->SetTextureUnit(0);

    m_pCubemapTex = new CubemapTexture(Directory,
                                       PosXFilename,
                                       NegXFilename,
                                       PosYFilename,
                                       NegYFilename,
                                       PosZFilename,
                                       NegZFilename);

    if (!m_pCubemapTex->Load()) {
        LOGE("fail to load texture for skybox");
        return false;
    }

    m_pMesh = new Mesh();

    if (m_pMesh->LoadMesh("sphere.obj")) {
        my_status = IPlugin::PLUGIN_STATUS_NEXT;
        return true;
    }

    LOGE("fail to load mesh for skybox");

    return false;
}


bool SkyBox::Render() {
    m_pSkyboxTechnique->Enable();

    GLint OldCullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

    Pipeline p;
    p.Scale(20.0f, 20.0f, 20.0f);
    p.Rotate(0.0f, 0.0f, 0.0f);
    p.WorldPos(AppCamera::GetInstance()->GetPos().x, AppCamera::GetInstance()->GetPos().y, AppCamera::GetInstance()->GetPos().z);
    p.SetCamera(AppCamera::GetInstance()->GetPos(), AppCamera::GetInstance()->GetTarget(), AppCamera::GetInstance()->GetUp());
    p.SetPerspectiveProj(AppCamera::GetInstance()->GetPersProjInfo());
    m_pSkyboxTechnique->SetWVP(p.GetWVPTrans());
    m_pCubemapTex->Bind(GL_TEXTURE0);
    m_pMesh->Render();

    glCullFace(OldCullFaceMode);
    glDepthFunc(OldDepthFuncMode);

    return true;
}

bool SkyBox::Init(int32_t width, int32_t height) {

    return Init("",
                "sp3right.png",
                "sp3left.png",
                "sp3top.png",
                "sp3bot.png",
                "sp3front.png",
                "sp3back.png");
}

bool SkyBox::Draw() {
    return Render();
}

int32_t SkyBox::KeyHandler(AInputEvent *event) {
    return 1;
}

IPlugin::PLUGIN_STATUS SkyBox::status() {
    return my_status; // example of never finish a plugin
}