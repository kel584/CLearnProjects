#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
    Vector3 position;
    float rotationX;
    float rotationY;
    float distance;
} Camera3DController;

typedef struct {
    Vector3 vertices[8];
    bool unfolded;
    float unfoldProgress;
} Cube;

void InitializeCube(Cube* cube) {
    float size = 1.0f;
    cube->vertices[0] = (Vector3){ -size, -size, -size };
    cube->vertices[1] = (Vector3){ size, -size, -size };
    cube->vertices[2] = (Vector3){ size, size, -size };
    cube->vertices[3] = (Vector3){ -size, size, -size };
    cube->vertices[4] = (Vector3){ -size, -size, size };
    cube->vertices[5] = (Vector3){ size, -size, size };
    cube->vertices[6] = (Vector3){ size, size, size };
    cube->vertices[7] = (Vector3){ -size, size, size };
    
    cube->unfolded = false;
    cube->unfoldProgress = 0.0f;
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

void RenderCustomCube(Cube* cube) {
    Color faces[6] = {
        WHITE,  // Up (U)
        YELLOW, // Down (D)
        ORANGE, // Left (L)
        GREEN,  // Front (F)
        RED,    // Right (R)
        BLUE    // Back (B)
    };
    
    if (!cube->unfolded) {
        // Draw regular cube
        DrawCubeV((Vector3){0,0,0}, (Vector3){2,2,2}, WHITE);
        DrawCubeWiresV((Vector3){0,0,0}, (Vector3){2,2,2}, BLACK);
    } else {
        float progress = cube->unfoldProgress;
        float spacing = 2.1f; // Slight gap between faces
        float height = 2.0f * (1.0f - progress); // Height for folded faces
        
        // Face positions in unfolded state (on XZ plane)
        Vector3 positions[6] = {
            (Vector3){0, 0, -spacing},     // Up
            (Vector3){0, 0, spacing},      // Down
            (Vector3){-spacing, 0, 0},     // Left
            (Vector3){0, 0, 0},           // Front (center)
            (Vector3){spacing, 0, 0},      // Right
            (Vector3){spacing * 2, 0, 0}   // Back
        };
        
        // Draw each face
        for (int i = 0; i < 6; i++) {
            Vector3 pos = positions[i];
            pos.y = height * 0.5f; // Adjust height during animation
            
            DrawCubeV(pos, (Vector3){2.0f, 0.1f, 2.0f}, faces[i]);
            DrawCubeWiresV(pos, (Vector3){2.0f, 0.1f, 2.0f}, BLACK);
        }
    }
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "3D Cube Viewer");
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
        
        if (IsKeyPressed(KEY_SPACE)) {
            cube.unfolded = !cube.unfolded;
        }
        
        if (cube.unfolded && cube.unfoldProgress < 1.0f) {
            cube.unfoldProgress += 0.05f;
        } else if (!cube.unfolded && cube.unfoldProgress > 0.0f) {
            cube.unfoldProgress -= 0.05f;
        }
        
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