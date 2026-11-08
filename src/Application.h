// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

struct GLFWwindow;
class ShaderProgram;
class Model;
class DrawableObject;
class Scene;
class Transformation;
class TransformationComposite;
class Material;
class TextureManager;
class CubeMap;
class Skybox;

struct Bounds
{
    glm::vec3 min = glm::vec3(0.0f);
    glm::vec3 max = glm::vec3(0.0f);
};

class Application {
public:
    Application();
    ~Application();

    void initialization();
    void createShaders();
    void createModels();
    void run();

private:
    static void errorCallback(int error, const char* description);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double x, double y);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    void processInput(float deltaTime);
    void updateClock();
    void activateScene(std::size_t index);

    template <class T, class... Args>
    T* make(Args&&... args)
    {
        auto owned = std::make_unique<T>(std::forward<Args>(args)...);
        T* pointer = owned.get();
        transformations.push_back(std::move(owned));
        return pointer;
    }

    ShaderProgram* createProgram(const char* vertexPath, const char* fragmentPath);
    TransformationComposite* makeComposite(Scene* scene);
    DrawableObject* createObject(Model* model, ShaderProgram* program, Transformation* transformation);
    Scene* createScene();
    void addSignature(Scene* scene);
    void addClock(Scene* scene, const glm::vec3& position, float angleY, float scale);

    void plantForest(Scene* scene, ShaderProgram* treeShader, ShaderProgram* bushShader, const Material& material);
    void createForestScene();
    void createClockScene();
    void createSolarSystemScene();
    void createOrderScene();
    void createModelMatrixScene();
    void createLoginScene();
    void createSpheresScene();
    void createNormalsScene();
    void createBackSpecularScene();
    void createNightForestScene();
    void createShrekScene();
    void createTextureScene();
    void createClassroomScene();

    static std::string pathOrPlaceholder(const std::string& path, const std::string& placeholder);
    Bounds loadObject(Scene* scene, const std::string& path, const glm::vec3& position, float rotationY, float targetHeight, ShaderProgram* program, const Material& material);
    Skybox* getSkybox();
    void setSpheresProgram(ShaderProgram* program, const char* name);

    void releaseResources();

    GLFWwindow* window = nullptr;
    double lastCursorX = 0.0;
    double lastCursorY = 0.0;

    ShaderProgram* colorProgram = nullptr;
    ShaderProgram* normalProgram = nullptr;
    ShaderProgram* treeProgram = nullptr;
    ShaderProgram* bushProgram = nullptr;
    ShaderProgram* clockProgram = nullptr;
    ShaderProgram* hudProgram = nullptr;
    ShaderProgram* constantProgram = nullptr;
    ShaderProgram* lambertProgram = nullptr;
    ShaderProgram* phongProgram = nullptr;
    ShaderProgram* blinnProgram = nullptr;
    ShaderProgram* nightTreeProgram = nullptr;
    ShaderProgram* nightBushProgram = nullptr;
    ShaderProgram* skyboxProgram = nullptr;
    std::vector<ShaderProgram*> cameraPrograms;
    std::vector<ShaderProgram*> litPrograms;
    std::vector<DrawableObject*> testSpheres;
    std::size_t fovIndex = 2;

    int normalMode = 2;
    bool cutBackSpecular = true;
    bool correctResize = true;
    bool uvTest = false;

    Model* centeredTriangleModel = nullptr;
    Model* groundModel = nullptr;
    Model* plainModel = nullptr;
    Model* sphereModel = nullptr;
    Model* treeModel = nullptr;
    Model* bushModel = nullptr;
    Model* loginTextModel = nullptr;
    Model* loginColorModel = nullptr;
    Model* sunModel = nullptr;
    Model* earthModel = nullptr;
    Model* moonModel = nullptr;
    Model* clockModelGpu = nullptr;
    Model* rocketModel = nullptr;
    Model* skycubeModel = nullptr;
    Model* floorModel = nullptr;

    std::vector<std::unique_ptr<ShaderProgram>> shaderPrograms;
    std::vector<std::unique_ptr<Model>> models;
    std::vector<std::unique_ptr<Transformation>> transformations;
    std::vector<std::unique_ptr<DrawableObject>> objects;
    std::vector<std::unique_ptr<Scene>> scenes;
    std::unique_ptr<TextureManager> textureManager;
    std::vector<std::unique_ptr<CubeMap>> cubeMaps;
    std::vector<std::unique_ptr<Skybox>> skyboxes;
    std::size_t activeScene = 0;
};
