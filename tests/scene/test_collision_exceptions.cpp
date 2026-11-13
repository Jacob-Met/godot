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
#include "servers/physics_3d/physics_server_3d.h"
#endif

namespace TestCollisionExceptions {

TEST_CASE("[SceneTree][PhysicsServer2D] Freeing bodies removes incoming collision exceptions") {
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
TEST_CASE("[SceneTree][PhysicsServer3D] Freeing bodies removes incoming collision exceptions") {
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
#endif

} // namespace TestCollisionExceptions
