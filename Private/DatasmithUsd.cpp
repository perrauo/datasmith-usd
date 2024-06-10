// DatasmithUsdPlugin.cpp
#include "DatasmithUsd.h"
#include "DatasmithUsdUtils.h"

#include "Modules/ModuleManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Paths.h"
#include "Misc/CommandLine.h"
#include "DatasmithExporterManager.h"
#include "DatasmithExportOptions.h"
#include "DatasmithSceneExporter.h"
#include "DatasmithMesh.h"
#include "DatasmithMeshExporter.h"
#include "IDatasmithExporterUIModule.h"
#include "IDirectLinkUI.h"
#include "IDatasmithSceneElements.h"
#include "DatasmithSceneFactory.h"
#include "DatasmithDirectLink.h"
#include "HAL/PlatformTime.h"
#include "Logging/LogMacros.h"
#include "Async/Async.h"

#include "USDIncludesStart.h"
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/xformable.h>
#include <boost/python.hpp>
#include "USDIncludesEnd.h"

#include <iostream>
#include <string>
#include <map>

namespace DatasmithUsd
{
	TArray<FString> FEndpointObserver::GetConnectionStatus()
	{
		FScopeLock Lock(&ConnectionStatusListCriticalSection);
		return ConnectionStatusList;
	}

	void FEndpointObserver::OnStateChanged(const DirectLink::FRawInfo& RawInfo)
	{
		using namespace DirectLink;

		FScopeLock Lock(&ConnectionStatusListCriticalSection);
		ConnectionStatusList.Reset();

		for (const FRawInfo::FStreamInfo& StreamInfo : RawInfo.StreamsInfo)
		{
			if (StreamInfo.ConnectionState != EStreamConnectionState::Active)
			{
				continue;
			}

			const FRawInfo::FDataPointInfo* SourceDataPointInfo = RawInfo.DataPointsInfo.Find(StreamInfo.Source);
			if (!SourceDataPointInfo || !SourceDataPointInfo->bIsOnThisEndpoint)
			{
				continue;
			}

			const FRawInfo::FDataPointInfo* DestinationDataPointInfo = RawInfo.DataPointsInfo.Find(StreamInfo.Destination);

			if (!DestinationDataPointInfo)
			{
				continue;
			}


			const FRawInfo::FEndpointInfo* SourceEndpointInfo = RawInfo.EndpointsInfo.Find(SourceDataPointInfo->EndpointAddress);
			const FRawInfo::FEndpointInfo* DestinationEndpointInfo = RawInfo.EndpointsInfo.Find(DestinationDataPointInfo->EndpointAddress);

			ConnectionStatusList.Add(SourceDataPointInfo->Name);
			ConnectionStatusList.Add(SourceEndpointInfo->Name);
			ConnectionStatusList.Add(SourceEndpointInfo->UserName);
			ConnectionStatusList.Add(SourceEndpointInfo->ExecutableName);
			ConnectionStatusList.Add(SourceEndpointInfo->ComputerName);

			ConnectionStatusList.Add(DestinationEndpointInfo->Name);
			ConnectionStatusList.Add(DestinationDataPointInfo->Name);
			ConnectionStatusList.Add(DestinationEndpointInfo->Name);
			ConnectionStatusList.Add(DestinationEndpointInfo->UserName);
			ConnectionStatusList.Add(DestinationEndpointInfo->ExecutableName);
			ConnectionStatusList.Add(DestinationEndpointInfo->ComputerName);
		}
	}

	struct FVisitContext
	{
		FModule* Module = nullptr;
		pxr::UsdPrim Prim;
		TSharedPtr<IDatasmithActorElement> ParentElement = nullptr;
		TMap<FString, int32>* Names = nullptr;
	};

