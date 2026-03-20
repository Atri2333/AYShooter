// Copyright Ayy3

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimpleOctreeActor.generated.h"

template<typename ElementType>
class TSimpleOctreeNode
{
public:
	FBox Bounds;
	TArray<ElementType> Elements;
	TArray<TUniquePtr<TSimpleOctreeNode<ElementType>>> Children;
	bool bIsLeaf;
	
	TSimpleOctreeNode(const FBox& InBounds)
		: Bounds(InBounds)
		, bIsLeaf(true)
	{
		Children.SetNum(8);
	}
	
	/** 根据点坐标获取子节点索引（0-7） */
	int32 GetChildIndex(const FVector& Point) const
	{
		int32 Index = 0;
		if (Point.X > Bounds.GetCenter().X) Index |= 1;
		if (Point.Y > Bounds.GetCenter().Y) Index |= 2;
		if (Point.Z > Bounds.GetCenter().Z) Index |= 4;
		return Index;
	}
};

template<typename ElementType, typename GetBoundsFunc>
class TSimpleOctree
{
public:
	TUniquePtr<TSimpleOctreeNode<ElementType>> Root; // 根节点
	int32 MaxElementsPerNode; // 每个节点最大元素数量, 超过则分裂
	float MinNodeSize; // 最小节点尺寸, 小于则不再分裂（考虑到同一个点可能有多个元素）
	GetBoundsFunc GetBounds; // 获取元素包围盒的函数对象
	
	TSimpleOctree(const FBox& RootBounds, int32 InMaxElementsPerNode, float InMinNodeSize, GetBoundsFunc InGetBounds)
		: MaxElementsPerNode(InMaxElementsPerNode)
		, MinNodeSize(InMinNodeSize)
		, GetBounds(InGetBounds)
	{
		Root = MakeUnique<TSimpleOctreeNode<ElementType>>(RootBounds);
	}
	
	/** 插入元素 */
	void Insert(const ElementType& Element)
	{
		InsertRecursive(Root.Get(), Element);
	}

	/** 删除元素（通过相等比较，需要 ElementType 重载 operator==） */
	bool Remove(const ElementType& Element)
	{
		return RemoveRecursive(Root.Get(), Element);
	}

	/** 球体范围查询 */
	void QuerySphere(const FVector& Center, float Radius, TArray<ElementType>& OutElements) const
	{
		QuerySphereRecursive(Root.Get(), Center, Radius, OutElements);
	}

	/** AABB 范围查询 */
	void QueryBox(const FBox& Box, TArray<ElementType>& OutElements) const
	{
		QueryBoxRecursive(Root.Get(), Box, OutElements);
	}
	
private:
	// 递归插入
	void InsertRecursive(TSimpleOctreeNode<ElementType>* Node, const ElementType& Element)
	{
		if (Node->bIsLeaf && Node->Elements.Num() >= MaxElementsPerNode && Node->Bounds.GetSize().Length() >= MinNodeSize)
		{
			SplitNode(Node);
		}
		
		if (Node->bIsLeaf)
		{
			Node->Elements.Add(Element);
		}
		else
		{
			const FBox ElementBounds = GetBounds(Element);
			const FVector ElementCenter = ElementBounds.GetCenter();
			const int32 ChildIndex = Node->GetChildIndex(ElementCenter);
			if (Node->Children[ChildIndex] && Node->Children[ChildIndex]->Bounds.IsInside(ElementCenter))
			{
				InsertRecursive(Node->Children[ChildIndex].Get(), Element);
			}
			else
			{
				Node->Elements.Add(Element);
			}
		}
	}
	
	// 分裂节点
	void SplitNode(TSimpleOctreeNode<ElementType>* Node)
	{
		const FVector Center = Node->Bounds.GetCenter();
		const FVector Extent = Node->Bounds.GetExtent();
		const FVector HalfExtent = Extent * 0.5f;
		
		// 创建8个子节点
		for (int32 X = 0; X < 2; ++X)
		{
			for (int32 Y = 0; Y < 2; ++Y)
			{
				for (int32 Z = 0; Z < 2; ++Z)
				{
					FVector NewCenter = Center + FVector(
						(X == 0 ? -HalfExtent.X : HalfExtent.X),
						(Y == 0 ? -HalfExtent.Y : HalfExtent.Y),
						(Z == 0 ? -HalfExtent.Z : HalfExtent.Z)
					);
					FBox NewBounds(NewCenter - HalfExtent, NewCenter + HalfExtent);
					const int32 ChildIdx = (X << 0) | (Y << 1) | (Z << 2);
					Node->Children[ChildIdx] = MakeUnique<TSimpleOctreeNode<ElementType>>(NewBounds);
				}
			}
		}
		
		// 重新分配现有元素
		TArray<ElementType> OldElements = MoveTemp(Node->Elements);
		Node->Elements.Empty();
		Node->bIsLeaf = false;
		
		for (const ElementType& Elem : OldElements)
		{
			// 重新插入元素
			InsertRecursive(Node, Elem);
		}
	}
	
