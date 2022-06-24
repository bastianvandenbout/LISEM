#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include "QString"
#include "map.h"

typedef struct ShaderData
{

    std::vector<QString> Shaders;
    std::vector<std::vector<QString>> m_TextureFiles;
    std::vector<std::vector<cTMap *>> m_TextureMaps;


    //save to file

    //load from file




} ShaderData;



#endif // SHADER_H
