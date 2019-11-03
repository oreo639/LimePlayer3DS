#include "lang.hpp"

namespace i18n {
	void Init(void);
	void Exit(void);
	staticString LocalizeStatic(int lang, const std::string& val);
	std::string Localize(int lang, const std::string& val);
	int Code2Int(const std::string& str);
	std::string Int2Code(int lang);
}
