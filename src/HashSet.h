#pragma once

#include <cstdint>
#include <set>
#include <string>

// HashSet is a set with found hashes, not to be confused with the hash set data structure
class HashSet
{
private:
	std::set<uint64_t> m_hashes;

public:
	HashSet();

	// Try to insert a hash and return true if it didn't exist
	bool try_insert(uint64_t hash);
};