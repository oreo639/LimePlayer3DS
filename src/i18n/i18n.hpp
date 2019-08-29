#include "lang.hpp"

namespace i18n {
	void Init(void);
	void Exit(void);
	C2D_Text* Localize(int lang, const std::string& val);
	int Code2Int(const std::string& str);
	std::string Int2Code(int lang);
}
