#pragma once

#include <iostream>

#include "RecordEvent.h"
#include "config.h"

class Datastore;

class Journal {
public:
	Journal(std::iostream& data)
		: _dataStream(data)
	{}

	/**
	 * @brief Writes a single Transaction to the Journal.
	 * @details In addition to writing the transaction, the data buffer is flushed to
	 * ensure that the data persists.
	 *
	 * @param events The events to write.
	 */
	void writeTransaction(const std::vector<RecordEvent>& events);

	/**
	 * @brief Applies the Journal's datastream to an arbitrary Datastore
	 * @details The process consists of deserializing the RecordEvents contained
	 * in the provided data stream and then applying each of those, sequentially,
	 * to the Datastore.
	 *
	 * @param ds The Datastore to apply the journaled events to
	 * @return True if successful, false otherwise
	 */
	bool applyToDatastore(Datastore* ds);

private:
	std::iostream& _dataStream;
};