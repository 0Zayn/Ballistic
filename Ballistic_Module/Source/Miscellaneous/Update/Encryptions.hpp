/* Updated for version-b591875ddfbc4294 */

#pragma once
#include "../EncryptionManager.hpp"

#define PROTO_MEMBER1_ENC VMValue4
#define PROTO_MEMBER2_ENC VMValue1 
#define PROTO_DEBUGISN_ENC VMValue1
#define PROTO_TYPEINFO_ENC VMValue3
#define PROTO_DEBUGNAME_ENC VMValue4

#define LSTATE_STACKSIZE_ENC VMValue3
#define LSTATE_GLOBAL_ENC VMValue3

#define CLOSURE_FUNC_ENC VMValue4
#define CLOSURE_CONT_ENC VMValue2
#define CLOSURE_DEBUGNAME_ENC VMValue1

#define TABLE_MEMBER_ENC VMValue4
#define TABLE_META_ENC VMValue4

#define UDATA_META_ENC VMValue3

#define TSTRING_HASH_ENC VMValue1 
#define TSTRING_LEN_ENC VMValue3

#define GSTATE_TTNAME_ENC VMValue3
#define GSTATE_TMNAME_ENC VMValue3