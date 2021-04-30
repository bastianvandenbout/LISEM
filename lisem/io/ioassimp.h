#pragma once

#include "defines.h"
#include "models/3dmodel.h"

class ModelGeometry;

LISEM_API ModelGeometry * Read3DModel(QString path);
LISEM_API void Write3DModel(ModelGeometry *,QString path,QString format = "obj");
