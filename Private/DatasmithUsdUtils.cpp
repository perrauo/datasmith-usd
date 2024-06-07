#include "DatasmithUsdUtils.h"

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
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/base/gf/matrix4f.h>
#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/gf/vec4d.h>
#include <pxr/base/gf/vec3d.h>
#include <pxr/base/gf/vec4f.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/rotation.h>
#include <pxr/base/gf/quaternion.h>
#include <pxr/base/gf/transform.h>
#include "USDIncludesEnd.h"

PXR_NAMESPACE_OPEN_SCOPE

static const float UnitToCentimeter = 1;

// USD uses GfVec3f for points and vectors
static FVector ToDatasmithVector(const GfVec3f& Point)
{
	// Flip Y and Z axes and convert to centimeters
	return FVector(UnitToCentimeter * Point[0],
		UnitToCentimeter * Point[2],
		UnitToCentimeter * -Point[1]);
}

static FVector ToDatasmithNormal(const GfVec3f& Normal)
{
	// Flip Y and Z axes for normals
	return FVector(Normal[0], Normal[2], -Normal[1]);
}

static FColor ToDatasmithColor(const GfVec3f& Color)
{
	// Assuming the USD color is also encoded from 0 to 1
	return FColor(Color[0] * 255, Color[2] * 255, -Color[1] * 255);
}

static void UsdToUnrealCoordinates(
const GfMatrix4d& Matrix
, FVector& Translation
, FQuat& Rotation
, FVector& Scale
)
{
	// Extract translation
	GfVec3d Trans = Matrix.ExtractTranslation();
	Translation.X = Trans[0] * UnitToCentimeter;
	Translation.Y = Trans[2] * UnitToCentimeter;
	Translation.Z = -Trans[1] * UnitToCentimeter;

	// Extract rotation
	GfRotation Rot = GfMatrix4d(Matrix).ExtractRotation();
	GfQuaternion Quat = Rot.GetQuaternion();
	Rotation = FQuat(Quat.GetReal(), Quat.GetImaginary()[0], Quat.GetImaginary()[2], -Quat.GetImaginary()[1]);

	// Manually extract scale
	GfVec4d RowX = Matrix.GetRow(0).GetNormalized();
	GfVec4d RowY = Matrix.GetRow(1).GetNormalized();
	GfVec4d RowZ = Matrix.GetRow(2).GetNormalized();
	Scale.X = RowX.GetLength();
	Scale.Y = RowZ.GetLength();
	Scale.Z = -RowY.GetLength();
}


static GfMatrix4d GetTransformFromUsdPrim(const UsdPrim& prim) {
	UsdGeomXformable xformable(prim);
	bool resetXformStack = false;
	GfMatrix4d localXform;

	// Check if the prim is transformable
	if (xformable) {
		// Get the local transformation matrix at the default time
		if (xformable.GetLocalTransformation(&localXform, &resetXformStack, UsdTimeCode::Default())) {
			// Convert the matrix to a GfTransform and return it
			return localXform;
		}
	}

	// Return an identity transform if the prim is not transformable or if there's no transform
	return GfMatrix4d();
}

void _FillDatasmithMeshFromUsdPrim(
const UsdPrim& InUsdMeshPrim
//, TSet<uint16>& SupportedChannels
//, TMap<int32, int32>& UVChannelsMap
, FDatasmithMesh& OutDatasmithMesh
)
{
	// Retrieve mesh data from UsdPrim
	UsdGeomMesh UsdMesh(InUsdMeshPrim);
	VtArray<GfVec3f> Points;
	UsdMesh.GetPointsAttr().Get(&Points, UsdTimeCode::Default());
	VtArray<int> FaceVertexCounts;
	UsdMesh.GetFaceVertexCountsAttr().Get(&FaceVertexCounts, UsdTimeCode::Default());
	VtArray<int> FaceVertexIndices;
	UsdMesh.GetFaceVertexIndicesAttr().Get(&FaceVertexIndices, UsdTimeCode::Default());

	const int NumFaces = FaceVertexCounts.size();
	const int NumVerts = Points.size();

	OutDatasmithMesh.SetVerticesCount(NumVerts);
	OutDatasmithMesh.SetFacesCount(NumFaces);

	// Vertices
	for (int i = 0; i < NumVerts; i++)
	{
		GfVec3f Point = Points[i];
		GfMatrix4d Transform  = GetTransformFromUsdPrim(InUsdMeshPrim);
		Point = Transform.Transform(Point);
		FVector Vertex = ToDatasmithVector(Point);
		OutDatasmithMesh.SetVertex(i, Vertex.X, Vertex.Y, Vertex.Z);
	}

	// TODO: Add logic for Vertex Colors, UVs, and Normals similar to the original function
	// This will involve retrieving the corresponding USD attributes and converting them

	// Faces
	int FaceIndex = 0;
	for (int i = 0; i < NumFaces; i++)
	{
		int NumVertsInFace = FaceVertexCounts[i];
		// Assuming faces are triangles for simplicity, but this should be adapted to handle arbitrary polygon faces
		if (NumVertsInFace == 3)
		{
			//int MaterialId = bForceSingleMat ? 0 : /* logic to determine material ID from UsdPrim */;
			int MaterialId = 0;

			//SupportedChannels.Add(MaterialId);

			OutDatasmithMesh.SetFace(FaceIndex, FaceVertexIndices[i * 3], FaceVertexIndices[i * 3 + 1], FaceVertexIndices[i * 3 + 2], MaterialId + 1);
			// TODO: Set face smoothing mask based on USD normals
			FaceIndex++;
		}
	}

	// TODO: Handle normals, tangents, and other mesh attributes as needed
}


// todo: paralelize calls to ExportToUObject 
bool ConvertUsdPrimToDatasmith(FUsdPrimConversionContext& InContext)
{
	_FillDatasmithMeshFromUsdPrim(
		InContext.Prim
		, InContext.RenderMesh
	);
	return true;
}

PXR_NAMESPACE_CLOSE_SCOPE