	// 递归删除
	bool RemoveRecursive(TSimpleOctreeNode<ElementType>* Node, const ElementType& Element)
	{
		int32 Index = Node->Elements.Find(Element);
		if (Index != INDEX_NONE)
		{
			Node->Elements.RemoveAt(Index);
			// 删除后，若节点变为空且为叶子，则尝试向上合并
			if (Node->bIsLeaf && Node->Elements.Num() == 0)
			{
				// 合并由上层调用者处理（这里只返回成功）
			}
			return true;
		}
		
		if (!Node->bIsLeaf)
		{
			const FBox ElementBounds = GetBounds(Element);
			const FVector ElementCenter = ElementBounds.GetCenter();
			const int32 ChildIndex = Node->GetChildIndex(ElementCenter);
			if (Node->Children[ChildIndex] && Node->Children[ChildIndex]->Bounds.IsInside(ElementCenter))
			{
				if (RemoveRecursive(Node->Children[ChildIndex].Get(), Element))
				{
					// 检查是否可以合并：所有子节点都是叶子且为空
					bool bAllEmpty = true;
					for (auto& Child : Node->Children)
					{
						if (Child && (!Child->bIsLeaf || Child->Elements.Num() > 0))
						{
							bAllEmpty = false;
							break;
						}
					}
					if (bAllEmpty)
					{
						// 合并：删除所有子节点，当前节点变为叶子
						Node->Children.Reset();
						Node->Children.SetNum(8);
						Node->bIsLeaf = true;
					}
					return true;
				}
			}
			else
			{
				// 前面已经处理过
			}
		}
		return false;
	}
	
	static bool IsBoxIntersectingSphere(const FBox& Box, const FVector& SphereCenter, float SphereRadius)
	{
		if (!Box.IsValid) return false;
		FVector ClosestPoint;
		ClosestPoint.X = FMath::Clamp(SphereCenter.X, Box.Min.X, Box.Max.X);
		ClosestPoint.Y = FMath::Clamp(SphereCenter.Y, Box.Min.Y, Box.Max.Y);
		ClosestPoint.Z = FMath::Clamp(SphereCenter.Z, Box.Min.Z, Box.Max.Z);
		const float DistSq = FVector::DistSquared(ClosestPoint, SphereCenter);
		return DistSq <= FMath::Square(SphereRadius);
	}
	
	static bool IsBoxIntersectingBox(const FBox& Box, const FBox& Box2)
	{
		if (!Box.IsValid || !Box2.IsValid) return false; 
		if (Box.Min.X > Box2.Max.X || Box.Max.X < Box2.Min.X) return false;
		if (Box.Min.Y > Box2.Max.Y || Box.Max.Y < Box2.Min.Y) return false;
		if (Box.Min.Z > Box2.Max.Z || Box.Max.Z < Box2.Min.Z) return false;
		return true;
	}
	
	/** 递归球体查询 */
	void QuerySphereRecursive(const TSimpleOctreeNode<ElementType>* Node, const FVector& Center, float Radius, TArray<ElementType>& OutElements) const
	{
		// 快速剔除：节点和查询球体不相交
		if (!IsBoxIntersectingSphere(Node->Bounds, Center, Radius)) return;
		
		// 检查当前节点元素
		for (const ElementType& Elem : Node->Elements)
		{
			const FBox ElementBounds = GetBounds(Elem);
			if (IsBoxIntersectingSphere(ElementBounds, Center, Radius))
			{
				OutElements.Add(Elem);
			}
		}
		
		// 递归查询子节点
		if (!Node->bIsLeaf)
		{
			for (const auto& Child : Node->Children)
			{
				if (Child)
				{
					QuerySphereRecursive(Child.Get(), Center, Radius, OutElements);
				}
			}
		}
	}
	
	/** 递归 AABB 查询 */
	void QueryBoxRecursive(const TSimpleOctreeNode<ElementType>* Node, const FBox& Box, TArray<ElementType>& OutElements) const
	{
		if (!IsBoxIntersectingBox(Node->Bounds, Box)) return;
		for (const ElementType& Elem : Node->Elements)
		{
			const FBox ElementBounds = GetBounds(Elem);
			if (IsBoxIntersectingBox(ElementBounds, Box))
			{
				OutElements.Add(Elem);
			}
		}
		
		if (!Node->bIsLeaf)
		{
			for (const auto& Child : Node->Children)
			{
				if (Child)
				{
					QueryBoxRecursive(Child.Get(), Box, OutElements);
				}
			}
		}
	}
};

USTRUCT()
struct FPointElement
{
	GENERATED_BODY()
	
	FVector Position;
	float Radius;
	FColor Color;
	
	bool operator==(const FPointElement& rhs) const
	{
		return Position == rhs.Position && Radius == rhs.Radius && Color == rhs.Color;
	}
};

/**
 * 使用简化八叉树的 Actor，支持可视化
 */
UCLASS()
class ASimpleOctreeActor : public AActor
{
	GENERATED_BODY()

public:
	ASimpleOctreeActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** 生成随机点并插入八叉树 */
	UFUNCTION(BlueprintCallable, Category = "Octree")
	void GenerateRandomPoints(int32 NumPoints, float Radius, const FBox& Area);

	/** 清除所有点 */
	UFUNCTION(BlueprintCallable, Category = "Octree")
	void ClearPoints();

	/** 查询指定球体内的元素（结果会高亮显示） */
	UFUNCTION(BlueprintCallable, Category = "Octree")
	void QueryElements(const FVector& Center, float Radius);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostRegisterAllComponents() override;

private:
	/** 获取元素包围盒 */
	static FBox GetElementBounds(const FPointElement& Element)
	{
		return FBox(Element.Position - FVector(Element.Radius), Element.Position + FVector(Element.Radius));
	}

	/** 八叉树类型 */
	using FSimpleOctree = TSimpleOctree<FPointElement, decltype(&GetElementBounds)>;

	TUniquePtr<FSimpleOctree> Octree;
	TArray<FPointElement> AllElements;     // 所有元素，用于绘制
	TArray<FPointElement> QueryResult;     // 查询结果

	/** 初始化八叉树 */
	void SetupOctree();

	/** 递归绘制节点 */
	void DrawNode(const TSimpleOctreeNode<FPointElement>* Node, int32 Depth) const;
};