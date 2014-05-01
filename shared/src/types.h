#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

typedef boost::archive::text_iarchive InputArchiveType;
typedef boost::archive::text_oarchive OutputArchiveType;

#include <string>
#include <vector>

typedef std::vector<std::string> Transaction;
typedef std::vector<std::string> Result;