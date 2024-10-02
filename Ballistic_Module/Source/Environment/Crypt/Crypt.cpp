#include "Crypt.hpp"

using namespace Libraries;

enum ECryptMode
{
	/* AES */
	AES_CBC,
	AES_CFB,
	AES_CTR,
	AES_OFB,
	AES_GCM,
	AES_EAX,

	/* Blowfish */
	BF_CBC,
	BF_CFB,
	BF_OFB
};

enum EHashMode
{
	/* MD5 */
	MD5,

	/* SHA1 */
	SHA1,

	/* SHA2 */
	SHA224,
	SHA256,
	SHA384,
	SHA512,

	/* SHA3 */
	SHA3_256,
	SHA3_384,
	SHA3_512,
};

std::map<std::string, ECryptMode> CryptTranslationMap = {
	/* AES */
	{ "aes-cbc", AES_CBC },
	{ "aes_cbc", AES_CBC },

	{ "aes-cfb", AES_CFB },
	{ "aes_cfb", AES_CFB },

	{ "aes-ctr", AES_CTR },
	{ "aes_ctr", AES_CTR },

	{ "aes-ofb", AES_OFB },
	{ "aes_ofb", AES_OFB },

	{ "aes-gcm", AES_GCM },
	{ "aes_gcm", AES_GCM },

	{ "aes-eax", AES_EAX },
	{ "aes_eax", AES_EAX },

	/* Blowfish */
	{ "blowfish-cbc", BF_CBC },
	{ "blowfish_cbc", BF_CBC },
	{ "bf-cbc", BF_CBC },
	{ "bf_cbc", BF_CBC },

	{ "blowfish-cfb", BF_CFB },
	{ "blowfish_cfb", BF_CFB },
	{ "bf-cfb", BF_CFB },
	{ "bf_cfb", BF_CFB },

	{ "blowfish-ofb", BF_OFB },
	{ "blowfish_ofb", BF_OFB },
	{ "bf-ofb", BF_OFB },
	{ "bf_ofb", BF_OFB },
};

std::map<std::string, EHashMode> HashTranslationMap = {
	/* MD5 */
	{ "md5", MD5 },

	/* SHA1 */
	{ "sha1", SHA1 },

	/* SHA2 */
	{ "sha224", SHA224 },
	{ "sha256", SHA256 },
	{ "sha384", SHA384 },
	{ "sha512", SHA512 },

	/* SHA3 */
	{ "sha3-256", SHA3_256 },
	{ "sha3_256", SHA3_256 },
	{ "sha3-384", SHA3_384 },
	{ "sha3_384", SHA3_384 },
	{ "sha3-512", SHA3_512 },
	{ "sha3_512", SHA3_512 },
};

__forceinline std::string Base64Decode(const std::string& EncodedString)
{
	std::string Decoded;
	CryptoPP::StringSource StringSource(EncodedString, true,
		new CryptoPP::Base64Decoder(
			new CryptoPP::StringSink(Decoded)
		)
	);

	return Decoded;
}

__forceinline std::string Base64Encode(byte* BytesToEncode, size_t Len)
{
	std::string Encoded;
	CryptoPP::StringSource StringSource(BytesToEncode, Len, true,
		new CryptoPP::Base64Encoder(
			new CryptoPP::StringSink(Encoded),
			false
		)
	);

	return Encoded;
}

template<typename T>
__forceinline std::string EncryptWithAlgo(lua_State* L, std::string& Data, std::string& KeySize, std::string& IvcStr)
{
	try
	{
		std::string Encrypted;

		T Encryptor;
		Encryptor.SetKeyWithIV((byte*)KeySize.c_str(), KeySize.size(), (byte*)IvcStr.c_str(), IvcStr.length());

		CryptoPP::StringSource ss(Data, true,
			new CryptoPP::StreamTransformationFilter(Encryptor,
				new CryptoPP::StringSink(Encrypted)
			)
		);

		return Base64Encode((unsigned char*)Encrypted.c_str(), Encrypted.size());
	}
	catch (CryptoPP::Exception& Exception)
	{
		luaL_error(L, Exception.what());
		return "";
	}
}

