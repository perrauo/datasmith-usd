// DatasmithUsdPlugin.h
#ifndef DATASMITHUSD_H
#define DATASMITHUSD_H

#include "CoreMinimal.h"

#include "DirectLinkEndpoint.h" // IEndpointObserver
#include "DatasmithMesh.h"

#include "HAL/PlatformTime.h"
#include "HAL/CriticalSection.h"
#include "Containers/Array.h"
#include "Containers/Map.h"

#include "USDIncludesStart.h"
#include "pxr/usd/usd/common.h"
#include "USDIncludesEnd.h"

#include <string>

class FEngineLoop;

namespace DirectLink
{
	struct FRawInfo;
}
class FDatasmithDirectLink;
class IDatasmithScene;
class FDatasmithSceneExporter;

namespace DatasmithUsd
{

	class FEndpointObserver : public DirectLink::IEndpointObserver
	{
	public:

		TArray<FString> ConnectionStatusList;

		FCriticalSection ConnectionStatusListCriticalSection;

		TArray<FString> GetConnectionStatus();

		FORCENOINLINE void OnStateChanged(const DirectLink::FRawInfo& RawInfo) override;
	};

	class FModuleContext {
	public:
		std::string UnicastEndpoint;
		std::string MulticastEndpoint;
		pxr::UsdStageRefPtr Stage;
	};

	class FModule {
	public:
		FModule();
		~FModule();
		void EnableDirectLink(const FModuleContext& InContext);
		void DisableDirectLink();
		void AddStage(const FModuleContext& InContext);
		void Sync(const FModuleContext& InContext);

		TSharedPtr<FDatasmithDirectLink> DirectLink;
		TSharedPtr<IDatasmithScene> DatasmithScene;
		TSharedPtr<FDatasmithSceneExporter> SceneExporter;
		TSharedPtr<FEndpointObserver> StateObserver;
		pxr::UsdStageRefPtr Stage;
		FString UnicastEndpoint = "";
		FString MulticastEndpoint = "";
	};
}

#endif // DATASMITHUSDPLUGIN_H
