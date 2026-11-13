/**************************************************************************/
/*  test_generic_6dof_joint_3d.cpp                                        */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "tests/test_macros.h"

TEST_FORCE_LINK(test_collision_exceptions)

#include "servers/physics_2d/physics_server_2d.h"

#ifndef PHYSICS_3D_DISABLED
#include "scene/3d/physics/area_3d.h"
#include "scene/3d/physics/soft_body_3d.h"
#include "scene/3d/physics/static_body_3d.h"
#include "servers/physics_3d/physics_server_3d.h"
#endif

namespace TestCollisionExceptions {

TEST_CASE("[SceneTree][PhysicsServer2D] Freeing a body clears incoming exceptions") {
	PhysicsServer2D *server = PhysicsServer2D::get_singleton();
	REQUIRE(server != nullptr);

	const RID a = server->body_create();
	const RID b = server->body_create();
	const RID c = server->body_create();
	server->body_add_collision_exception(a, b);
	server->body_add_collision_exception(a, b);
	server->body_add_collision_exception(a, c);
	server->body_add_collision_exception(c, a);

	server->free_rid(b);
	List<RID> exceptions;
	server->body_get_collision_exceptions(a, &exceptions);
	CHECK(exceptions.size() == 1);
	for (const RID &rid : exceptions) {
		CHECK(rid == c);
	}

	server->free_rid(a);
	List<RID> remaining;
	server->body_get_collision_exceptions(c, &remaining);
	CHECK(remaining.is_empty());
	server->free_rid(c);
}

#ifndef PHYSICS_3D_DISABLED
TEST_CASE("[SceneTree][PhysicsServer3D] Freeing a body clears incoming exceptions") {
	PhysicsServer3D *server = PhysicsServer3D::get_singleton();
	REQUIRE(server != nullptr);

	const RID a = server->body_create();
	const RID b = server->body_create();
	const RID c = server->body_create();
	server->body_add_collision_exception(a, b);
	server->body_add_collision_exception(a, b);
	server->body_add_collision_exception(a, c);
	server->body_add_collision_exception(c, a);

	server->free_rid(b);
	List<RID> exceptions;
	server->body_get_collision_exceptions(a, &exceptions);
	CHECK(exceptions.size() == 1);
	for (const RID &rid : exceptions) {
		CHECK(rid == c);
	}

	server->free_rid(a);
	List<RID> remaining;
	server->body_get_collision_exceptions(c, &remaining);
	CHECK(remaining.is_empty());
	server->free_rid(c);
}

TEST_CASE("[SceneTree][PhysicsServer3D] Soft body exceptions are removed when either endpoint is freed") {
	PhysicsServer3D *server = PhysicsServer3D::get_singleton();
	REQUIRE(server != nullptr);

	const RID body_a = server->body_create();
	const RID soft_a = server->soft_body_create();
	server->soft_body_add_collision_exception(soft_a, body_a);
	server->soft_body_add_collision_exception(soft_a, body_a);
	server->free_rid(body_a);
	List<RID> soft_exceptions;
	server->soft_body_get_collision_exceptions(soft_a, &soft_exceptions);
	CHECK(soft_exceptions.is_empty());
	server->free_rid(soft_a);

	const RID body_b = server->body_create();
	const RID soft_b = server->soft_body_create();
	server->body_add_collision_exception(body_b, soft_b);
	server->free_rid(soft_b);
	List<RID> body_exceptions;
	server->body_get_collision_exceptions(body_b, &body_exceptions);
	CHECK(body_exceptions.is_empty());
	server->free_rid(body_b);
}

TEST_CASE("[SceneTree][PhysicsServer3D] Freeing an area clears body and soft body exceptions") {
	PhysicsServer3D *server = PhysicsServer3D::get_singleton();
	REQUIRE(server != nullptr);

	const RID area = server->area_create();
	const RID body = server->body_create();
	const RID soft_body = server->soft_body_create();
	server->body_add_collision_exception(body, area);
	server->soft_body_add_collision_exception(soft_body, area);
	server->free_rid(area);

	List<RID> body_exceptions;
	server->body_get_collision_exceptions(body, &body_exceptions);
	CHECK(body_exceptions.is_empty());
	List<RID> soft_exceptions;
	server->soft_body_get_collision_exceptions(soft_body, &soft_exceptions);
	CHECK(soft_exceptions.is_empty());

	server->free_rid(body);
	server->free_rid(soft_body);
}

TEST_CASE("[SceneTree][PhysicsServer3D] Clearing or freeing a joint restores body collisions") {
	PhysicsServer3D *server = PhysicsServer3D::get_singleton();
	REQUIRE(server != nullptr);

	const RID a = server->body_create();
	const RID b = server->body_create();

	RID joint = server->joint_create();
	server->joint_make_pin(joint, a, Vector3(), b, Vector3());
	server->joint_disable_collisions_between_bodies(joint, true);
	server->joint_disable_collisions_between_bodies(joint, true);
	List<RID> before;
	server->body_get_collision_exceptions(a, &before);
	CHECK(before.size() == 1);
	server->free_rid(joint);

	List<RID> after_free;
	server->body_get_collision_exceptions(a, &after_free);
	CHECK(after_free.is_empty());

	joint = server->joint_create();
	server->joint_make_pin(joint, a, Vector3(), b, Vector3());
	server->joint_disable_collisions_between_bodies(joint, true);
	server->joint_clear(joint);

	List<RID> after_clear;
	server->body_get_collision_exceptions(a, &after_clear);
	CHECK(after_clear.is_empty());
	server->free_rid(joint);

	server->body_add_collision_exception(a, b);
	joint = server->joint_create();
	server->joint_make_pin(joint, a, Vector3(), b, Vector3());
	server->joint_disable_collisions_between_bodies(joint, false);
	server->free_rid(joint);
	List<RID> manual;
	server->body_get_collision_exceptions(a, &manual);
	CHECK(manual.size() == 1);

	server->free_rid(a);
	server->free_rid(b);
}

TEST_CASE("[SceneTree][PhysicsServer3D] Multiple joints and explicit exceptions retain independent collision exclusions") {
	PhysicsServer3D *server = PhysicsServer3D::get_singleton();
	REQUIRE(server != nullptr);

	const RID a = server->body_create();
	const RID b = server->body_create();
	RID joint_a = server->joint_create();
	RID joint_b = server->joint_create();
	server->joint_make_pin(joint_a, a, Vector3(), b, Vector3());
	server->joint_make_pin(joint_b, a, Vector3(), b, Vector3());
	server->joint_disable_collisions_between_bodies(joint_a, true);
	server->joint_disable_collisions_between_bodies(joint_b, true);

	server->free_rid(joint_a);
	List<RID> while_joint_b_alive;
	server->body_get_collision_exceptions(a, &while_joint_b_alive);
	CHECK(while_joint_b_alive.size() == 1);

	server->body_add_collision_exception(a, b);
	server->free_rid(joint_b);
	List<RID> explicit_survives;
	server->body_get_collision_exceptions(a, &explicit_survives);
	CHECK(explicit_survives.size() == 1);
	server->body_remove_collision_exception(a, b);
	List<RID> after_remove;
	server->body_get_collision_exceptions(a, &after_remove);
	CHECK(after_remove.is_empty());

	server->free_rid(a);
	server->free_rid(b);
}

TEST_CASE("[SceneTree][PhysicsServer3D] Area collision exceptions return the live area node") {
	Area3D *area = memnew(Area3D);
	StaticBody3D *body = memnew(StaticBody3D);
	SoftBody3D *soft_body = memnew(SoftBody3D);

	body->add_collision_exception_with(area);
	soft_body->add_collision_exception_with(area);

	const TypedArray<CollisionObject3D> body_exceptions = body->get_collision_exceptions();
	REQUIRE(body_exceptions.size() == 1);
	CHECK((Object *)body_exceptions[0] == area);

	const TypedArray<CollisionObject3D> soft_exceptions = soft_body->get_collision_exceptions();
	REQUIRE(soft_exceptions.size() == 1);
	CHECK((Object *)soft_exceptions[0] == area);

	memdelete(area);
	CHECK(body->get_collision_exceptions().is_empty());
	CHECK(soft_body->get_collision_exceptions().is_empty());

	memdelete(body);
	memdelete(soft_body);
}
#endif

} // namespace TestCollisionExceptions