template<typename T>
__forceinline std::string EncryptAuthenticatedWithAlgo(lua_State* L, const std::string& Data, const std::string& KeySize, const std::string& IvcStr)
{
	try
	{
		std::string Encrypted;

		T Encryptor;
		Encryptor.SetKeyWithIV((byte*)KeySize.c_str(), KeySize.size(), (byte*)IvcStr.c_str(), IvcStr.size());

		CryptoPP::AuthenticatedEncryptionFilter Aef(Encryptor,
			new CryptoPP::StringSink(Encrypted)
		);

		Aef.Put((byte*)Data.data(), Data.size());
		Aef.MessageEnd();

		return Base64Encode((unsigned char*)Encrypted.c_str(), Encrypted.size());
	}
	catch (CryptoPP::Exception& Exception)
	{
		luaL_error(L, Exception.what());
		return "";
	}
}

template<typename T>
__forceinline std::string DecryptWithAlgo(lua_State* L, std::string& CipherText, std::string& Key, std::string& IV)
{
	try
	{
		std::string Decrypted;

		T Decryptor;
		Decryptor.SetKeyWithIV((byte*)Key.c_str(), Key.size(), (byte*)IV.c_str(), IV.length());

		auto Base = Base64Decode(CipherText);

		CryptoPP::StringSource ss(Base, true,
			new CryptoPP::StreamTransformationFilter(Decryptor,
				new CryptoPP::StringSink(Decrypted)
			)
		);

		return Decrypted;
	}
	catch (CryptoPP::Exception& Exception)
	{
		luaL_error(L, Exception.what());
		return "";
	}
}

template<typename T>
__forceinline std::string DecryptAuthenticatedWithAlgo(lua_State* L, const std::string& CipherText, const std::string& Key, const std::string& IV)
{
	try
	{
		std::string Decrypted;

		T Decryptor;
		Decryptor.SetKeyWithIV((byte*)Key.c_str(), Key.size(), (byte*)IV.c_str(), IV.size());

		auto Base = Base64Decode(CipherText);

		CryptoPP::AuthenticatedDecryptionFilter Adf(Decryptor,
			new CryptoPP::StringSink(Decrypted)
		);

		Adf.Put((byte*)Base.data(), Base.size());
		Adf.MessageEnd();

		return Decrypted;
	}
	catch (CryptoPP::Exception& Exception)
	{
		luaL_error(L, Exception.what());
		return "";
	}
}

void SplitString(std::string Str, std::string By, std::vector<std::string>& Tokens)
{
	Tokens.push_back(Str);
	auto SplitLen = By.size();
	while (true)
	{
		auto Frag = Tokens.back();
		auto SplitAt = Frag.find(By);
		if (SplitAt == std::string::npos)
			break;

		Tokens.back() = Frag.substr(0, SplitAt);
		Tokens.push_back(Frag.substr(SplitAt + SplitLen, Frag.size() - (SplitAt + SplitLen)));
	}
}

using yield_ret = std::function<int(lua_State* L)>;
template<typename T>
__forceinline static std::string HashWithAlgorithm(std::string& Input)
{
	T Hash;
	std::string Digest;

	CryptoPP::StringSource StringSource(Input, true,
		new CryptoPP::HashFilter(Hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(Digest), false
			)
		)
	);

	return Digest;
}

__forceinline static int EncryptStringCustom(lua_State* L)
{
	luaL_stackcheck(L, 4, 4);
	std::string Algorithm = luaL_checklstring(L, 1, nullptr);
	std::string Data = luaL_checklstring(L, 2, nullptr);
	std::string KeySize = luaL_checklstring(L, 3, nullptr);
	std::string IvcStr = luaL_checklstring(L, 4, nullptr);

	std::transform(Algorithm.begin(), Algorithm.end(), Algorithm.begin(), tolower);

	if (!CryptTranslationMap.count(Algorithm)) 
	{
		luaL_argerror(L, 1, "non-existant algorithm");
		return 0;
	}

	auto SelectedAlgorithm = CryptTranslationMap[Algorithm];
	std::string EncryptedData;

	if (SelectedAlgorithm == AES_CBC)
		EncryptedData = EncryptWithAlgo<CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == AES_CFB)
		EncryptedData = EncryptWithAlgo<CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == AES_CTR)
		EncryptedData = EncryptWithAlgo<CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == AES_OFB)
		EncryptedData = EncryptWithAlgo<CryptoPP::OFB_Mode<CryptoPP::AES>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == AES_GCM)
		EncryptedData = EncryptAuthenticatedWithAlgo<CryptoPP::GCM<CryptoPP::AES>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == AES_EAX)
		EncryptedData = EncryptAuthenticatedWithAlgo<CryptoPP::EAX<CryptoPP::AES>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == BF_CBC)
		EncryptedData = EncryptWithAlgo<CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == BF_CFB)
		EncryptedData = EncryptWithAlgo<CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == BF_OFB)
		EncryptedData = EncryptWithAlgo<CryptoPP::OFB_Mode<CryptoPP::Blowfish>::Encryption>(L, Data, KeySize, IvcStr);
	else
	{
		luaL_argerror(L, 1, "non-existent algorithm");
		return 0;
	}

	lua_pushlstring(L, EncryptedData.c_str(), EncryptedData.size());
	return 1;
};

