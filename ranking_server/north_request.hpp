
class Request
{
public:
	enum class Type { Ranking, Index };

	ubjson::Value formUbjsonReuqest();
	Type type;
	Query query;
	TextIndex text_index;
	TextID text_id;
};
