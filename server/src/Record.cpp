#include "Record.h"
#include "../../shared/src/utility.h"

const Record gNullRecord;

Record::Record(Record&& source) {
	_type = source._type;
	source._type = RecordType::kUndefined;
	
	_assocChildren = std::move(source._assocChildren);
	_arrayChildren = std::move(source._arrayChildren);
	_val = std::move(source._val);
}

Record::Record(std::string source) {
	_type = RecordType::kString;
	_val = std::move(source);
}

Record& Record::operator=(std::string rhs) {
	if (_type != RecordType::kString && _type != RecordType::kUndefined) {
		throw RecordException(Format("cannot assign string to record of type %s", RecordTypeStr(_type)));
	}
	_type = RecordType::kString;
	_val = std::move(rhs);
	return *this;
}

Record& Record::operator=(Record&& rhs) {
	if (&rhs == this) {
		return *this;
	}

	_type = rhs._type;
	rhs._type = RecordType::kUndefined;
	
	_assocChildren = std::move(rhs._assocChildren);
	_arrayChildren = std::move(rhs._arrayChildren);
	_val = std::move(rhs._val);

	return *this;
}

const Record& Record::operator[](size_t arrayIndex) const {
	if (_type != RecordType::kArray) {
		throw RecordException(Format("cannot access index of children for records of type %s", RecordTypeStr(_type)));
	}

	return _arrayChildren.at(arrayIndex);
}

Record& Record::operator[](size_t arrayIndex) {
	// Cast *this to const, strip away the const of the resulting child Record.
	// See http://stackoverflow.com/questions/123758/how-do-i-remove-code-duplication-between-similar-const-and-non-const-member-func
	return const_cast<Record&>(static_cast<const Record&>(*this)[arrayIndex]);
}

template <>
const Record& Record::operator[](const char* assocKey) const {
	if (_type != RecordType::kAssocArray) {
		throw RecordException(Format("cannot access keys of children for records of type %s", RecordTypeStr(_type)));
	}

	auto it = _assocChildren.find(assocKey);

	if (it != _assocChildren.end()) {
		return it->second;
	}
	return gNullRecord;
}

template <>
Record& Record::operator[](const char* assocKey) {
	if (_type != RecordType::kAssocArray) {
		if (_type != RecordType::kUndefined) {
			throw RecordException(Format("cannot access keys of children for records of type %s", RecordTypeStr(_type)));	
		}
		_type = RecordType::kAssocArray; 
	}

	return _assocChildren[assocKey];
}

bool Record::operator==(const std::string& rhs) {
	if (_type != RecordType::kString) {
		throw RecordException(Format("cannot compare string to record of type %s", RecordTypeStr(_type)));
	}
	return rhs == _val;
}

bool Record::operator==(const char* rhs) {
	if (_type != RecordType::kString) {
		throw RecordException(Format("cannot compare string to record of type %s", RecordTypeStr(_type)));
	}
	return strcmp(rhs, _val.c_str()) == 0;
}

void Record::push_back(std::string val) {
	if (_type != RecordType::kArray) {
		if (_type != RecordType::kUndefined) {
			throw RecordException(Format("push_back not applicable to records of type %s", RecordTypeStr(_type)));
		}
		_type = RecordType::kArray;
	}

	_arrayChildren.push_back(std::move(val));
}

size_t Record::numChildren() const {
	if (_type == RecordType::kArray) {
		return _arrayChildren.size();	
	} else if (_type == RecordType::kAssocArray) {
		return _assocChildren.size();
	}
	throw RecordException(Format("numChildren not applicable to records of type %s", RecordTypeStr(_type)));	
}

const std::string& Record::getVal() {
	if (_type != RecordType::kString) {
		throw RecordException(Format("getVal not applicable to records of type %s", RecordTypeStr(_type)));			
	}
	return _val;
}