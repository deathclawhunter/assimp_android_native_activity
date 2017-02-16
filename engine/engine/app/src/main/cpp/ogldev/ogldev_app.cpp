/*

	Copyright 2014 Etay Meiri

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


#include <app/Config.h>
#include <plugins/TextPlugin.h>
#include <plugins/PluginManager.h>
#include "ogldev_app.h"
#include "ogldev_util.h"

OgldevApp::OgldevApp() {
    m_frameCount = 0;
    m_frameTime = 0;
    m_fps = 0;

    m_frameTime = m_startTime = GetCurrentTimeMillis();
}


void OgldevApp::CalcFPS() {
    m_frameCount++;

    long long time = GetCurrentTimeMillis();

    if (time - m_frameTime >= 1000) {

        m_frameTime = time;
        m_fps = m_frameCount;
        m_frameCount = 0;
    }
}

void OgldevApp::RenderFPS() {

    if (Config::GetInstance(NULL)->GetbCfg(Config::CFG_SHOW_FPS, Config::DEFAULT_CFG_SHOW_FPS)) {
        TextPlugin *Text = (TextPlugin *) PluginManager::GetInstance()->GetPlugin(PluginManager::PLUGIN_TYPE_TEXT);
        if (Text != NULL) {
            char text[32];
            ZERO_MEM(text);
            SNPRINTF(text, sizeof(text), "FPS: %d", m_fps);

            Text->DisplayText(text, 10, 10, 2, 2);
        }
    }
}

float OgldevApp::GetRunningTime() {
    float RunningTime = (float) ((double) GetCurrentTimeMillis() - (double) m_startTime) / 1000.0f;
    return RunningTime;
}

void OgldevApp::ResetRunningTime() {
    m_startTime = GetCurrentTimeMillis();
}
