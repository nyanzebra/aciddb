#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <exception>

enum class RecordType {
	kAssocArray,
	kArray,
	kString,
	kUndefined,
};

inline constexpr const char* RecordTypeStr(RecordType type) {
	switch(type) {
		case RecordType::kAssocArray:	return "associative array";
		case RecordType::kArray:		return "array";
		case RecordType::kString:		return "string";
		case RecordType::kUndefined:	return "undefined";
	}
	return "";
}

class RecordException : public std::runtime_error {
	using std::runtime_error::runtime_error;
};

class Record {
public:
	Record() {};
	Record(const Record&) = delete;
	Record(Record&& source);
	Record(std::string source);

	Record& operator=(std::string rhs);
	Record& operator=(Record&& rhs);
	Record& operator=(const Record& rhs) = delete;

	const Record& operator[](size_t arrayIndex) const;
	Record& operator[](size_t arrayIndex);

	// These templates are unimplemented though their specializations are below.
	// It's a trick to make compiler "prefer" size_t overloads when provided key of 0.
	// See http://stackoverflow.com/questions/4672152/call-of-overloaded-function-is-ambiguous
	template <class T>
	const Record& operator[](const T* assocKey) const;
	template <class T>
	Record& operator[](const T* assocKey);

	bool operator==(const std::string& rhs);
	bool operator==(const char* rhs);

	void push_back(std::string val);

	inline RecordType getType() const { return _type; }

	size_t numChildren() const;

	const std::string& getVal();
private:
	RecordType _type = RecordType::kUndefined;
	std::unordered_map<std::string, Record> _assocChildren;
	std::vector<Record> _arrayChildren;
	std::string _val;
};

template <> const Record& Record::operator[](const char* assocKey) const;
template <> Record& Record::operator[](const char* assocKey);
