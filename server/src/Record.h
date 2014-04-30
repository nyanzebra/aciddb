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

	Record& operator[](const char* key);

	Record& getChild(const char* key);

	bool operator==(const std::string& rhs);
	bool operator==(const char* rhs);

	void push_back(std::string val);

	inline RecordType getType() const { return _type; }

	size_t numChildren() const;

	/**
	 * @brief Removes a child from the record.
	 * @details The record maintains its type even if the last child is removed.
	 *
	 * @param key Child key to remove
	 */
	void removeChild(const char* key);

	void removeAllChildren();

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

extern const Record gNullRecord;