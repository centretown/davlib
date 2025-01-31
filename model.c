#include "davlib.h"
#include <assert.h>

void DrawModelShape(Shape *shape) {
  ModelShape *model = shape->shapePtr;
  DrawModelEx(model->model, model->position, model->rotationAxis,
              model->rotationAngle, model->scale, model->tint);
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
  UnloadModel(model->model);
}

void InitModelShape(Shape *shape, Texture2D texture) {
  Mesh mesh = {0};
  Model model = {0};
  assert(shape);
  assert(shape->shapePtr);
  ModelShape *modelShape = shape->shapePtr;
  assert(modelShape->genPtr);

  switch (shape->typeID) {
  case MESH_POLY: {
    MeshPoly *poly = modelShape->genPtr;
    mesh = GenMeshPoly(poly->sides, poly->radius);
    model = LoadModelFromMesh(mesh);
    break;
  }
  case MESH_PLANE: {
    MeshPlane *plane = modelShape->genPtr;
    mesh = GenMeshPlane(plane->width, plane->length, plane->resX, plane->resZ);
    model = LoadModelFromMesh(mesh);
    break;
  }
  case MESH_CUBE: {
    MeshCube *cube = modelShape->genPtr;
    mesh = GenMeshCube(cube->width, cube->height, cube->length);
    model = LoadModelFromMesh(mesh);
    break;
  }
  case MESH_SPHERE: {
    MeshSphere *sphere = modelShape->genPtr;
    mesh = GenMeshSphere(sphere->radius, sphere->rings, sphere->slices);
    model = LoadModelFromMesh(mesh);
    break;
  }
  case MESH_HEMISPHERE: {
    MeshHemisphere *hemisphere = modelShape->genPtr;
    mesh = GenMeshHemiSphere(hemisphere->radius, hemisphere->rings,
                             hemisphere->slices);
    model = LoadModelFromMesh(mesh);
    break;
  }
  case MESH_CYLINDER: {
    MeshCylinder *cylinder = modelShape->genPtr;
    mesh =
        GenMeshCylinder(cylinder->radius, cylinder->slices, cylinder->slices);
    model = LoadModelFromMesh(mesh);
    break;
  }
  case MESH_CONE: {
    MeshCone *cone = modelShape->genPtr;
    mesh = GenMeshCone(cone->radius, cone->height, cone->slices);
    model = LoadModelFromMesh(mesh);
    break;
  }
  case MESH_TORUS: {
    MeshTorus *torus = modelShape->genPtr;
    mesh =
        GenMeshTorus(torus->radius, torus->size, torus->radSeg, torus->sides);
    model = LoadModelFromMesh(mesh);
    break;
  }
  case MESH_KNOT: {
    MeshKnot *knot = modelShape->genPtr;
    mesh = GenMeshKnot(knot->radius, knot->size, knot->radSeg, knot->sides);
    model = LoadModelFromMesh(mesh);
    break;
  }
  // case MESH_HEIGHTMAP: {
  //   mesh = GenMeshHeightmap();
  //   model = LoadModelFromMesh(mesh);
  //   break;
  // }
  // case MESH_CUBICMAP: {
  //   mesh = GenMeshCubicmap();
  //   model = LoadModelFromMesh(mesh);
  //   break;
  default:
    return;
  }

  Material material = LoadMaterialDefault();
  // TODO UnloadMaterial(material);
  //  UnloadTexture( texture);
  // Set texture for a material map type (MATERIAL_MAP_DIFFUSE,
  // MATERIAL_MAP_SPECULAR...)
  SetMaterialTexture(&material, MATERIAL_MAP_DIFFUSE, texture);
  modelShape->model = model;
  shape->Draw = DrawModelShape;
  shape->Move = MoveModelShape;
  shape->Position = ModelShapePosition;
  shape->Unload = UnloadModelShape;
}
