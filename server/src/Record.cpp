#include "Record.h"
#include "../../shared/src/utility.h"

const Record gNullRecord;

Record::Record(Record&& source) {
	_type = source._type;
	source._type = RecordType::kUndefined;

	_assocChildren = std::move(source._assocChildren);
	_arrayChildren = std::move(source._arrayChildren);

	_val = std::move(source._val);

	source._assocChildren.clear();
	source._arrayChildren.clear();
	source._val.clear();
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

	rhs._assocChildren.clear();
	rhs._arrayChildren.clear();
	rhs._val.clear();

	return *this;
}

Record& Record::getChild(const std::string& key) {
	if (_type != RecordType::kAssocArray && _type != RecordType::kArray) {
		throw RecordException(Format("cannot access keys of children for records of type %s", RecordTypeStr(_type)));
	}

	if (_type == RecordType::kAssocArray) {
		auto it = _assocChildren.find(key);
		if (it != _assocChildren.end()) {
			return it->second;
		}
	} else if (_type == RecordType::kArray) {
		try {
			return _arrayChildren.at(std::stoll(key));
		} catch (...) {}
	}

	throw RecordException("child with key not found");
}

Record& Record::operator[](const std::string& key) {
	if (_type != RecordType::kAssocArray && _type != RecordType::kArray) {
		if (_type != RecordType::kUndefined) {
			throw RecordException(Format("cannot access keys of children for records of type %s", RecordTypeStr(_type)));
		}
		_type = RecordType::kAssocArray; // default to creating an assoc array
	}

	if (_type == RecordType::kAssocArray) {
		return _assocChildren[key];
	}
	// _type must be RecordType::kArray
	try {
		return _arrayChildren.at(std::stoll(key));
	} catch (...) {}
	throw RecordException("array index key out of bounds");
}

bool Record::operator==(const std::string& rhs) {
	if (_type != RecordType::kString) {
		throw RecordException(Format("cannot compare string to record of type %s", RecordTypeStr(_type)));
	}
	return rhs == _val;
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

void Record::removeChild(const std::string& key) {
	if (_type == RecordType::kArray) {
		size_t i = 0;
		try {
			i = std::stoll(key);
			if (i < _arrayChildren.size()) {
				_arrayChildren.erase(_arrayChildren.begin() + i);
				return;
			}
		} catch (...) {}
	} else if (_type == RecordType::kAssocArray) {
		auto it = _assocChildren.find(key);
		if (it != _assocChildren.end()) {
			_assocChildren.erase(it);
			return;
		}
	}

	throw RecordException(Format("cannot remove child: invalid key %d", key));
}

void Record::removeAllChildren() {
	if (_type != RecordType::kArray && _type != RecordType::kAssocArray) {
		throw RecordException(Format("cannot remove children in record of type %s", RecordTypeStr(_type)));
	}

	_assocChildren.clear();
	_arrayChildren.clear();
}

const std::string& Record::getVal() {
	if (_type != RecordType::kString) {
		throw RecordException(Format("getVal not applicable to records of type %s", RecordTypeStr(_type)));
	}
	return _val;
}