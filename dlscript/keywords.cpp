#include "keywords.h"

keyword_type is_keyword(const std::string& kw) {
	if (kw == kw_function) return keyword_type_function;
	if (kw == kw_true) return keyword_type_true;
	if (kw == kw_false) return keyword_type_false;
	if (kw == kw_nullptr) return keyword_type_nullptr;
	if (kw == kw_null) return keyword_type_null;
	return keyword_type_unknown;
}

