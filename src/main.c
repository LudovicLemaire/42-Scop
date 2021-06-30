#include <scop.h>

int main(int ac, char *av[]) {
	// Exit if obj file is missing
    if (ac != 2) {
        printf("\x1b[91mT'AS PAS LINK LE NOM DU FICHIER GROS DÉBILE\x1b[0m\n");
        return 0;
    }
    GLFWwindow* window;

    srand(time(NULL));
    // Initialize GLFW library
    if (!glfwInit())
        return -1;
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(1920, 1080, "Tamer la fenêtre", NULL, NULL); //2560, 1440

	// Exit if window creation failed
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (gl3wInit()) {
		printf("\x1b[91mFailed to initialize OpenGL\x1b[0m\n");
        return -1;
    }

	// Print OpenGL version
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    GLfloat *g_vertex_buffer_data;
    int bufferSize;
    t_obj_spec *mm;
    mm = calloc(1, sizeof(t_obj_spec));
	if (!mm)
		mallocFailed("min/max failed");
	int hasVT = 0;
	// Get vertex buffer, and Min/Max to rescale
    parser(&g_vertex_buffer_data, &bufferSize, av[1], mm, &hasVT);

    t_keys_hook *keys_hook;
    keys_hook = calloc(1, sizeof(t_keys_hook));
    if (!keys_hook)
		mallocFailed("keys_hook failed");

    GLuint vertexbuffer;
    GLuint vao;

    glGenBuffers(1, &vertexbuffer);  
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(float), g_vertex_buffer_data, GL_STATIC_DRAW);
    free(g_vertex_buffer_data);
    g_vertex_buffer_data = NULL;

    //glVertexAttribPointer(position_shader, nombre_de_data_à_recuperer_par_pointeur, type, false, nombre_de_data_par_vertex, offset)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, BUFFER_LENGTH * sizeof(float), (void*)0); // points
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, BUFFER_LENGTH * sizeof(float), (void*)(3 * sizeof(float))); // couleurs random
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, BUFFER_LENGTH * sizeof(float), (void*)(6 * sizeof(float))); // couleurs MTL
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, BUFFER_LENGTH * sizeof(float), (void*)(9 * sizeof(float))); // normals
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, BUFFER_LENGTH * sizeof(float), (void*)(12 * sizeof(float))); // vertex texture
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, BUFFER_LENGTH * sizeof(float), (void*)(14 * sizeof(float))); // vertex texture MTL
	
    

    glEnableVertexAttribArray(0); // points
    glEnableVertexAttribArray(1); // couleurs random
    glEnableVertexAttribArray(2); // couleurs MTL
	glEnableVertexAttribArray(3); // normals
	glEnableVertexAttribArray(4); // vertex texture
	glEnableVertexAttribArray(5); // vertex texture MTL

    // glDisableVertexAttribArray(0);
	// Create program through shaders
    GLuint program = shaderCreateProgram();
    glUseProgram(program);

	// Generate uniforms
    GLint locRGB = glGetUniformLocation(program, "rgb");
    GLint locIsColored = glGetUniformLocation(program, "isColored");
	GLint locIsMtlColored = glGetUniformLocation(program, "isMtlColored");
	GLint locIsNoise = glGetUniformLocation(program, "isNoise");
	GLint locIsBnW = glGetUniformLocation(program, "isBnW");
	GLint locIsTextured = glGetUniformLocation(program, "isTextured");
    GLint locMatriceFinal = glGetUniformLocation(program, "matriceFinal");
    GLint locPointSize = glGetUniformLocation(program, "pointSize");
	GLint locTransitionTexture = glGetUniformLocation(program, "transitionTexture");
	GLint locTransitionNoise = glGetUniformLocation(program, "transitionNoise");
	GLint locTransitionBnW = glGetUniformLocation(program, "transitionBnW");
	GLint locTransitionMtlColor = glGetUniformLocation(program, "transitionMtlColor");
	GLint locTransitionRColor = glGetUniformLocation(program, "transitionRColor");
    GLint locMatricePerspective = glGetUniformLocation(program, "matricePerspective");
	GLint locTextureVType = glGetUniformLocation(program, "textureVType");
    t_array_mat mat_scale;
    t_array_mat mat_translation;
    t_array_mat mat_translation_center;
    t_array_mat mat_rotation;
    t_array_mat mat_final;
    t_array_mat mat_perspective = matrice_perspective(0.00001, 1000.0, 1.0472, 1920.0 / 1080.0);

    // Rescale object to current windows
    float rescale = 1;
    float y_length = mm->y_length;
    float x_length = mm->x_length;
    float z_length = mm->z_length;
    while ((y_length < 4 || x_length < 10 || z_length < 10) && (y_length < 6 && x_length < 12 && z_length < 12)) {
        rescale *= 1.1;
        y_length = mm->y_length *rescale;
        x_length = mm->x_length *rescale;
        z_length = mm->z_length *rescale;
    }
    while (y_length > 6 || x_length > 12 || z_length > 12) {
        rescale *= 0.9;
        y_length = mm->y_length *rescale;
        x_length = mm->x_length *rescale;
        z_length = mm->z_length *rescale;
    }

	// Initialize all Hook Params
    t_hook_params *hook_params;
    hook_params = calloc(1, sizeof(t_hook_params));
    hook_params->pointSize = 1;
    hook_params->translationX = 0;
    hook_params->translationY = 0;
    hook_params->translationZ = 0 - 5;
    hook_params->rotationX = 0;
    hook_params->rotationY = 0;
    hook_params->rotationZ = 0;
    hook_params->speed = 0.1;
    hook_params->rgb.r = 0.25; // 1
    hook_params->rgb.g = 0.25; // 0.75
    hook_params->rgb.b = 0.25; // 0.79
    hook_params->isColored = 0.0;
	hook_params->isMtlColored = 0.0;
	hook_params->isNoise = 0.0;
	hook_params->isBnW = 0.0;
	hook_params->isTextured = 0.0;
	hook_params->currTexture = 0;
	hook_params->textureVType = -1;
	if (hasVT == 1)
		hook_params->textureVType = 1;
    mat_translation_center = matriceTranslation(-mm->x_center, -mm->y_center, -mm->z_center);
    free(mm);
    mm = NULL;

	// Disable/Enable backface culling and Depth
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //lFrontFace(GL_CW);  
    //lEnable(GL_CULL_FACE);  
    //lCullFace(GL_FRONT);
	glDisable(GL_CULL_FACE);

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);

	// Initialize variable for transitions
	double previousTimeFpsCount = 0;
	double previousTime = 0;
	double transitionTexture = 0;
	double transitionNoise = 0;
	double transitionBnW = 0;
	double transitionMtlColor = 0;
	double transitionRColor = 0;

	int frameCount = 0;


	// Create and check if selected texture images exist
	hook_params->totalTexture = 14;
	const char *textures[hook_params->totalTexture];
	textures[0] = "texture/container.jpg";
	textures[1] = "texture/leaf.jpeg";
	textures[2] = "texture/wood.jpeg";
	textures[3] = "texture/minecraft.jpeg";
	textures[4] = "texture/psyduck.jpg";
	textures[5] = "texture/colorfull1.jpg";
	textures[6] = "texture/colorfull10.jpeg";
	textures[7] = "texture/rgb1.jpeg";
	textures[8] = "texture/colorfull2.jpg";
	textures[9] = "texture/colorfull4.jpg";
	textures[10] = "texture/rgb2.jpeg";
	textures[11] = "texture/hyper.jpg";
	textures[12] = "texture/wall.jpg";
	textures[13] = "/sgoinfre/Perso/lulemair/texture/you.jpeg";
	int lenTextures = hook_params->totalTexture;
	int currTexture = 0;
	while (--lenTextures >= 0) {
		if (access(textures[currTexture], F_OK ) != 0) {
			printf("\x1b[91mTexture Path\x1b[0m: %s\n", textures[currTexture]);
			exit(-1);
		}
		printf("\x1b[92mTexture Path\x1b[0m: %s\n", textures[currTexture]);
		++currTexture;
	}
	generateTextureMapping(textures[0]);

    
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handle Hooks
        keyCallback(window, keys_hook, hook_params, textures);
      
        glUniform1f(locIsColored, hook_params->isColored);
		glUniform1f(locIsMtlColored, hook_params->isMtlColored);
		glUniform1f(locIsNoise, hook_params->isNoise);
		glUniform1f(locIsBnW, hook_params->isBnW);
		glUniform1f(locIsTextured, hook_params->isTextured);
        glUniform3f(locRGB, hook_params->rgb.r, hook_params->rgb.g, hook_params->rgb.b);
        glUniform1f(locPointSize, hook_params->pointSize);
		if (hook_params->textureVType == 1)
			glUniform1f(locTextureVType, 1);
		else
			glUniform1f(locTextureVType, 0);

		// Matrice calcs
        mat_scale = matriceScale(rescale, rescale, rescale);
        mat_translation = matriceTranslation(hook_params->translationX, hook_params->translationY,hook_params->translationZ);
        mat_rotation = matriceRotation(hook_params->rotationX, hook_params->rotationY, hook_params->rotationZ);
        mat_final = m4_mult(m4_mult(mat_translation_center, m4_mult(mat_scale, mat_rotation)), mat_translation);
        glUniformMatrix4fv(locMatriceFinal, 1, GL_FALSE, mat_final.res[0]);
        glUniformMatrix4fv(locMatricePerspective, 1, GL_FALSE, mat_perspective.res[0]);

        // Draw the triangles
        glDrawArrays(GL_TRIANGLES, 0, bufferSize / BUFFER_LENGTH); // Starting from vertex 0; 3 vertices total -> 1 triangle

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();

		double currentTime = glfwGetTime();
    	frameCount++;

		// Handle Texture ratio
		if (hook_params->isTextured > 0) {
			transitionTexture = fmin(transitionTexture + (currentTime - previousTime), TIME_TRANSITION_S);
			glUniform1f(locTransitionTexture, transitionTexture / TIME_TRANSITION_S);
		} else {
			transitionTexture = fmax(transitionTexture - (currentTime - previousTime), 0.0);
			glUniform1f(locTransitionTexture, transitionTexture / TIME_TRANSITION_S);
		}
		// Handle Noise ratio
		if (hook_params->isNoise > 0) {
			transitionNoise = fmin(transitionNoise + (currentTime - previousTime), TIME_TRANSITION_S);
			glUniform1f(locTransitionNoise, transitionNoise / TIME_TRANSITION_S);
		} else {
			transitionNoise = fmax(transitionNoise - (currentTime - previousTime), 0.0);
			glUniform1f(locTransitionNoise, transitionNoise / TIME_TRANSITION_S);
		}
		// Handle Black n White ratio
		if (hook_params->isBnW > 0) {
			transitionBnW = fmin(transitionBnW + (currentTime - previousTime), TIME_TRANSITION_S);
			glUniform1f(locTransitionBnW, transitionBnW / TIME_TRANSITION_S);
		} else {
			transitionBnW = fmax(transitionBnW - (currentTime - previousTime), 0.0);
			glUniform1f(locTransitionBnW, transitionBnW / TIME_TRANSITION_S);
		}
		// Handle Mtl Color ratio
		if (hook_params->isMtlColored > 0) {
			transitionMtlColor = fmin(transitionMtlColor + (currentTime - previousTime), TIME_TRANSITION_S);
			glUniform1f(locTransitionMtlColor, transitionMtlColor / TIME_TRANSITION_S);
		} else {
			transitionMtlColor = fmax(transitionMtlColor - (currentTime - previousTime), 0.0);
			glUniform1f(locTransitionMtlColor, transitionMtlColor / TIME_TRANSITION_S);
		}
		// Handle Random Color ratio
		if (hook_params->isColored > 0) {
			transitionRColor = fmin(transitionRColor + (currentTime - previousTime), TIME_TRANSITION_S);
			glUniform1f(locTransitionRColor, transitionRColor / TIME_TRANSITION_S);
		} else {
			transitionRColor = fmax(transitionRColor - (currentTime - previousTime), 0.0);
			glUniform1f(locTransitionRColor, transitionRColor / TIME_TRANSITION_S);
		}

		// Calculate FPS
		if (currentTime - previousTimeFpsCount >= 1.0) {
			_show_fps_counter(window, frameCount);

			frameCount = 0;
			previousTimeFpsCount = currentTime;
		}
		previousTime = currentTime;
    }

    glfwTerminate();
    // free before exit
    free(keys_hook);
    keys_hook = NULL;
    free(hook_params);
    hook_params = NULL;
    return 0;
}