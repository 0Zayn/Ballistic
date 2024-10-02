#include "RenderView.hpp"

using namespace RBX::Graphics;

uintptr_t CRenderView::Get()
{
    const auto LatestLog = LogReader->GetLogInfo();
    if (LatestLog.empty())
        return 0;

    const auto RelevantLine = LogReader->ReadFile(LatestLog);
    if (RelevantLine.empty())
        return 0;

    return LogReader->ExtractPointer(RelevantLine);
}