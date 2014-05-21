#pragma once

#define ERROR_STRING          "ERROR"
#define INTERNAL_ERROR_STRING "INTERNAL ERROR"
#define OK_STRING             "OK"
#define NOT_FOUND_STRING      "KEY NOT FOUND"
#define INVALID_PATH_STRING   "INVALID PATH"

class Datastore;

enum class RecordEventType : uint8_t {
	kUndefined = 0x0,
	kGet       = 0x1,
	kSet       = 0x2,
	kMove      = 0x3,
	kDelete    = 0x4,
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
	std::string operator()(Datastore* ds) const;

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