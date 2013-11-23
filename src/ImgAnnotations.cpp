#include "ImgAnnotations.hpp"

// C++ STD Lib
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <vector>

// Boost Lib
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

// using namespaces
using namespace IA;
using std::string;
//using __gnu_cxx::hash_map;
using std::map;
using std::list;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using boost::lexical_cast;
using boost::bad_lexical_cast;
using boost::algorithm::starts_with;
using boost::algorithm::trim;
using boost::algorithm::split;
using boost::algorithm::to_lower;
using boost::algorithm::to_lower_copy;
using boost::algorithm::is_any_of;
using boost::algorithm::contains;



// ========== ImgAnnotations ==========

IA::ImgAnnotations::ImgAnnotations()
{
	_highestObjID = 0;
}

IA::ImgAnnotations::~ImgAnnotations()
{
	// delete all allocated Dir references .. the rest is done automatically
	typedef std::pair<string, Dir*> thePair;
	BOOST_FOREACH(thePair entry, _dirs)
		delete entry.second;
}

void IA::ImgAnnotations::loadFromFile(const string &filePath)
{
	// try to open the file
 	std::ifstream in(filePath.c_str());
 	if (!in) {
		cerr << "IA::ImgAnnotations::loadFromFile()" << endl;
		cerr << "\tError opening the file: " << filePath << endl;
		return;
 	}
 	
 	// save annotations file path
 	_annotationsFilePath = filePath;

	// clear the current content
	clear();

	// loop over all lines
	string currentFilePath;
	string line;
	ID currentObjID = -1;
	int lineCounter = 0;
	bool error = false;
	Object* currentObj = 0;
	File* currentFile = 0;
	while (!std::getline(in, line).eof()) {
		// read next line
		trim(line);
		lineCounter++;

		// ignore empty lines and comment lines
		if (line.empty() || starts_with(line, "#") || !contains(line, ":"))
			continue;

		// split the line in its key and value (up to ':' and the value)
		string::size_type found = line.find(':');
		string key(line, 0, found);
		string value;
		if (found != string::npos) value = string(line, ++found, string::npos);
		trim(key);
		to_lower(key);
		trim(value);

		// decide what to do based on the key value
		if ("file" == key) {
			// add a new empty file .. if it doesn't exist already
			if (!existsFile(value))
				addFile(value);

			// update currentFile and currentObj (to zero, since we added a new file)
			currentFilePath = value;
			currentObjID = -1;
			currentObj = 0;
			currentFile = getFile(currentFilePath);
		}
		else if ("object" == key) {
			// we have a new object given
			if (!currentFile)
				continue;

			// the object is given with its object id, parse its id and check for validity
			ID objID;
			try {
				objID = lexical_cast<ID>(value);
			}
			catch(bad_lexical_cast &e) {
				error = true;
				objID = -1;
			}
			if (objID == 0 || objID < -1 || existsObject(objID)) {
				error = true;
				objID = -1;
			}

			// add a new empty object
			currentObj = newObject(currentFilePath, objID);
			assert(NULL != currentObj);
			assert(existsObject(currentObj->getID()));
			currentObjID = currentObj->getID();
		}
		else {
			// if we have not seen an object tag so far, these are properties
			// for the image file
			if (!currentObj && currentFile) {
				currentFile->set(key, value);
			}
			// otherwise these are properties for the object
			else if (currentObj)
				currentObj->set(key, value);
		}
	}

	if (error) {
		cerr << "IA::ImgAnnotations::loadFromFile()" << endl;
		cerr << "\tOne or more errors occured due to invalid object ids, new ids have been choosen." << endl;
		cerr << "\tWARNING: If you use references to IDs this is going to result in a mess!" << endl;
		cerr << "\t         I assume you know what you do :) !" << endl;
	}
}

