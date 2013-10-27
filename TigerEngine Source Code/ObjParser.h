#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <mutex>
#include "WavefrontObject.h"


namespace Tiger {
	class WavefrontObject;

	
	class ObjParser {
		friend class WavefrontObject;
	public:
		ObjParser(const std::string file);	// Constructor

		void loadData();
		std::string getFileName() const { return fileName; }
		WavefrontObject* getObjectByName(std::string objName);
		bool isReady();
		WavefrontObject* getFirstObject();
		float getPercentRead();
	private:
		//Functions
		void setReady(bool _ready);
		void setPercentRead(float percent);
		void _loadData();

		//Other
		typedef std::map<std::string, WavefrontObject*> ObjectMap;
		const std::string fileName;
		ObjectMap objectsMap;
		std::mutex finishedMutex;
		std::mutex percentMutex;
		bool ready;
		float percentRead;
		
	};
}
