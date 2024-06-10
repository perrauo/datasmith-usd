#include "DatasmithUsdUtils.h"

#include "CoreMinimal.h"
#include "DatasmithMesh.h"
#include "DatasmithMeshExporter.h"
#include "DatasmithSceneFactory.h"

#include "USDIncludesStart.h"
#include <pxr/usd/usd/stage.h>
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
#include <pxr/usd/usdGeom/xformable.h>
#include <pxr/usd/usdGeom/xformOp.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/matrix4f.h>
#include <pxr/base/gf/quatf.h>
#include "USDIncludesEnd.h"

namespace DatasmithUsd
{

	static const float UnitToCentimeter = 1;

	// USD uses GfVec3f for points and vectors
	FVector ToDatasmithVector(const pxr::GfVec3f& Point)
	{
		// Flip Y and Z axes and convert to centimeters
		return FVector(UnitToCentimeter * Point[0],
			UnitToCentimeter * Point[2],
			UnitToCentimeter * -Point[1]);
	}

	FVector ToDatasmithNormal(const pxr::GfVec3f& Normal)
	{
		// Flip Y and Z axes for normals
		return FVector(Normal[0], Normal[2], -Normal[1]);
	}

	FColor ToDatasmithColor(const pxr::GfVec3f& Color)
	{
		// Assuming the USD color is also encoded from 0 to 1
		return FColor(Color[0] * 255, Color[2] * 255, -Color[1] * 255);
	}

	void UsdToUnrealCoordinates(
		const pxr::GfMatrix4d& Matrix
		, FVector& Translation
		, FQuat& Rotation
		, FVector& Scale
	)
	{
		using namespace pxr;
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


	pxr::GfMatrix4d GetTransformFromUsdPrim(const pxr::UsdPrim& prim) {

		using namespace pxr;

		pxr::UsdGeomXformable xformable(prim);
		bool resetXformStack = false;
		pxr::GfMatrix4d localXform;

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

	void FillDatasmithMeshFromUsdPrim(
		const pxr::UsdPrim& InUsdMeshPrim,
		FDatasmithMesh& OutDatasmithMesh
	)
	{
		using namespace pxr;
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

		// Vertices
		GfMatrix4d Transform = GetTransformFromUsdPrim(InUsdMeshPrim);
		for (int i = 0; i < NumVerts; i++)
		{
			GfVec3f Point = Points[i];
			Point = Transform.Transform(Point);
			FVector Vertex = ToDatasmithVector(Point);
			OutDatasmithMesh.SetVertex(i, Vertex.X, Vertex.Y, Vertex.Z);
		}

		// Faces
		// Datasmith only lets you specify tris, so let's do that..
		int FaceIndex = 0;
		int VertexIndex = 0;
		for (int i = 0; i < NumFaces; i++)
		{
			int NumVertsInFace = FaceVertexCounts[i];
			if (NumVertsInFace == 4)
			{
				int MaterialId = 0;

				if (VertexIndex + 3 < FaceVertexIndices.size())
				{
					OutDatasmithMesh.SetFace(FaceIndex, FaceVertexIndices[VertexIndex], FaceVertexIndices[VertexIndex + 1], FaceVertexIndices[VertexIndex + 2], MaterialId + 1);
					OutDatasmithMesh.SetFace(FaceIndex + 1, FaceVertexIndices[VertexIndex], FaceVertexIndices[VertexIndex + 2], FaceVertexIndices[VertexIndex + 3], MaterialId + 1);
					FaceIndex += 2;
				}
				VertexIndex += 4;
			}
			else if (NumVertsInFace == 3)
			{
				int MaterialId = 0;

				if (VertexIndex + 2 < FaceVertexIndices.size())
				{
					OutDatasmithMesh.SetFace(FaceIndex, FaceVertexIndices[VertexIndex], FaceVertexIndices[VertexIndex + 1], FaceVertexIndices[VertexIndex + 2], MaterialId + 1);
					FaceIndex++;
				}
				VertexIndex += 3;
			}
		}
		OutDatasmithMesh.SetFacesCount(FaceIndex);

		// TODO: Handle normals, tangents, and other mesh attributes as needed
	}


	void ConvertUsdGeomXformableToTransform(const pxr::UsdGeomXformable& Xformable, FVector& OutTranslation, FQuat& OutQuat, FVector& OutScale)
	{
		using namespace pxr;
		// Get the transformation matrix
		GfMatrix4d TransformMatrix;
		bool ResetXformStack;
		Xformable.GetLocalTransformation(&TransformMatrix, &ResetXformStack);

		// Extract translation
		GfVec3d Translation = TransformMatrix.ExtractTranslation();
		OutTranslation = FVector(Translation[0], Translation[1], Translation[2]);

		// Extract rotation
		GfQuatd RotationQuat = TransformMatrix.ExtractRotationQuat();
		OutQuat = FQuat(RotationQuat.GetReal(), RotationQuat.GetImaginary()[0], RotationQuat.GetImaginary()[1], RotationQuat.GetImaginary()[2]);

		// Extract scale
		// Manually extract scale
		GfVec4d RowX = TransformMatrix.GetRow(0).GetNormalized();
		GfVec4d RowY = TransformMatrix.GetRow(1).GetNormalized();
		GfVec4d RowZ = TransformMatrix.GetRow(2).GetNormalized();
		OutScale.X = RowX.GetLength();
		OutScale.Y = RowZ.GetLength();
		OutScale.Z = -RowY.GetLength();
	}
}
