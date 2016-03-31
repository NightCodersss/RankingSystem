
class Request
{
public:
	enum class Type { Ranking, Index };

	//! text_id have to correspond to using text_index if we are asking index_server. Correspondence map is in RankingStruct 
	ubjson::Value formUbjsonReuqest(TextID text_id = EmptyTextID);
	Type type;
	Query query;
	TextIndex text_index;
}
