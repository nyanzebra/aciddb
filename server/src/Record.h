#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <exception>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>

enum class RecordType : uint8_t {
	kUndefined	= 0x0,
	kAssocArray	= 0x1,
	kArray		= 0x2,
	kString		= 0x3,
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
	Record(const Record&) = default;
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

	friend class boost::serialization::access;
	
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) {

		ar & _type;

		switch (_type) {
			case RecordType::kUndefined: break;
			case RecordType::kAssocArray: {
				ar & _assocChildren;
				break;
			}
			case RecordType::kArray: {
				ar & _arrayChildren;
				break;
			}
			case RecordType::kString: {
				ar & _val;
				break;
			}
		}
	}

};

template <> const Record& Record::operator[](const char* assocKey) const;
template <> Record& Record::operator[](const char* assocKey);

extern const Record gNullRecord;