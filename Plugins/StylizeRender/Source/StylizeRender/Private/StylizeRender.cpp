#include "StylizeRender.h"

#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogStylizeRender);

#define LOCTEXT_NAMESPACE "FStylizeRenderModule"

void FStylizeRenderModule::StartupModule()
{
}

void FStylizeRenderModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FStylizeRenderModule, StylizeRender)
