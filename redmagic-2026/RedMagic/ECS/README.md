# RedMagic ECS System

In RedMagic, Entities are referred by a single integer, and used through a 
proxy type. Likewise, components are referred through a single integer and a 
`ComponentRef<T>` template proxy class.

Thise refer to indices in an indexed vector, a type that is index stable on 
removal, insertion, iteration, and sorting[^1].

## Entities

## Components

### Transforms

Positions in 2D are handled by the `Transform2DComponent`. This component 
also handles parenting of objects.

## Systems

Systems are implemented classes managed in a similar way to the components 
and entities.

[^1]: When sorting, the sorting algorithm used is insertion sort. That means 
that keeping the array sorted is cheaper than only sorting it occasionally. 