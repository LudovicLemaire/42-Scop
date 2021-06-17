// gcc -I./include tamer.c src/gl3w.c -lglfw -framework Cocoa -framework OpenGL -framework IOKit -o tamer && ./tamer

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <wow.h>

const char *vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aCol;\n"
"out vec4 vertexColor;\n"
"uniform mat4 matriceFinal;\n"
"uniform mat4 matricePerspective;\n"
"uniform float pointSize;\n"
"void main() {\n"
"   gl_PointSize = pointSize;\n"
"   gl_Position = (matricePerspective * matriceFinal ) * vec4(aPos, 1.0);\n"
"   vertexColor = vec4(aCol, 1.0);\n"
"}";

const char *fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec3 rgb;\n"
"uniform float isColored;\n"
"in vec4 vertexColor;\n"
"void main() {\n"
"   float ambientStrength = 1;\n"
"   vec3 lightColor = rgb;\n"
"   vec3 ambient = ambientStrength * lightColor;\n"
"   vec3 objectColor = vec3(vertexColor.x, vertexColor.y, vertexColor.z);\n"
"   vec3 result;\n"
"   if (isColored == 0.0) {result = ambient * vec3(1.0, 1.0, 1.0);}\n"
"   else {result = ambient * objectColor;}\n"
"   FragColor = vec4(result, 1.0);\n"
"}";

t_array_mat m4_mult(t_array_mat a, t_array_mat b)
{
    t_array_mat     res;
    int        i;
    int        j;
    int        k; 
    
    for (i = 0; i < 4; i++) 
    { 
        for (j = 0; j < 4; j++) 
        { 
            res.res[i][j] = 0; 
            for (k = 0; k < 4; k++) 
                res.res[i][j] += a.res[i][k] * b.res[k][j]; 
        } 
    }

    return (res);
}

int shaderCreateProgram() { // refaire parsershader
    int     program = 0;
    int     vertex;
    int     fragment;
    char    log[512];
    int     success;


    vertex      = glCreateShader(GL_VERTEX_SHADER);
    fragment    = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex, 1, &vertexShaderSource, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, log);
        printf("[Vertex shader] Linking failed\n%s\n", log);
        exit(0);
    }
    glShaderSource(fragment, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, log);
        printf("[Fragment shader] Linking failed\n%s\n", log);
        exit(0);
    }

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, log);
        printf("[Program] Linking failed\n%s\n", log);
        exit(0);
    }
    return (program);

}

void checkKey(GLFWwindow *window, int key, int *stored_value) {
    /*
     1: PRESS
     2: REPEAT
     0: nothing
    -1: RELEASE
    */
    if (glfwGetKey(window, key) == GLFW_PRESS) {
        if (*stored_value == 0 || *stored_value == -1) {
            *stored_value = 1;
        } else if (*stored_value == 1) {
            *stored_value = 2;
        }
    } else {
        if (*stored_value == 1 || *stored_value == 2) {
            *stored_value = -1;
        } else if (*stored_value == -1) {
            *stored_value = 0;
        }
    }
}

// get random color
float getRC() {
    float color = (float)rand() / (float)RAND_MAX;
    return color;
}

