#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/level.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/vector.hpp>

#include <string>
#include <vector>

class Datastore;

enum class RecordEventType : uint8_t {
	kUndefined = 0x0,
	kSet       = 0x1,
	kMove      = 0x2,
	kDelete    = 0x3,
};

class RecordEvent {
public:
	RecordEvent() = default;

	RecordEvent(RecordEventType type, std::vector<std::string> args)
		: _args(std::move(args))
		, _type(type)
	{}

	template <typename ... Args>
	RecordEvent(RecordEventType type, std::string arg, Args&& ... args)
		: _args({std::move(arg), std::forward<Args>(args)...})
		, _type(type)
	{}

	RecordEventType getType() const { return _type; }
	bool operator()(Datastore* ds) const;

private:
	std::vector<std::string> _args;
	RecordEventType _type = RecordEventType::kUndefined;

	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & _type;
		ar & _args;
	}
};

// elminate serialization overhead at the cost of
// never being able to increase the version.
BOOST_CLASS_IMPLEMENTATION(RecordEvent, boost::serialization::object_serializable);

// eliminate object tracking (even if serialized through a pointer)
// at the risk of a programming error creating duplicate objects.
BOOST_CLASS_TRACKING(RecordEvent, boost::serialization::track_never);