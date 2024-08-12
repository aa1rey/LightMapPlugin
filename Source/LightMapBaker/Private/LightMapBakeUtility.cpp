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
#include "StaticMeshEditorSubsystem.h"

using namespace UE::Geometry::VectorUtil;

void ULightMapBakeUtility::SetLightMapDensity(float Density)
{
	if (Density <= 0) return;
	TArray<AActor*> OutActors = UEditorUtilityLibrary::GetSelectionSet();
	if (OutActors.Num() == 0) return;

	// Iterate through selected actors with static mesh component
	for (const AActor* actor : OutActors)
	{
		UStaticMeshComponent* LocalStaticMeshComponent = actor->GetComponentByClass<UStaticMeshComponent>();
		float adjustedComponentSize = LocalStaticMeshComponent->CalcBounds(LocalStaticMeshComponent->GetComponentTransform()).BoxExtent.Size();

		if (UStaticMesh* LocalMesh = LocalStaticMeshComponent->GetStaticMesh().Get())
		{
			

			LocalMesh->bAllowCPUAccess = true;
			LocalMesh->GetSourceModel(0).BuildSettings.bGenerateLightmapUVs = 1;

			// Calculate UVs Area
			double UVsArea = GetStaticMeshArea(LocalMesh);

			// Calculate Scaled Mesh Area
			double MeshArea = GetStaticMeshArea(LocalMesh, LocalStaticMeshComponent->GetComponentScale());

			LocalMesh->SetLightmapUVDensity(Density);

			// Calculate Ideal LightMap Resolution by given Density
			int32 Resolution = FMath::Clamp(sqrt(MeshArea * Density), 4, 4096);

			// The Resolution must be the multiply of 4 (dxt block size)
			Resolution = Resolution > 4 ? FMath::Clamp(Resolution - (Resolution % 4), 4, 4096) : 4;

			LocalMesh->GetSourceModel(0).BuildSettings.MinLightmapResolution = GetMinLightMapResolutionFromCurrent(Resolution);
			LocalMesh->SetLightMapResolution(Resolution);

			// Save changes
			LocalMesh->Build();
			LocalMesh->MarkPackageDirty();

			UE_LOG(LogTemp, Display, TEXT("Object Name: %s | LM Res: %i | LM Density: %f | Mesh Area: %f"),
				*LocalMesh->GetName(),
				Resolution,
				LocalMesh->GetLightmapUVDensity(),
				MeshArea);

			/*UE_LOG(LogTemp, Display, TEXT("Object Name: %s | LM Res: %i | LM Density: %f | Mesh bounds: %f"),
				*LocalMesh->GetName(),
				Resolution,
				LocalMesh->GetLightmapUVDensity(),
				adjustedComponentSize);*/
		}
	}
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

			double MeshArea = GetStaticMeshArea(LocalMesh);

			LocalMesh->SetLightmapUVDensity(MeshArea / pow(Resolution, 2) / 0.2f/* / UVArea*/);

			UE_LOG(LogTemp, Warning, TEXT("LM Res: 'i' | LM Density: 'f' | Mesh Area: 'i'"),
				Resolution,
				LocalMesh->GetLightmapUVDensity(),
				MeshArea);


		}
	}

	//UGeometryScriptLibrary_MeshUVFunctions::GetMeshUVSizeInfo(DynamicMeshComponent->GetDynamicMesh(), 0, {}, MeshArea, UVArea, MeshBounds, UVBounds, bIsValidUVSet, bUVUnset);

}

double ULightMapBakeUtility::GetStaticMeshArea(UStaticMesh* Mesh, FVector Scale)
{
	if (!Mesh) return 0.f;
	
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(Mesh, 0, 0, Vertices, Triangles, Normals, UVs, Tangents);

	// Calculate UVsArea that is a sum of all triangles area
	double Area = 0.f;

	// Рассчитываем площадь каждого треугольника
	for (int32 i = 0; i < Triangles.Num(); i += 3)
	{
		// Получаем координаты вершин треугольника
		FVector Corner = Vertices[Triangles[i]] * Scale;
		FVector A = Vertices[Triangles[i + 1]] * Scale - Corner;
		FVector B = Vertices[Triangles[i + 2]] * Scale - Corner;

		// Используем векторное произведение для вычисления площади треугольника
		Area += FVector::CrossProduct(A, B).Size() * 0.5;
	}

	/*int32 TrianglesNum = Triangles.Num() / 3;
	for (int32 i = 0; i < TrianglesNum; i += 3)
	{
		const int32 Indice1 = Triangles[i];
        const int32 Indice2 = Triangles[i + 1];
        const int32 Indice3 = Triangles[i + 2];

        // Координаты вершин треугольника с учетом масштаба
        const FVector A = Vertices[Indice1] * Scale;
        const FVector B = Vertices[Indice2] * Scale;
        const FVector C = Vertices[Indice3] * Scale;

        // Длины сторон треугольника
        const double AB = FVector::Dist(A, B);
        const double BC = FVector::Dist(B, C);
        const double AC = FVector::Dist(A, C);

        // Полупериметр
        const double p = (AB + BC + AC) / 2.0;

        // Площадь треугольника по формуле Герона
        Area += sqrt(p * (p - AB) * (p - BC) * (p - AC));
	}*/

	return Area;
}

int32 ULightMapBakeUtility::GetMinLightMapResolutionFromCurrent(int32 CurrentResolution)
{
	int32 PowerOfTwo = 2;
	while (!(CurrentResolution >= PowerOfTwo && CurrentResolution <= PowerOfTwo * 2))
		PowerOfTwo *= 2;

	return (CurrentResolution - PowerOfTwo < PowerOfTwo * 2 - CurrentResolution ? PowerOfTwo : PowerOfTwo * 2) / 2;
}