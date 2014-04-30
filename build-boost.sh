#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

BOOST_DIR=$DIR/boost

BOOST_FLAGS="--with-serialization --with-system --with-thread --with-date_time --with-program_options"

# run b2 twice because it doesn't install fully the first time?
cd $BOOST_DIR && ./bootstrap.sh
cd $BOOST_DIR && ./b2 $BOOST_FLAGS link=static --build-dir=$DIR/deps/build --stage-dir=$DIR/deps/stage --prefix=$DIR/deps/ install