void IA::ImgAnnotations::saveToFile(const string &filePath)
{
	// try to open the file
 	std::ofstream out(filePath.c_str(), std::ios::trunc);
 	if (!out) {
		cerr << "IA::ImgAnnotations::saveToFile()" << endl;
		cerr << "\tError opening the file: " << filePath << endl;
		return;
	}
 	
 	// save annotations file path
 	_annotationsFilePath = filePath;

	// loop over all elements in our datastructur and write their data to the file
	BOOST_FOREACH (File *iFile, getFiles()) {
		// output the fileName
		assert(NULL != iFile);
		out << "########## NEW FILE ##########\n";
		out << "file: " << iFile->getFilePath() << "\n";
		
		// output file properties
		BOOST_FOREACH (const string& iProperty, iFile->getProperties())
			out << iProperty << ": " << iFile->get(iProperty) << "\n";
		out << "\n";

		// loop over the properties
		BOOST_FOREACH (Object *iObj, iFile->getObjects()) {
			// output the object tag with its id
			assert(NULL != iObj);
			out << "object: " << iObj->getID() << "\n";

			// output the properties
			BOOST_FOREACH (const string& iProperty, iObj->getProperties())
				out << iProperty << ": " << iObj->get(iProperty) << "\n";

			// a additional empty line at the end of an object
			out << "\n";
		}
	}
}

StrList IA::ImgAnnotations::getAllProperties(const IA::PropList& propList) const
{
	map<string, bool> propertiesHash;
	StrList outList;

	// go through all properties of all objects
	BOOST_FOREACH (const PropertyMap* propMap, propList) {
		assert(NULL != propMap);
		BOOST_FOREACH (const string& property, propMap->getProperties())
			if (!propertiesHash[property]) {
				// the first occurrence of the property, i.e. add it to the list
				propertiesHash[property] = true;
				outList.push_back(property);
			}
	}

	return outList;
}

StrList IA::ImgAnnotations::getAllPropertyValues(const IA::PropList& propList, const string& property) const
{
	map<string, bool> valuesHash;
	StrList outList;

	// go through all objects	
	BOOST_FOREACH (const PropertyMap* propMap, propList) {
		assert(NULL != propMap);
		if (propMap->isSet(property) && !propMap->get(property).empty()) {
			valuesHash[propMap->get(property)] = true;
			outList.push_back(property);
		}
	}

	return outList;
}

StrList IA::ImgAnnotations::getAllObjProperties() const
{
	ObjList objList = getObjects();
	PropList propList;
	std::copy(objList.begin(), objList.end(), std::inserter(propList, propList.end()));
	return getAllProperties(propList);
}

StrList IA::ImgAnnotations::getAllObjPropertyValues(const string& property) const
{
	PropList propList;
	ObjList objList = getObjects(property);
	std::copy(objList.begin(), objList.end(), std::inserter(propList, propList.end()));
	return getAllPropertyValues(propList, property);
}

StrList IA::ImgAnnotations::getAllFileProperties() const
{
	PropList propList;
	FileList fileList = getFiles();
	std::copy(fileList.begin(), fileList.end(), std::inserter(propList, propList.end()));
	return getAllProperties(propList);
}

StrList IA::ImgAnnotations::getAllFilePropertyValues(const string& property) const
{
	PropList propList;
	FileList fileList = getFiles(property);
	std::copy(fileList.begin(), fileList.end(), std::inserter(propList, propList.end()));
	return getAllPropertyValues(propList, property);
}

size_type IA::ImgAnnotations::numOfDirs() const
{
	return _dirs.size();
}

size_type IA::ImgAnnotations::numOfFiles(const string &dirPath) const
{
	size_type num = 0;

	// if the dir is not given, return number of all existing files
	if (dirPath.empty()) {
		BOOST_FOREACH (const Dir *iDir, getDirs())
			num += iDir->numOfFiles();
	}
	// otherwise return number of all files in the specified directory
	else if (existsDir(dirPath)) {
		const Dir *dir = getDir(dirPath);
		assert(NULL != dir);
		num = dir->numOfFiles();
	}

	return num;
}

size_type IA::ImgAnnotations::numOfObjects(const string &dirPath, const string &fileName) const
{
	size_type num = 0;

	// if the dir is not given, return number of all existing objects
	if (dirPath.empty()) {
		BOOST_FOREACH (const Dir *iDir, getDirs())
			BOOST_FOREACH (const File *iFile, iDir->getFiles())
				num += iFile->numOfObjects();
	}
	// if dir is given, but not file, return number of all objects in the specified directory
	else if (existsDir(dirPath) && fileName.empty()) {
		const Dir *dir = getDir(dirPath);
		assert(NULL != dir);
		BOOST_FOREACH (const File *iFile, dir->getFiles())
			num += iFile->numOfObjects();
	}
	// if dir and file are given, return number of all objects assigned to the specified file
	else if (existsFile(filePath(dirPath, fileName))) {
		const File *file = getFile(filePath(dirPath, fileName));
		assert(NULL != file);
		num = file->numOfObjects();
	}

	return num;
}

