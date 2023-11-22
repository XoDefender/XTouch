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

MsgTypes operator|(MsgTypes first_type, MsgTypes second_type)
{
    using underlying_type = std::underlying_type_t<MsgTypes>;
    return static_cast<MsgTypes>(static_cast<underlying_type>(first_type) | static_cast<underlying_type>(second_type));
}
