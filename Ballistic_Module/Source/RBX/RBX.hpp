#pragma once
#include "DataModel/DataModel.hpp"
#include "Graphics/Graphics.hpp"
#include "IdentityManager/IdentityManager.hpp"
#include "LogReader/LogReader.hpp"
#include "TaskScheduler/TaskScheduler.hpp"
#include "Yielder/Yielder.hpp"

namespace RBX 
{
    namespace Concepts 
    {
        template<typename Derived, typename Base>
        concept _TypeConstraint = std::is_base_of_v<Base, Derived>;
    }

    namespace Security 
    {
        enum Permissions : uint32_t 
        {
            None = 0x0,
            Plugin = 0x1,
            LocalUser = 0x3,
            WritePlayer = 0x4,
            RobloxScript = 0x5,
            RobloxEngine = 0x6,
            NotAccessible = 0x7,
            TestLocalUser = 0x3
        };
    }

    namespace Reflection
    {
        enum ReflectionType : uint32_t
        {
            ReflectionType_Void = 0x0,
            ReflectionType_Bool = 0x1,
            ReflectionType_Int = 0x2,
            ReflectionType_Int64 = 0x3,
            ReflectionType_Float = 0x4,
            ReflectionType_Double = 0x5,
            ReflectionType_String = 0x6,
            ReflectionType_ProtectedString = 0x7,
            ReflectionType_Instance = 0x8,
            ReflectionType_Instances = 0x9,
            ReflectionType_Ray = 0xa,
            ReflectionType_Vector2 = 0xb,
            ReflectionType_Vector3 = 0xc,
            ReflectionType_Vector2Int16 = 0xd,
            ReflectionType_Vector3Int16 = 0xe,
            ReflectionType_Rect2d = 0xf,
            ReflectionType_CoordinateFrame = 0x10,
            ReflectionType_Color3 = 0x11,
            ReflectionType_Color3uint8 = 0x12,
            ReflectionType_UDim = 0x13,
            ReflectionType_UDim2 = 0x14,
            ReflectionType_Faces = 0x15,
            ReflectionType_Axes = 0x16,
            ReflectionType_Region3 = 0x17,
            ReflectionType_Region3Int16 = 0x18,
            ReflectionType_CellId = 0x19,
            ReflectionType_GuidData = 0x1a,
            ReflectionType_PhysicalProperties = 0x1b,
            ReflectionType_BrickColor = 0x1c,
            ReflectionType_SystemAddress = 0x1d,
            ReflectionType_BinaryString = 0x1e,
            ReflectionType_Surface = 0x1f,
            ReflectionType_Enum = 0x20,
            ReflectionType_Property = 0x21,
            ReflectionType_Tuple = 0x22,
            ReflectionType_ValueArray = 0x23,
            ReflectionType_ValueTable = 0x24,
            ReflectionType_ValueMap = 0x25,
            ReflectionType_Variant = 0x26,
            ReflectionType_GenericFunction = 0x27,
            ReflectionType_WeakFunctionRef = 0x28,
            ReflectionType_ColorSequence = 0x29,
            ReflectionType_ColorSequenceKeypoint = 0x2a,
            ReflectionType_NumberRange = 0x2b,
            ReflectionType_NumberSequence = 0x2c,
            ReflectionType_NumberSequenceKeypoint = 0x2d,
            ReflectionType_InputObject = 0x2e,
            ReflectionType_Connection = 0x2f,
            ReflectionType_ContentId = 0x30,
            ReflectionType_DescribedBase = 0x31,
            ReflectionType_RefType = 0x32,
            ReflectionType_QFont = 0x33,
            ReflectionType_QDir = 0x34,
            ReflectionType_EventInstance = 0x35,
            ReflectionType_TweenInfo = 0x36,
            ReflectionType_DockWidgetPluginGuiInfo = 0x37,
            ReflectionType_PluginDrag = 0x38,
            ReflectionType_Random = 0x39,
            ReflectionType_PathWaypoint = 0x3a,
            ReflectionType_FloatCurveKey = 0x3b,
            ReflectionType_RotationCurveKey = 0x3c,
            ReflectionType_SharedString = 0x3d,
            ReflectionType_DateTime = 0x3e,
            ReflectionType_RaycastParams = 0x3f,
            ReflectionType_RaycastResult = 0x40,
            ReflectionType_OverlapParams = 0x41,
            ReflectionType_LazyTable = 0x42,
            ReflectionType_DebugTable = 0x43,
            ReflectionType_CatalogSearchParams = 0x44,
            ReflectionType_OptionalCoordinateFrame = 0x45,
            ReflectionType_CSGPropertyData = 0x46,
            ReflectionType_UniqueId = 0x47,
            ReflectionType_Font = 0x48,
            ReflectionType_Blackboard = 0x49,
            ReflectionType_Max = 0x4a
        };

