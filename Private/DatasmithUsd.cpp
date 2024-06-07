// DatasmithUsdPlugin.cpp
#include "DatasmithUsd.h"
#include "DatasmithUsdUtils.h"

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
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <boost/python.hpp>
#include "USDIncludesEnd.h"

#include <iostream>
#include <string>

PXR_NAMESPACE_OPEN_SCOPE

static void _VisitPrim(FDatasmithUsd* InApp, const UsdStageRefPtr& InStage, const UsdPrim& InPrim)
{
	for (const UsdPrim& ChildPrim : InPrim.GetAllChildren())
	{
		if (ChildPrim == InPrim)
			continue;
		if (ChildPrim == InStage->GetPseudoRoot())
			continue;	

		if (!ChildPrim.IsA<UsdGeomMesh>())
		{
			FUsdPrimConversionContext Context;
			Context.Prim = ChildPrim;
			ConvertUsdPrimToDatasmith(Context);

			InApp->DatasmithSceneRef->AddMesh(Context.DatasmithMeshElement);

			// todo: parallelize this
			FDatasmithMeshExporter DatasmithMeshExporter;
			DatasmithMeshExporter.ExportToUObject(
				Context.DatasmithMeshElement
				, InApp->SceneExporterRef->GetAssetsOutputPath()
				, Context.RenderMesh
				// TODO
				//, Context.CollisionMesh
				, nullptr
				, FDatasmithExportOptions::LightmapUV
			);
		}
		if (!ChildPrim.GetChildren().empty())
		{
			_VisitPrim(InApp, InStage, ChildPrim);
		}
	}
}

void FDatasmithUsd::AddStage(const UsdStageRefPtr & InStage)
{
	auto Prim = InStage->GetDefaultPrim();
	std::cout << Prim.GetName().data() << std::endl;


	FString Version = FString::FromInt(0) + TEXT(".") + FString::FromInt(0) + TEXT(".") + FString::FromInt(0);
	DatasmithSceneRef->SetProductVersion(*Version);

	_VisitPrim(this, InStage, InStage->GetDefaultPrim());
}

void FDatasmithUsd::EnableDirectLink() 
{	
	DatasmithSceneRef = FDatasmithSceneFactory::CreateScene(TEXT(""));
	SceneExporterRef = MakeShared<FDatasmithSceneExporter>();

	//MSTR Renderer;
	FString Host;
	Host = TEXT("USD View ") + FString::FromInt(0) + TEXT(".") + FString::FromInt(0) + TEXT(".") + FString::FromInt(0);
	//GetCOREInterface()->GetCurrentRenderer()->GetClassName(Renderer);

	DatasmithSceneRef->SetProductName(TEXT("USD View"));
	DatasmithSceneRef->SetHost(*(Host));

	// Set the vendor name of the application used to build the scene.
	DatasmithSceneRef->SetVendor(TEXT("Pixar"));
}

void FDatasmithUsd::DisableDirectLink()
{
	DatasmithSceneRef.Reset();
	SceneExporterRef.Reset();

}

PXR_NAMESPACE_CLOSE_SCOPE

BOOST_PYTHON_MODULE(DatasmithUsd)
{
	using namespace boost::python;
	class_<pxr::FDatasmithUsd>("DatasmithUsd")
		.def(init<>()) // Default constructor
		.def("enableDirectLink", &pxr::FDatasmithUsd::EnableDirectLink)
		.def("disableDirectLink", &pxr::FDatasmithUsd::DisableDirectLink)
		.def("addStage", &pxr::FDatasmithUsd::AddStage);
}