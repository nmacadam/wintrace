#include "ConsoleWindow.h"
#include "geometry.h"
#include "raytracing.h"
#include <chrono>
#include <cfloat>

const float PI = 3.14f;

// console window size (in characters)
const int width = 120;
const int height = 40;

// user input
const float mouseSensitivity = .01f;
const float moveSpeed = 5.f;

// corrective scalar (monospace characters are not square, they are rectangular)
const float consoleViewportCorrection = .5f;

// Character shading
char shadingTable[] = 
{ ' ', '.', ':', '-', '=', '+', '*', '#', '%', '@' };

// Doesn't work well for the standard console window size but might work better if you make the
// window huge
char extendedShadingTable[] =
{ 
	' ', '\'', '`', '^', '\"', ',', ':', ';', 'I', 'l', '!', 'i', '>', 
	'<', '~', '+', '_', '-', '?', ']', '[', '}', '{', '1', ')', '(', '|',
	'\\', '/', 't', 'f', 'j', 'r', 'x', 'n', 'u', 'v', 'c', 'z', 
	'X', 'Y', 'U', 'J', 'C', 'L', 'Q', '0', 'O', 'Z', 'm', 'w',
	'q', 'p', 'd', 'b', 'k', 'h', 'a', 'o', '*', '#', 'M', 'W', '&', '8',
	'%', 'B', '@', '$'
};

// Lookup shading character by 0-1 float value
char getShadingChar(float value)
{
	size_t n = sizeof(shadingTable) / sizeof(shadingTable[0]);

	// value remap to index
	int i = value * n;
	// correct for outside of 0-1 range
	if (i < 0) i = 0;
	if (i > n) i = n - 1;

	return shadingTable[i];
}

// check all scene objects for intersections
bool scene_intersect(const vec3 &orig, const vec3 &dir, const std::vector<Sphere> &spheres, vec3 &hit, vec3 &N, Material &material) {
	float spheres_dist = FLT_MAX;
	for (size_t i = 0; i < spheres.size(); i++) {
		float dist_i;
		if (spheres[i].ray_intersect(Ray(orig, dir), dist_i) && dist_i < spheres_dist) {
			spheres_dist = dist_i;
			hit = orig + dir * dist_i;
			N = (hit - spheres[i].center).normalize();
			material = spheres[i].material;
		}
	}
	return spheres_dist < 1000;
}

// do ray tracing
float cast_ray(const vec3 &orig, const vec3 &dir, std::vector<Sphere> &spheres, const std::vector<Light> &lights) {
	vec3 point, N;
	Material material;

	// if the ray doesn't intersect any scene objects, return 0 for no light
	if (!scene_intersect(orig, dir, spheres, point, N, material)) {
		return 0;
	}

	// calculate lighting
	float diffuse_light_intensity = 0, specular_light_intensity = 0;
	for (size_t i = 0; i < lights.size(); i++) 
	{
		vec3 light_dir = (lights[i].position - point).normalize();
		float light_distance = (lights[i].position - point).norm();

		// apply shadows
		vec3 shadow_orig = light_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3; // checking if the point lies in the shadow of the lights[i]
		vec3 shadow_pt, shadow_N;
		Material tmpmaterial;
		if (scene_intersect(shadow_orig, light_dir, spheres, shadow_pt, shadow_N, tmpmaterial) && (shadow_pt - shadow_orig).norm() < light_distance)
			continue;

		// add values for different lighting types
		diffuse_light_intensity += lights[i].intensity * max(0.f, (light_dir * N));
		specular_light_intensity += powf(max(0.f, -reflect(-light_dir, N)*dir), material.specular_exponent)*lights[i].intensity;
	}

	// calculate final output color value
	float out = (material.diffuse_color * diffuse_light_intensity * material.albedo[0] + vec3(1., 1., 1.)*specular_light_intensity * material.albedo[1]).x;

	// todo: change
	return max(out, .01f);
}