	static void _VisitPrim(const FVisitContext& InContext)
	{
		using namespace pxr;
		auto& Prim = InContext.Prim;
		auto& Module = InContext.Module;
		auto& Stage = Module->Stage;
		auto Names = InContext.Names;

		TSharedPtr<IDatasmithActorElement> ChildActorElement;
		FString ElementName(Prim.GetName().data());
		if (int32* Found = Names->Find(ElementName))
		{
			Names->Add(ElementName, *Found + 1);
			ElementName += FString::FromInt(*Found + 1);
		}
		else
		{
			Names->Add(ElementName, 1);
		}

		if (Prim.IsA<UsdGeomMesh>())
		{
			FDatasmithMesh RenderMesh;
			FillDatasmithMeshFromUsdPrim(Prim, RenderMesh);

			TSharedPtr<IDatasmithMeshElement> MeshElement = FDatasmithSceneFactory::CreateMesh(*ElementName);
			MeshElement->SetLabel(*ElementName);
			MeshElement->SetName(*ElementName);

			const TCHAR* AssetsOutputPath = Module->SceneExporter->GetAssetsOutputPath();
			FGCScopeGuard GCGuard;
			FDatasmithMeshExporter DatasmithMeshExporter;
			DatasmithMeshExporter.ExportToUObject(
				MeshElement
				, AssetsOutputPath
				, RenderMesh
				, nullptr // CollisionMesh
				, FDatasmithExportOptions::LightmapUV
			);

			TSharedPtr<IDatasmithMeshActorElement> MeshActorElement = FDatasmithSceneFactory::CreateMeshActor(*ElementName);
			MeshActorElement->SetLabel(*ElementName);
			MeshActorElement->SetName(*ElementName);
			ChildActorElement = MeshActorElement;

			Module->DatasmithScene->AddMesh(MeshElement);
		}
		else if (Prim.IsA<UsdGeomXformable>())
		{
			ChildActorElement = FDatasmithSceneFactory::CreateActor(*ElementName);
			ChildActorElement->SetLabel(*ElementName);
			ChildActorElement->SetName(*ElementName);
		}

		if (ChildActorElement)
		{
			pxr::UsdGeomXformable XForm(Prim);
			FVector Translation;
			FQuat Quat;
			FVector Scale;
			ConvertUsdGeomXformableToTransform(XForm, Translation, Quat, Scale);
			ChildActorElement->SetTranslation(Translation);
			ChildActorElement->SetRotation(Quat);
			ChildActorElement->SetScale(Scale);
			if (InContext.ParentElement != nullptr)
			{
				InContext.ParentElement->AddChild(ChildActorElement);
			}

			Module->DatasmithScene->AddActor(ChildActorElement);
		}

		for (const UsdPrim& ChildPrim : Prim.GetAllChildren())
		{
			if (ChildPrim == Prim)
				continue;
			if (ChildPrim == Stage->GetPseudoRoot())
				continue;						

			FVisitContext VisitContext;
			VisitContext.ParentElement = ChildActorElement;
			VisitContext.Module = InContext.Module;
			VisitContext.Prim = ChildPrim;
			VisitContext.Names = InContext.Names;

			_VisitPrim(VisitContext);			
		}
	}

	void FModule::AddStage(const FModuleContext& InContext)
	{
		if (auto AddedStage = InContext.Stage)
		{
			Stage = AddedStage;
			auto Prim = Stage->GetDefaultPrim();
			std::cout << Prim.GetName().data() << std::endl;

			FVisitContext VisitContext;
			VisitContext.Module = this;
			VisitContext.Prim = Prim;
			TMap<FString, int32> Names;
			VisitContext.Names = &Names;
			_VisitPrim(VisitContext);

			SceneExporter->Export(DatasmithScene.ToSharedRef(), true);

			DirectLink->UpdateScene(DatasmithScene.ToSharedRef());
		}
	}


