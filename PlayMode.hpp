#include "Mode.hpp"

#include "Scene.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	float fps;
	bool show_fps = false;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//hexapod leg to wobble:
	Scene::Transform *hip = nullptr;
	Scene::Transform *upper_leg = nullptr;
	Scene::Transform *lower_leg = nullptr;
	glm::quat hip_base_rotation;
	glm::quat upper_leg_base_rotation;
	glm::quat lower_leg_base_rotation;
	float wobble = 0.0f;
	
	//player and camera:
	Scene::Transform *player = nullptr;
	Scene::Camera *camera = nullptr;
	// track pitch separately for clamping
	// idea from https://stackoverflow.com/questions/34971046/clamping-pitch-in-first-person-camera-using-quaternion-orientation
	float camera_pitch;
	const float cam_pitch_min = -glm::pi<float>()/2.0f;
	const float cam_pitch_max = glm::pi<float>()/2.0f;

	Scene::Transform *hand = nullptr;
	Scene::Transform *club = nullptr;

};
