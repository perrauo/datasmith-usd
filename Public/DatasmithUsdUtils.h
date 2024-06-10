// DatasmithUsdPlugin.h
#ifndef DATASMITHUSD_UTILS_H
#define DATASMITHUSD_UTILS_H

#include "CoreMinimal.h"

#include "DatasmithMesh.h"

#include "USDIncludesStart.h"
#include "pxr/usd/usd/common.h"
#include "pxr/usd/usd/prim.h"
#include "USDIncludesEnd.h"

#include <Containers/Map.h>

#include <string>

PXR_NAMESPACE_OPEN_SCOPE
class GfVec3f;
class GfMatrix4d;
class UsdGeomXformable;
PXR_NAMESPACE_CLOSE_SCOPE

class IDatasmithMeshElement;

namespace DatasmithUsd
{
	class FModule;

	struct FUsdPrimConversionContext
	{
		// Inputs
		FModule* Plugin = nullptr;
		FDatasmithMesh RenderMesh;
		int32 SelectedLightmapUVChannel = -1;
		bool bHasCollision = false;
		pxr::UsdPrim Prim;

		// Outputs

		TSharedPtr<IDatasmithMeshElement> DatasmithMeshElement;
		TSet<uint16> SupportedChannels;
		TMap<int32, int32> UVChannelsMap;
	};

	// todo: paralelize calls to ExportToUObject 
	//bool ConvertUsdPrimToDatasmith(FUsdPrimConversionContext& InContext);

	// USD uses GfVec3f for points and vectors
	FVector ToDatasmithVector(const pxr::GfVec3f& Point);

	FVector ToDatasmithNormal(const pxr::GfVec3f& Normal);

	FColor ToDatasmithColor(const pxr::GfVec3f& Color);

	void UsdToUnrealCoordinates(
		const pxr::GfMatrix4d& Matrix
		, FVector& Translation
		, FQuat& Rotation
		, FVector& Scale
	);

	pxr::GfMatrix4d GetTransformFromUsdPrim(const pxr::UsdPrim& prim);


	void ConvertUsdGeomXformableToTransform(const pxr::UsdGeomXformable& Xformable, FVector& OutTranslation, FQuat& OutRotation, FVector& OutScale);

	void FillDatasmithMeshFromUsdPrim(const pxr::UsdPrim& InUsdMeshPrim, FDatasmithMesh& OutDatasmithMesh);
}

#endif