	void FModule::EnableDirectLink(const FModuleContext& InContext)
	{
		if (!InContext.Stage)
		{
			std::cout << "Stage provided is null." << std::endl;
		}

		DatasmithScene = FDatasmithSceneFactory::CreateScene(TEXT(""));
		SceneExporter = MakeShared<FDatasmithSceneExporter>();

		FString Host;
		Host = TEXT("USD View ") + FString::FromInt(0) + TEXT(".") + FString::FromInt(0) + TEXT(".") + FString::FromInt(0);	
		DatasmithScene->SetProductName(TEXT("USD View"));
		DatasmithScene->SetHost(*(Host));
		FString Version = FString::FromInt(0) + TEXT(".") + FString::FromInt(0) + TEXT(".") + FString::FromInt(0);
		DatasmithScene->SetProductVersion(*Version);
		DatasmithScene->SetVendor(TEXT("Pixar"));

		FDatasmithExporterManager::FInitOptions Options;
		Options.bEnableMessaging = true;
		Options.bSuppressLogs = false;
		Options.bSaveLogToUserDir = false;

		FDatasmithExporterManager::Initialize(Options);
		SceneExporter->PreExport();

		Stage = InContext.Stage;
		// Force enable UDP messaging
		FString CmdString = FCommandLine::Get();
		if (!CmdString.Contains("-Messaging"))
		{
			CmdString += " -Messaging";
		}
		CmdString += " -UDPMESSAGING_TRANSPORT_ENABLE";
		if (InContext.UnicastEndpoint != "")
		{
			UnicastEndpoint = InContext.UnicastEndpoint.c_str();
			CmdString += FString::Format(
				TEXT(" -UDPMESSAGING_TRANSPORT_UNICAST={0}"),
				{
					UnicastEndpoint
				});
		}
		if (InContext.UnicastEndpoint != "")
		{
			MulticastEndpoint = InContext.MulticastEndpoint.c_str();
			CmdString += FString::Format(
				TEXT(" -UDPMESSAGING_TRANSPORT_MULTICAST={0}"),
				{
					MulticastEndpoint
				});
		}

		FCommandLine::Set(*CmdString);
		DirectLink = MakeShared<FDatasmithDirectLink>();

		FString StageName = Stage->GetRootLayer()->GetIdentifier().c_str();
		StageName.RemoveFromEnd(".usd");
		TArray<FString> Segments;
		StageName.ParseIntoArray(Segments, TEXT("/"), true);
		if (Segments.Num() > 0)
		{
			StageName = Segments.Last();
		}
		SceneExporter->SetName(*StageName);
		DatasmithScene->SetName(*StageName);
		DatasmithScene->SetLabel(*StageName);

		SceneExporter->SetOutputPath(TEXT("C:\\DLCacheFolder"));
		DatasmithScene->SetResourcePath(SceneExporter->GetOutputPath());

		DirectLink->ValidateCommunicationSetup();
		DirectLink->InitializeForScene(DatasmithScene.ToSharedRef());
		DirectLink->GetEnpoint()->AddEndpointObserver(StateObserver.Get());

		SceneExporter->Export(DatasmithScene.ToSharedRef(), true);
	}

	void FModule::DisableDirectLink()
	{
		if (SceneExporter)
		{
			FDatasmithExporterManager::Shutdown();

			if (DirectLink)
			{
				DirectLink->CloseCurrentSource();
				DirectLink.Reset();
			}

			DatasmithScene.Reset();
			SceneExporter.Reset();
		}
	}

	void FModule::Sync(const FModuleContext& InContext)
	{
		DirectLink->UpdateScene(DatasmithScene.ToSharedRef());
	}

	FModule::FModule()
	{
	}

	FModule::~FModule()
	{
		DisableDirectLink();
		SceneExporter.Reset();
	}

}


BOOST_PYTHON_MODULE(DatasmithUsd)
{
	using namespace boost::python;
	using namespace DatasmithUsd;
	using namespace pxr;
	class_<FModuleContext>("ModuleContext")
		.def(init<>())
		.def_readwrite("stage", &FModuleContext::Stage)
		.def_readwrite("unicastEndpoint", &FModuleContext::UnicastEndpoint)
		.def_readwrite("multicastEndpoint", &FModuleContext::MulticastEndpoint);

	class_<FModule>("Module")
		.def(init<>()) // Default constructor
		.def("enableDirectLink", &FModule::EnableDirectLink)
		.def("disableDirectLink", &FModule::DisableDirectLink)
		.def("addStage", &FModule::AddStage)
		.def("sync", &FModule::Sync);
}