// DatasmithUsdPlugin.h
#ifndef DATASMITHUSD_UTILS_H
#define DATASMITHUSD_UTILS_H

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
#include "pxr/usd/usd/prim.h"
#include "USDIncludesEnd.h"

#include <Containers/Map.h>
#include <Containers/Set.h>

#include <string>

PXR_NAMESPACE_OPEN_SCOPE

struct FUsdPrimConversionContext
{
	// Inputs

	FDatasmithMesh RenderMesh;
	int32 SelectedLightmapUVChannel = -1;
	bool bHasCollision = false;
	UsdPrim Prim;

	// Outputs

	TSharedPtr<IDatasmithMeshElement> DatasmithMeshElement;
	TSet<uint16> SupportedChannels;
	TMap<int32, int32> UVChannelsMap;
};

// todo: paralelize calls to ExportToUObject 
bool ConvertUsdPrimToDatasmith(FUsdPrimConversionContext& InContext);

PXR_NAMESPACE_CLOSE_SCOPE

#endif