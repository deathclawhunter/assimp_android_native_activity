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

#include <stdio.h>
#include <GLES2/gl2.h>

#include "ogldev_util.h"
#include "technique.h"

#include <android/log.h>

#define LOG_TAG "Technique"

#include <utils/AppLog.h>
#include <utils/GLError.h>

Technique::Technique() {
    m_ShaderProg = 0;
}


Technique::~Technique() {
    // Delete the intermediate shader objects that have been added to the program
    // The list will only contain something if shaders were compiled but the object itself
    // was destroyed prior to linking.
    for (ShaderObjList::iterator it = m_ShaderObjList.begin(); it != m_ShaderObjList.end(); it++) {
        glDeleteShader(*it);
    }

    if (m_ShaderProg != 0) {
        glDeleteProgram(m_ShaderProg);
        m_ShaderProg = 0;
    }
}


bool Technique::Init() {
    m_ShaderProg = glCreateProgram();

    if (m_ShaderProg == 0) {
        LOGE("Error creating shader program\n");
        GLCheckError();
        return false;
    }

    return true;
}

// Use this method to add shaders to the program. When finished - call finalize()
bool Technique::AddShader(GLenum ShaderType, const char *pFilename) {
    string s;

    if (!ReadFile(pFilename, s)) {
        return false;
    }

    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        return false;
    }

    // Save the shader object - will be deleted in the destructor
    m_ShaderObjList.push_back(ShaderObj);

    const GLchar *p[1];
    p[0] = s.c_str();
    GLint Lengths[1] = {(GLint) s.size()};

    glShaderSource(ShaderObj, 1, p, Lengths);

    // Add by Davis - port to GL es 2.0
    // void glBindAttribLocation(GLuint program​, GLuint index​, const GLchar *name​);
    // TODO: need to modulize the code a little bit here
    /* main_shader.vs
        # layout (location = 0) in vec3 Position;
        attribute vec3 Position
        # layout (location = 1) in vec2 TexCoord;
        attribute vec2 TexCoord
        # layout (location = 2) in vec3 Normal;
        attribute vec3 Normal;
        # layout (location = 3) in ivec4 BoneIDs;
        attribute ivec4 BoneIDs;
        # layout (location = 4) in vec4 Weights;
        attribute vec4 Weights;
     */
    // if (ShaderType == GL_VERTEX_SHADER) {

        /* glBindAttribLocation(m_ShaderProg, POSITION_LOCATION, "Position");
        glBindAttribLocation(m_ShaderProg, TEXCOORD_LOCATION, "TexCoord");
        glBindAttribLocation(m_ShaderProg, NORMAL_LOCATION, "Normal");
        glBindAttribLocation(m_ShaderProg, BONE_LOCATION, "BoneIDs");
        glBindAttribLocation(m_ShaderProg, WEIGHT_LOCATION, "Weights"); */
    // }

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        LOGI("Error compiling '%s': '%s'\n", pFilename, InfoLog);
        return false;
    }

    glAttachShader(m_ShaderProg, ShaderObj);

    return true;
}


// After all the shaders have been added to the program call this function
// to link and validate the program.
bool Technique::Finalize() {
    GLint Success = 0;
    GLchar ErrorLog[1024] = {0};

    glLinkProgram(m_ShaderProg);

    glGetProgramiv(m_ShaderProg, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(m_ShaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        LOGE("Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }

    glValidateProgram(m_ShaderProg);
    glGetProgramiv(m_ShaderProg, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(m_ShaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        LOGE("Invalid shader program: '%s'\n", ErrorLog);
        //   return false;
    }

    // Delete the intermediate shader objects that have been added to the program
    for (ShaderObjList::iterator it = m_ShaderObjList.begin(); it != m_ShaderObjList.end(); it++) {
        glDeleteShader(*it);
    }

    m_ShaderObjList.clear();

    // Add by Davis : ignore checking error
    // return GLCheckError();
    return true;
}


void Technique::Enable() {
    glUseProgram(m_ShaderProg);
}


GLint Technique::GetUniformLocation(const char *pUniformName) {
    GLuint Location = glGetUniformLocation(m_ShaderProg, pUniformName);

    if (Location == INVALID_UNIFORM_LOCATION) {
        LOGE("Warning! Unable to get the location of uniform '%s'\n", pUniformName);
    }

    return Location;
}

GLint Technique::GetProgramParam(GLint param) {
    GLint ret;
    glGetProgramiv(m_ShaderProg, param, &ret);
    return ret;
}
