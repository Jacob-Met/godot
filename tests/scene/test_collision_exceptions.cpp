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
