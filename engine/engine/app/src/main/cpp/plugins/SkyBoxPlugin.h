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

#ifndef SKYBOX_H
#define SKYBOX_H

#include "ogldev_camera.h"
#include "ogldev_cubemap_texture.h"
#include "mesh.h"
#include "plugin.h"
#include "BasicMesh.h"

class SkyBox : public IPlugin {
public:
    SkyBox();

    ~SkyBox();

    bool Init(const string &Directory,
              const string &PosXFilename,
              const string &NegXFilename,
              const string &PosYFilename,
              const string &NegYFilename,
              const string &PosZFilename,
              const string &NegZFilename);

    bool Render();

    // Plugin API
    bool Init(int32_t width, int32_t height);

    bool Draw();

    int32_t KeyHandler(AInputEvent *event);

    IPlugin::PLUGIN_STATUS status();

private:
    BasicTechnique m_Renderer;
    DirectionalLight m_DirectionalLight;
    BasicMesh *m_pMesh;
    int m_Width, m_Height;
    IPlugin::PLUGIN_STATUS m_Status = IPlugin::PLUGIN_STATUS_INIT_RIGHT_NOW;
};

#endif	/* SKYBOX_H */

