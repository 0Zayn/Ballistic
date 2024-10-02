#include "Communication.hpp"

void CCommunication::ExecuteScript(const std::string& Script)
{
    nlohmann::json JsonBody;
    JsonBody["Script"] = Script;

    const auto Response = cpr::Post(
        cpr::Url{ std::format("http://localhost:1337/{}/Scheduled", Configuration::Name) },
        cpr::Body{ JsonBody.dump() },
        cpr::Header{ { "Content-Type", "application/json" } }
    );
}