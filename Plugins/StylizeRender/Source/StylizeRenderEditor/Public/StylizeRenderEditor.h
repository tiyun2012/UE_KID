#pragma once

#include "Modules/ModuleInterface.h"

class FStylizeRenderEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