void keyCallback(GLFWwindow *window, t_keys_hook *keys_hook, t_hook_params *hook_params) {
    checkKey(window, GLFW_KEY_R, &keys_hook->r);
    checkKey(window, GLFW_KEY_ESCAPE, &keys_hook->escape);
    checkKey(window, GLFW_KEY_T, &keys_hook->t);
    checkKey(window, GLFW_KEY_1, &keys_hook->one);
    checkKey(window, GLFW_KEY_2, &keys_hook->two);
    checkKey(window, GLFW_KEY_3, &keys_hook->three);
    checkKey(window, GLFW_KEY_MINUS, &keys_hook->minus);
    checkKey(window, GLFW_KEY_EQUAL, &keys_hook->plus);
    checkKey(window, GLFW_KEY_KP_ADD, &keys_hook->kp_plus);
    checkKey(window, GLFW_KEY_KP_SUBTRACT, &keys_hook->kp_minus);
    checkKey(window, GLFW_KEY_W, &keys_hook->w);
    checkKey(window, GLFW_KEY_A, &keys_hook->a);
    checkKey(window, GLFW_KEY_S, &keys_hook->s);
    checkKey(window, GLFW_KEY_D, &keys_hook->d);
    checkKey(window, GLFW_KEY_SPACE, &keys_hook->space);
    checkKey(window, GLFW_KEY_LEFT_CONTROL, &keys_hook->ctrl);
    checkKey(window, GLFW_KEY_KP_9, &keys_hook->kp_9);
    checkKey(window, GLFW_KEY_KP_8, &keys_hook->kp_8);
    checkKey(window, GLFW_KEY_KP_7, &keys_hook->kp_7);
    checkKey(window, GLFW_KEY_KP_6, &keys_hook->kp_6);
    checkKey(window, GLFW_KEY_KP_5, &keys_hook->kp_5);
    checkKey(window, GLFW_KEY_KP_4, &keys_hook->kp_4);
    checkKey(window, GLFW_KEY_KP_3, &keys_hook->kp_3);
    checkKey(window, GLFW_KEY_KP_2, &keys_hook->kp_2);
    checkKey(window, GLFW_KEY_KP_1, &keys_hook->kp_1);

    // exit
    if (keys_hook->escape == 1)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // polygon modes (Filled, Wireframe, Point)
    if (keys_hook->one == 1)
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
    if (keys_hook->two == 1)
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
    if (keys_hook->three == 1)
        glPolygonMode( GL_FRONT_AND_BACK, GL_POINT);

    // epileptic mode
    if (keys_hook->r == 2) 
        glClearColor(getRC(), getRC(), getRC(), 1);

    // point size
    if (keys_hook->plus == 2) 
        hook_params->pointSize += 0.1;
    if (keys_hook->minus == 2) 
        hook_params->pointSize -= 0.1;

    // translation
    if (keys_hook->w == 2) 
        hook_params->translationZ += hook_params->speed;
    if (keys_hook->s == 2) 
        hook_params->translationZ -= hook_params->speed;
    if (keys_hook->a == 2) 
        hook_params->translationX += hook_params->speed;
    if (keys_hook->d == 2) 
        hook_params->translationX -= hook_params->speed;
    if (keys_hook->space == 2) 
        hook_params->translationY -= hook_params->speed;
    if (keys_hook->ctrl == 2) 
        hook_params->translationY += hook_params->speed;

    // rotation
    if (keys_hook->kp_7 == 2) 
        hook_params->rotationZ -= hook_params->speed;
    if (keys_hook->kp_9 == 2) 
        hook_params->rotationZ += hook_params->speed;
    if (keys_hook->kp_4 == 2) 
        hook_params->rotationY -= hook_params->speed;
    if (keys_hook->kp_6 == 2) 
        hook_params->rotationY += hook_params->speed;
    if (keys_hook->kp_8 == 2) 
        hook_params->rotationX += hook_params->speed;
    if (keys_hook->kp_5 == 2) 
        hook_params->rotationX -= hook_params->speed;
    
    // light color
    if (keys_hook->kp_1 == 2) 
        hook_params->r += 0.01;
    if (keys_hook->kp_2 == 2) 
        hook_params->g += 0.01;
    if (keys_hook->kp_3 == 2) 
        hook_params->b += 0.01;

    // speed
    if (keys_hook->kp_plus == 1) 
        hook_params->speed += 0.025;
    if (keys_hook->kp_minus == 1) 
        hook_params->speed -= 0.025;
    
    // colored mode
    if (keys_hook->t == 1)
        hook_params->isColored = 1.0;
    if (keys_hook->t == -1)
        hook_params->isColored = 0.0;
}


void _update_fps_counter(GLFWwindow* window) {
    static double previous_seconds = 0;
    static int frame_count;
    double current_seconds = glfwGetTime();
    double elapsed_seconds = current_seconds - previous_seconds;
    if (elapsed_seconds > 0.25) {
        previous_seconds = current_seconds;
        double fps = (double)frame_count / elapsed_seconds;
        char tmp[128];
        sprintf(tmp, "Tamer la fenetre: %.0f fps", fps);
        glfwSetWindowTitle(window, tmp);
        frame_count = 0;
    }
    frame_count++;
}

t_array_mat matriceIdentity() {
    t_array_mat matrice;

    matrice.res[0][0] = 1;
    matrice.res[0][1] = 0;
    matrice.res[0][2] = 0;
    matrice.res[0][3] = 0;
    matrice.res[1][0] = 0;
    matrice.res[1][1] = 1;
    matrice.res[1][2] = 0;
    matrice.res[1][3] = 0;
    matrice.res[2][0] = 0;
    matrice.res[2][1] = 0;
    matrice.res[2][2] = 1;
    matrice.res[2][3] = 0;
    matrice.res[3][0] = 0;
    matrice.res[3][1] = 0;
    matrice.res[3][2] = 0;
    matrice.res[3][3] = 1;
    return matrice;
}

t_array_mat matrice_perspective(float near, float far, float fov, float ratio) {
    t_array_mat mat_perspective = matriceIdentity();
    float e = tan(fov / 2.0);

    mat_perspective.res[0][0] = 1.0f / (ratio * e);
    mat_perspective.res[1][1] = 1.0f / (e);
    mat_perspective.res[2][2] = (far + near) / (far - near) * -1;
    mat_perspective.res[2][3] = -1.0f;
    mat_perspective.res[3][2] = (2.0f * far * near) / (far - near) * -1;
    return mat_perspective;
}

