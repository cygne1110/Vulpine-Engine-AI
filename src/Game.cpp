#include <Game.hpp>
#include <Globals.hpp>
#include <GameObject.hpp>
#include <CompilingOptions.hpp>
#include <MathsUtils.hpp>
#include <Audio.hpp>
#include <NavGraph.hpp>
#include <Helpers.hpp>
#include <EntityAI.hpp>

#include <thread>
#include <fstream>

Game::Game(GLFWwindow *window) : App(window) {}

void Game::init(int paramSample)
{
    App::init();

    setIcon("ressources/icon.png");

    setController(&spectator);

    ambientLight = vec3(0.1);

    finalProcessingStage = ShaderProgram(
        "shader/post-process/final composing.frag",
        "shader/post-process/basic.vert",
        "",
        globals.standartShaderUniform2D());

    finalProcessingStage.addUniform(ShaderUniform(Bloom.getIsEnableAddr(), 10));

    camera.init(radians(70.0f), globals.windowWidth(), globals.windowHeight(), 0.1f, 1E4f);
    // camera.setMouseFollow(false);
    // camera.setPosition(vec3(0, 1, 0));
    // camera.setDirection(vec3(1, 0, 0));
    auto myfile = std::fstream("saves/cameraState.bin", std::ios::in | std::ios::binary);
    if(myfile)
    {
        CameraState buff;
        myfile.read((char*)&buff, sizeof(CameraState));
        myfile.close();
        camera.setState(buff);
    }

    /* Loading 3D Materials */
    depthOnlyMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/depthOnly.frag",
            "shader/foward/basic.vert",
            ""));

    depthOnlyStencilMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/depthOnlyStencil.frag",
            "shader/foward/basic.vert",
            ""));

    depthOnlyInstancedMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/depthOnlyStencil.frag",
            "shader/foward/basicInstance.vert",
            ""));

    GameGlobals::PBR = MeshMaterial(
        new ShaderProgram(
            "shader/foward/PBR.frag",
            "shader/foward/basic.vert",
            "",
            globals.standartShaderUniform3D()));

    GameGlobals::PBRstencil = MeshMaterial(
        new ShaderProgram(
            "shader/foward/PBR.frag",
            "shader/foward/basic.vert",
            "",
            globals.standartShaderUniform3D()));

    GameGlobals::PBRinstanced = MeshMaterial(
        new ShaderProgram(
            "shader/foward/PBR.frag",
            "shader/foward/basicInstance.vert",
            "",
            globals.standartShaderUniform3D()));

    skyboxMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/foward/Skybox.frag",
            "shader/foward/basic.vert",
            "",
            globals.standartShaderUniform3D()));

    GameGlobals::PBRstencil.depthOnly = depthOnlyStencilMaterial;
    GameGlobals::PBRinstanced.depthOnly = depthOnlyInstancedMaterial;
    scene.depthOnlyMaterial = depthOnlyMaterial;

    /* UI */
    FUIfont = FontRef(new FontUFT8);
    FUIfont->readCSV("ressources/fonts/Roboto/out.csv");
    FUIfont->setAtlas(Texture2D().loadFromFileKTX("ressources/fonts/Roboto/out.ktx"));
    defaultFontMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/2D/sprite.frag",
            "shader/2D/sprite.vert",
            "",
            globals.standartShaderUniform2D()));

    defaultSUIMaterial = MeshMaterial(
        new ShaderProgram(
            "shader/2D/fastui.frag",
            "shader/2D/fastui.vert",
            "",
            globals.standartShaderUniform2D()));

    fuiBatch = SimpleUiTileBatchRef(new SimpleUiTileBatch);
    fuiBatch->setMaterial(defaultSUIMaterial);
    fuiBatch->state.position.z = 0.0;
    fuiBatch->state.forceUpdate();

    NavGraphRef graph(new NavGraph(0));
    graph->addNode(vec3(0, 0, 0));
    graph->addNode(vec3(1, 0, 0));
    graph->addNode(vec3(1, 0, -1));
    graph->addNode(vec3(4, 0, -1));
    graph->addNode(vec3(4, 0, 0));
    graph->addNode(vec3(2, 0, 0));
    graph->addNode(vec3(2, 0, 1));
    graph->addNode(vec3(4, 0, 1));
    graph->addNode(vec3(0, 0, 2));
    graph->addNode(vec3(1, 0, 2));
    graph->addNode(vec3(1, 0, 3));
    graph->addNode(vec3(2, 0, 3));
    graph->addNode(vec3(2, 0, 2));
    graph->addNode(vec3(3, 0, 3));
    graph->addNode(vec3(3, 0, 2));
    graph->addNode(vec3(4, 0, 3));
    graph->addNode(vec3(4, 0, 2));
    graph->connectNodes(0, 1);
    graph->connectNodes(1, 2);
    graph->connectNodes(2, 3);
    graph->connectNodes(3, 4);
    graph->connectNodes(4, 5);
    graph->connectNodes(4, 7);
    graph->connectNodes(5, 6);
    graph->connectNodes(6, 7);
    graph->connectNodes(0, 8);
    graph->connectNodes(8, 9);
    graph->connectNodes(9, 10);
    graph->connectNodes(10, 11);
    graph->connectNodes(11, 12);
    graph->connectNodes(12, 14);
    graph->connectNodes(13, 14);
    graph->connectNodes(13, 15);
    graph->connectNodes(16, 15);
    graph->connectNodes(16, 7);

    vec3 start = vec3(0.0f, 0.0f, 0.0f);
    vec3 end = vec3(3.0f, 0.0f, 2.0f);

    Path path(start, end);
    path.update(graph);

    path.print();

    scene.add(NavGraphHelperRef(new NavGraphHelper(graph)));
    scene.add(PathHelperRef(new PathHelper(path, graph)));

    /* VSYNC and fps limit */
    globals.fpsLimiter.activate();
    globals.fpsLimiter.freq = 144.f;
    glfwSwapInterval(0);
}

