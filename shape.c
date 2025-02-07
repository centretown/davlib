#include "davlib.h"
#include "raylib.h"
#include <assert.h>

void UnloadNone(Shape *shape) {}

void DrawCubeShape(Shape *shape) {
  CubeShape *cube = shape->shapePtr;
  DrawCubeV(cube->position, cube->size, cube->color);
}
void MoveCubeShape(Shape *shape, Vector3 position) {
  CubeShape *cube = shape->shapePtr;
  cube->position = position;
}
Vector3 CubeShapePosition(Shape *shape) {
  CubeShape *cube = shape->shapePtr;
  return cube->position;
}

void DrawSphereShape(Shape *shape) {
  SphereShape *sphere = shape->shapePtr;
  DrawSphere(sphere->position, sphere->radius, sphere->color);
}
void MoveSphereShape(Shape *shape, Vector3 position) {
  SphereShape *sphere = shape->shapePtr;
  sphere->position = position;
}
Vector3 SphereShapePosition(Shape *shape) {
  SphereShape *sphere = shape->shapePtr;
  return sphere->position;
}

void DrawCylinderShape(Shape *shape) {
  CylinderShape *cylinder = shape->shapePtr;
  DrawCylinder(cylinder->position, cylinder->radiusTop, cylinder->radiusBottom,
               cylinder->height, cylinder->slices, cylinder->color);
}
void MoveCylinderShape(Shape *shape, Vector3 position) {
  CylinderShape *cylinder = shape->shapePtr;
  cylinder->position = position;
}
Vector3 CylinderShapePosition(Shape *shape) {
  CylinderShape *cylinder = shape->shapePtr;
  return cylinder->position;
}

void DrawCapsuleShape(Shape *shape) {
  CapsuleShape *capsule = shape->shapePtr;
  DrawCapsule(capsule->startPos, capsule->endPos, capsule->radius,
              capsule->slices, capsule->rings, capsule->color);
}
void MoveCapsuleShape(Shape *shape, Vector3 position) {
  CapsuleShape *capsule = shape->shapePtr;
  Vector3 diff = Vector3Subtract(position, capsule->startPos);
  // print_vectors(2, capsule->startPos, capsule->endPos);
  capsule->startPos = position;
  capsule->endPos = Vector3Add(diff, capsule->endPos);
  // print_vectors(3, diff, capsule->startPos, capsule->endPos);
}
Vector3 CapsuleShapePosition(Shape *shape) {
  CapsuleShape *capsule = shape->shapePtr;
  return capsule->startPos;
}

void InitShape(Shape *shape) {
  switch (shape->typeID) {
  case CUBE:
    shape->Draw = DrawCubeShape;
    shape->Move = MoveCubeShape;
    shape->Position = CubeShapePosition;
    break;

  case SPHERE: {
    shape->Draw = DrawSphereShape;
    shape->Move = MoveSphereShape;
    shape->Position = SphereShapePosition;
    break;
  }
  case CYLINDER: {
    shape->Draw = DrawCylinderShape;
    shape->Move = MoveCylinderShape;
    shape->Position = CylinderShapePosition;
    break;
  }
  case CAPSULE: {
    shape->Draw = DrawCapsuleShape;
    shape->Move = MoveCapsuleShape;
    shape->Position = CapsuleShapePosition;
    break;
  }
  default:
    assert(0);
    return;
  }

  shape->home = shape->Position(shape);
  shape->Unload = UnloadNone;
}

void InitShapes(int count, Shape *shapes[], Material material) {
  for (int i = 0; i < count; i++) {
    Shape *shape = shapes[i];
    if (shape->typeID < MESH_FIRST)
      InitShape(shapes[i]);
    else
      InitMeshShape(shapes[i], material);
  }
}

void DrawShapes(int count, Shape *shapes[]) {
  assert(count >= 0);
  assert(shapes);
  for (int i = 0; i < count; i++) {
    Shape *shape = shapes[i];
    shape->Draw(shape);
  }
}