bool IA::ImgAnnotations::existsDir(const std::string &dirPath) const
{
	return NULL != getDir(dirPath);
}

bool IA::ImgAnnotations::existsFile(const std::string &filePath) const
{
	return NULL != getFile(filePath);
}

bool IA::ImgAnnotations::existsObject(ID objID) const
{
	return NULL != getObject(objID);
}

StrList IA::ImgAnnotations::getDirPaths() const
{
	StrList outList;
//	typedef std::pair<string, Dir*> thePair;
//	BOOST_FOREACH(const thePair entry, _dirs)
//		outList.push_back(entry.first);
	BOOST_FOREACH(Dir *dir, getDirs())
		outList.push_back(dir->getDirPath());
	return outList;
}

StrList IA::ImgAnnotations::getFilePaths(const string &dirPath) const
{
	StrList outList;

//	// if the dir is not given, return all existing files
//	if (dirPath.empty()) {
//		BOOST_FOREACH (const Dir *iDir, getDirs())
//			BOOST_FOREACH (const File *iFile, iDir->getFiles())
//				outList.push_back(iFile->getFilePath());
//	}
//	// otherwise return all files in the specified directory
//	else if (existsDir(dirPath)) {
//		const Dir *dir = getDir(dirPath);
//		assert(NULL != dir);
//		BOOST_FOREACH (const File *iFile, dir->getFiles())
//			outList.push_back(iFile->getFilePath());
//	}
	BOOST_FOREACH(File *file, getFiles(dirPath))
		outList.push_back(file->getFilePath());

	return outList;
}

IDList IA::ImgAnnotations::getObjectIDs(const string &dirPath, const string &fileName) const
{
	IDList outList;

	BOOST_FOREACH(Object *obj, getObjects(dirPath, fileName))
		outList.push_back(obj->getID());

	return outList;
}

DirList IA::ImgAnnotations::getDirs() const
{
	DirList dirList;
	typedef std::pair<string, Dir*> thePair;
	BOOST_FOREACH (const thePair entry, _dirs)
		dirList.push_back(entry.second);
	return dirList;
}

FileList IA::ImgAnnotations::getFiles(const string &dirPath) const
{
	FileList fileList;

	// if the dir is not given, return all existing files
	if (dirPath.empty()) {
		BOOST_FOREACH (Dir *iDir, getDirs()) {
			FileList tmpList = iDir->getFiles();
			fileList.splice(fileList.end(), tmpList);
		}
	}
	// otherwise return all files in the specified directory
	else if (existsDir(dirPath)) {
		Dir *dir = getDir(dirPath);
		assert(NULL != dir);
		return dir->getFiles();
	}

	return fileList;
}

ObjList IA::ImgAnnotations::getObjects(const string &dirPath, const string &fileName) const
{
	ObjList objList;

	// if the dir is not given, return number of all existing objects
	if (dirPath.empty()) {
		BOOST_FOREACH (Dir *iDir, getDirs())
			BOOST_FOREACH (File *iFile, iDir->getFiles()) {
				ObjList tmpList = iFile->getObjects();
				objList.splice(objList.end(), tmpList);
			}
	}
	// if dir is given, but not file, return number of all objects in the specified directory
	else if (existsDir(dirPath) && fileName.empty()) {
		Dir *dir = getDir(dirPath);
		assert(NULL != dir);
		BOOST_FOREACH (File *iFile, dir->getFiles()) {
			ObjList tmpList = iFile->getObjects();
			objList.splice(objList.end(), tmpList);
		}
	}
	// if dir and file are given, return number of all objects assigned to the specified file
	else if (existsFile(filePath(dirPath, fileName))) {
		File *file = getFile(filePath(dirPath, fileName));
		assert(NULL != file);
		return file->getObjects();
	}

	return objList;
}

IA::Object *IA::ImgAnnotations::getObject(ID objID) const
{
	Object *obj = NULL;

	// get the pointer to the corresponding file
	IDStrMap::const_iterator iFilePath = _objFilePaths.find(objID);
	if (iFilePath != _objFilePaths.end()) {
		File *file = getFile(iFilePath->second);
		assert(NULL != file); // should exist since it is in our cache

		// get the pointer to the corresponding object
		obj = file->getObject(objID);
		assert(NULL != obj); // should exist since it is in our cache
	}

	return obj;
}