int main()
{
	// Initialize console window as a buffer
	ConsoleWindow window(width, height);

	// Create some materials
	Material shiny(vec2(0.6, 0.3), vec3(0.4, 0.4, 0.3), 50.);
	Material dull(vec2(0.9, 0.1), vec3(0.3, 0.1, 0.1), 10.);

	// Create some spheres to render
	std::vector<Sphere> spheres;
	spheres.push_back(Sphere(vec3(1.5, 0.5, -18), 3, dull));
	spheres.push_back(Sphere(vec3(-6, 0, -16), 2, shiny));
	spheres.push_back(Sphere(vec3(-2.5, 2.5, -12), 2, dull));
	spheres.push_back(Sphere(vec3(7, 5, -18), 4, shiny));

	// Add a light
	std::vector<Light> lights;
	lights.push_back(Light(vec3(-20, 20, 20), 1.5));

	const double fov = PI / 4.;

	// Initialize variables for tracking application runtime duration
	auto start = std::chrono::system_clock::now();
	auto current = std::chrono::system_clock::now();

	// Initialize variables for tracking delta time
	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	vec3 cameraPosition;
	vec3 cameraRotation;

	// Get initial mouse position to calculate offset
	POINT initialPos;
	GetCursorPos(&initialPos);

	bool isRunning = true;

	// render loop
	while(isRunning)
	{
		// get delta time
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		// get application runtime/duration
		current = std::chrono::system_clock::now();
		std::chrono::duration<float> timeSinceStart = current - start;
		float time = timeSinceStart.count();

		// Get mouse offset amount
		POINT currentPos;
		GetCursorPos(&currentPos);
		vec2 mouseOffset(currentPos.x - initialPos.x, currentPos.y - initialPos.y);

		// Capture keyboard input
		vec3 movement;
		vec3 lightMovement;

		// Handle camera movement
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			movement.x -= 1;
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			movement.x += 1;
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
			movement.z -= 1;
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
			movement.z += 1;

		// Handle light movement
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			lightMovement.x -= 1;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			lightMovement.x += 1;
		if (GetAsyncKeyState(VK_UP) & 0x8000)
			lightMovement.y -= 1;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			lightMovement.y += 1;

		// Handle application exit
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			isRunning = false;

		// Set the camera rotation using the 
		cameraRotation = vec3(mouseOffset.y, -mouseOffset.x, 0) * mouseSensitivity;

		// clamp rotation; note that the mouse position makes this map such that 1 : 90 degrees
		// in an real application you'd want to remap these values so the rotation holds degrees directly
		// frankly you'd want to use a transformation matrix but hey, im trying to showcase ray tracing not linear algebra
		if (cameraRotation.x > .95) cameraRotation.x = .95;
		if (cameraRotation.x < -.95) cameraRotation.x = -.95;

		// update camera position using user input
		cameraPosition = cameraPosition + movement * moveSpeed * fElapsedTime;

		// update light position (ignoring the unsafe access here)
		lights[0].position = lights[0].position + lightMovement * moveSpeed * fElapsedTime;

		// Move the spheres around a bit
		/*for (int i = 0; i < spheres.size(); i++)
		{
			spheres[i].center.y += sin(time + i) / 100.f;
		}*/

		// Cast rays
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				float x = (2 * (i + 0.5) / (float)width - 1) * tan(fov / 2.) * width * consoleViewportCorrection / (float)height;
				float y = -(2 * (j + 0.5) / (float)height - 1) * tan(fov / 2.);
				vec3 dir = vec3(x, y, -1).normalize();

				// apply y-axis rotation to the ray direction
				vec3 yRot(
					dir.x * cos(cameraRotation.y) + dir.z * sin(cameraRotation.y),
					dir.y,
					-dir.x * sin(cameraRotation.y) + dir.z * cos(cameraRotation.y)
				);

				// apply x-axis rotation to the ray direction
				vec3 xRot(
					yRot.x,
					yRot.y * cos(cameraRotation.x) - yRot.z * sin(cameraRotation.x),
					yRot.y * sin(cameraRotation.x) + yRot.z * cos(cameraRotation.x)
				);

				// get monochrome color result of cast
				float val = cast_ray(cameraPosition, xRot, spheres, lights);

				// set console window character by color value
				window.setPixel(i, j, getShadingChar(val));
			}
		}

		// Write debug info
		swprintf_s(window.getBuffer(), 120, L"Console Raytracer by Nathan MacAdam  FPS:%3.2f   Camera Pos:%3.2f, %3.2f, %3.2f  Camera Rot:%3.2f, %3.2f, %3.2f"
			, 1.0f / fElapsedTime, cameraPosition.x, cameraPosition.y, cameraPosition.z, cameraRotation.x, cameraRotation.y, cameraRotation.z);

		// draw output
		window.draw();
	}

	return 0;
}