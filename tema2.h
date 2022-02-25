#pragma once

#include <vector>
#include <fstream>

#include "components/simple_scene.h"
#include "lab_m1/tema2/lab_camera.h"

#define SPEED 3
#define SPEEDENEMY 1
#define LIFETIME 400
#define MAX_HEALTH 100
#define HEALTH_BAR_WIDTH 5
#define HEALTH_BAR_LENGTH 3


namespace m1
{

    class MazeWall {
    public:
        float x;
        float y;
        float z;

        MazeWall(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }


        ~MazeWall() {}
    };

    class MazeEnemy {
    public:
        float initialPositionX;
        float initialPositionY;
        float initialPositionZ;
        glm::vec3 initialPosition;
        glm::vec3 currentPosition;
        float speed;
        float dirX;
        float dirZ;
        bool hasCollided;
        float tx;
        float ty;
        float tz;
        float time;
        int destroyed;

        MazeEnemy(float initialPositionX, float initialPositionY, float initialPositionZ, float speed, float dirX, float dirZ, float time, int destroyed) {
            this->initialPositionX = initialPositionX;
            this->initialPositionY = initialPositionY;
            this->initialPositionZ = initialPositionZ;
            this->speed = speed;
            this->dirX = dirX;
            this->dirZ = dirZ;
            this->time = time;
            this->destroyed = destroyed;
        }


        ~MazeEnemy() {}
    };

    class ProjectilePlayer {
    public:
        float initialPositionX;
        float initialPositionY;
        float initialPositionZ;
        glm::vec3 initialPosition;
        glm::vec3 currentPosition;
        float speed;
        float lifetime;
        float dirX;
        float dirZ;
        bool hasCollided;
        float tx;
        float ty;
        float tz;

        ProjectilePlayer(float initialPositionX, float initialPositionY, float initialPositionZ, float speed, float lifetime, float dirX, float dirZ) {
            this->initialPositionX = initialPositionX;
            this->initialPositionY = initialPositionY;
            this->initialPositionZ = initialPositionZ;
            this->speed = speed;
            this->lifetime = lifetime;
            this->dirX = dirX;
            this->dirZ = dirZ;
        }


        ~ProjectilePlayer() {}
    };

    class tema2 : public gfxc::SimpleScene
    {
     public:
         tema2();
        ~tema2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
        void RenderSimpleMeshEnemy(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int isDestCurrentProjectile, float t);

        void setFirstPerson();
        void setThirdPerson();

        void ChooseMaze();
        void BuildMaze();
        void InitEnemiesAndWalls();
        void DrawMaze(float deltaTimeSeconds);
        void DrawAndMoveEnemies(float deltaTimeSeconds);

        void DrawPlayer();
        void CreateProjectile(glm::vec3 pos);
        void ShootProjectile(float deltaTimeSeconds);

        void CheckPlayerWallsCollision();
        void CheckPlayerEnemyCollision();
        void CheckProjectileEnemyCollision(ProjectilePlayer &projectile);
        void CheckProjectileWallCollision(ProjectilePlayer& projectile);

        void DrawHealthBarLines();
        void DrawTimeBarLines(float deltaTimeSeconds);
        void CheckIfPlayerExitedMaze();

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
        implementedHomework::Camera *camera;
        glm::mat4 projectionMatrix;
        bool attackMode;
        float r, l, b, t, angle, playerAngle, playerAngleL, playerAngleR;
        bool type;
        float playerX, playerY, playerZ;
        float playerInitX, playerInitZ;
        float helpX, helpZ;
        std::vector<ProjectilePlayer> projectiles;
        std::vector<MazeEnemy> enemies;
        std::vector<MazeWall> walls;
        glm::vec3 pos;
        float projectileAngle;
        int mazeType;
        int maze[10][10];
        int count0;
        std::vector<int> index;
        std::vector<int> iarray;
        std::vector<int> jarray;
        int playerPosInMazeX;
        int playerPosInMazeZ;
        int playerPosInMaze;
        bool collisionF, collisionB, collisionR, collisionL, collision;
        float playerTX, playerTZ;
        glm::vec2 healthBarCorner;
        float scaleFactor, scaleTime;
        int health;
        int time;
        float mazeMinX, mazeMaxX, mazeMinZ, mazeMaxZ;
        bool playerExitedMaze;
        bool isWPressed, isSPressed, isAPressed, isDPressed;

    };
}   // namespace m1