__forceinline static int EncryptString(lua_State* L)
{
	luaL_stackcheck(L, 2, 2);
	std::string Data = luaL_checklstring(L, 1, nullptr);
	std::string Key = luaL_checklstring(L, 2, nullptr);

	CryptoPP::AutoSeededRandomPool SeededRandomPool;
	byte IV[12];
	SeededRandomPool.GenerateBlock(IV, 12);

	byte DerivedKey[32];
	CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA384> KDF;
	KDF.DeriveKey(DerivedKey, 32, 0, (byte*)Key.c_str(), Key.size(), nullptr, 0, 10000);

	auto EncryptedData = EncryptAuthenticatedWithAlgo<CryptoPP::GCM<CryptoPP::AES>::Encryption>(L,
		std::string(Data.c_str(), Data.size()),
		std::string((const char*)DerivedKey, 32),
		std::string((const char*)IV, 12)
	);

	EncryptedData += "|" + Base64Encode(IV, 12);
	EncryptedData = Base64Encode((byte*)EncryptedData.data(), EncryptedData.size());


	lua_pushlstring(L, EncryptedData.c_str(), EncryptedData.size());
	lua_pushlstring(L, reinterpret_cast<char*>(IV), sizeof(IV));
	return 2;
}

__forceinline static int DecryptStringCustom(lua_State* L)
{
	luaL_stackcheck(L, 4, 4);
	std::string Algorithm = luaL_checklstring(L, 1, nullptr);
	std::string Data = luaL_checklstring(L, 2, nullptr);
	std::string KeySize = luaL_checklstring(L, 3, nullptr);
	std::string IvcStr = luaL_checklstring(L, 4, nullptr);

	std::transform(Algorithm.begin(), Algorithm.end(), Algorithm.begin(), tolower);

	if (!CryptTranslationMap.count(Algorithm)) 
	{
		luaL_argerror(L, 1, "non-existent algorithm");
		return 0;
	}

	auto SelectedAlgorithm = CryptTranslationMap[Algorithm];
	std::string EncryptedData;

	if (SelectedAlgorithm == AES_CBC)
		EncryptedData = DecryptWithAlgo<CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == AES_CFB) 
		EncryptedData = DecryptWithAlgo<CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == AES_CTR) 
		EncryptedData = DecryptWithAlgo<CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == AES_OFB) 
		EncryptedData = DecryptWithAlgo<CryptoPP::OFB_Mode<CryptoPP::AES>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == AES_GCM) 
		EncryptedData = DecryptAuthenticatedWithAlgo<CryptoPP::GCM<CryptoPP::AES>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == AES_EAX) 
		EncryptedData = DecryptAuthenticatedWithAlgo<CryptoPP::EAX<CryptoPP::AES>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == BF_CBC) 
		EncryptedData = EncryptWithAlgo<CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == BF_CFB)
		EncryptedData = DecryptWithAlgo<CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Encryption>(L, Data, KeySize, IvcStr);
	else if (SelectedAlgorithm == BF_OFB) 
		EncryptedData = DecryptWithAlgo<CryptoPP::OFB_Mode<CryptoPP::Blowfish>::Encryption>(L, Data, KeySize, IvcStr);
	else 
	{
		luaL_argerror(L, 1, "non-existent algorithm");
		return 0;
	}

	lua_pushlstring(L, EncryptedData.c_str(), EncryptedData.size());
	return 1;
};

