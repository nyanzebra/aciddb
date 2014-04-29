#pragma once

#include <string>

class Datastore;

class RecordEvent {
public:
	RecordEvent(std::string key)
		: _key(std::move(key))
	{}

	virtual bool operator()(Datastore* ds) const = 0;

	inline const std::string& getKey() const { return _key; }

private:
	const std::string _key;
};

class EventSet : public RecordEvent {
public:
	EventSet(std::string key, std::string data)
		: RecordEvent(std::move(key))
		, _data(std::move(data))
	{}

	bool operator()(Datastore* ds) const;

private:

	const std::string _data;
};

class EventMove : public RecordEvent {
public:
	EventMove(std::string key, std::string target)
		: RecordEvent(std::move(key))
		, _target(std::move(target))
	{}

	bool operator()(Datastore* ds) const;

private:
	const std::string _target;
};

class EventDelete : public RecordEvent {
public:
	bool operator()(Datastore* ds) const;
};