bool Game::userInput(GLFWKeyInfo input)
{
    if (baseInput(input))
        return true;

    if (input.action == GLFW_PRESS)
    {
        switch (input.key)
        {
        case GLFW_KEY_ESCAPE:
            state = quit;
            break;

        case GLFW_KEY_F2:
            globals.currentCamera->toggleMouseFollow();
            break;

        case GLFW_KEY_1:
            Bloom.toggle();
            break;

        case GLFW_KEY_2:
            SSAO.toggle();
            break;

        case GLFW_KEY_F5:
            #ifdef _WIN32
            system("cls");
            #else
            system("clear");
            #endif

            finalProcessingStage.reset();
            Bloom.getShader().reset();
            SSAO.getShader().reset();
            depthOnlyMaterial->reset();
            GameGlobals::PBR->reset();
            GameGlobals::PBRstencil->reset();
            skyboxMaterial->reset();
            break;

        case GLFW_KEY_F8:
            {
                auto myfile = std::fstream("saves/cameraState.bin", std::ios::out | std::ios::binary);
                myfile.write((char*)&camera.getState(), sizeof(CameraState));
                myfile.close();
            }
                break;

        default:
            break;
        }
    }

    return true;
};

void Game::physicsLoop()
{
    physicsTicks.freq = 45.f;
    physicsTicks.activate();

    while (state != quit)
    {
        physicsTicks.start();

        physicsMutex.lock();
        physicsEngine.update(1.f / physicsTicks.freq);
        physicsMutex.unlock();

        physicsTicks.waitForEnd();
    }
}

