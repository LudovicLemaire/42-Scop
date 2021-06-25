#include <scop.h>
#include <GLFW/glfw3.h>

void keyCallback(GLFWwindow *window, t_keys_hook *keys_hook, t_hook_params *hook_params, const char **textures) {
    checkKey(window, GLFW_KEY_R, &keys_hook->r);
    checkKey(window, GLFW_KEY_ESCAPE, &keys_hook->escape);
    checkKey(window, GLFW_KEY_T, &keys_hook->t);
    checkKey(window, GLFW_KEY_1, &keys_hook->one);
    checkKey(window, GLFW_KEY_2, &keys_hook->two);
    checkKey(window, GLFW_KEY_3, &keys_hook->three);
	checkKey(window, GLFW_KEY_4, &keys_hook->four);
	checkKey(window, GLFW_KEY_KP_MULTIPLY, &keys_hook->kp_multiply);
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
	checkKey(window, GLFW_KEY_LEFT_SHIFT, &keys_hook->shift);
	checkKey(window, GLFW_KEY_N, &keys_hook->n);
	checkKey(window, GLFW_KEY_B, &keys_hook->b);
	checkKey(window, GLFW_KEY_M, &keys_hook->m);
	checkKey(window, GLFW_KEY_C, &keys_hook->c);

    // Exit program on escape key
    if (keys_hook->escape == 1)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Polygon modes (Filled, Wireframe, Point)
    if (keys_hook->one == 1)
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
    if (keys_hook->two == 1)
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
    if (keys_hook->three == 1)
        glPolygonMode( GL_FRONT_AND_BACK, GL_POINT);
	// Switch textures file
	if (keys_hook->four == 1) {
		if (++hook_params->currTexture > hook_params->totalTexture-1)
			hook_params->currTexture = 0;
        generateTextureMapping(textures[hook_params->currTexture]);
	}
	// Switch texture vertex between mtl and custom
	if (keys_hook->kp_multiply == 1)
		hook_params->textureVType *= -1;

    // Epileptic mode 🌈
    if (keys_hook->r == 2) 
        glClearColor(getRC(), getRC(), getRC(), 1);

    // Change points size
    if (keys_hook->plus == 2) 
        hook_params->pointSize += 0.1;
    if (keys_hook->minus == 2) 
        hook_params->pointSize -= 0.1;

    // Translation on 3 axes
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

    // Rotation on 3 axes
    if (keys_hook->kp_7 == 2) 
        hook_params->rotationZ -= hook_params->speed * 0.35;
    if (keys_hook->kp_9 == 2) 
        hook_params->rotationZ += hook_params->speed * 0.35;
    if (keys_hook->kp_4 == 2) 
        hook_params->rotationY -= hook_params->speed * 0.35;
    if (keys_hook->kp_6 == 2) 
        hook_params->rotationY += hook_params->speed * 0.35;
    if (keys_hook->kp_8 == 2) 
        hook_params->rotationX += hook_params->speed * 0.35;
    if (keys_hook->kp_5 == 2) 
        hook_params->rotationX -= hook_params->speed * 0.35;
    
    // Modify ambient light color
    if (keys_hook->kp_1 == 2) 
        hook_params->rgb.r += 0.01;
    if (keys_hook->kp_2 == 2) 
        hook_params->rgb.g += 0.01;
    if (keys_hook->kp_3 == 2) 
        hook_params->rgb.b += 0.01;

    // Modify speed for rotation/translation, with a boost with Shift
	if (keys_hook->kp_plus == 1)
		hook_params->speed += 0.025;
	if (keys_hook->kp_minus == 1)
        hook_params->speed -= 0.025;
	if (keys_hook->shift == 1)
        hook_params->speed += 0.5;
	if (keys_hook->shift == -1)
        hook_params->speed -= 0.5;
    
    // Random color per face
    if (keys_hook->c == 1)
        hook_params->isColored = 1.0;
    if (keys_hook->c == -1)
        hook_params->isColored = 0.0;
	// Color through MTL file
    if (keys_hook->m == 1)
        hook_params->isMtlColored = 1.0;
    if (keys_hook->m == -1)
        hook_params->isMtlColored = 0.0;
	// Noise mode
    if (keys_hook->n == 1)
        hook_params->isNoise = 1.0;
    if (keys_hook->n == -1)
        hook_params->isNoise = 0.0;
	// Black n White mode
    if (keys_hook->b == 1)
        hook_params->isBnW = 1.0;
    if (keys_hook->b == -1)
        hook_params->isBnW = 0.0;
	// Texture mode from image
    if (keys_hook->t == 1)
        hook_params->isTextured = 1.0;
    if (keys_hook->t == -1)
        hook_params->isTextured = 0.0;
}