//#############################################################################
//  File:      SLMaterial.cpp
//  Author:    Marcus Hudritsch
//  Date:      July 2014
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/Coding-Style-Guidelines
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <stdafx.h> // Must be the 1st include followed by  an empty line

#ifdef SL_MEMLEAKDETECT    // set in SL.h for debug config only
#    include <debug_new.h> // memory leak detector
#endif

#include <SLApplication.h>
#include <SLMaterial.h>
#include <SLSceneView.h>

//-----------------------------------------------------------------------------
SLfloat SLMaterial::PERFECT = 1000.0f;
//-----------------------------------------------------------------------------
SLMaterial* SLMaterial::current = nullptr;
//-----------------------------------------------------------------------------
SLMaterial* SLMaterial::_defaultGray   = nullptr;
SLMaterial* SLMaterial::_diffuseAttrib = nullptr;
//-----------------------------------------------------------------------------
// Default ctor
SLMaterial::SLMaterial(const SLchar* name,
                       SLCol4f       amdi,
                       SLCol4f       spec,
                       SLfloat       shininess,
                       SLfloat       kr,
                       SLfloat       kt,
                       SLfloat       kn,
                       SLGLProgram*  program) : SLObject(name)
{
    _ambient = _diffuse = amdi;
    _specular           = spec;
    _emissive.set(0, 0, 0, 0);
    _shininess = shininess;
    _roughness = 0.5f;
    _metalness = 0.0f;
    _program   = program;

    _kr = kr;
    _kt = kt;
    _kn = kn;

    // sync the transparency coeffitient with the alpha value or vice versa
    if (_kt > 0) _diffuse.w = 1.0f - _kt;
    if (_diffuse.w > 1) _kt = 1.0f - _diffuse.w;

    // Add pointer to the global resource vectors for deallocation
    SLApplication::scene->materials().push_back(this);
}
//-----------------------------------------------------------------------------
// Ctor for textures
SLMaterial::SLMaterial(const SLchar* name,
                       SLGLTexture*  texture1,
                       SLGLTexture*  texture2,
                       SLGLTexture*  texture3,
                       SLGLTexture*  texture4,
                       SLGLProgram*  shaderProg) : SLObject(name)
{
    _ambient.set(1, 1, 1);
    _diffuse.set(1, 1, 1);
    _specular.set(1, 1, 1);
    _emissive.set(0, 0, 0, 0);
    _shininess = 125;
    _roughness = 0.5f;
    _metalness = 0.0f;

    if (texture1) _textures.push_back(texture1);
    if (texture2) _textures.push_back(texture2);
    if (texture3) _textures.push_back(texture3);
    if (texture4) _textures.push_back(texture4);

    _program = shaderProg;

    _kr        = 0.0f;
    _kt        = 0.0f;
    _kn        = 1.0f;
    _diffuse.w = 1.0f - _kt;

    // Add pointer to the global resource vectors for deallocation
    SLApplication::scene->materials().push_back(this);
}
// Ctor for textures
SLMaterial::SLMaterial(const SLchar* name,
                       SLGLProgram*  shaderProg) : SLObject(name)
{

    _program = shaderProg;

    // Add pointer to the global resource vectors for deallocation
    SLApplication::scene->materials().push_back(this);
}
//-----------------------------------------------------------------------------
// Ctor for Cook-Torrance shading
SLMaterial::SLMaterial(const SLchar* name,
                       SLCol4f       diffuse,
                       SLfloat       roughness,
                       SLfloat       metalness)
{
    _ambient.set(0, 0, 0); // not used in Cook-Torrance
    _diffuse = diffuse;
    _specular.set(1, 1, 1);                   // not used in Cook-Torrance
    _emissive.set(0, 0, 0, 0);                // not used in Cook-Torrance
    _shininess = (1.0f - roughness) * 500.0f; // not used in Cook-Torrance
    _roughness = roughness;
    _metalness = metalness;
    _kr        = 0.0f;
    _kt        = 0.0f;
    _kn        = 1.0f;
    _program   = SLApplication::scene->programs(SP_perPixCookTorrance);

    // Add pointer to the global resource vectors for deallocation
    SLApplication::scene->materials().push_back(this);
}
//-----------------------------------------------------------------------------
// Ctor for uniform color material without lighting
SLMaterial::SLMaterial(SLCol4f uniformColor, const SLchar* name)
{
    _ambient.set(0, 0, 0);
    _diffuse = uniformColor;
    _specular.set(0, 0, 0);
    _emissive.set(0, 0, 0, 0);
    _shininess = 125;
    _roughness = 0.5f;
    _metalness = 0.0f;

    _program = SLApplication::scene->programs(SP_colorUniform);

    _kr = 0.0f;
    _kt = 0.0f;
    _kn = 1.0f;

    // Add pointer to the global resource vectors for deallocation
    SLApplication::scene->materials().push_back(this);
}
//-----------------------------------------------------------------------------
/*! 
The destructor doesn't delete attached the textures or shader program because
Such shared resources get deleted in the arrays of SLScene.
*/
SLMaterial::~SLMaterial()
{
}
//-----------------------------------------------------------------------------
/*!
 * Upload material settings to the GPU for a given Shader Program
 * This is useful if one wants to handle shader and material separatly from one another
 */