void Game::mainloop()
{
    /* Loading Models and setting up the scene */
    ModelRef skybox = newModel(skyboxMaterial);
    skybox->loadFromFolder("ressources/models/skybox/", true, false);

    // skybox->invertFaces = true;
    skybox->depthWrite = true;
    skybox->state.frustumCulled = false;
    skybox->state.scaleScalar(1E6);
    scene.add(skybox);

    ModelRef floor = newModel(GameGlobals::PBR);
    floor->loadFromFolder("ressources/models/ground/");

    int gridSize = 10;
    int gridScale = 10;
    float floorY = -0.25;
    for (int i = -gridSize; i < gridSize; i++)
        for (int j = -gridSize; j < gridSize; j++)
        {
            ModelRef f = floor->copyWithSharedMesh();
            f->state
                .scaleScalar(gridScale)
                .setPosition(vec3(i * gridScale * 1.80, floorY, j * gridScale * 1.80));
            scene.add(f);
        }

    int forestSize = 0.;
    float treeScale = 0.5;

    ModelRef leaves = newModel(GameGlobals::PBRstencil);
    leaves->loadFromFolder("ressources/models/fantasy tree/");
    leaves->noBackFaceCulling = true;

    ModelRef trunk = newModel(GameGlobals::PBR);
    trunk->loadFromFolder("ressources/models/fantasy tree/trunk/");

    for (int i = -forestSize; i < forestSize; i++)
        for (int j = -forestSize; j < forestSize; j++)
        {
            ObjectGroupRef tree = newObjectGroup();
            tree->add(trunk->copyWithSharedMesh());
            ModelRef l = leaves->copyWithSharedMesh();
            l->noBackFaceCulling = true;
            tree->add(l);
            tree->state
                .scaleScalar(treeScale)
                .setPosition(vec3(i * treeScale * 40, 0, j * treeScale * 40));

            scene.add(tree);
        }

    /* Instanced Mesh example */
    // InstancedModelRef trunk = newInstancedModel();
    // trunk->setMaterial(PBRinstanced);
    // trunk->loadFromFolder("ressources/models/fantasy tree/trunk/");
    // trunk->allocate(2E4);

    // for(int i = -forestSize; i < forestSize; i++)
    // for(int j = -forestSize; j < forestSize; j++)
    // {
    //     ModelInstance &inst = *trunk->createInstance();
    //     inst.scaleScalar(treeScale)
    //         .setPosition(vec3(i*treeScale*40, 0, j*treeScale*40));
    //     inst.update();
    // }
    // trunk->updateInstances();
    // scene.add(trunk);

    SceneDirectionalLight sun = newDirectionLight(
        DirectionLight()
            .setColor(vec3(143, 107, 71) / vec3(255))
            .setDirection(normalize(vec3(-1.0, -1.0, 0.0)))
            .setIntensity(5.0));

    sun->cameraResolution = vec2(2048);
    sun->shadowCameraSize = vec2(90, 90);
    sun->activateShadows();
    scene.add(sun);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(3.0);

    /* Setting up the UI */
    FastUI_context ui(fuiBatch, FUIfont, scene2D, defaultFontMaterial);
    FastUI_valueMenu menu(ui, {});

    menu->state.setPosition(vec3(-0.9, 0.5, 0)).scaleScalar(0.8);
    globals.appTime.setMenuConst(menu);
    globals.cpuTime.setMenu(menu);
    globals.gpuTime.setMenu(menu);
    globals.fpsLimiter.setMenu(menu);

    

    // physicsTicks.setMenu(menu);
    // sun->setMenu(menu, U"Sun");

    BenchTimer cullTimer("Frustum Culling");
    cullTimer.setMenu(menu);

    menu.batch();
    scene2D.updateAllObjects();
    fuiBatch->batch();

    state = AppState::run;
    std::thread physicsThreads(&Game::physicsLoop, this);

    /* Music ! 
    AudioFile music1;
    music1.loadOGG("ressources/musics/Endless Space by GeorgeTantchev.ogg");

    AudioSource musicSource;
    musicSource
        .generate()
        .setBuffer(music1.getHandle())
        .setPosition(vec3(0, 0, 3))
        .play();

    // alSourcei(musicSource.getHandle(), AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(musicSource.getHandle(), AL_DIRECTION, 0.0, 0.0, 0.0);
    */

    // ModelRef lanterne = newModel(GameGlobals::PBR);
    // lanterne->loadFromFolder("ressources/models/lantern/");
    // lanterne->state
    //     .scaleScalar(0.01)
    //     .setPosition(vec3(2, 2, 0));
    // scene.add(lanterne);

    // Test Entity
    ObjectGroupRef EntityAIGroup = newObjectGroup();
    EntityAIGroup->add(SphereHelperRef(new SphereHelper(vec3(1.0f, 0.0f, 0.0f), 0.5f)));

    EntityRef entity = newEntity(
        "ALSAK LIVE REACTION",
        EntityModel(EntityAIGroup),
        EntityPosition3D(vec3(0, 5, 0), 0.1f),
        EntityDestination3D(vec3(5, 5, 5), true),
        EntityPathfinding()
    );

    /* Main Loop */
    while (state != AppState::quit)
    {
        mainloopStartRoutine();

        for (GLFWKeyInfo input; inputs.pull(input); userInput(input));

        menu.trackCursor();
        menu.updateText();

        mainloopPreRenderRoutine();

        /* UI & 2D Render */
        glEnable(GL_BLEND);
        glEnable(GL_FRAMEBUFFER_SRGB);

        scene2D.updateAllObjects();
        fuiBatch->batch();
        screenBuffer2D.activate();
        fuiBatch->draw();
        scene2D.cull();
        scene2D.draw();
        screenBuffer2D.deactivate();

        /* 3D Pre-Render */
        glDisable(GL_FRAMEBUFFER_SRGB);
        glDisable(GL_BLEND);
        glDepthFunc(GL_GREATER);
        glEnable(GL_DEPTH_TEST);

        scene.updateAllObjects();
        scene.generateShadowMaps();
        renderBuffer.activate();

        cullTimer.start();
        scene.cull();
        cullTimer.end();

        /* 3D Early Depth Testing */
        scene.depthOnlyDraw(*globals.currentCamera, true);
        glDepthFunc(GL_EQUAL);

        /* 3D Render */
        skybox->bindMap(0, 4);
        scene.genLightBuffer();
        scene.draw();
        renderBuffer.deactivate();

        /* Post Processing */
        renderBuffer.bindTextures();
        SSAO.render(*globals.currentCamera);
        Bloom.render(*globals.currentCamera);

        /* Final Screen Composition */
        glViewport(0, 0, globals.windowWidth(), globals.windowHeight());
        finalProcessingStage.activate();
        sun->shadowMap.bindTexture(0, 6);
        screenBuffer2D.bindTexture(0, 7);
        globals.drawFullscreenQuad();

        // Move towards goal
        System<EntityPosition3D, EntityDestination3D>([](Entity &entity){
            auto &pos = entity.comp<EntityPosition3D>();
            auto &dest = entity.comp<EntityDestination3D>();

            // CLAMP THE DISTANCE YEP
            if(dest.hasDestination) {
                pos.direction = normalize(dest.destination - pos.position);
                pos.position += pos.speed*pos.direction;
            }
        });

        // Update model position
        System<EntityModel, EntityPosition3D>([](Entity &entity){
            entity.comp<EntityModel>()->state.setPosition(
                entity.comp<EntityPosition3D>().position
            );
        });

        /* ECS Garbage Collector */
        ManageGarbage<EntityModel>();

        /* Main loop End */
        mainloopEndRoutine();
    }

    physicsThreads.join();
}
