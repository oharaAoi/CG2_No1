#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include "VertexData.h"
#include "Material.h"

struct ModelData {
	std::vector<VertexData> vertices;
	Material::MaterialData material;
};

ModelData LoadObjFile(const std::string& directorPath, const std::string& fileName);