void SLMaterial::upload(SLuint program){
    glUniform4fv(glGetUniformLocation(program, "u_matAmbient"), 1, (SLfloat*) &this->_ambient);
    glUniform4fv(glGetUniformLocation(program, "u_matDiffuse"), 1, (SLfloat*) &this->_diffuse);
    glUniform4fv(glGetUniformLocation(program, "u_matSpecular"), 1, (SLfloat*) &this->_specular);
    glUniform4fv(glGetUniformLocation(program, "u_matEmissive"), 1, (SLfloat*) &this->_emissive);

    glUniform1f(glGetUniformLocation(program, "u_matShininess"), this->_shininess);
    glUniform1f(glGetUniformLocation(program, "u_matRoughness"), this->_roughness);
    glUniform1f(glGetUniformLocation(program, "u_matMetallic"), this->_metalness);

    glUniform1f(glGetUniformLocation(program, "u_matKr"), this->_kr);
    glUniform1f(glGetUniformLocation(program, "u_matKt"), this->_kt);
    glUniform1f(glGetUniformLocation(program, "u_matKn"), this->_kn);
}


/*!
SLMaterial::activate applies the material parameter to the global render state
and activates the attached shader
*/
void SLMaterial::activate(SLGLState* state, SLDrawBits drawBits)
{
    SLScene* s = SLApplication::scene;

    // Deactivate shader program of the current active material
    if (current && current->program())
        current->program()->endShader();

    // Set this material as the current material
    current = this;

    // If no shader program is attached add the default shader program
    if (!_program)
    {
        if (_textures.size() > 0)
            program(s->programs(SP_perVrtBlinnTex));
        else
            program(s->programs(SP_perVrtBlinn));
    }

    // Check if shader had compile error and the error texture should be shown
    if (_program && _program->name().find("ErrorTex") != string::npos)
    {
        _textures.clear();
        _textures.push_back(new SLGLTexture("CompileError.png"));
    }

    // Set material in the state
    state->matAmbient   = _ambient;
    state->matDiffuse   = _diffuse;
    state->matSpecular  = _specular;
    state->matEmissive  = _emissive;
    state->matShininess = _shininess;
    state->matRoughness = _roughness;
    state->matMetallic  = _metalness;

    // Determine use of shaders & textures
    SLbool useTexture = !drawBits.get(SL_DB_TEXOFF);

    // Enable or disable texturing
    if (useTexture && _textures.size() > 0)
    {
        for (SLuint i = 0; i < _textures.size(); ++i)
            _textures[i]->bindActive((SLint)i);
    }

    // Activate the shader program now
    program()->beginUse(this);
}
//-----------------------------------------------------------------------------
/*! 
Getter for the global default gray material
*/
SLMaterial* SLMaterial::defaultGray()
{
    if (!_defaultGray)
    {
        _defaultGray = new SLMaterial("default", SLVec4f::GRAY, SLVec4f::WHITE);
        _defaultGray->ambient({0.2f, 0.2f, 0.2f});
    }
    return _defaultGray;
}
//-----------------------------------------------------------------------------
/*! 
The destructor doesn't delete attached the textures or shader program because
Such shared resources get deleted in the arrays of SLScene.
*/
void SLMaterial::defaultGray(SLMaterial* mat)
{
    if (mat == _defaultGray)
        return;

    if (_defaultGray)
    {
        SLVMaterial& list = SLApplication::scene->materials();
        list.erase(remove(list.begin(), list.end(), _defaultGray), list.end());
        delete _defaultGray;
    }

    _defaultGray = mat;
}
//-----------------------------------------------------------------------------
/*! 
Getter for the global diffuse per vertex color attribute material
*/
SLMaterial* SLMaterial::diffuseAttrib()
{
    if (!_diffuseAttrib)
    {
        _diffuseAttrib = new SLMaterial("diffuseAttrib");
        _diffuseAttrib->specular(SLCol4f::BLACK);
        _diffuseAttrib->program(SLApplication::scene->programs(SP_perVrtBlinnColorAttrib));
    }
    return _diffuseAttrib;
}
//-----------------------------------------------------------------------------
/*! 
The destructor doesn't delete attached the textures or shader program because
Such shared resources get deleted in the arrays of SLScene.
*/
void SLMaterial::diffuseAttrib(SLMaterial* mat)
{
    if (mat == _diffuseAttrib)
        return;

    if (_diffuseAttrib)
    {
        SLVMaterial& list = SLApplication::scene->materials();
        list.erase(remove(list.begin(), list.end(), _diffuseAttrib), list.end());
        delete _diffuseAttrib;
    }

    _diffuseAttrib = mat;
}
//-----------------------------------------------------------------------------
