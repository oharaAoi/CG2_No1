#include "Model.h"

ModelData LoadObjFile(const std::string& directorPath, const std::string& fileName){
	ModelData modelData; // 構築するmodelData
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals;// 法線
	std::vector<Vector2> texcoords;// テクスチャ座標
	std::string line;// ファイルから読み込んだ1行を格納する物

	// ファイルを開く
	std::ifstream file(directorPath + "/" + fileName);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier; 
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// 頂点情報を読む
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f;
			//position.y *= -1.0f;
			positions.push_back(position);

		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);

		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			//normal.y *= -1.0f;
			normals.push_back(normal);

		} else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形固定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition; 
				s >> vertexDefinition;
				// 頂点の要素へのindexが[位置/UV/法線]で格納されているので分解ぢてindexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];

				for (uint32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}

				// 要素へのindexから、実際の要素を取得して頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				VertexData vertex = { position, texcoord, normal };
				modelData.vertices.push_back(vertex);

				triangle[faceVertex] = { position, texcoord, normal };
			}

			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);

		} else if (identifier == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;

			modelData.material = Material::LoadMaterialTemplateFile(directorPath, materialFilename);
		}
	}

	return modelData;
}
