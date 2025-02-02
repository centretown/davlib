#include "davlib.h"
#include "raylib.h"
#include "raymath.h"
#include <assert.h>
#include <stdio.h>

void DrawModelShape(Shape *shape) {
  ModelShape *model = shape->shapePtr;
  Matrix mx =
      MatrixTranslate(model->position.x, model->position.y, model->position.z);
  if (!IsMaterialValid(model->material)) {
    printf("MATERIAL INVALID\n");
  }
  DrawMesh(model->mesh, model->material, mx);
  // DrawModelEx(model->model, model->position, model->rotationAxis,
  //             model->rotationAngle, model->scale, model->tint);
}

void MoveModelShape(Shape *shape, Vector3 position) {
  ModelShape *model = shape->shapePtr;
  model->position = position;
}

Vector3 ModelShapePosition(Shape *shape) {
  ModelShape *model = shape->shapePtr;
  return model->position;
}

void UnloadModelShape(Shape *shape) {
  ModelShape *model = shape->shapePtr;
  UnloadMesh(model->mesh);
}

void InitModelShape(Shape *shape, Material material) {
  assert(shape);
  assert(shape->shapePtr);
  ModelShape *modelShape = shape->shapePtr;
  assert(modelShape->genPtr);

  switch (shape->typeID) {
  case MESH_POLY: {
    MeshPoly *poly = modelShape->genPtr;
    modelShape->mesh = GenMeshPoly(poly->sides, poly->radius);
    break;
  }
  case MESH_PLANE: {
    MeshPlane *plane = modelShape->genPtr;
    modelShape->mesh =
        GenMeshPlane(plane->width, plane->length, plane->resX, plane->resZ);
    break;
  }
  case MESH_CUBE: {
    MeshCube *cube = modelShape->genPtr;
    modelShape->mesh = GenMeshCube(cube->width, cube->height, cube->length);
    break;
  }
  case MESH_SPHERE: {
    MeshSphere *sphere = modelShape->genPtr;
    modelShape->mesh =
        GenMeshSphere(sphere->radius, sphere->rings, sphere->slices);
    break;
  }
  case MESH_HEMISPHERE: {
    MeshHemisphere *hemisphere = modelShape->genPtr;
    modelShape->mesh = GenMeshHemiSphere(hemisphere->radius, hemisphere->rings,
                                         hemisphere->slices);
    break;
  }
  case MESH_CYLINDER: {
    MeshCylinder *cylinder = modelShape->genPtr;
    modelShape->mesh =
        GenMeshCylinder(cylinder->radius, cylinder->height, cylinder->slices);
    break;
  }
  case MESH_CONE: {
    MeshCone *cone = modelShape->genPtr;
    modelShape->mesh = GenMeshCone(cone->radius, cone->height, cone->slices);
    break;
  }
  case MESH_TORUS: {
    MeshTorus *torus = modelShape->genPtr;
    modelShape->mesh =
        GenMeshTorus(torus->radius, torus->size, torus->radSeg, torus->sides);
    break;
  }
  case MESH_KNOT: {
    MeshKnot *knot = modelShape->genPtr;
    modelShape->mesh =
        GenMeshKnot(knot->radius, knot->size, knot->radSeg, knot->sides);
    break;
  }
  // case MESH_HEIGHTMAP: {
  //   mesh = GenMeshHeightmap();
  //   break;
  // }
  // case MESH_CUBICMAP: {
  //   mesh = GenMeshCubicmap();
  //   break;
  default:
    return;
  }

  // Model model = {0};
  // model = LoadModelFromMesh(mesh);
  // TODO UnloadMaterial(material);
  //  UnloadTexture( texture);
  // Set texture for a material map type (MATERIAL_MAP_DIFFUSE,
  // MATERIAL_MAP_SPECULAR...)
  //

  modelShape->material = material;
  modelShape->matrix = MatrixIdentity();
  shape->Draw = DrawModelShape;
  shape->Move = MoveModelShape;
  shape->Position = ModelShapePosition;
  shape->home = shape->Position(shape);
  shape->Unload = UnloadModelShape;
}
