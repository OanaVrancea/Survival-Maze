#include "lab_m1/tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "lab_m1/tema2/object2Dtema2.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


tema2::tema2()
{
}


tema2::~tema2()
{
}

void tema2::setFirstPerson() {
    camera->TranslateForward(camera->distanceToTarget);
}

void tema2::setThirdPerson() {
    camera->TranslateForward(-camera->distanceToTarget);
}


void tema2::Init()
{
    playerAngle = 0.0f;
    attackMode = false;
    bool type = true;
    scaleFactor = 1;
    health = MAX_HEALTH;
    time = LIFETIME;
    scaleTime = 1;
    playerX = 0;
    playerZ = 3.5f;

    collisionB = false;
    collisionF = false;
    collisionL = false;
    collisionR = false;
    collision = false;

    playerExitedMaze = false;

    isAPressed = false;
    isWPressed = false;
    isSPressed = false;
    isDPressed = false;

    glm::ivec2 resolution = window->GetResolution();

    healthBarCorner.y = resolution.y - HEALTH_BAR_LENGTH;
    healthBarCorner.x = resolution.x - HEALTH_BAR_WIDTH;

    r = 10.0f;
    l = 0.5f;
    b = 0.5f;
    t = 10.0f;
    angle = 60.f;


    camera = new implementedHomework::Camera();
    camera->Set(glm::vec3(playerX, 2, playerZ), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));


    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // healthBar

     {
        Mesh* healthBarLines = object2Dtema2::CreateRectangle("healthBarLines", glm::vec3(0,0,0),HEALTH_BAR_WIDTH, HEALTH_BAR_LENGTH, glm::vec3(1,1,1));
        AddMeshToList(healthBarLines);
    }

    {
        Mesh* healthBarFilling = object2Dtema2::CreateRectangle("healthBarFilling", glm::vec3(0, 0, 0), HEALTH_BAR_WIDTH, HEALTH_BAR_LENGTH, glm::vec3(1,1,1));
        AddMeshToList(healthBarFilling);
    }

    //timeBar

    {
        Mesh* timeBarLines = object2Dtema2::CreateRectangle("timeBarLines", glm::vec3(0, 0, 0), HEALTH_BAR_WIDTH, HEALTH_BAR_LENGTH, glm::vec3(1, 1, 1));
        AddMeshToList(timeBarLines);
    }

    {
        Mesh* timeBarFilling = object2Dtema2::CreateRectangle("timeBarFilling", glm::vec3(0, 0, 0), HEALTH_BAR_WIDTH, HEALTH_BAR_LENGTH, glm::vec3(1, 1, 1));
        AddMeshToList(timeBarFilling);
    }

     {
        Shader* shader = new Shader("ShaderTema");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

     {
         Shader* healthBarShader = new Shader("healthBarShader");
         healthBarShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "HealthBarVertexShader.glsl"), GL_VERTEX_SHADER);
         healthBarShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "HealthBarFragmentShader.glsl"), GL_FRAGMENT_SHADER);
         healthBarShader->CreateAndLink();
         shaders[healthBarShader->GetName()] = healthBarShader;
     }

     {
         Shader* enemyShader = new Shader("enemyShader");
         enemyShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "EnemyVertexShader.glsl"), GL_VERTEX_SHADER);
         enemyShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "EnemyFragmentShader.glsl"), GL_FRAGMENT_SHADER);
         enemyShader->CreateAndLink();
         shaders[enemyShader->GetName()] = enemyShader;
     }


    // TODO(student): After you implement the changing of the projection
    // parameters, remove hardcodings of these parameters
    projectionMatrix = glm::perspective(RADIANS(90), window->props.aspectRatio, zNear, zFar);

    //choose a maze from a range of mazes
    ChooseMaze();
    BuildMaze();

    playerInitX = camera->GetTargetPosition().x;
    playerInitZ = camera->GetTargetPosition().z;

    playerPosInMaze = (std::rand() % (index.size() - 1 + 1));

    playerPosInMazeX = jarray.at(playerPosInMaze);
    playerPosInMazeZ = iarray.at(playerPosInMaze);

    InitEnemiesAndWalls();

}


void tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void tema2::Update(float deltaTimeSeconds)
{
    //render chosen maze
    DrawMaze(deltaTimeSeconds);
   
    //draw player
    if (attackMode == false) {
        DrawPlayer();
    }

    //create health bar
    DrawHealthBarLines();
    //create time bar
    DrawTimeBarLines(deltaTimeSeconds);
    //check if the player found an exit
    CheckIfPlayerExitedMaze();

    CheckPlayerWallsCollision();

    DrawAndMoveEnemies(deltaTimeSeconds);

    CheckPlayerEnemyCollision();

    ShootProjectile(deltaTimeSeconds);

    if (health == 0 || scaleTime <= 0 || playerExitedMaze == true) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cout << "Game over ! " << endl;
        exit(0);
    }

}


void tema2::ChooseMaze() {
    mazeType = 1 + (std::rand() % (3 - 1 + 1));
}

void tema2::BuildMaze() {
    int nr = 0;
    if (mazeType == 1) {
        int number;
        ifstream file1("src/lab_m1/tema2/maze1.txt");
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                maze[i][j] = 0;
            }
        }

        if (!file1.is_open()) {
            cout << "Could not open the file" << endl;
        }

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10;j++) {
                 file1 >> number;
                 maze[i][j] = number;
                 if (number == 0) {
                     count0++;
                     iarray.push_back(i);
                     jarray.push_back(j);
                     index.push_back(nr);
                     nr++;
                 }
             }
        }

        file1.close();
    }
    else if (mazeType == 2) {

        int number;
        ifstream file2("src/lab_m1/tema2/maze2.txt");
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                maze[i][j] = 0;
            }
        }

        if (!file2.is_open()) {
            cout << "Could not open the file" << endl;
        }

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                file2 >> number;
                maze[i][j] = number;
                if (number == 0) {
                    count0++;
                    iarray.push_back(i);
                    jarray.push_back(j);
                    index.push_back(nr);
                    nr++;
                }
            }
        }

        file2.close();
    } 
    else if (mazeType == 3) {

        int number;
        ifstream file3("src/lab_m1/tema2/maze3.txt");
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                maze[i][j] = 0;
            }
        }

        if (!file3.is_open()) {
            cout << "Could not open the file" << endl;
        }

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                file3 >> number;
                maze[i][j] = number;
                if (number == 0) {
                    count0++;
                    iarray.push_back(i);
                    jarray.push_back(j);
                    index.push_back(nr);
                    nr++;
                }
            }
        }

        file3.close();
    }

}

void tema2::InitEnemiesAndWalls() {
    float x = playerInitX - playerPosInMazeX * 1;
    float y = 0.5f;
    float z = playerInitZ - playerPosInMazeZ * 1 - 1;
    mazeMinX = x - 1;
    mazeMaxX = x + 10;
    mazeMinZ = z - 1;
    mazeMaxZ = z + 10;
    for (int i = 0; i < 10; i++) {
         for (int j = 0; j < 10; j++) {
             if (maze[i][j] == 2) {
                float ex = x - 0.5f;
                float ey = y;
                float ez = z - 0.5f;
                MazeEnemy e = MazeEnemy(ex, ey, ez, SPEEDENEMY, 1, 1, 10.0f , 0);
                e.currentPosition.x = e.initialPositionX;
                e.currentPosition.y = e.initialPositionY;
                e.currentPosition.z = e.initialPositionZ;
                enemies.push_back(e);
             }
             else if (maze[i][j] == 1) {
                    float wx = x;
                    float wy = y;
                    float wz = z;
                    MazeWall w = MazeWall(wx, wy, wz);
                    walls.push_back(w);
                }
                x = x + 1;
            }
          x = playerInitX - playerPosInMazeX * 1;
          z = z + 1;
        }
}

void tema2::DrawTimeBarLines(float deltaTimeSeconds)
{
    // Turn on wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Draw the timeBar
    glm::mat4 modelMatrix2D = glm::mat3(1);
    //modelMatrix2D = glm::translate(modelMatrix2D, glm::vec3(1,1,0));
    modelMatrix2D = glm::scale(modelMatrix2D, glm::vec3(0.1f, 0.1f, 0.1f));
    modelMatrix2D = glm::translate(modelMatrix2D, glm::vec3(-10, 7, 0));
    RenderSimpleMesh(meshes["timeBarLines"], shaders["healthBarShader"], modelMatrix2D, glm::vec3(1, 1, 1));

    // Turn off wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // glm::mat4 modelMatrix2D;
    modelMatrix2D = glm::mat4(1);
    modelMatrix2D = glm::scale(modelMatrix2D, glm::vec3(0.1f, 0.1f, 0.1f));
    modelMatrix2D = glm::translate(modelMatrix2D, glm::vec3(-10, 7, 0));
    modelMatrix2D = glm::scale(modelMatrix2D, glm::vec3(scaleTime, 1, 1));

    RenderSimpleMesh(meshes["timeBarFilling"], shaders["healthBarShader"], modelMatrix2D, glm::vec3(0, 0, 1));

    if (scaleTime > 0) {
        scaleTime -= 0.02f * deltaTimeSeconds;
    }

}


