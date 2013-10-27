#include "WavefrontObject.h"

using namespace std;
using namespace Tiger;

Tiger::WavefrontObject::WavefrontObject()
	: varSupportsNormals(true),
	  varSupportsUVs(true),
	  damaged(false),
	  nFaceErrors(0)
{
}


void Tiger::WavefrontObject::finishedReading() {

	loadMaterials();

}

void Tiger::WavefrontObject::loadMaterials() {




}