__forceinline static int DecryptString(lua_State* L) 
{
	luaL_stackcheck(L, 2, 2);
	std::string Data = luaL_checklstring(L, 1, nullptr);
	std::string Key = luaL_checklstring(L, 2, nullptr);

	byte DerivedKey[32];
	CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA384> KDF;
	KDF.DeriveKey(DerivedKey, 32, 0, (byte*)Key.c_str(), Key.size(), nullptr, 0, 10000);

	std::vector<std::string> Split;
	SplitString(Base64Decode(Data), "|", Split);

	if (Split.size() != 2) 
	{
		luaL_argerror(L, 1, "Invalid encrypted string specified");
		return 0;
	}

	auto Decrypted = DecryptAuthenticatedWithAlgo<CryptoPP::GCM<CryptoPP::AES>::Decryption>(L,
		Split.at(0),
		std::string((char*)DerivedKey, 32),
		Base64Decode(Split.at(1))
	);

	lua_pushlstring(L, Decrypted.c_str(), Decrypted.size());
	return 1;
}

__forceinline static int HashString(lua_State* L) 
{
	luaL_stackcheck(L, 1, 1);
	std::string Data = luaL_checklstring(L, 1, nullptr);

	auto Hash = HashWithAlgorithm<CryptoPP::SHA384>(Data);
	lua_pushlstring(L, Hash.c_str(), Hash.size());
	return 1;
}

__forceinline static int HashStringCustom(lua_State* L)
{
	luaL_stackcheck(L, 2, 2);
	std::string Algorithm = luaL_checklstring(L, 1, nullptr);
	std::string Data = luaL_checklstring(L, 2, nullptr);

	std::transform(Algorithm.begin(), Algorithm.end(), Algorithm.begin(), tolower);

	if (!HashTranslationMap.count(Algorithm))
	{
		luaL_argerror(L, 1, "non-existent hash algorithm");
		return 0;
	}

	auto SelectedAlgorithm = HashTranslationMap[Algorithm];

	std::string EncryptedData;

	if (SelectedAlgorithm == MD5) 
		EncryptedData = HashWithAlgorithm<CryptoPP::Weak::MD5>(Data);
	else if (SelectedAlgorithm == SHA1) 
		EncryptedData = HashWithAlgorithm<CryptoPP::SHA1>(Data);
	else if (SelectedAlgorithm == SHA224) 
		EncryptedData = HashWithAlgorithm<CryptoPP::SHA224>(Data);
	else if (SelectedAlgorithm == SHA256) 
		EncryptedData = HashWithAlgorithm<CryptoPP::SHA256>(Data);
	else if (SelectedAlgorithm == SHA384) 
		EncryptedData = HashWithAlgorithm<CryptoPP::SHA384>(Data);
	else if (SelectedAlgorithm == SHA512) 
		EncryptedData = HashWithAlgorithm<CryptoPP::SHA512>(Data);
	else if (SelectedAlgorithm == SHA3_256)
		EncryptedData = HashWithAlgorithm<CryptoPP::SHA3_256>(Data);
	else if (SelectedAlgorithm == SHA3_384) 
		EncryptedData = HashWithAlgorithm<CryptoPP::SHA3_384>(Data);
	else if (SelectedAlgorithm == SHA3_512) 
		EncryptedData = HashWithAlgorithm<CryptoPP::SHA3_512>(Data);
	else 
	{
		luaL_argerror(L, 1, "non-existent hash algorithm");
		return 0;
	}

	lua_pushlstring(L, EncryptedData.c_str(), EncryptedData.size());
	return 1;
}

__forceinline static int RandomString(lua_State* L) 
{
	luaL_stackcheck(L, 1, 1);

	auto Size = luaL_checkinteger(L, 1);
	if (Size > 1024)
	{
		luaL_argerror(L, 1, "exceeded maximum size (1024)");
		return 0;
	}

	if (Size < 0)
	{
		luaL_argerror(L, 1, "negative size specified");
		return 0;
	}

	CryptoPP::AutoSeededRandomPool SeededRandomPool;
	auto Alloc = (byte*)operator new(Size);
	SeededRandomPool.GenerateBlock(Alloc, Size);

	lua_pushlstring(L, (char*)Alloc, Size);

	delete Alloc;
	return 1;
}

__forceinline static int DeriveString(lua_State* L) 
{
	luaL_stackcheck(L, 2, 2);

	std::string Data = luaL_checklstring(L, 1, nullptr);
	auto Size = luaL_checkinteger(L, 2);

	if (Size > 1024)
	{
		luaL_argerror(L, 1, "exceeded maximum size (1024)");
		return 0;
	}

	if (Size < 0)
	{
		luaL_argerror(L, 1, "negative size specified");
		return 0;
	}

	auto Alloc = (byte*)operator new(Size);
	CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA384> KDF;
	KDF.DeriveKey(Alloc, Size, 0, (byte*)Data.c_str(), Data.size(), nullptr, 0, 10000);

	lua_pushlstring(L, (char*)Alloc, Size);

	delete Alloc;
	return 1;
}

