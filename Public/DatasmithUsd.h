// DatasmithUsdPlugin.h
#ifndef DATASMITHUSD_H
#define DATASMITHUSD_H

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Paths.h"
#include "DatasmithExporterManager.h"
#include "DatasmithExportOptions.h"
#include "DatasmithSceneExporter.h"
#include "DatasmithMesh.h"
#include "DatasmithMeshExporter.h"
#include "IDatasmithExporterUIModule.h"
#include "IDirectLinkUI.h"
#include "DatasmithSceneFactory.h"
#include "DatasmithDirectLink.h"
#include "HAL/PlatformTime.h"
#include "Logging/LogMacros.h"
#include "Async/Async.h"

#include "USDIncludesStart.h"
#include "pxr/usd/usd/common.h"
#include "USDIncludesEnd.h"

#include <string>

PXR_NAMESPACE_OPEN_SCOPE

class FDatasmithUsd {
public:
    void EnableDirectLink();
	void DisableDirectLink();
	void AddStage(const UsdStageRefPtr& InStage);

	TSharedPtr<IDatasmithScene> DatasmithSceneRef;
	TSharedPtr<FDatasmithSceneExporter> SceneExporterRef;
};

PXR_NAMESPACE_CLOSE_SCOPE


#endif // DATASMITHUSDPLUGIN_H
