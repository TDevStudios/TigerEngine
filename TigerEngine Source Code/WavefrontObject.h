#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <array>
#include "ObjParser.h"
#include <map>

namespace Tiger {

	class WavefrontObject {
		friend class ObjParser;
	public:
		WavefrontObject();		//Constructor

		std::vector<float>& getVertices() { return vertices; }
		std::vector<float>& getNormals() { return normals; }
		std::vector<float>& getUVs() { return UVs; }

		 
		bool isDamaged() const { return damaged; }
		bool supportsNormals() const { return varSupportsNormals; }
		bool supportsUVs() const { return varSupportsUVs; }
		unsigned getFaceErrorsNumber() const { return nFaceErrors; }
	private:
		//Functions
		void finishedReading();
		void loadMaterials();
		//Vectors
		std::vector<float> vertices;
		std::vector<float> UVs;
		std::vector<float> normals;
		std::vector<float> vertices_unsorted;
		std::vector<float> UVs_unsorted;
		std::vector<float> normals_unsorted;
		std::vector<std::array<unsigned int,3>> faces;

		//Materials
		std::map<std::string, unsigned> materialMap;

		//Other
		std::string objName;
		std::vector<std::string> activeMaterials;
		bool varSupportsNormals;
		bool varSupportsUVs;
		bool damaged;

		unsigned nFaceErrors;
	};

}
