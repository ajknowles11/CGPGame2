#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint level_meshes_for_lit_color_texture_program = 0;

Load< MeshBuffer > level0_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("levels/lvl0.pnct"));
	level_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > level0_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("levels/lvl0.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = level0_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = level_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< MeshBuffer > level1_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("levels/lvl1.pnct"));
	level_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > level1_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("levels/lvl1.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = level1_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = level_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< MeshBuffer > level2_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("levels/lvl2.pnct"));
	level_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > level2_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("levels/lvl2.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = level2_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = level_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< MeshBuffer > level3_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("levels/lvl3.pnct"));
	level_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > level3_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("levels/lvl3.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = level3_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = level_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

std::vector< Load <MeshBuffer> > level_meshes_vec;
std::vector< Load <Scene> > scene_vec;

PlayMode::PlayMode() {
	level_meshes_vec.emplace_back(level0_meshes);
	scene_vec.emplace_back(level0_scene);

	level_meshes_vec.emplace_back(level1_meshes);
	scene_vec.emplace_back(level1_scene);
	
	level_meshes_vec.emplace_back(level2_meshes);
	scene_vec.emplace_back(level2_scene);

	level_meshes_vec.emplace_back(level3_meshes);
	scene_vec.emplace_back(level3_scene);


	init();
}

PlayMode::~PlayMode() {
}

void PlayMode::init() {
	scene = *scene_vec[lvl_index];
	for (auto &transform : scene.transforms) {
		if (transform.name == "Player") player = &transform;
		else if (transform.name == "Hand") hand = &transform;
		else if (transform.name == "AimHand") aimhand = &transform;
		else if (transform.name == "Club") club = &transform;

		else if (transform.name == "Hole") {
			hole = std::make_shared<Scene::RigidBody>(&transform, std::make_shared<Scene::SphereCollider>(glm::vec3(0), hole_radius_start));
			hole->is_hole = true;
		}

		else if (transform.name == "Ball") {
			ball = std::make_shared<Scene::RigidBody>(&transform, std::make_shared<Scene::SphereCollider>(glm::vec3(0), ball_radius_start));
			ball->is_ball = true;
		}
		
		
		else if (transform.name == "Ground") collision_objects.emplace_back(std::make_shared<Scene::CollisionObject>(&transform, std::make_shared<Scene::PlaneCollider>(glm::vec3(0,0,1.0f), 0.0f), 0.7f));

		else if (transform.name.substr(0, 4) == "Wall") {
			const Mesh &mesh = level_meshes_vec[lvl_index]->lookup(transform.name);
			collision_objects.emplace_back(std::make_shared<Scene::CollisionObject>(&transform, std::make_shared<Scene::BoxCollider>(mesh.min, mesh.max)));
		}
		else if (transform.name.substr(0, 4) == "Item") {
			std::shared_ptr<Scene::CollisionObject> item = std::make_shared<Scene::CollisionObject>(&transform, std::make_shared<Scene::SphereCollider>(glm::vec3(0), item_radius));
			item->is_pickup = true;
			collision_objects.emplace_back(item);
		}
	}

	if (player == nullptr) throw std::runtime_error("Player not found.");
	if (hand == nullptr) throw std::runtime_error("Hand not found.");
	if (aimhand == nullptr) throw std::runtime_error("Hand not found.");
	if (club == nullptr) throw std::runtime_error("Club not found.");

	if (hole == nullptr) throw std::runtime_error("Hole not found.");
	if (ball == nullptr) throw std::runtime_error("Ball not found.");


	collision_objects.emplace_back(ball);
	collision_objects.emplace_back(hole);
	ball->transform->position += glm::vec3(0,0,0.2f);
	hole->transform->position += glm::vec3(0,0,0.2f);

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	loading = false;
}

void PlayMode::cleanup_go_next() {
	loading = true;

	swing_acc = 0;
	swing_power = 0;
	backswinging = false;
	swinging = false;
	should_swing = false;

	hole_scale = 1.0f;
	player = nullptr;
	camera = nullptr;

	camera_pitch = glm::pi<float>()/2;
	is_sprinting = false;
	
	hand = nullptr;
	aimhand = nullptr;
	club = nullptr;

	collision_objects = std::vector<std::shared_ptr<Scene::CollisionObject>>();

	if (++lvl_index < level_meshes_vec.size()) {
		init();
	}
	else {
		lvl_index = 0;
		init();
	}
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (loading) {
		if (evt.type == SDL_KEYDOWN) {
			if (evt.key.keysym.sym == SDLK_ESCAPE) {
				SDL_SetRelativeMouseMode(SDL_FALSE);
				return true;
			}
		}
		return false;
	}

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_LSHIFT) {
			is_sprinting = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_F3) {
			show_fps = !show_fps;
			return true;
		} else if (show_fps && evt.key.keysym.sym == SDLK_DOWN) {
			player->position.z -= 0.2f;
			return true;
		} else if (show_fps && evt.key.keysym.sym == SDLK_UP) {
			player->position.z += 0.2f;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_r) {
			lvl_index--;
			cleanup_go_next();
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_LSHIFT) {
			is_sprinting = false;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
		else if (evt.button.button == SDL_BUTTON_LEFT && !swinging && camera_pitch < cam_pitch_aim_start) {
			backswinging = true;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONUP) {
		if (evt.button.button == SDL_BUTTON_LEFT && backswinging) {
			backswinging = false;
			swinging = true;
			should_swing = true;
			return true;
		}
	}
	else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			player->rotation = glm::normalize(
				player->rotation * glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 0.0f, 1.0f))
			);
			const float adjust_camera_pitch = glm::clamp(camera_pitch + motion.y * camera->fovy, cam_pitch_min, cam_pitch_max) - camera_pitch;
			if (glm::abs(adjust_camera_pitch) >= glm::epsilon<float>()) {
				camera->transform->rotation = glm::normalize(
					camera->transform->rotation * glm::angleAxis(adjust_camera_pitch, glm::vec3(1.0f, 0.0f, 0.0f))
				);
				camera_pitch += adjust_camera_pitch;
			}
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	if (loading) return;
	fps = 1.0f / elapsed;

	//move player:
	{

		//combine inputs into a move:
		constexpr float PlayerSpeed = 2.0f;
		float speed_mult = is_sprinting ? 3.0f : 1.0f;
		glm::vec2 move = glm::vec2(0.0f);
		if (left.pressed && !right.pressed) move.x =-1.0f;
		if (!left.pressed && right.pressed) move.x = 1.0f;
		if (down.pressed && !up.pressed) move.y =-1.0f;
		if (!down.pressed && up.pressed) move.y = 1.0f;

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * speed_mult * elapsed;

		glm::mat4x3 frame = player->make_local_to_parent();
		glm::vec3 frame_right = frame[0];
		//glm::vec3 up = frame[1];
		glm::vec3 frame_forward = frame[1];

		player->position += move.x * frame_right + move.y * frame_forward;
	}

	{ // club swing
		if (backswinging) {
			swing_acc += elapsed;
			if (swing_acc > swing_max) swing_acc = swing_max;
			swing_power = swing_acc;
		}
		else if (swinging) {
			swing_acc -= swing_power * 8.0f * elapsed;
			if (swing_acc < -swing_power) {
				swing_acc = -swing_power;
				swinging = false;
			}
		}
		else if (swing_acc < 0) {
			swing_acc += swing_power * 6.0f * elapsed;
		}
		else {
			swing_acc = 0;
		}

		if (should_swing) {
			swing();
		}
	}

	{ // attach club to hand
		// we lerp between hand and aimhand (which is where club held while hitting ball)
		float alpha = 1-(glm::clamp(glm::eulerAngles(camera->transform->rotation).x, cam_pitch_aim_end, cam_pitch_aim_start) - cam_pitch_aim_end) / (cam_pitch_aim_start - cam_pitch_aim_end);
		
		glm::mat4x3 camera_world_transform = camera->transform->make_local_to_world(); // save like 0.00001s by computing once instead of twice
		glm::mat4x3 hand_world = camera_world_transform * glm::mat4(hand->make_local_to_parent());
		glm::mat4x3 aimhand_world = camera_world_transform * glm::mat4(aimhand->make_local_to_parent());

		glm::vec3 start_pos = hand_world * glm::vec4(0,0,0,1.0f);
		glm::vec3 end_pos = aimhand_world * glm::vec4(0,0,0,1.0f);

		club->position = glm::mix(start_pos, end_pos, alpha);
		club->rotation = glm::angleAxis(glm::eulerAngles(player->rotation).z, glm::vec3(0.0f,0.0f,1.0f))
		* glm::angleAxis(-swing_acc, glm::vec3(0,1.0f,0));
	}

	// handle physics, thanks Winterdev (https://www.youtube.com/watch?v=-_IspRG548E)
	// and https://winter.dev/articles/physics-engine
	handle_physics(elapsed);

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	if (cleanup_next_update) {
		cleanup_next_update = false;
		cleanup_go_next();
	}
}

void PlayMode::handle_physics(float elapsed) {

	// find collisions
	std::vector<Scene::Collision> collisions;
	for (auto obj_a : collision_objects) {
		for (auto obj_b : collision_objects) {
			if (obj_a == obj_b) break;

			if (!obj_a->collider || !obj_b->collider)
				continue;
			
			Scene::CollisionPoints points = Scene::test_collision(
				obj_a->collider, obj_a->transform,
				obj_b->collider, obj_b->transform
			);

			if (points.has_collision) {
				collisions.emplace_back(obj_a, obj_b, points);
			}
		}
	}

	uint8_t delete_count = 0;

	// solve collisions
	for (auto col : collisions) {
		if (col.obj_a->to_delete || col.obj_b->to_delete) continue;
		if (!col.obj_a->is_dynamic && !col.obj_b->is_dynamic) continue;
		else if (!col.obj_a->is_dynamic) { // b is moving
			if (col.obj_b->is_hole) {
				if (col.obj_a->is_pickup) { // grow black hole, add mass and delete pickup
					delete_count++;
					hole_scale += col.obj_a->pickup_value;
					hole->transform->scale = glm::vec3(hole_scale);
					hole->mass = hole_scale;
					col.obj_a->to_delete = true;
					col.obj_a->transform->scale = glm::vec3(0);
					continue;
				}
			}

			std::shared_ptr<Scene::RigidBody> body_b = std::static_pointer_cast<Scene::RigidBody>(col.obj_b);
			if (glm::length(body_b->velocity) < 0.00001f) return;
			glm::vec3 out_velocity = col.obj_b->friction * (body_b->velocity - 2.0f * col.obj_a->damp * glm::dot(body_b->velocity, -col.points.normal) * -col.points.normal);
			glm::vec3 out_force = body_b->mass * gravity - 2.0f * glm::dot(body_b->mass * gravity, col.points.normal) * col.points.normal;
			glm::vec3 out_dir = glm::normalize(out_velocity);
			// move body_b back to where it hit
			glm::vec3 vel_dir = glm::normalize(body_b->velocity);
			float angled_dist = col.points.depth / glm::dot(col.points.normal, vel_dir);
			body_b->transform->position = body_b->transform->position - vel_dir * angled_dist + out_dir * angled_dist;

			// apply new physics
			body_b->velocity = out_velocity;
			body_b->force = out_force;
		}
		else if (!col.obj_b->is_dynamic) { // a is moving
			if (col.obj_a->is_hole) {
				if (col.obj_b->is_pickup) {
					delete_count++;
					hole_scale += col.obj_b->pickup_value;
					hole->transform->scale = glm::vec3(hole_scale);
					hole->mass = hole_scale;
					col.obj_b->to_delete = true;
					col.obj_b->transform->scale = glm::vec3(0);
					continue;
				}
			}

			std::shared_ptr<Scene::RigidBody> body_a = std::static_pointer_cast<Scene::RigidBody>(col.obj_a);
			if (glm::length(body_a->velocity) < 0.00001f) return;
			glm::vec3 out_velocity = col.obj_b->friction * (body_a->velocity - 2.0f * col.obj_b->damp * glm::dot(body_a->velocity, col.points.normal) * col.points.normal);
			glm::vec3 out_force = body_a->mass * gravity - 2.0f * glm::dot(body_a->mass * gravity, -col.points.normal) * col.points.normal;
			glm::vec3 out_dir = glm::normalize(out_velocity);
			// move body_a back to where it hit
			glm::vec3 vel_dir = glm::normalize(body_a->velocity);
			float angled_dist = col.points.depth / glm::dot(-col.points.normal, vel_dir);
			body_a->transform->position = body_a->transform->position - vel_dir * angled_dist + out_dir * angled_dist;

			// apply new physics
			body_a->velocity = out_velocity;
			body_a->force = out_force;
		}
		else {// currently this only happens when we finish a hole but it could happen if we make items vacuum
			if ((col.obj_a->is_ball && col.obj_b->is_hole) || (col.obj_a->is_hole && col.obj_b->is_ball)) {
				// win level
				if (col.points.depth > 0.1f)
					cleanup_next_update = true;
			}
		}
	}

	// move dynamics
	for (auto obj : collision_objects) {
		if (obj->is_dynamic) {
			std::shared_ptr<Scene::RigidBody> body = std::static_pointer_cast<Scene::RigidBody>(obj);
			body->force += body->mass * gravity;
			body->force += -body->velocity * drag;
			if (body->transform->name == "Ball") {
				const float hole_dist = glm::max(0.005f, glm::distance(body->transform->position, hole->transform->position));
				const float pull_strength = (hole_pull_strength_start*hole_scale*hole_scale/hole_dist);
				if (pull_strength >= min_pull_strength) {
					body->force += glm::normalize(hole->transform->position - body->transform->position) * pull_strength * body->mass;
				}
			}

			body->velocity += body->force / body->mass * elapsed;
			body->transform->position += body->velocity * elapsed;

			body->force = glm::vec3(0);
		}
	}

}

void PlayMode::swing() {
	should_swing = false;
	const glm::vec3 hole_pos = hole->transform->make_local_to_world() * glm::vec4(0,0,0,1);
	const float dist = glm::distance(hole_pos, player->position);
	
	if (swing_power > 0 && dist <= hit_radius + hole_radius_start * hole_scale) {//can hit hole here
		hole->velocity += hole->mass * swing_power/swing_max * player->make_local_to_world() * glm::vec4(-max_hit_velocity,0,0,0);
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	if (loading) return;
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.1f, 0.045f, 0.24f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	GL_ERRORS(); //print any errors produced by this setup code

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		lines.draw_text("Mouse + WASD to move, LSHIFT to run. Look down + hold LMB to swing. R to reset level.",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Mouse + WASD to move, LSHIFT to run. Look down + hold LMB to swing. R to reset level.",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));

		if (show_fps) {
			lines.draw_text(std::to_string(fps), 
			glm::vec3(-aspect + 0.1f * H, 1.0 - H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f), 
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
		}
	}
}
