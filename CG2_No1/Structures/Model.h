#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>

#include "VertexData.h"
#include "Material.h"
#include "Mesh.h"

struct ModelData {
	std::vector<VertexData> vertices;
	Material::MaterialData material;
};

ModelData LoadObjFile(const std::string& directorPath, const std::string& fileName);

class Model {
public:



private:

	std::vector<std::unique_ptr<Mesh>> meshArray_;

};