void tema2::DrawHealthBarLines()
{
    // Turn on wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Draw the healthBar
    glm::mat4 modelMatrix2D = glm::mat3(1);
    //modelMatrix2D = glm::translate(modelMatrix2D, glm::vec3(1,1,0));
    modelMatrix2D = glm::scale(modelMatrix2D, glm::vec3(0.1f, 0.1f, 0.1f));
    modelMatrix2D = glm::translate(modelMatrix2D, glm::vec3(5, 7, 0));
    RenderSimpleMesh(meshes["healthBarLines"], shaders["healthBarShader"], modelMatrix2D, glm::vec3(1, 1, 1));

    // Turn off wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // glm::mat4 modelMatrix2D;
    modelMatrix2D = glm::mat4(1);
    modelMatrix2D = glm::scale(modelMatrix2D, glm::vec3(0.1f, 0.1f, 0.1f));
    modelMatrix2D = glm::translate(modelMatrix2D, glm::vec3(5, 7, 0));
    modelMatrix2D = glm::scale(modelMatrix2D, glm::vec3(scaleFactor, 1, 1));

    RenderSimpleMesh(meshes["healthBarFilling"], shaders["healthBarShader"], modelMatrix2D, glm::vec3(1, 0, 0));

}


void tema2::DrawMaze(float deltaTimeSeconds) {

    glm::mat4 modelMatrix = glm::mat4(1);
   // modelMatrix = glm::translate(modelMatrix, glm::vec3((mazeMinX + mazeMaxX) / 2.0f , 0, (mazeMinZ + mazeMaxZ) / 2.0f));
  //  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f, 0, 0.4f));
    RenderSimpleMesh(meshes["plane"], shaders["ShaderTema"], modelMatrix, glm::vec3(0.8f, 0.8f, 0.5f));

    float x = playerInitX - playerPosInMazeX * 1;
    float y = 0.5f;
    //trebuie 1 offset din cauza pozitiei initiale 
    float z = playerInitZ - playerPosInMazeZ * 1 - 1;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (maze[i][j] == 1) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 2, 2));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(x,y,z));
                RenderSimpleMesh(meshes["box"], shaders["ShaderTema"], modelMatrix, glm::vec3(0, 0.5f, 0.1f));
            }
                x = x + 1;
        }
        x = playerInitX - playerPosInMazeX * 1;
        z = z + 1;
    }
}


void tema2::DrawPlayer() {
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f, 0.35f, 0.35f));
        modelMatrix = glm::rotate(modelMatrix, playerAngle, glm::vec3(0, 1, 0));
        RenderSimpleMesh(meshes["box"], shaders["ShaderTema"], modelMatrix, glm::vec3(0.8f, 0.7f, 0.5f));
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() - glm::vec3(0, 0.425, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f, 0.5f, 0.35f));
        modelMatrix = glm::rotate(modelMatrix, playerAngle, glm::vec3(0, 1, 0));
        RenderSimpleMesh(meshes["box"], shaders["ShaderTema"], modelMatrix, glm::vec3(1, 0, 0));
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.25f, 0.2f));
        modelMatrix = glm::rotate(modelMatrix, playerAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, -glm::vec3(-1.25f, 1.3f, 0.f));
        RenderSimpleMesh(meshes["box"], shaders["ShaderTema"], modelMatrix, glm::vec3(1, 0, 0));
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.25f, 0.2f));
        modelMatrix = glm::rotate(modelMatrix, playerAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, -glm::vec3(1.25f, 1.3f, 0.f));
        RenderSimpleMesh(meshes["box"], shaders["ShaderTema"], modelMatrix, glm::vec3(1, 0, 0));
    }


    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.25f, 0.2f));
        modelMatrix = glm::rotate(modelMatrix, playerAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, -glm::vec3(-1.25f, 2.3f, 0.f));
        RenderSimpleMesh(meshes["box"], shaders["ShaderTema"], modelMatrix, glm::vec3(0.8f, 0.7f, 0.5f));
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.25f, 0.2f));
        modelMatrix = glm::rotate(modelMatrix, playerAngle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, -glm::vec3(1.25f, 2.3f, 0.f));
        RenderSimpleMesh(meshes["box"], shaders["ShaderTema"], modelMatrix, glm::vec3(0.8f, 0.7f, 0.5f));
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() - glm::vec3(0, 0.925, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f, 0.5f, 0.35f));
        modelMatrix = glm::rotate(modelMatrix, playerAngle, glm::vec3(0, 1, 0));
        RenderSimpleMesh(meshes["box"], shaders["ShaderTema"], modelMatrix, glm::vec3(0, 0, 1));
    }


    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() - glm::vec3(0, 1.275, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f, 0.2f, 0.35f));
        modelMatrix = glm::rotate(modelMatrix, playerAngle, glm::vec3(0, 1, 0));
        RenderSimpleMesh(meshes["box"], shaders["ShaderTema"], modelMatrix, glm::vec3(0.4f, 0.2f, 0.1f));
    }

}

