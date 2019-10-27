#include "error.hpp"

static std::vector<LimeError_t> _err;

void Error::Add(int err) {
	// Error codes of 0 are not allowed
	if (err != 0) {
		LimeError_t tmperr;
		tmperr.err_code = err;
		_err.push_back(tmperr);
	}
}

void Error::Add(int err, const std::string& extraInfo) {
	// Error codes of 0 are not allowed
	if (err != 0) {
		LimeError_t tmperr;
		tmperr.err_code = err;
		tmperr.extra_info = extraInfo;
		_err.push_back(tmperr);
	}
}

void Error::Remove(void) {
	_err.pop_back();
}

LimeError_t Error::Get(void) {
	return _err[_err.size()-1];
}

bool Error::IsQuered(void) {
	return _err.size();
}

void Error::Clear(void) {
	_err.clear();
}
