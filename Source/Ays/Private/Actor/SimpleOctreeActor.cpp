// Copyright Ayy3


#include "Actor/SimpleOctreeActor.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogSimpleOctree, Log, All);

ASimpleOctreeActor::ASimpleOctreeActor()
{
    PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(true);
}

void ASimpleOctreeActor::BeginPlay()
{
    Super::BeginPlay();
    SetupOctree();
    
}

void ASimpleOctreeActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 绘制八叉树节点
    if (Octree && Octree->Root)
    {
        DrawNode(Octree->Root.Get(), 0);
    }

    // 绘制所有元素（绿色）
    for (const FPointElement& Elem : AllElements)
    {
        DrawDebugSphere(GetWorld(), Elem.Position, Elem.Radius, 12, FColor::Green);
    }

    // 绘制查询结果（红色）
    for (const FPointElement& Elem : QueryResult)
    {
        DrawDebugSphere(GetWorld(), Elem.Position, Elem.Radius, 12, FColor::Red);
    }
}

void ASimpleOctreeActor::GenerateRandomPoints(int32 NumPoints, float Radius, const FBox& Area)
{
    if (!Octree)
        SetupOctree();

    for (int32 i = 0; i < NumPoints; ++i)
    {
        FPointElement Elem;
        Elem.Radius = Radius;
        Elem.Position = FMath::RandPointInBox(Area);
        Elem.Color = FColor::Green;
        Octree->Insert(Elem);
        AllElements.Add(Elem);
    }
}

void ASimpleOctreeActor::ClearPoints()
{
    if (Octree)
    {
        // 重建八叉树（简单清空）
        SetupOctree();
    }
    AllElements.Empty();
    QueryResult.Empty();
}

void ASimpleOctreeActor::QueryElements(const FVector& Center, float Radius)
{
    QueryResult.Empty();
    if (Octree)
    {
        Octree->QuerySphere(Center, Radius, QueryResult);
    }
}

void ASimpleOctreeActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    SetupOctree();
}

void ASimpleOctreeActor::PostRegisterAllComponents()
{
    Super::PostRegisterAllComponents();
    SetupOctree();
}

void ASimpleOctreeActor::SetupOctree()
{
    // 定义根节点包围盒（可根据需求调整）
    FBox RootBounds(FVector(-5000), FVector(5000));
    Octree = MakeUnique<FSimpleOctree>(RootBounds, 8, 50.0f, &GetElementBounds);
    // 重新插入所有现有元素
    for (const FPointElement& Elem : AllElements)
    {
        Octree->Insert(Elem);
    }
}

void ASimpleOctreeActor::DrawNode(const TSimpleOctreeNode<FPointElement>* Node, int32 Depth) const
{
    if (!Node) return;

    // 根据深度选择颜色
    FColor Color;
    switch (Depth % 3)
    {
    case 0: Color = FColor::Blue; break;
    case 1: Color = FColor::Cyan; break;
    case 2: Color = FColor::Yellow; break;
    default: Color = FColor::White; break;
    }

    DrawDebugBox(GetWorld(), Node->Bounds.GetCenter(), Node->Bounds.GetExtent(), Color, false, -1.0f, 0, 1.0f);

    if (!Node->bIsLeaf)
    {
        for (const auto& Child : Node->Children)
        {
            if (Child)
            {
                DrawNode(Child.Get(), Depth + 1);
            }
        }
    }
}
