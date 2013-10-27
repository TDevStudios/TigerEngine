#include "ObjParser.h"
#include <iostream>
#include "additionalFunctions.h"
#include <thread>
#include <stdexcept>
#include <sys/stat.h>

using namespace std;
using namespace Tiger;

struct Cluster{
	Cluster(): v(0),vt(0),vn(0){}
	unsigned v;
	unsigned vt;
	unsigned vn;
};


ObjParser::ObjParser(const string file)
	: fileName(file),
	  ready(false)
{
}


void ObjParser::loadData()
{

	thread loadingThread(&ObjParser::_loadData, this);
	loadingThread.detach();

}


void ObjParser::_loadData()
{
	//Do some initialisation
	WavefrontObject *object = NULL;
	ifstream in(fileName, ios::in | ios::binary);//open stream
	
	
	//Get stream size
	struct stat infoStruct;
	stat(fileName.c_str(), &infoStruct);
	long fileSize = infoStruct.st_size;
	cout<<"Streamsize " << fileSize;

	//Check wether opening fails
	if(!in.is_open()) {
		fprintf(stderr,"ObjParser: E: Couldn't open file!\n");
		throw exception("ObjParser: Couldn't open file!");
	}

	//Start parsing
	string lineSpecifier;
	bool alreadyNextLineRead = false;
	while( in >> lineSpecifier ) //For each line
	{
				
		if(object != NULL) {
			if(lineSpecifier == "v") {					//model-vertex
				float vertex;
				for(int i=0;i < 3;++i){
					in>>vertex;
					object->vertices_unsorted.push_back(vertex);
				}

			}else if(lineSpecifier == "vt") {			//texture-vertex
				float textureVertex;
				
				in>>textureVertex;
				object->UVs_unsorted.push_back(textureVertex);

				in>>textureVertex;						//Invert v coord of tex coord
				object->UVs_unsorted.push_back(1.0f-textureVertex);
				

			}else if(lineSpecifier == "vn") {			//normal-vertex
				float normalVertex;
				for(int i=0;i < 3;++i){
					in>>normalVertex;
					object->normals_unsorted.push_back(normalVertex);
				}
				
			}else if(lineSpecifier == "f") {			//face in the form: "f 1/2 4//3 9/2/7"
				//Start: face parsing
				string line;
				in.ignore(numeric_limits<streamsize>::max(), ' ');
				getline(in, line);

				vector<string> threeStrings = split(line,' ');   // = {"1//2", "3//4", 4//5};
				
				//Cluster parsing
				for(register unsigned l=0; l < threeStrings.size(); l++) {
					vector<string> clusterBricks = split(threeStrings[l],'/');
					Cluster tmpCluster;							// = {1,0,2};
					
					//Assigning string values
					switch(clusterBricks.size()) {
					case 0:  //nothing: Object is damaged
						object->damaged = true;
						break;
					case 1: //no UVs, no normals
						tmpCluster.v  = strtoul(clusterBricks[0].c_str(), NULL, 10);
						if(tmpCluster.v == 0)
							object->damaged = true;
						object->varSupportsUVs = false;
						object->varSupportsNormals = false;
						break;
					case 2: //no normals
						tmpCluster.v  = strtoul(clusterBricks[0].c_str(), NULL, 10);
						if(tmpCluster.v == 0)
							object->damaged = true;
						tmpCluster.vt = strtoul(clusterBricks[1].c_str(), NULL, 10);
						if(tmpCluster.vt == 0)
							object->varSupportsUVs = false;
						object->varSupportsNormals = false;
						break;
					case 3: //everything great!
						tmpCluster.v  = strtoul(clusterBricks[0].c_str(), NULL, 10);
						if(tmpCluster.v == 0)
							object->damaged = true;
						tmpCluster.vt = strtoul(clusterBricks[1].c_str(), NULL, 10);
						if(tmpCluster.vt == 0)
							object->varSupportsUVs = false;
						tmpCluster.vn = strtoul(clusterBricks[2].c_str(), NULL, 10);
						if(tmpCluster.vn == 0)
							object->varSupportsNormals = false;
						break;


					}

					//Sort vertices:
					
					if(!object->damaged){
						register unsigned int k;
						try{
							for(k=0; k < 3; k++){																				
								object->vertices.push_back(object->vertices_unsorted.at((tmpCluster.v-1)*3+k));			//Read v index if possible
							}
						}catch(const std::out_of_range &e){ 
							object->nFaceErrors++;
							cout<<"Exeption catched: " << e.what() << endl;
							for(;k > 0; k--){
								object->vertices.pop_back();
							}
							
						}
						try{
							if(object->varSupportsUVs) {
								for(k=0; k < 2; k++){
									object->UVs.push_back(object->UVs_unsorted.at((tmpCluster.vt-1)*2+k));					//Read vt index
								}
							}
						}catch(const std::out_of_range &e){ 
							object->nFaceErrors++;
							cout<<"Exeption catched: " << e.what() << endl;
							for(;k > 0; k--) {
								object->UVs.pop_back();
							}
						}
						try{
							if(object->varSupportsNormals)
								for(k=0; k < 3; k++){																			
									object->normals.push_back(object->normals_unsorted.at((tmpCluster.vn-1)*3+k));			//Read vn index if possible
								}
						}catch(const std::out_of_range &e){ 
							object->nFaceErrors++;
							cout<<"Exeption catched: " << e.what() << endl;
							for(;k > 0; k--) {
								object->normals.pop_back();
							}
						}
					}
				}
				
				//End: face parsing
			}else if(lineSpecifier == "usemtl") {		//Specifies the Texture
				string tempMaterial;
				in >> tempMaterial;
				object->activeMaterials.push_back(tempMaterial);
			}else if(lineSpecifier == "o"){				//Declares new object
				object = new WavefrontObject();
				in >> object->objName;
				objectsMap[object->objName] = object;	//Save old object
			}else{
				in.ignore(numeric_limits<streamsize>::max(), '\n');
			}
		}else{
			if(lineSpecifier == "o"){					//Declares new object
				object->finishedReading();
				object = new WavefrontObject();
				in >> object->objName;
				objectsMap[object->objName] = object;	//Save old object
			}
		}

		//set percentage parsed
		setPercentRead((in.tellg()/(float)fileSize)*100);

	}

	//close Stream
	in.close();

	setReady(true);
	
}


WavefrontObject* ObjParser::getObjectByName(string objName) {
	ObjectMap::iterator iter = objectsMap.find(objName);

	if(iter != objectsMap.end()){
		return iter->second;
	}else{
		cout<<"Couldn't find object "<<objName<<"!";
		return NULL;
	}
}

WavefrontObject* ObjParser::getFirstObject() {
	ObjectMap::iterator iter = objectsMap.begin();

	if(iter != objectsMap.end()){
		return iter->second;
	}else{
		cout<<"Couldn't find first object!";
		return NULL;
	}
}


bool ObjParser::isReady() {
	finishedMutex.lock();
	bool tmpReady = ready;
	finishedMutex.unlock();
	return tmpReady;
}

void ObjParser::setReady(bool _ready) {
	finishedMutex.lock();
	ready = _ready;
	finishedMutex.unlock();
}


float ObjParser::getPercentRead() {
	percentMutex.lock();
	float tmpPercent = percentRead;
	percentMutex.unlock();
	return tmpPercent;
}


void ObjParser::setPercentRead(float percent) {
	percentMutex.lock();
	percentRead = percent;
	percentMutex.unlock();
}