        struct ClassDescriptor;
        struct Descriptor 
        {
            enum ThreadSafety : uint32_t { Unset = 0x0, Unsafe = 0x1, ReadSafe = 0x3, LocalSafe = 0x7, Safe = 0xf };
            struct Attributes 
            {
                bool isDeprecated;
                class RBX::Reflection::Descriptor* preferred;
                enum RBX::Reflection::Descriptor::ThreadSafety threadSafety;
            };
            void* vftable;
            std::string& name;
            struct RBX::Reflection::Descriptor::Attributes attributes;
        };

        struct Type : RBX::Reflection::Descriptor 
        {
            std::string& tag;
            enum RBX::Reflection::ReflectionType reflectionType;
            bool isFloat;
            bool isNumber;
            bool isEnum;
            bool isOptional;
        };

        struct EnumDescriptor : RBX::Reflection::Type 
        {
            std::vector<void*> allItems;
            std::uint64_t enumCount;
            const char _0[0x60];
        };

        struct MemberDescriptor : RBX::Reflection::Descriptor 
        {
            std::string& category;
            class RBX::Reflection::ClassDescriptor* owner;
            enum RBX::Security::Permissions permissions;
            int32_t _0;
        };

        struct EventDescriptor : RBX::Reflection::MemberDescriptor {};

        struct PropertyDescriptorVFT {};

        struct PropertyDescriptor : RBX::Reflection::MemberDescriptor
        {
        public:
            union {
                uint32_t bIsEditable;
                uint32_t bCanReplicate;
                uint32_t bCanXmlRead;
                uint32_t bCanXmlWrite;
                uint32_t bAlwaysClone;
                uint32_t bIsScriptable;
                uint32_t bIsPublic;
            } __bitfield;
            RBX::Reflection::Type* type;
            bool bIsEnum;
            RBX::Security::Permissions scriptWriteAccess;

            bool IsScriptable() { return (this->__bitfield.bIsScriptable > 32) & 1; }

            void SetScriptable(const uint8_t bIsScriptable) 
            {
                this->__bitfield.bIsScriptable = (this->__bitfield.bIsScriptable) ^ (bIsScriptable ? 33 : 32);
            }

            bool IsEditable() { return ((this->__bitfield.bIsEditable) & 1); }

            void SetEditable(const uint8_t bIsEditable) 
            {
                this->__bitfield.bIsEditable = (this->__bitfield.bIsEditable) ^ ((~bIsEditable & 0xFF));
            }

            bool IsCanXmlRead() { return ((this->__bitfield.bCanXmlRead >> 3) & 1); }
            void SetCanXmlRead(const uint8_t bCanXmlRead) 
            {
                this->__bitfield.bCanXmlRead = (this->__bitfield.bCanXmlRead) ^ ((~bCanXmlRead & 0xFF << 3));
            }

            bool IsCanXmlWrite() { return ((this->__bitfield.bCanXmlWrite >> 4) & 1); }
            void SetCanXmlWrite(const uint8_t bCanXmlWrite) 
            {
                this->__bitfield.bCanXmlWrite = (this->__bitfield.bCanXmlWrite) ^ ((~bCanXmlWrite & 0xFF << 4));
            }

            bool IsPublic() { return ((this->__bitfield.bIsPublic >> 6) & 1); }
            void SetIsPublic(const uint8_t bIsPublic) 
            {
                this->__bitfield.bIsPublic =
                    (this->__bitfield.bIsPublic) ^ static_cast<uint32_t>(~bIsPublic & 0xFF << 6);
            }

            bool IsCanReplicate() { return ((this->__bitfield.bCanReplicate >> 2) & 1); }
            void SetCanReplicate(const uint8_t bCanReplicate) 
            {
                this->__bitfield.bCanReplicate = (this->__bitfield.bCanReplicate) ^ ((~bCanReplicate & 0xFF << 2));
            }

            bool IsAlwaysClone() { return ((this->__bitfield.bAlwaysClone) & 1); }
            void SetAlwaysClone(const uint8_t bAlwaysClone) 
            {
                this->__bitfield.bAlwaysClone = (this->__bitfield.bAlwaysClone) ^ (~bAlwaysClone & 0xFF);
            }

            PropertyDescriptorVFT* GetVFT() { return static_cast<PropertyDescriptorVFT*>(this->vftable); }
        };

        struct EnumPropertyDescriptor : RBX::Reflection::PropertyDescriptor 
        {
            RBX::Reflection::EnumDescriptor* enumDescriptor;
        };

        template<Concepts::_TypeConstraint<RBX::Reflection::MemberDescriptor> U>
        struct MemberDescriptorContainer
        {
            std::vector<U*> descriptors;
            const char _0[144];
        };

