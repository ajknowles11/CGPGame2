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

	// golf club swinging
	const float swing_max = 0.5f;
	float swing_acc = 0;
	float swing_power = 0;
	bool backswinging = false;
	bool swinging = false;
	bool should_swing = false;

	float max_hit_velocity = 8.0f;
	void swing();


	// hole/ball stuff
	const float hole_radius_start = 0.075f;
	const float ball_radius_start = 0.02135f;
	const float hole_pull_strength_start = 0.1f;
	const float min_pull_strength = 0.2f;
	float hole_scale = 1.0f;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;
	
	//player and camera:
	Scene::Transform *player = nullptr;
	Scene::Camera *camera = nullptr;
	// track pitch separately for clamping
	// idea from https://stackoverflow.com/questions/34971046/clamping-pitch-in-first-person-camera-using-quaternion-orientation
	float camera_pitch = glm::pi<float>()/2;
	const float cam_pitch_min = 0;
	const float cam_pitch_max = glm::pi<float>();

	const float cam_pitch_aim_start = 0.65f;
	const float cam_pitch_aim_end = 0.35f;

	bool is_sprinting = false;

	Scene::Transform *hand = nullptr;
	Scene::Transform *aimhand = nullptr;
	Scene::Transform *club = nullptr;
	Scene::RigidBody *ball = nullptr;
	Scene::RigidBody *hole = nullptr;

	float drag = 0.3f;

	// physics
	void handle_physics(float elapsed);
	const glm::vec3 gravity = glm::vec3(0,0,-9.8f);
	std::vector<Scene::CollisionObject*> collision_objects;

};