void tema2::DrawAndMoveEnemies(float deltaTimeSeconds) {

    for (int counterEnemies = 0; counterEnemies < enemies.size(); counterEnemies++) {

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2, 2, 2));

        float startX = enemies[counterEnemies].currentPosition.x;
        float startY = enemies[counterEnemies].initialPositionY;
        float startZ = enemies[counterEnemies].currentPosition.z;

        modelMatrix = glm::translate(modelMatrix, glm::vec3(startX , startY, startZ));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

        if (enemies[counterEnemies].hasCollided == true) {
            enemies[counterEnemies].destroyed = 1;
            enemies[counterEnemies].time -= 0.1f;
        }

        RenderSimpleMeshEnemy(meshes["sphere"], shaders["enemyShader"], modelMatrix, glm::vec3(0.5f, 0, 0.8f), enemies[counterEnemies].destroyed,
                               enemies[counterEnemies].time);

        if (startX <= enemies[counterEnemies].initialPositionX + 1 && startZ <= enemies[counterEnemies].initialPositionZ ) {
            enemies[counterEnemies].currentPosition.x += deltaTimeSeconds * SPEEDENEMY;
            startX = enemies[counterEnemies].currentPosition.x;
        }
        else if (startX >= enemies[counterEnemies].initialPositionX + 1 && startZ <= enemies[counterEnemies].initialPositionZ + 1) {
            enemies[counterEnemies].currentPosition.z += deltaTimeSeconds * SPEEDENEMY;
            startZ = enemies[counterEnemies].currentPosition.z;
        }
        else if (startX >= enemies[counterEnemies].initialPositionX  && startZ >= enemies[counterEnemies].initialPositionZ) {
            enemies[counterEnemies].currentPosition.x +=  (-1) * deltaTimeSeconds * SPEEDENEMY;
            startX = enemies[counterEnemies].currentPosition.x;
        }
        else if (startX <= enemies[counterEnemies].initialPositionX && startZ >= enemies[counterEnemies].initialPositionZ) {
            enemies[counterEnemies].currentPosition.z += (-1) * deltaTimeSeconds * SPEEDENEMY;
            startZ = enemies[counterEnemies].currentPosition.z;
        }

        if (enemies[counterEnemies].time <= 0) {
            enemies.erase(enemies.begin() + counterEnemies);
        }
    }
}


void tema2::CreateProjectile(glm::vec3 pos) {

    glm::vec3 initPosition = pos;

    initPosition.x = pos.x;
    initPosition.y = pos.y;
    initPosition.z = pos.z;

    float x = camera->forward.x;
    float z = camera->forward.z;

    ProjectilePlayer currentProjectile = ProjectilePlayer(initPosition.x, initPosition.y, initPosition.z, SPEED, LIFETIME, x, z);

    currentProjectile.hasCollided = false;
    currentProjectile.tx = 0;
    currentProjectile.ty = 0;
    currentProjectile.tz = 0;
    projectiles.push_back(currentProjectile);     
}

