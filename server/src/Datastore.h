#pragma once

#include <iosfwd>
#include "Record.h"
//#include "DRME.h"

#define RECORD_KEY_SEPARATOR ':'

class Datastore {
public:
	Datastore() = default;
	Datastore(std::istream& in);

	const char* getKey(const char* path);
	// TODO: implement DRMEs
	//bool modifyRecord(DRME event);

	bool write(std::ostream out);

	inline bool good() const { return _good; }

private:
	void _parse(std::istream& in);
	std::vector<std::string> _tokenizePath(const char* path);
	Record* _getRecord(const char* path);
	bool _good = true;
	Record _root;
};