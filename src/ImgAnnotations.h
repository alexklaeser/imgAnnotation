#ifndef ImgAnnotation_H
#define ImgAnnotation_H

#include <list>
#include <string>
#include <iterator>
//#include <ext/hash_map>
#include <map>

#include <boost/functional/hash/hash.hpp>


/**
 * Define the hash function for strings used in the hash_map
 * Bug: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=13342
 */
//namespace __gnu_cxx
//{
//	using namespace std;
//
//	template<>
//	struct hash<string>
//	{
//		size_t operator()(const string& s) const
//		{
//			const collate<char>& c = use_facet<collate<char> >(locale::classic());
//			return c.hash(s.c_str(), s.c_str() + s.size());
//		}
//	};
//}

namespace IA
{
	// forward declarations
	class ImgAnnotations;
	class PropertyMap;
	class Object;
	class File;
	class Dir;

	// typedefs
	typedef int ID;
	typedef int size_type;

	typedef std::map<std::string, File*> 		StrFileMap;
	typedef std::map<std::string, Dir*> 			StrDirMap;
	typedef std::map<std::string, Object*> 		StrObjMap;
	typedef std::map<std::string, std::string> 	StrStrMap;
	typedef std::map<ID, Object*> 				IDObjMap;
	typedef std::map<ID, std::string> 			IDStrMap;

	typedef std::pair<std::string, std::string>		StrStrPair;

	typedef std::list<std::string> 	StrList;
	typedef std::list<ID> 			IDList;
	typedef std::list<File*> 		FileList;
	typedef std::list<Dir*> 		DirList;
	typedef std::list<Object*> 		ObjList;
	typedef std::list<PropertyMap*> PropList;

	// define a point as a child of ublas::vector with two dimensions
//	class Point : public boost::numeric::ublas::vector<float> {
//		Point() : boost::numeric::ublas::vector<float>(2) { }
//	};

//	// define a bounding box as a child of ublas::vector with four dimensions
//	class Bounding : public boost::numeric::ublas::vector<float> {
//		Point() : boost::numeric::ublas::vector<float>(2) { }
//	};



	class ImgAnnotations {

	private:
		// hash that stores the directories
		StrDirMap _dirs;
		// cache the highest object id
		ID _highestObjID;
		// cache the dir and file name for each object ID
		IDStrMap _objFilePaths;
		std::string _annotationsFilePath;

	private:
		StrList getAllProperties(const PropList& propList) const;
		StrList getAllPropertyValues(const PropList& propList, const std::string& property) const;
	
	public:
		ImgAnnotations();
		~ImgAnnotations();

		void loadFromFile(const std::string &fileName);
		void saveToFile(const std::string &fileName);
		std::string getAnnotationsFilePath() const
		{
			return _annotationsFilePath;
		}
		
		StrList getAllObjProperties() const;
		StrList getAllObjPropertyValues(const std::string& property) const;
		StrList getAllFileProperties() const;
		StrList getAllFilePropertyValues(const std::string& property) const;

		size_type numOfDirs() const;
		size_type numOfFiles(const std::string &dirPath = std::string()) const;
		size_type numOfObjects(const std::string &dirPath = std::string(), const std::string &fileName = std::string()) const;

		bool existsDir(const std::string &dirPath) const;
		bool existsFile(const std::string &filePath) const;
		bool existsObject(ID objID) const;

		StrList getDirPaths() const;
		StrList getFilePaths(const std::string &dirPath = std::string()) const;
		IDList getObjectIDs(const std::string &dirPath = std::string(), const std::string &fileName = std::string()) const;

		DirList getDirs() const;
		FileList getFiles(const std::string &dirPath = std::string()) const;
		ObjList getObjects(const std::string &dirPath = std::string(), const std::string &fileName = std::string()) const;

		IA::Object *getObject(ID objID) const;
		IA::File *getFile(const std::string &filePath) const;
		IA::Dir *getDir(const std::string &dirPath) const;

		IA::Object *newObject(const std::string &filePath, ID objID = -1);
		void addFile(const std::string &filePath);
		void addFiles(const StrList &filePaths);
		void removeFiles(const StrList &);
		void removeFile(const std::string &);
		void removeObjects(const IDList &objIDList);
		void removeObject(ID objID);
		void clear();

		// split the path to a file into its dirPath and fileName
		static std::string dirPath(const std::string &filePath);
		static std::string fileName(const std::string &filePath);
		static std::string filePath(const std::string &dirPath, const std::string &fileName);
	};



	class PropertyMap {
		friend class ImgAnnotations;

	private:
		StrStrMap _properties;

	public:
		PropertyMap();
		bool isEmpty();

		// get/set methods for the object properties
		std::string get(const std::string &property) const;
		template<typename T>
		T getAs(const std::string& property) const;
		template<typename T>
		void set(const std::string &property, const T& value);
		bool isSet(const std::string &property) const;
		void clear(const std::string &property);
		StrList getProperties() const;
		PropertyMap& operator=(const PropertyMap& p2);
	};



	class Dir {
		friend class ImgAnnotations;

	private:
		StrFileMap _files;
		std::string _dirPath;

	public:
		Dir(const std::string &dirPath);
		~Dir();
		std::string getDirPath() const;
		StrList getFileNames() const;
		FileList getFiles() const;
		size_type numOfFiles() const;
		IA::File *getFile(const std::string &fileName) const;
	};



	class File : public PropertyMap {
		friend class ImgAnnotations;

	private:
		IDObjMap _objects;
		std::string _filePath;

	public:
		File(const std::string &filePath);
		~File();
		std::string getFilePath() const;
		IDList getObjectIDs() const;
		ObjList getObjects() const;
		size_type numOfObjects() const;
		IA::Object *getObject(ID objID) const;
	};



	class Object : public PropertyMap {
		friend class ImgAnnotations;

	private:
		ID _id;
		std::string _filePath;

	public:
		Object(const std::string &filePath, ID id);
		ID getID() const;
		std::string getFilePath() const;
	};

} // namespace

#endif
