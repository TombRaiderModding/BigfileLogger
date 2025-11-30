#include "HashSet.h"

HashSet::HashSet() : m_hashes()
{
}

bool HashSet::try_insert(uint64_t hash)
{
	if (m_hashes.find(hash) == m_hashes.end())
	{
		m_hashes.insert(hash);

		return true;
	}

	return false;
}