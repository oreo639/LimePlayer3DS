#include <vector>

#include "error.hpp"

static std::vector<int> _err;

void Error::Add(int err) {
	// Error codes of 0 are not allowed
	if (err != 0)
		_err.push_back(err);
}

void Error::Remove(void) {
	_err.pop_back();
}

int Error::Get(void) {
	return _err[_err.size()-1];
}

bool Error::IsQuered(void) {
	return _err.size();
}

void Error::Clear(void) {
	_err.clear();
}
