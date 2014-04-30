#pragma once

#include <iosfwd>
#include "Record.h"
#include "RecordEvent.h"

#define RECORD_KEY_SEPARATOR ':'

class Datastore {
public:
	Datastore() = default;
	Datastore(std::istream& in);

	const std::string& getValue(const char* key);

	bool write(std::ostream& out);

	Record* getRecord(const char* path);

	inline Record* getRoot() { return &_root; }

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
	 * the newly created record will have a RecordType::kUndefined type.
	 */
	Record* createPath(const char* path);

	/**
	 * @brief Removes a record from the Datastore
	 * @details Provided a path, the Datastore will remove that record and all of
	 * that record's children from the tree. If the parent no longer has any
	 * children, the parent will be removed as well. This process will continue
	 * until a record is found that has valid children.
	 *
	 * @param path Path of record to remove
	 */
	void removeRecord(const char* path);

	inline bool good() const { return _good; }

private:
	std::vector<std::string> _tokenizePath(const char* path);

	/**
	 * @brief Returns an array of pointers representing records on a path.
	 *
	 * @details This function will return as many pointers as the input
	 * array had tokens in the path. If the path doesn't represent enough valid
	 * Records, null pointers will fill invalid record spots.
	 *
	 * @param path Path to traverse
	 * @return Array of Record pointers that are on the path.
	 */
	std::vector<Record*> _recordsOnPath(const std::vector<std::string>& path);

	bool _good = true;
	Record _root;
};