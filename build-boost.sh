#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

BOOST_DIR=$DIR/boost

# run b2 twice because it doesn't install fully the first time?
cd $BOOST_DIR && ./bootstrap.sh
cd $BOOST_DIR && ./b2 --with-serialization link=static --build-dir=$DIR/deps/build --stage-dir=$DIR/deps/stage --prefix=$DIR/deps/ install