__forceinline static std::string RandomString(int Len) 
{
	static const char* Characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZbcdefghijklmnopqrstuvwxyz";

	std::string Ret;
	Ret.reserve(Len);

	for (auto i = 0; i < Len; ++i)
		Ret += Characters[rand() % (strlen(Characters) - 1)];

	return Ret;
}

__forceinline static int GenerateBytes(lua_State* L) 
{
	luaL_stackcheck(L, 1, 1);

	auto Len = luaL_checkinteger(L, 1);
	if (Len > 1024)
	{
		luaL_argerror(L, 1, "exceeded maximum size (1024)");
		return 0;
	}

	if (Len < 0)
	{
		luaL_argerror(L, 1, "negative size specified");
		return 0;
	}

	std::string Data = RandomString(Len);
	auto Encoded = Base64Encode((unsigned char*)Data.c_str(), Data.size());
	lua_pushlstring(L, Encoded.c_str(), Encoded.size());

	return 1;
}

__forceinline static int GenerateKey(lua_State* L)
{
	luaL_stackcheck(L, 0, 0);

	std::string Data = RandomString(32);
	auto Encoded = Base64Encode((unsigned char*)Data.c_str(), Data.size());
	lua_pushlstring(L, Encoded.c_str(), Encoded.size());

	return 1;
}

__forceinline static int Base64Encode(lua_State* L)
{
	luaL_stackcheck(L, 1, 1);

	std::string Data = luaL_checklstring(L, 1, nullptr);
	auto Encoded = Base64Encode((unsigned char*)Data.c_str(), Data.size());
	lua_pushlstring(L, Encoded.c_str(), Encoded.size());

	return 1;
}

__forceinline static int Base64Decode(lua_State* L) 
{
	luaL_stackcheck(L, 1, 1);

	auto Data = luaL_checklstring(L, 1, nullptr);
	auto Decoded = Base64Decode(Data);

	lua_pushlstring(L, Decoded.c_str(), Decoded.size());

	return 1;
}

static luaL_Reg LibraryFunctions[] = {
	{"encrypt", EncryptString},

	{"decrypt", DecryptString},

	{"hash", HashString},

	{"random", RandomString},

	{"derive", DeriveString},

	{"generatebytes", GenerateBytes},

	{"generatekey", GenerateKey},

	{"base64encode", Base64Encode},

	{"base64decode", Base64Decode},

	{"base64_encode", Base64Encode},

	{"base64_decode", Base64Decode},

	{nullptr, nullptr},
};

static luaL_Reg Base64Library[] = {
	{"encode", Base64Encode},

	{"decode", Base64Decode},

	{nullptr, nullptr},
};

#define ADD_TO_ENVIRONMENT(L, fn, fn_name) lua_pushcclosure(L, fn, fn_name, 0); \
lua_setfield(L, -2, fn_name);

void Crypt::Initialize(lua_State* L)
{
	lua_createtable(L, 0, 0);
	lua_setglobal(L, "base64");

	luaL_register(L, "base64", Base64Library);

	lua_getglobal(L, "base64");
	lua_setreadonly(L, -1, true);
	lua_settop(L, 0);

	lua_createtable(L, 0, 0);
	lua_setglobal(L, "crypt");

	luaL_register(L, "crypt", LibraryFunctions);
	lua_getglobal(L, "crypt");

	lua_createtable(L, 0, 0);
	ADD_TO_ENVIRONMENT(L, EncryptStringCustom, "encrypt");
	ADD_TO_ENVIRONMENT(L, DecryptStringCustom, "decrypt");
	ADD_TO_ENVIRONMENT(L, HashStringCustom, "hash");

	lua_setreadonly(L, -1, true);
	lua_setfield(L, -2, "custom");

	lua_getglobal(L, "base64");
	lua_setfield(L, -2, "base64");
	lua_setreadonly(L, -1, true);

	lua_pushvalue(L, LUA_GLOBALSINDEX);
	ADD_TO_ENVIRONMENT(L, Base64Encode, "base64encode");
	ADD_TO_ENVIRONMENT(L, Base64Decode, "base64decode");
	ADD_TO_ENVIRONMENT(L, Base64Encode, "base64_encode");
	ADD_TO_ENVIRONMENT(L, Base64Decode, "base64_decode");

	lua_settop(L, 0);
}