IA::File *IA::ImgAnnotations::getFile(const string &filePath) const
{
	File *file = NULL;

	// get the file name and the dir path
	string dirPath = this->dirPath(filePath);
	string fileName = this->fileName(filePath);

	// get the pointer to the corresponding dir
	Dir *dir = getDir(dirPath);

	// get the pointer to the corresponding file
	if (NULL != dir)
		file = dir->getFile(fileName);

	return file;
}

IA::Dir *IA::ImgAnnotations::getDir(const string &dirPath) const
{
	Dir *dir = NULL;
	StrDirMap::const_iterator iDirPath = _dirs.find(dirPath);
	if (iDirPath != _dirs.end())
		dir = iDirPath->second;
	return dir;
}

IA::Object *IA::ImgAnnotations::newObject(const string &filePath, ID objID)
{
	assert(objID < 0 || !existsObject(objID));

	Object *newObj = NULL;

	// create a new object only if its file exists in our database
	// and only if its ID is valid (i.e. not used or automatically chosen)
	if (!existsFile(filePath))
		addFile(filePath);
	File *file = getFile(filePath);
	assert(NULL != file);

	// get the object ID
	if (objID < 0)
		objID = ++_highestObjID;
	else if (objID > _highestObjID)
		_highestObjID = objID;

	// ensure that the object ID does not exist in the file
	assert(NULL == file->getObject(objID));

	// add the new object in the system
	newObj = new Object(filePath, objID);
	file->_objects[objID] = newObj;
	_objFilePaths[objID] = filePath;

	return newObj;
}

void IA::ImgAnnotations::addFile(const string& filePath)
{
	// only add the file if the file does not already exist
	if (!existsFile(filePath)) {
		string dirPath = this->dirPath(filePath);
		string fileName = this->fileName(filePath);

		// add a new directory if it does not already exist
		// otherwise get its reference
		Dir *dir = NULL;
		if (!existsDir(dirPath)) {
			dir = new Dir(dirPath);
			_dirs[dirPath] = dir;
		}
		else
			dir = getDir(dirPath);

		// add the file
		dir->_files[fileName] = new File(filePath);
	}
}

void IA::ImgAnnotations::addFiles(const StrList& filePaths)
{
	// add all given files to the database
	BOOST_FOREACH (string filePath, filePaths)
		addFile(filePath);
}

void IA::ImgAnnotations::removeFile(const string &filePath)
{
	// only remove the file if the file does exist
	if (existsFile(filePath)) {
		string fileName = this->fileName(filePath);
		string dirPath = this->dirPath(filePath);

		// get the file and its directory
		File *file = getFile(filePath);
		assert(NULL != file);
		Dir *dir = getDir(dirPath);
		assert(NULL != dir);

		// get all object ids and remove them from our cache
		BOOST_FOREACH (ID id, file->getObjectIDs()) {
			IDStrMap::size_type nRemoves = _objFilePaths.erase(id);
			assert(nRemoves == 1);
		}

		// delete the file entry from its directory
		StrFileMap::size_type nRemoves = dir->_files.erase(fileName);
		assert(nRemoves == 1);
		delete file;

		// delete the directory in case it is now empty
		if (dir->numOfFiles() <= 0) {
			assert(NULL != getDir(dirPath));
			StrDirMap::size_type nRemoves = _dirs.erase(dirPath);
			assert(nRemoves == 1);
			delete dir;
		}
	}
}

void IA::ImgAnnotations::removeFiles(const StrList &filePaths)
{
	// remove all given files from the database
	BOOST_FOREACH (string filePath, filePaths)
		removeFile(filePath);
}

void IA::ImgAnnotations::removeObject(ID objID)
{
	if (existsObject(objID)) {
		Object *obj = getObject(objID);
		assert(NULL != obj);
		File *file = getFile(obj->getFilePath());

		// delete object id from our cache
		IDStrMap::size_type nRemoves = _objFilePaths.erase(objID);
		assert(nRemoves == 1);

		// delete the object entry from its file
		nRemoves = file->_objects.erase(objID);
		assert(nRemoves == 1);
		delete obj;
	}
}

void IA::ImgAnnotations::removeObjects(const IDList &objIDList)
{
	// remove all given objects from the database
	BOOST_FOREACH (ID objID, objIDList)
		removeObject(objID);
}

void IA::ImgAnnotations::clear()
{
	_dirs.clear();
	_objFilePaths.clear();
	_highestObjID = 0;
}

