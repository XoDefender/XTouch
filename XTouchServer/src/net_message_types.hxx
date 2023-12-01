enum class MsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,

	PasswordLogin,

	GetModels,
	GetModelFiles,

	AddModelToFavorite,
	RemoveModelFromFavorite,

	GetModelByName,
	GetFavoriteModels,

	SortModels,
	SortFavoriteModels,
	SortFiles,

	GetModelFile,
};
