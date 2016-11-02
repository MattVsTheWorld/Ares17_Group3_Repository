// rt3dObjLoader.cpp taken from the labs
// A simple OBJ loader
// (c) Daniel Livingstone 2013
// Please don't share - this was hacked together quite quickly and is for demonstration use only
//
// Limitations:
// Only works with triangulated OBJ models
// Does not support groups or multiple meshes per file
// Does not support anything other than very straightforward OBJ models
// Will not generate normals if the model is missing them - or any other missing data
#include "rt3dObjLoader.h"
#include "MeshManager.h"
#include <iostream>
#include <sstream>
#include <map>

#define FORMAT_UNKNOWN 0
#define FORMAT_V 1
#define FORMAT_VT 2
#define FORMAT_VTN 3
#define FORMAT_VN 4

namespace rt3d {

	struct position {
		GLfloat x;
		GLfloat y;
		GLfloat z;
	};

	struct faceIndex {
		int v;
		int t;
		int n;
	};

	int determineFaceFormat(std::string fString) {
		unsigned int delim1 = fString.find('/');
		if (delim1 == std::string::npos)
			return FORMAT_V;		
		unsigned int delim2 = fString.rfind('/');
		if (delim1 == delim2)
			return FORMAT_VT;
		if (delim2 == (delim1+1))
			return FORMAT_VN;
		return FORMAT_VTN;
	}

	faceIndex getFace(std::string fString, int fFormat) {
		// still need to sort out how to handle v/t properly, as well as v//n
		int delim1 = fString.find('/');
		int delim2 = fString.rfind('/');
		
		std::string vstr = fString.substr(0, delim1);
		std::stringstream buffer;
		if ( fFormat == FORMAT_VT ) {
			// v/t format
			std::string tstr = fString.substr(delim1+1, fString.size() );
			buffer << vstr << " " << tstr << " " << 0 << " ";
		}
		else {
			// v/t/n format or v//n format
			std::string nstr = fString.substr(delim2+1, fString.size()-delim2 );
			if ( fFormat == FORMAT_VTN ) {
				std::string tstr = fString.substr(delim1+1, delim2-(delim1+1) );
				buffer << vstr << " " << tstr << " " << nstr << " ";
			}
			else
				buffer << vstr << " " << 0 << " " << nstr << " ";
		}
		
		faceIndex f;
		buffer >> f.v >> f.t >> f.n;
		f.v--; f.t--, f.n--;
		buffer.clear();
		return f;
	}


	void addVertex(std::string fString1, std::map<std::string,GLuint> indexMap, 
		std::vector<position> &inVerts, std::vector<position> &inCoords, std::vector<position> &inNorms, 
		std::vector<GLfloat> &verts, std::vector<GLfloat> &texcoords, std::vector<GLfloat> &norms, 
		std::vector<GLuint> &indices, int fFormat, int &index) {

				auto itr = indexMap.find(fString1);
					if (itr == indexMap.end()) {
						faceIndex f = getFace(fString1, fFormat);
						verts.push_back(inVerts[f.v].x);
						verts.push_back(inVerts[f.v].y);
						verts.push_back(inVerts[f.v].z);
						if (fFormat < FORMAT_VN) {
							texcoords.push_back(inCoords[f.t].x);
							texcoords.push_back(inCoords[f.t].y);
						}
						if (fFormat > FORMAT_VT) {
							norms.push_back(inNorms[f.n].x);
							norms.push_back(inNorms[f.n].y);
							norms.push_back(inNorms[f.n].z);
						}
						indices.push_back(index++);
					}
					else {
						indices.push_back( itr->second );
					}


	}



	void loadObj(const char* filename, std::vector<GLfloat> &verts, std::vector<GLfloat> &norms,
		std::vector<GLfloat> &texcoords, std::vector<GLuint> &indices) {

		GLint fileLength;
		char *fileSource = ShaderManager::loadFile(filename, fileLength);

		if (fileLength == 0)
			// should report error here too
			return;

		std::stringstream  fileStream;
		fileStream << fileSource;

		char line[256];
		std::string lineHeader;
		std::string fString1;
		std::string fString2;
		std::string fString3;
		std::stringstream buffer;
		std::vector<position> inVerts;
		std::vector<position> inNorms;
		std::vector<position> inCoords;
		//std::vector<GLint> indexVector;

		GLfloat x,y,z;
		GLint a,b,c;

		int i=0, iCount = 0;
		position tmp;
		std::map<std::string,GLuint> indexMap;
		int fFormat = FORMAT_UNKNOWN;

		std::cout << "started parsing obj image..." << std::endl;

		while (fileStream.good()) {
			//fileStream.getline(line, 256);
			fileStream >> lineHeader;
			switch (lineHeader[0]) {
			case 'v':
				if ( lineHeader.length() > 1)
					switch (lineHeader[1]) {
					case 't': 
						fileStream >> tmp.x >> tmp.y;
						inCoords.push_back(tmp);
						break;
					case 'n':
						fileStream >> tmp.x >> tmp.y >> tmp.z;
						inNorms.push_back(tmp);
						break;
					default:
						break;
				}
				else {
					fileStream >> tmp.x >> tmp.y >> tmp.z;
					inVerts.push_back(tmp);
				}
				break;
			case 'f':
				fileStream >> fString1 >> fString2 >> fString3;
				if (!fFormat)
					fFormat = determineFaceFormat(fString1);
				if (fFormat > FORMAT_V) {
					addVertex(fString1, indexMap, inVerts, inCoords, inNorms, verts, texcoords, norms, indices, fFormat, iCount);
					addVertex(fString2, indexMap, inVerts, inCoords, inNorms, verts, texcoords, norms, indices, fFormat, iCount);
					addVertex(fString3, indexMap, inVerts, inCoords, inNorms, verts, texcoords, norms, indices, fFormat, iCount);
				}
				else {
					buffer << fString1 << " " << fString2 << " " << fString3;
					buffer >> a >> b >> c;
					buffer.clear();
					indices.push_back(a-1);
					indices.push_back(b-1);
					indices.push_back(c-1);
				}
				break;
			case '#': 
				fileStream.getline(line, 256); // ignore line
				break;
			default:
				fileStream.getline(line, 256); // ignore line
			}

		}

		// copy vertex data to output vectors in case only single index was provided....
		if (fFormat == FORMAT_V) {
			for (int v = 0; v < inVerts.size(); v++) {
				verts.push_back(inVerts[v].x);
				verts.push_back(inVerts[v].y);
				verts.push_back(inVerts[v].z);			
			}
		}

		std::cout << "finished parsing obj image..." << std::endl;


	}


}
