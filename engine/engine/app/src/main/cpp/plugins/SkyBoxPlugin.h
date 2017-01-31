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

#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "ogldev_camera.h"
#include "BasicMesh.h"
#include "InteractivePlugin.h"

class SkyBox : public InteractivePlugin {
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

    IPlugin::PLUGIN_STATUS status();

private:
    BasicTechnique m_Renderer;
    DirectionalLight m_DirectionalLight;
    BasicMesh *m_pMesh;
    const float m_Near = 1.0f;
    const float m_Far = 2000.0f;
    IPlugin::PLUGIN_STATUS m_Status = IPlugin::PLUGIN_STATUS_INIT_RIGHT_NOW;
};

#endif	/* _SKYBOX_H_ */

