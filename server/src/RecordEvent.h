#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/level.hpp>
#include <boost/serialization/tracking.hpp>

#include <string>

class Datastore;

enum class RecordEventType : uint8_t {
	kSet    = 0x1,
	kMove   = 0x2,
	kDelete = 0x3,
};

class RecordEvent {
public:
	RecordEvent(std::string key) : key(std::move(key)) {}
	virtual ~RecordEvent() = default;

	virtual RecordEventType getType() const = 0;
	virtual bool operator()(Datastore* ds) const = 0;

	const std::string key;
};

class EventSet : public RecordEvent {
public:
	RecordEventType getType() const { return RecordEventType::kSet; }

	EventSet(std::string key, std::string data)
		: RecordEvent(std::move(key))
		, data(std::move(data))
	{}

	bool operator()(Datastore* ds) const;

	const std::string data;
};

class EventMove : public RecordEvent {
public:
	RecordEventType getType() const { return RecordEventType::kMove; }

	EventMove(std::string key, std::string target)
		: RecordEvent(std::move(key))
		, target(std::move(target))
	{}

	bool operator()(Datastore* ds) const;

	const std::string target;
};

class EventDelete : public RecordEvent {
public:
	RecordEventType getType() const { return RecordEventType::kDelete; }

	using RecordEvent::RecordEvent;
	bool operator()(Datastore* ds) const;
};

template <typename Archive>
void serialize(Archive& ar, RecordEvent& event, const unsigned int version) {
	auto t = event.getType();

	ar & t;

	switch (t) {
		case RecordEventType::kSet:
			ar & static_cast<EventSet&>(event).data;
			break;
		case RecordEventType::kMove:
			ar & static_cast<EventMove&>(event).target;
			break;
		case RecordEventType::kDelete:
			break;
	}
}

// elminate serialization overhead at the cost of
// never being able to increase the version.
BOOST_CLASS_IMPLEMENTATION(RecordEvent, boost::serialization::object_serializable);

// eliminate object tracking (even if serialized through a pointer)
// at the risk of a programming error creating duplicate objects.
BOOST_CLASS_TRACKING(RecordEvent, boost::serialization::track_never);