void tema2::ShootProjectile(float deltaTimeSeconds) {

    for (int i = 0; i < projectiles.size(); i++) {

        projectiles[i].initialPosition.x = projectiles[i].initialPositionX;
        projectiles[i].initialPosition.y = projectiles[i].initialPositionY;
        projectiles[i].initialPosition.z = projectiles[i].initialPositionZ;

        glm::vec3 translation = glm::vec3(projectiles[i].tx, projectiles[i].ty, projectiles[i].tz);

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, projectiles[i].initialPosition + translation);
        RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema"], modelMatrix, glm::vec3(0.f, 0.9f, 1.0f));

        projectiles[i].currentPosition = projectiles[i].initialPosition + translation;
        projectiles[i].tx += projectiles[i].dirX * deltaTimeSeconds * SPEED;
        projectiles[i].tz += projectiles[i].dirZ * deltaTimeSeconds * SPEED;

        projectiles[i].lifetime -= 0.5f;

        CheckProjectileEnemyCollision(projectiles[i]);

        CheckProjectileWallCollision(projectiles[i]);

        if (projectiles[i].hasCollided == true || projectiles[i].lifetime <= 0) {
            projectiles.erase(projectiles.begin() + i);
        }

    }

}

void tema2::CheckPlayerWallsCollision() {

    float sphereX = camera->GetTargetPosition().x / 2.0f;
    float sphereY = 0;
    float sphereZ = camera->GetTargetPosition().z / 2.0f;

    for (int i = 0; i < walls.size(); i++) {

        float minX = walls[i].x - 0.5f;
        float minY = 0;
        float minZ = walls[i].z - 0.5f;

        float maxX = walls[i].x + 0.5f;
        float maxY = 0;
        float maxZ = walls[i].z + 0.5f;

        float x = fmax(minX, fmin(sphereX, maxX));
        float y = fmax(minY, fmin(sphereY, maxY));
        float z = fmax(minZ, fmin(sphereZ, maxZ));

        float dist = sqrt((x - sphereX) * (x - sphereX) +
            (z - sphereZ) * (z - sphereZ));

        if (dist < 0.1f && isAPressed == true) {
            collisionL = true;
            collisionR = false;
            collisionB = false;
            collisionF = false;
        }
        else if (dist < 0.1f && isDPressed == true) {
            collisionL = false;
            collisionR = true;
            collisionB = false;
            collisionF = false;
        }
        else if (dist < 0.1f && isSPressed == true) {
            collisionL = false;
            collisionR = false;
            collisionB = true;
            collisionF = false;
        }
        else if (dist < 0.1f && isWPressed == true) {
            collisionL = false;
            collisionR = false;
            collisionB = false;
            collisionF = true;
        }

    }

}

void tema2::CheckPlayerEnemyCollision() {
    for (int i = 0; i < enemies.size(); i++) {

        float dist = sqrt((camera->GetTargetPosition().x /2.0f - enemies[i].currentPosition.x) *
            (camera->GetTargetPosition().x /2.0f - enemies[i].currentPosition.x) +
            (camera->GetTargetPosition().z /2.0f - enemies[i].currentPosition.z) *
            (camera->GetTargetPosition().z /2.0f - enemies[i].currentPosition.z));

        if (dist <= 0.1f) {
            health = health - 1;
            scaleFactor = (double)(health % MAX_HEALTH) / 100;
        }
    }
}

void tema2::CheckProjectileEnemyCollision(ProjectilePlayer& projectile) {

    for (int i = 0; i < enemies.size(); i++) {
        float dist = sqrt((projectile.currentPosition.x / 3.0f - enemies[i].currentPosition.x) * (projectile.currentPosition.x / 3.0f - enemies[i].currentPosition.x) +
            (projectile.currentPosition.y / 3.0f - enemies[i].currentPosition.y) * (projectile.currentPosition.y / 3.0f - enemies[i].currentPosition.y) +
            (projectile.currentPosition.z / 3.0f - enemies[i].currentPosition.z) * (projectile.currentPosition.z /3.0f - enemies[i].currentPosition.z));
        if (dist < 0.75f) {
            projectile.hasCollided = true;
            enemies[i].hasCollided = true;
        }
    }
}

