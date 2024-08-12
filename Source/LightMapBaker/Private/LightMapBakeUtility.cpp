// Fill out your copyright notice in the Description page of Project Settings.


#include "LightMapBakeUtility.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "EditorUtilityLibrary.h"
#include "Components/StaticMeshComponent.h"

void ULightMapBakeUtility::SetLightMapDensity(float LightMapDensity, int32 MinLightMapRes, int32 LightMapCoordinateIndex)
{
	if (LightMapDensity <= 0) return;
	TArray<AActor*> OutActors = UEditorUtilityLibrary::GetSelectionSet();
	if (OutActors.Num() == 0) return;

	// Iterate through selected actors with static mesh component
	for (const AActor* actor : OutActors)
	{
		UStaticMeshComponent* LocalStaticMeshComponent = actor->GetComponentByClass<UStaticMeshComponent>();
		if (!LocalStaticMeshComponent) continue;

		if (UStaticMesh* LocalMesh = LocalStaticMeshComponent->GetStaticMesh().Get())
		{
			// Calculate UVs Area
			double MeshArea_x1 = GetStaticMeshArea(LocalMesh);

			// Calculate Scaled Mesh Area
			double MeshArea_Scaled = GetStaticMeshArea(LocalMesh, LocalStaticMeshComponent->GetComponentScale());

			// Calculate Ideal LightMap Resolution by given Density without Scale
			int32 Resolution_x1 = FMath::Clamp(sqrt(MeshArea_x1) * LightMapDensity, 4, 4096);

			// Calculate Ideal LightMap Resolution by given Density with Scale
			int32 Resolution_Scaled = FMath::Clamp(sqrt(MeshArea_Scaled) * LightMapDensity, 4, 4096);

			// The Resolution must be the multiply of 4 (dxt block size)
			//Resolution = Resolution > 4 ? FMath::Clamp(Resolution - (Resolution % 4), 4, 4096) : 4;

			// Set StaticMesh asset lightmap parameters without scale
			LocalMesh->GetSourceModel(0).BuildSettings.bGenerateLightmapUVs = 1;
			if (LightMapCoordinateIndex >= 0) LocalMesh->SetLightMapCoordinateIndex(LightMapCoordinateIndex);
			LocalMesh->GetSourceModel(0).BuildSettings.MinLightmapResolution = MinLightMapRes != 0 ? MinLightMapRes : GetMinLightMapResolutionFromCurrent(Resolution_x1);
			LocalMesh->SetLightmapUVDensity(LightMapDensity);
			LocalMesh->SetLightMapResolution(Resolution_x1);

			LocalMesh->MarkPackageDirty();
			LocalMesh->Build();

			// Set StaticMeshComponent lightmap parameters with scale
			LocalStaticMeshComponent->bOverrideLightMapRes = true;
			LocalStaticMeshComponent->OverriddenLightMapRes = Resolution_Scaled;

			UE_LOG(LogTemp, Display,
				TEXT("Object Name: %s | LM Res Scaled: %i | LM Density: %f | Mesh Area Scaled: %f"),
				*LocalMesh->GetName(), Resolution_Scaled, LightMapDensity, MeshArea_Scaled
			);
		}
	}
}

TArray<AActor*> ULightMapBakeUtility::GetSelectedActors()
{
	return UEditorUtilityLibrary::GetSelectionSet();
}

double ULightMapBakeUtility::GetStaticMeshArea(UStaticMesh* Mesh, FVector Scale)
{
	if (!Mesh) return 0.f;

	double Area = 0.f;
	int32 SectionsNum = Mesh->GetRenderData()->LODResources[0].Sections.Num();

	for (int32 SectionInd = 0; SectionInd < SectionsNum; SectionInd++)
	{
		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;
		TArray<FVector2D> UVs;
		TArray<FProcMeshTangent> Tangents;

		UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(Mesh, 0, SectionInd, Vertices, Triangles, Normals, UVs, Tangents);

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

		/*for (int32 i = 0; i < Triangles.Num(); i += 3)
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
	}

	return Area;
}

int32 ULightMapBakeUtility::GetMinLightMapResolutionFromCurrent(int32 CurrentResolution)
{
	int32 PowerOfTwo = 2;
	while (!(CurrentResolution >= PowerOfTwo && CurrentResolution <= PowerOfTwo * 2))
		PowerOfTwo *= 2;

	return (CurrentResolution - PowerOfTwo < PowerOfTwo * 2 - CurrentResolution ? PowerOfTwo : PowerOfTwo * 2);
}

UWorld* ULightMapBakeUtility::GetWorld() const
{
	return GEditor->GetEditorWorldContext().World();
}
