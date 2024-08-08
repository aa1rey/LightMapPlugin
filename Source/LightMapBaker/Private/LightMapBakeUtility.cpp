// Fill out your copyright notice in the Description page of Project Settings.


#include "LightMapBakeUtility.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/ObjectLibrary.h"
#include "Layers/LayersSubsystem.h"
#include "GeometryScript/MeshUVFunctions.h"
#include "GeometryScript/MeshAssetFunctions.h"
#include "EditorFramework/AssetImportData.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "EditorUtilityLibrary.h"

void ULightMapBakeUtility::SetLightMapDensity(UStaticMesh* Mesh, float Density)
{
	Mesh->SetLightmapUVDensity(Density);
}

TArray<AActor*> ULightMapBakeUtility::GetSelectedActors()
{
	return UEditorUtilityLibrary::GetSelectionSet();
}

void ULightMapBakeUtility::SetMinLightMapRes(int32 Resolution)
{
	TArray<AActor*> OutActors = UEditorUtilityLibrary::GetSelectionSet();
	if (OutActors.Num() == 0) return;

	// Find the nearest PowerOfTwo number for the Min LightMap Resolution
	int32 MinLightMapRes = GetMinLightMapResolutionFromCurrent(Resolution);

	// Iterate through selected actors with static mesh component
	for (auto actor : OutActors)
	{
		if (UStaticMesh* LocalMesh = actor->GetComponentByClass<UStaticMeshComponent>()->GetStaticMesh().Get())
		{
			LocalMesh->bAllowCPUAccess = true;
			LocalMesh->GetSourceModel(0).BuildSettings.bGenerateLightmapUVs = true;
			LocalMesh->GetSourceModel(0).BuildSettings.MinLightmapResolution = MinLightMapRes;
			LocalMesh->SetLightMapResolution(Resolution);

			double MeshArea = GetStaticMeshMeshArea(LocalMesh);

			LocalMesh->SetLightmapUVDensity(MeshArea / pow(Resolution, 2) / 0.2f/* / UVArea*/);

			UE_LOG(LogTemp, Warning, TEXT("LM Res: 'i' | LM Density: 'f' | Mesh Area: 'i'"),
				Resolution,
				LocalMesh->GetLightmapUVDensity(),
				MeshArea);


		}
	}

	//UGeometryScriptLibrary_MeshUVFunctions::GetMeshUVSizeInfo(DynamicMeshComponent->GetDynamicMesh(), 0, {}, MeshArea, UVArea, MeshBounds, UVBounds, bIsValidUVSet, bUVUnset);

}

double ULightMapBakeUtility::GetStaticMeshMeshArea(UStaticMesh* Mesh)
{
	if (!Mesh) return 0.f;

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(Mesh, 0, 0, Vertices, Triangles, Normals, UVs, Tangents);

	// Calculate MeshArea that is a sum of all triangles area
	double MeshArea = 0;
	int32 TrianglesNum = Triangles.Num() / 3;
	for (int32 TrianglesIdx = 0; TrianglesIdx < TrianglesNum; TrianglesIdx += 3)
	{
		const int32 Indice1 = Triangles[TrianglesIdx];
		const int32 Indice2 = Triangles[TrianglesIdx + 1];
		const int32 Indice3 = Triangles[TrianglesIdx + 2];

		const FVector A = Vertices[Indice1];
		const FVector B = Vertices[Indice2];
		const FVector C = Vertices[Indice3];

		const FVector AB = FVector(B.X - A.X, B.Y - A.Y, B.Z - A.Z);
		const FVector BC = FVector(C.X - B.X, C.Y - B.Y, C.Z - B.Z);
		const FVector AC = FVector(C.X - A.X, C.Y - A.Y, C.Z - A.Z);

		const double p = (AB.Length() + BC.Length() + AC.Length()) / 2.f;
		MeshArea += sqrt(p * (p - AB.Length()) * (p - BC.Length()) * (p - AC.Length()));
	}

	return MeshArea;
}

int32 ULightMapBakeUtility::GetMinLightMapResolutionFromCurrent(int32 CurrentResolution)
{
	int32 PowerOfTwo = 2;
	while (!(CurrentResolution >= PowerOfTwo && CurrentResolution <= PowerOfTwo * 2))
		PowerOfTwo *= 2;

	return (CurrentResolution - PowerOfTwo < PowerOfTwo * 2 - CurrentResolution ? PowerOfTwo : PowerOfTwo * 2) / 2;
}