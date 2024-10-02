#pragma once
#include "../Library.hpp"

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include "cryptopp/aes.h"
#include "cryptopp/rsa.h"
#include "cryptopp/gcm.h"
#include "cryptopp/eax.h"
#include "cryptopp/md2.h"
#include "cryptopp/md5.h"
#include "cryptopp/sha.h"
#include "cryptopp/sha3.h"
#include "cryptopp/osrng.h"
#include "cryptopp/hex.h"
#include "cryptopp/pssr.h"
#include "cryptopp/base64.h"
#include "cryptopp/modes.h"
#include "cryptopp/filters.h"
#include "cryptopp/serpent.h"
#include "cryptopp/pwdbased.h"

#include "cryptopp/blowfish.h"
#include "cryptopp/modes.h"

namespace Libraries
{
	class Crypt : public Library
	{
	public:
		void Initialize(lua_State*) override;
	};
}