void tema2::CheckProjectileWallCollision(ProjectilePlayer& projectile) {

    for (int i = 0; i < walls.size(); i++) {

        float sphereX = projectile.initialPosition.x + projectile.tx / 3.0f;
        float sphereY = 0;
        float sphereZ = projectile.initialPosition.z + projectile.tz / 3.0f;

        float minX = walls[i].x - 0.5f;
        float minY = 0;
        float minZ = walls[i].z - 0.5f;

        float maxX = walls[i].x + 0.5f;
        float maxY = 0;
        float maxZ = walls[i].z + 0.5f; 

        float x = fmax(minX, fmin(sphereX, maxX));
        float y = fmax(minY, fmin(sphereY, maxY));
        float z = fmax(minZ, fmin(sphereZ, maxZ));

        float dist = sqrt((x - sphereX) * (x - sphereX) +
            (z - sphereZ) * (z - sphereZ));

        if (dist < 0.1f) {
            projectile.hasCollided = true;
        }

    }
}

void tema2::CheckIfPlayerExitedMaze() {
    if (camera->GetTargetPosition().x / 2.0f >= mazeMaxX || camera->GetTargetPosition().x / 2.0f <= mazeMinX ||
        camera->GetTargetPosition().z / 2.0f >= mazeMaxZ || camera->GetTargetPosition().z / 2.0f <= mazeMinZ) {
        playerExitedMaze = true;
    }
}


void tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    int colorLocation = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(colorLocation, color.r, color.g, color.b);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void tema2::RenderSimpleMeshEnemy(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int isDestCurrentEnemy, float t)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    GLint isDestroyed = glGetUniformLocation(shader->program, "isDestroyed");
    glUniform1i(isDestroyed, isDestCurrentEnemy);

    float time = glGetUniformLocation(shader->program, "time");
    glUniform1f(time, t);

    int colorLocation = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(colorLocation, color.r, color.g, color.b);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void tema2::OnInputUpdate(float deltaTime, int mods)
{

    float cameraSpeed = 2.0f;
    if (attackMode == false) {
        if (window->KeyHold(GLFW_KEY_W) && collisionF == false) {
            // TODO(student): Translate the camera forward
            isWPressed = true;
          //  isAPressed = false;
           // isSPressed = false;
          //  isDPressed = false;
            camera->MoveForward(deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_S) && collisionB == false) {
            // TODO(student): Translate the camera forward
         //   isWPressed = false;
         //   isAPressed = false;
            isSPressed = true;
         //   isDPressed = false;
            camera->MoveForward(-deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_A) && collisionL == false) {
            // TODO(student): Translate the camera forward
         //   isWPressed = false;
            isAPressed = true;
         //   isSPressed = false;
         //   isDPressed = false;
            camera->TranslateRight(-deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_D) && collisionR == false) {
            // TODO(student): Translate the camera forward
          //  isWPressed = false;
          //  isAPressed = false;
          //  isSPressed = false;
            isDPressed = true;
            camera->TranslateRight(deltaTime * cameraSpeed);
        }
    }

}


void tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
  

}


void tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
    if (key == GLFW_KEY_W) {
        isWPressed = false;
    }
    if (key == GLFW_KEY_A) {
        isAPressed = false;
    }
    if (key == GLFW_KEY_S) {
        isSPressed = false;
    }
    if (key == GLFW_KEY_D) {
        isDPressed = false;
    }
}


void tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
 
    float sensivityOX = 0.001f;
    float sensivityOY = 0.003f;

     if (attackMode == true) {
         camera->RotateFirstPerson_OX(sensivityOX * -deltaY);
         camera->RotateFirstPerson_OY(sensivityOY * -deltaX);
     }

     if (attackMode == false) {
         glm::vec3 c = camera->forward;
         glm::vec3 p = glm::vec3(0, 0, -1);
         float dotProduct = c.x * p.x + c.y * p.y + c.z * p.z;
         float magnitudeC = sqrt(c.x * c.x + c.y * c.y + c.z * c.z);
         float magnitudeP = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
         float magProduct = magnitudeC * magnitudeP;
         playerAngle = acos(dotProduct / magProduct);
         camera->RotateThirdPerson_OX(sensivityOX * -deltaY);
         camera->RotateThirdPerson_OY(sensivityOY * -deltaX);
      }

}


void tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if(IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
    {
        attackMode = !attackMode;
        if (attackMode == false) {
            setThirdPerson();
        }
        else {
            setFirstPerson();
        }
    }

    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        if (attackMode == true) {
            pos = camera->GetTargetPosition();
            CreateProjectile(pos);
        }
    }
}


void tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void tema2::OnWindowResize(int width, int height)
{
}
