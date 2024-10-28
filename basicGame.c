#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include "rlgl.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
    Vector3 position;
    float rotationX;
    float rotationY;
    float distance;
} Camera3DController;

typedef struct {
    Vector3 position;    // Current position
    Vector3 targetPos;   // Final unfolded position
    Vector3 rotation;    // Current rotation angles (x, y, z)
    Vector3 targetRot;   // Final rotation angles
    Vector3 foldedPos;   // Position in folded state
    Vector3 foldedRot;   // Rotation in folded state
    Color color;         // Face color
    float progress;      // Animation progress (0.0 to 1.0)
} CubeFace;

typedef struct {
    CubeFace faces[6];  // 0:Up, 1:Down, 2:Left, 3:Front, 4:Right, 5:Back
    bool unfolded;
    float animationSpeed;
} Cube;

void InitializeCube(Cube* cube) {
    const float size = 1.0f;  // Half-size of cube faces
    const float spacing = 2.1f;
    cube->unfolded = false;
    cube->animationSpeed = 0.02f;
    
    // Initialize all faces
    Color colors[6] = { WHITE, YELLOW, ORANGE, GREEN, RED, BLUE };
    
    // Folded positions (forming a cube)
    Vector3 foldedPositions[6] = {
        (Vector3){ 0.0f, size, 0.0f },    // Up (top face)
        (Vector3){ 0.0f, -size, 0.0f },   // Down (bottom face)
        (Vector3){ -size, 0.0f, 0.0f },   // Left
        (Vector3){ 0.0f, 0.0f, size },    // Front
        (Vector3){ size, 0.0f, 0.0f },    // Right
        (Vector3){ 0.0f, 0.0f, -size },   // Back
    };
    
    // Folded rotations
    Vector3 foldedRotations[6] = {
        (Vector3){ 0.0f, 0.0f, 0.0f },      // Up
        (Vector3){ 180.0f, 0.0f, 0.0f },    // Down
        (Vector3){ 0.0f, 0.0f, -90.0f },    // Left
        (Vector3){ 90.0f, 0.0f, 0.0f },     // Front
        (Vector3){ 0.0f, 0.0f, 90.0f },     // Right
        (Vector3){ -90.0f, 0.0f, 0.0f }     // Back
    };
    
    // Unfolded positions (flat layout)
    Vector3 unfoldedPositions[6] = {
        (Vector3){ 0.0f, 0.0f, -spacing },      // Up
        (Vector3){ 0.0f, 0.0f, spacing },       // Down
        (Vector3){ -spacing, 0.0f, 0.0f },      // Left
        (Vector3){ 0.0f, 0.0f, 0.0f },          // Front
        (Vector3){ spacing, 0.0f, 0.0f },       // Right
        (Vector3){ spacing * 2, 0.0f, 0.0f }    // Back
    };
    
    // Unfolded rotations (flat)
    Vector3 unfoldedRotations[6] = {
        (Vector3){ 0.0f, 0.0f, 0.0f },    // Up
        (Vector3){ 0.0f, 0.0f, 0.0f },    // Down
        (Vector3){ 0.0f, 0.0f, 0.0f },    // Left
        (Vector3){ 0.0f, 0.0f, 0.0f },    // Front
        (Vector3){ 0.0f, 0.0f, 0.0f },    // Right
        (Vector3){ 0.0f, 0.0f, 0.0f }     // Back
    };
    
    for (int i = 0; i < 6; i++) {
        cube->faces[i].foldedPos = foldedPositions[i];
        cube->faces[i].foldedRot = foldedRotations[i];
        cube->faces[i].position = foldedPositions[i];
        cube->faces[i].targetPos = unfoldedPositions[i];
        cube->faces[i].rotation = foldedRotations[i];
        cube->faces[i].targetRot = unfoldedRotations[i];
        cube->faces[i].color = colors[i];
        cube->faces[i].progress = 0.0f;
    }
}

