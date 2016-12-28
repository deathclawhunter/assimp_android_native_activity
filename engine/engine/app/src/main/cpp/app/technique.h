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

#ifndef TECHNIQUE_H
#define	TECHNIQUE_H

#include <list>
// #include <GL/glew.h>
#include <GLES/gl.h>

/**
 * Davis: Should not hard code the attribute locations
 * when come to multiple shader case
 */
/*#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4
#define MESH_LOCATION 5 // Add by Davis
#define TEST_VB_LOCATION 6 // Add by Davis
#define TEST_INDEX_LOCATION 7 // Add by Davis*/

class Technique
{
public:

    Technique();

    virtual ~Technique();

    virtual bool Init();

    void Enable();

    /**
     * Fixed atttibute location
     */
    static const int POSITION_LOCATION = 0;
    static const int TEXCOORD_LOCATION = 1;
    static const int NORMAL_LOCATION = 2;
    static const int BONE_LOCATION = 3;
    static const int WEIGHT_LOCATION = 4;

protected:

    bool AddShader(GLenum ShaderType, const char* pFilename);

    bool Finalize();

    GLint GetUniformLocation(const char* pUniformName);
    
    GLint GetProgramParam(GLint param);
    
    GLuint m_ShaderProg;
    
private:

    typedef std::list<GLuint> ShaderObjList;
    ShaderObjList m_ShaderObjList;
};

#endif	/* TECHNIQUE_H */

