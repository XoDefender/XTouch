enum class MsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,

	PasswordLogin,

	GetModels,
	GetModelFiles,

	ChangeModelFavState,

	GetModelByName,
	GetFavoriteModels,

	GetModelFile,
};