void UpdateCameraController(Camera3D* camera, Camera3DController* controller) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mouseDelta = GetMouseDelta();
        controller->rotationY += mouseDelta.x * 0.5f;
        controller->rotationX += mouseDelta.y * 0.5f;
        controller->rotationX = Clamp(controller->rotationX, -85.0f, 85.0f);
    }
    
    float wheel = GetMouseWheelMove();
    controller->distance -= wheel * 0.5f;
    controller->distance = Clamp(controller->distance, 3.0f, 15.0f);
    
    float x = controller->distance * cosf(DEG2RAD * controller->rotationX) * sinf(DEG2RAD * controller->rotationY);
    float y = controller->distance * sinf(DEG2RAD * controller->rotationX);
    float z = controller->distance * cosf(DEG2RAD * controller->rotationX) * cosf(DEG2RAD * controller->rotationY);
    
    camera->position = (Vector3){ x, y, z };
    camera->target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera->up = (Vector3){ 0.0f, 1.0f, 0.0f };
}

Vector3 LerpVector3(Vector3 start, Vector3 end, float t) {
    return (Vector3){
        start.x + (end.x - start.x) * t,
        start.y + (end.y - start.y) * t,
        start.z + (end.z - start.z) * t
    };
}

void UpdateCube(Cube* cube) {
    if (IsKeyPressed(KEY_SPACE)) {
        cube->unfolded = !cube->unfolded;
    }
    
    for (int i = 0; i < 6; i++) {
        float delayOffset = i * 0.2f;
        
        if (cube->unfolded) {
            if (cube->faces[i].progress < 1.0f && GetTime() > delayOffset) {
                cube->faces[i].progress += cube->animationSpeed;
                if (cube->faces[i].progress > 1.0f) cube->faces[i].progress = 1.0f;
            }
        } else {
            if (cube->faces[i].progress > 0.0f && GetTime() > delayOffset) {
                cube->faces[i].progress -= cube->animationSpeed;
                if (cube->faces[i].progress < 0.0f) cube->faces[i].progress = 0.0f;
            }
        }
        
        // Smooth step interpolation
        float t = cube->faces[i].progress;
        t = t * t * (3.0f - 2.0f * t);
        
        // Interpolate between folded and unfolded states
        cube->faces[i].position = LerpVector3(
            cube->faces[i].foldedPos,
            cube->faces[i].targetPos,
            t
        );
        
        cube->faces[i].rotation = LerpVector3(
            cube->faces[i].foldedRot,
            cube->faces[i].targetRot,
            t
        );
    }
}

void RenderCustomCube(Cube* cube) {
    for (int i = 0; i < 6; i++) {
        CubeFace* face = &cube->faces[i];
        
        rlPushMatrix();
            rlTranslatef(face->position.x, face->position.y, face->position.z);
            rlRotatef(face->rotation.x, 1, 0, 0);
            rlRotatef(face->rotation.y, 0, 1, 0);
            rlRotatef(face->rotation.z, 0, 0, 1);
            
            DrawCube((Vector3){0, 0, 0}, 2.0f, 0.1f, 2.0f, face->color);
            DrawCubeWires((Vector3){0, 0, 0}, 2.0f, 0.1f, 2.0f, BLACK);
        rlPopMatrix();
    }
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "3D Cube Unfolding Animation");
    SetTargetFPS(60);
    
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 8.0f, 8.0f, 8.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    Camera3DController controller = {
        .position = camera.position,
        .rotationX = 45.0f,
        .rotationY = 45.0f,
        .distance = 12.0f
    };
    
    Cube cube;
    InitializeCube(&cube);
    
    while (!WindowShouldClose()) {
        UpdateCameraController(&camera, &controller);
        UpdateCube(&cube);
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            BeginMode3D(camera);
                RenderCustomCube(&cube);
                DrawGrid(20, 1.0f);
            EndMode3D();
            
            DrawText("Left mouse button to rotate", 10, 10, 20, DARKGRAY);
            DrawText("Mouse wheel to zoom", 10, 35, 20, DARKGRAY);
            DrawText("Space to unfold/fold cube", 10, 60, 20, DARKGRAY);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}