t_array_mat matriceScale(float x, float y, float z) {
    t_array_mat mat_scale = matriceIdentity();

    mat_scale.res[0][0] = x;
    mat_scale.res[1][1] = y;
    mat_scale.res[2][2] = z;

    return mat_scale;
}

t_array_mat matriceTranslation(float x, float y, float z) {
    t_array_mat mat = matriceIdentity();

    mat.res[3][0] = x;
    mat.res[3][1] = y;
    mat.res[3][2] = z;

    return mat;
}

t_array_mat matriceRotation(float x, float y, float z) {
    t_array_mat mat_rotationX = matriceIdentity();
    t_array_mat mat_rotationY = matriceIdentity();
    t_array_mat mat_rotationZ = matriceIdentity();

    mat_rotationX.res[1][1] = cos(x);
    mat_rotationX.res[1][2] = -sin(x);
    mat_rotationX.res[2][1] = sin(x);
    mat_rotationX.res[2][2] = cos(x);

    mat_rotationY.res[0][0] = cos(y);
    mat_rotationY.res[0][2] = sin(y);
    mat_rotationY.res[2][0] = -sin(y);
    mat_rotationY.res[2][2] = cos(y);

    mat_rotationZ.res[0][0] = cos(z);
    mat_rotationZ.res[0][1] = -sin(z);
    mat_rotationZ.res[1][0] = sin(z);
    mat_rotationZ.res[1][1] = cos(z);

    return m4_mult(m4_mult(mat_rotationX, mat_rotationY), mat_rotationZ);
}


int main(int ac, char *av[]) {
    if (ac != 2) {
        printf("\x1b[91mT'AS PAS LINK LE NOM DU FICHIER GROS DÉBILE\x1b[0m\n");
        return 0;
    }
    GLFWwindow* window;

    srand(time(NULL));
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1366, 768, "Tamer la fenêtre", NULL, NULL); //2560, 1440


    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (gl3wInit()) {
        printf("failed to initialize OpenGL\n");
        return -1;
    }

    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    GLfloat *g_vertex_buffer_data;
    int bufferSize;
    t_obj_spec *mm;
    mm = calloc(1, sizeof(t_obj_spec));
    parser(&g_vertex_buffer_data, &bufferSize, av[1], mm);

    t_keys_hook *keys_hook;
    keys_hook = calloc(1, sizeof(t_keys_hook));
    
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // points
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // couleurs
    

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // glDisableVertexAttribArray(0);

    GLuint program = shaderCreateProgram();
    glUseProgram(program);


    GLint locRGB = glGetUniformLocation(program, "rgb");
    GLint locIsColored = glGetUniformLocation(program, "isColored");
    GLint locMatriceFinal = glGetUniformLocation(program, "matriceFinal");
    GLint locPointSize = glGetUniformLocation(program, "pointSize");
    GLint locMatricePerspective = glGetUniformLocation(program, "matricePerspective");
    t_array_mat mat_scale;
    t_array_mat mat_translation;
    t_array_mat mat_translation_center;
    t_array_mat mat_rotation;
    t_array_mat mat_final;
    t_array_mat mat_perspective = matrice_perspective(0.00001, 1000.0, 1.0472, 1366.0 / 768.0);

    // rescale
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
    hook_params->r = 1;
    hook_params->g = 1;
    hook_params->b = 1;
    hook_params->isColored = 0.0;
    mat_translation_center = matriceTranslation(-mm->x_center, -mm->y_center, -mm->z_center);
    free(mm);
    mm = NULL;


    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);


    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        _update_fps_counter(window);
        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glEnable(GL_DEPTH_TEST);

        keyCallback(window, keys_hook, hook_params);
      
        glUniform1f(locIsColored, hook_params->isColored);
        glUniform3f(locRGB, hook_params->r, hook_params->g, hook_params->b);
        glUniform1f(locPointSize, hook_params->pointSize);

        mat_scale = matriceScale(rescale, rescale, rescale);
        mat_translation = matriceTranslation(hook_params->translationX, hook_params->translationY,hook_params->translationZ);
        mat_rotation = matriceRotation(hook_params->rotationX, hook_params->rotationY,hook_params->rotationZ);
        mat_final = m4_mult(m4_mult(mat_translation_center, m4_mult(mat_scale, mat_rotation)), mat_translation);
        glUniformMatrix4fv(locMatriceFinal, 1, GL_FALSE, mat_final.res[0]);
        glUniformMatrix4fv(locMatricePerspective, 1, GL_FALSE, mat_perspective.res[0]);
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, bufferSize); // Starting from vertex 0; 3 vertices total -> 1 triangle


        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    // free before exit
    free(keys_hook);
    keys_hook = NULL;
    free(hook_params);
    hook_params = NULL;
    return 0;
}