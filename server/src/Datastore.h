#pragma once

#include <iosfwd>
#include "Record.h"
#include "RecordEvent.h"

#define RECORD_KEY_SEPARATOR ':'

class Datastore {
public:
	Datastore() = default;
	Datastore(std::istream& in);

	const char* getValue(const char* key);
	bool modifyRecord(const RecordEvent& event);

	bool write(std::ostream& out);

	Record* getRecord(const char* key);

	/**
	 * @brief Creates a new path in the database and returns the record represented
	 * by that path.
	 *
	 * @details Creates a series of associative array records to satisfy the path
	 * provided. If the path already exists, it returns the record represented by
	 * the path. If the path can't be created as a result of invalid record types,
	 * a nullptr is returned instead.
	 *
	 * @param path Path to create
	 * @return Record representing the newly created path. If the path didn't exist,
	 * the newly created record will have an undefined type.
	 */
	Record* createPath(const char* path);

	inline bool good() const { return _good; }

private:
	void _parse(std::istream& in);
	std::vector<std::string> _tokenizePath(const char* key);
	bool _good = true;
	Record _root;
};