        struct ClassDescriptor : RBX::Reflection::Descriptor 
        {
            MemberDescriptorContainer<RBX::Reflection::PropertyDescriptor> propertyDescriptors;
            MemberDescriptorContainer<RBX::Reflection::EventDescriptor> eventDescriptors;
            void* boundFunctionDescription_Start;
            char _180[0x40];
            char _1c0[0x40];
            char _200[0x20];
            void* boundYieldFunctionDescription_Start;
            char _228[0x18];
            char _240[0x40];
            char _280[0x40];
            char _2c0[8];
            char _2c8[0x38];
            char _300[0x40];
            char _340[0x30];
            char _370[8];
            char _378[8];
            char _380[4];
            char _384[2];
            char _386[2];
            char _388[8];
            struct RBX::Reflection::ClassDescriptor* baseClass;
            char _398[8];
            char _3a0[8];
            char _3a8[0x18];
            char _3c0[0x20];
        };
    }

    struct SystemAddress 
    {
        struct PeerId 
        {
            uint32_t peerId;
        };

        PeerId remoteId;
    };

    namespace Lua 
    {
        struct LiveThreadRef
        {
            int unk_0;
            lua_State* Thread;
            int ThreadId;
        };

        struct WeakThreadRef 
        {
            std::atomic_int32_t _Refs;
            lua_State* thread;
            int thread_ref;
            int objectId;
        };
    }

    namespace Console 
    {
        enum MessageType : int 
        {
            Standard = 0,
            InformationBlue = 1,
            Warning = 2,
            Error = 3,
        };
    }

    namespace Luau 
    {
        enum TaskState : int8_t 
        {
            None = 0,
            Deferred = 1,
            Delayed = 2,
            Waiting = 3,
        };
    }

    enum DataModelType : int
    {
        DataModelType_Edit = 0,
        DataModelType_PlayClient = 1,
        DataModelType_PlayServer = 2,
        DataModelType_MainMenuStandalone = 3,
        DataModelType_Null = 4
    };

    static const char* GetStringFromSharedString(void* sharedString)
    {
        auto sharedDeref = *reinterpret_cast<void**>(sharedString);
        return reinterpret_cast<const char*>(reinterpret_cast<std::uintptr_t>(sharedDeref) + 0x10);
    }

    struct DataModel
    {
        void* vftable;
        char _8[0x28];
        std::string m_szstdstringJobId;
        char _50[0x10];
        uint64_t m_qwCreatorId;
        uint64_t m_qwGameId;
        uint64_t m_qwPlaceId;
        char _78[8];
        char _80[8];
        uint32_t m_dwPlaceVersion;
        char _90[0x30];
        char _c0[0x40];
        char _100[0x40];
        char _140[0x40];
        char _180[0x10];
        char _190[8];
        void* m_pScriptContext;
        char _1a0[0x20];
        char _1c0[0x40];
        char _200[0x40];
        char _240[0x38];
        char _278[8];
        char _280[0x18];
        char _298[8];
        uint64_t m_qwPrivateServerId;
        char _2a8[0x18];
        char _2c0[0x20];
        char _2e0[0x20];
        int64_t m_dwPlaceId;
        uint64_t m_qwPrivateServerOwnerId;
        char _310[0x30];
        char _340[0x40];
        char _380[8];
        char _388[0x38];
        char _3c0[0x40];
        char _400[8];
        bool m_bIsParallelPhase;
        char _40c[0x34];
        char _440[0x28];
        enum RBX::DataModelType m_dwDataModelType;
        char _46c[0x14];
        char _480[0x40];
        char _4c0[0x40];
        char _500[0x40];
        char _540[9];
        char _549[1];
        char _54a[1];
        bool m_bIsClosed;
        bool m_bIsContentLoaded;
        char _54d[0x33];
        char _580[0x40];
        char _5c0[0x28];
        char _5e8[0x10];
        bool m_bIsLoaded;
        char _5f9[2];
        bool m_bIsUniverseMetadataLoaded;
        char _5fc[4];
        char _600[0x40];
        char _640[0x40];
        char _680[0x40];
        char _6c0[0x40];
        char _700[0x40];
        char _740[0x40];
        char _780[0x40];
        char _7c0[0x40];
        char _800[0x40];
        char _840[0x40];
        char _880[0x40];
        char _8c0[0x40];
        char _900[0x28];
        char _928[0x18];
        char _940[5];
        bool m_bIsBeingClosed;
        int64_t m_qwGameLaunchIntent;
        char _950[0x28];
    };

    struct ModuleScript
    {
        char _0[8];
        char _1[8];
        char _8[0x38];
        char _40[0x40];
        char _80[0x40];
        char _c0[0x40];
        char _100[0x40];
        char _140[0x40];
        char _180[0x18];
        bool m_bIsRobloxScriptModule;
    };

    struct Instance
    {
        void* vftable;
        std::weak_ptr<RBX::Instance> self;
        RBX::Reflection::ClassDescriptor* classDescriptor;
    };
}