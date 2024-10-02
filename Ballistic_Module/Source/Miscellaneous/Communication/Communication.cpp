#include "Communication.hpp"

std::string CurrentScript;

__forceinline void HandleExecutionRequest(const httplib::Request& Request, httplib::Response& Response)
{
    try
    {
        CurrentScript = nlohmann::json::parse(Request.body).at(xorstr_("Script")).get<std::string>();
        Scheduler->AddScript(CurrentScript);
        CurrentScript.clear();

        Response.status = 200;
        Response.set_content(xorstr_("Script executed successfully"), xorstr_("text/plain"));
    }
    catch (...)
    {
        Response.status = 400;
        Response.set_content(xorstr_("Failed to process request"), xorstr_("text/plain"));
    }
}

void CCommunication::Initialize()
{
    cpr::Session Session;
    Session.SetUrl(std::format("http://localhost:1337/{}/Scheduled", Configuration::Name));

    std::thread([&]() -> void {
        this->Server.Post(std::format("/{}/Scheduled", Configuration::Name), HandleExecutionRequest);
        this->Server.listen(xorstr_("localhost"), 1337);
    }).detach();
}