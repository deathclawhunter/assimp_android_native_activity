/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2016, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
copyright notice, this list of conditions and the
following disclaimer.

* Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the
following disclaimer in the documentation and/or other
materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
contributors may be used to endorse or promote products
derived from this software without specific prior
written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/
#include "UnitTestPCH.h"
#include "TestIOSystem.h"

#include <assimp/IOSystem.hpp>

using namespace std;
using namespace Assimp;

class IOSystemTest : public ::testing::Test {
public:
    virtual void SetUp() { 
        pImp = new TestIOSystem(); 
    }
    
    virtual void TearDown() { 
        delete pImp; 
    }

protected:
    TestIOSystem* pImp;
};

/*
virtual bool PushDirectory( const std::string &path );
virtual const std::string &CurrentDirectory() const;
virtual bool PopDirectory();
*/

TEST_F( IOSystemTest, accessDirectoryStackTest ) {
    EXPECT_FALSE( pImp->PopDirectory() );
    EXPECT_EQ( 0, pImp->StackSize() );
    EXPECT_FALSE( pImp->PushDirectory( "" ) );
    std::string path = "test/";
    EXPECT_TRUE( pImp->PushDirectory( path ) );
    EXPECT_EQ( 1, pImp->StackSize() );
    EXPECT_EQ( path, pImp->CurrentDirectory() );
    EXPECT_TRUE( pImp->PopDirectory() );
    EXPECT_EQ( 0, pImp->StackSize() );
}