string IA::ImgAnnotations::dirPath(const string &filePath)
{
	string::size_type pos = filePath.rfind('/', string::npos);
	if (pos == string::npos)
		return "";
	return filePath.substr(0, pos);
}

string IA::ImgAnnotations::fileName(const string &filePath)
{
	string::size_type pos = filePath.rfind('/', string::npos);
	if (pos == string::npos)
		return "";
	return filePath.substr(pos + 1);
}

string IA::ImgAnnotations::filePath(const string &dirPath, const string &fileName)
{
	string outStr(dirPath);
	outStr.append("/");
	outStr.append(fileName);
	return outStr;
}



// ========== PropertyMap ==========

IA::PropertyMap::PropertyMap()
{
	// empty
}

bool IA::PropertyMap::isEmpty()
{
	return _properties.empty();
}

string IA::PropertyMap::get(const string &property) const
{
	string lowProperty = to_lower_copy(property);
	StrStrMap::const_iterator i = _properties.find(lowProperty);
	if (i == _properties.end())
		return string("");
	else
		return i->second;
}

bool IA::PropertyMap::isSet(const string &property) const
{
	string lowProperty = to_lower_copy(property);
	StrStrMap::const_iterator i = _properties.find(lowProperty);
	return i != _properties.end();
}

void IA::PropertyMap::clear(const std::string &property)
{
	string lowProperty = to_lower_copy(property);
	_properties.erase(lowProperty);
}

StrList IA::PropertyMap::getProperties() const
{
	StrList outList;
	typedef std::pair<string, string> thePair;
	BOOST_FOREACH(const thePair entry, _properties)
		outList.push_back(entry.first);
	return outList;
}

IA::PropertyMap& IA::PropertyMap::operator=(const IA::PropertyMap& p2)
{
	_properties = p2._properties;
	return *this;
}



// ========== Dir ==========

IA::Dir::Dir(const string &dirPath)
{
	_dirPath = dirPath;
}

IA::Dir::~Dir()
{
	// delete all allocated File references .. the rest is done automatically
	typedef std::pair<string, File*> thePair;
	BOOST_FOREACH(thePair entry, _files)
		delete entry.second;
}

string IA::Dir::getDirPath() const
{
	return _dirPath;
}

StrList IA::Dir::getFileNames() const
{
	StrList outList;
	typedef std::pair<string, File*> thePair;
	BOOST_FOREACH(const thePair entry, _files)
		outList.push_back(entry.first);
	return outList;
}

FileList IA::Dir::getFiles() const
{
	FileList outList;
	typedef std::pair<string, File*> thePair;
	BOOST_FOREACH(const thePair entry, _files)
		outList.push_back(entry.second);
	return outList;
}

size_type IA::Dir::numOfFiles() const
{
	return _files.size();
}

IA::File *IA::Dir::getFile(const string &fileName) const
{
	File *file = NULL;
	StrFileMap::const_iterator iFile = _files.find(fileName);
	if (iFile != _files.end())
		file = iFile->second;
	return file;
}


// ========== File ==========

IA::File::File(const string &filePath)
{
	_filePath = filePath;
}

IA::File::~File()
{
	// delete all allocated Object references .. the rest is done automatically
	typedef std::pair<int, Object*> thePair;
	BOOST_FOREACH(thePair entry, _objects)
		delete entry.second;
}

size_type IA::File::numOfObjects() const
{
	return _objects.size();
}

string IA::File::getFilePath() const
{
	return _filePath;
}

IDList IA::File::getObjectIDs() const
{
	IDList outList;
	typedef std::pair<ID, Object*> thePair;
	BOOST_FOREACH(const thePair entry, _objects)
		outList.push_back(entry.first);
	return outList;
}

ObjList IA::File::getObjects() const
{
	ObjList outList;
	typedef std::pair<ID, Object*> thePair;
	BOOST_FOREACH(const thePair entry, _objects)
		outList.push_back(entry.second);
	return outList;
}

IA::Object *IA::File::getObject(ID objID) const
{
	Object *obj = NULL;
	IDObjMap::const_iterator iObj = _objects.find(objID);
	if (iObj != _objects.end())
		obj = iObj->second;
	return obj;
}


// ========== Object ==========

IA::Object::Object(const string &filePath, ID id)
{
	_id = id;
	_filePath = filePath;
}

ID IA::Object::getID() const
{
	return _id;
}

string IA::Object::getFilePath() const
{
	return _filePath;
}

