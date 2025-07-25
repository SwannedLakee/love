/**
 * Copyright (c) 2006-2025 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#ifndef LOVE_PHYSICS_BOX2D_PHYSICS_H
#define LOVE_PHYSICS_BOX2D_PHYSICS_H

// LOVE
#include "common/Module.h"
#include "common/Vector.h"

#include "World.h"
#include "Contact.h"
#include "Body.h"
#include "Shape.h"
#include "CircleShape.h"
#include "PolygonShape.h"
#include "EdgeShape.h"
#include "ChainShape.h"
#include "Joint.h"
#include "MouseJoint.h"
#include "DistanceJoint.h"
#include "PrismaticJoint.h"
#include "RevoluteJoint.h"
#include "PulleyJoint.h"
#include "GearJoint.h"
#include "FrictionJoint.h"
#include "WeldJoint.h"
#include "WheelJoint.h"
#include "RopeJoint.h"
#include "MotorJoint.h"

namespace love
{
namespace physics
{
namespace box2d
{

class Physics : public Module
{
public:

	/**
	 * 30 pixels in one meter by default.
	 **/
	static const int DEFAULT_METER = 30;

	Physics();
	virtual ~Physics();

	/**
	 * Creates a new World.
	 * @param gx Gravity along x-axis.
	 * @param gy Gravity along y-axis.
	 * @param sleep Whether the World allows sleep.
	 **/
	World *newWorld(float gx, float gy, bool sleep);

	/**
	 * Creates a new Body at the specified position.
	 * @param world The world to create the Body in.
	 * @param x The position along the x-axis.
	 * @param y The position along the y-axis.
	 * @param type The type of body to create.
	 **/
	Body *newBody(World *world, float x, float y, Body::Type type);

	/**
	 * Creates a new Body at (0, 0)
	 * @param world The world to create the Body in.
	 * @param type The type of Body to create.
	 **/
	Body *newBody(World *world, Body::Type type);

	/**
	 * Convenience functions for creating a Body and Shape all in one call. The
	 * body's world position is the center/average of the given coordinates,
	 * and the shape is centered at the local origin.
	 **/
	Body *newCircleBody(World *world, Body::Type type, float x, float y, float radius);
	Body *newRectangleBody(World *world, Body::Type type, float x, float y, float w, float h, float angle);
	Body *newPolygonBody(World *world, Body::Type type, const Vector2 *coords, int count);
	Body *newEdgeBody(World *world, Body::Type type, float x1, float y1, float x2, float y2);
	Body *newEdgeBody(World *world, Body::Type type, float x1, float y1, float x2, float y2, float prevx, float prevy, float nextx, float nexty);
	Body *newChainBody(World *world, Body::Type type, bool loop, const Vector2 *coords, int count);

	// Necessary to support the deprecated newFixture API.
	Shape *newAttachedShape(Body *body, Shape *prototype, float density);

	/**
	 * Creates a new CircleShape at (x,y) in local coordinates.
	 * @param x The offset along the x-axis.
	 * @param y The offset along the y-axis.
	 * @param radius The radius of the circle.
	 **/
	CircleShape *newCircleShape(Body *body, float x, float y, float radius);

	/**
	 * Shorthand for creating rectangular PolygonShapes. The rectangle
	 * will be created at (x,y) in local coordinates.
	 * @param x The offset along the x-axis.
	 * @param y The offset along the y-axis.
	 * @param w The width of the rectangle.
	 * @param h The height of the rectangle.
	 * @param angle The angle of the rectangle. (rad)
	 **/
	PolygonShape *newRectangleShape(Body *body, float x, float y, float w, float h, float angle);

	/**
	 * Creates a new EdgeShape. The edge will be created from
	 * (x1,y1) to (x2,y2) in local coordinates.
	 * @param x1 The x coordinate of the first point.
	 * @param y1 The y coordinate of the first point.
	 * @param x2 The x coordinate of the second point.
	 * @param y2 The y coordinate of the second point.
	 **/
	EdgeShape *newEdgeShape(Body *body, float x1, float y1, float x2, float y2);
	EdgeShape *newEdgeShape(Body *body, float x1, float y1, float x2, float y2, float prevx, float prevy, float nextx, float nexty);

	/**
	 * Creates a new PolygonShape from a variable number of vertices.
	 **/
	PolygonShape *newPolygonShape(Body *body, const Vector2 *coords, int count);

	/**
	 * Creates a new ChainShape from a variable number of vertices.
	 **/
	ChainShape *newChainShape(Body *body, bool loop, const Vector2 *coords, int count);

	/**
	 * Creates a new DistanceJoint connecting body1 with body2.
	 * @param x1 Anchor1 along the x-axis. (World coordinates)
	 * @param y1 Anchor1 along the y-axis. (World coordinates)
	 * @param x2 Anchor2 along the x-axis. (World coordinates)
	 * @param y2 Anchor2 along the y-axis. (World coordinates)
	 * @param collideConnected Whether the connected bodies should collide with each other. Defaults to false.
	 **/
	DistanceJoint *newDistanceJoint(Body *body1, Body *body2, float x1, float y1, float x2, float y2, bool collideConnected);

	/**
	 * Creates a new MouseJoint connecting the body with an arbitrary point.
	 * @param x Anchor along the x-axis. (World coordinates)
	 * @param y Anchor along the y-axis. (World coordinates)
	 **/
	MouseJoint *newMouseJoint(Body *body, float x, float y);

	/**
	 * Creates a new RevoluteJoint connecting body1 with body2.
	 * @param xA Anchor for body 1 along the x-axis. (World coordinates)
	 * @param yA Anchor for body 1 along the y-axis. (World coordinates)
	 * @param xB Anchor for body 2 along the x-axis. (World coordinates)
	 * @param yB Anchor for body 2 along the y-axis. (World coordinates)
	 * @param collideConnected Whether the connected bodies should collide with each other. Defaults to false.
	 * @param referenceAngle The reference angle.
	 **/
	RevoluteJoint *newRevoluteJoint(Body *body1, Body *body2, float xA, float yA, float xB, float yB, bool collideConnected);

	RevoluteJoint *newRevoluteJoint(Body *body1, Body *body2, float xA, float yA, float xB, float yB, bool collideConnected, float referenceAngle);

	/**
	 * Creates a new PrismaticJoint connecting body1 with body2.
	 * @param xA World-anchor for body1 along the x-axis.
	 * @param yA World-anchor for body1 along the y-axis.
	 * @param xB World-anchor for body2 along the x-axis.
	 * @param yB World-anchor for body2 along the y-axis.
	 * @param ax The x-component of the world-axis.
	 * @param ay The y-component of the world-axis.
	 * @param collideConnected Whether the connected bodies should collide with each other. Defaults to false.
	 * @param referenceAngle The reference angle.
	 **/
	PrismaticJoint *newPrismaticJoint(Body *body1, Body *body2, float xA, float yA, float xB, float yB, float ax, float ay, bool collideConnected);

	PrismaticJoint *newPrismaticJoint(Body *body1, Body *body2, float xA, float yA, float xB, float yB, float ax, float ay, bool collideConnected, float referenceAngle);

	/**
	 * Creates a new PulleyJoint connecting body1 with body2.
	 * @param groundAnchor1 World ground-anchor for body1.
	 * @param groundAnchor2 World ground-anchor for body2.
	 * @param anchor1 World anchor on body1.
	 * @param anchor2 World anchor on body2.
	 * @param ratio The pulley ratio.
	 * @param collideConnected Whether the connected bodies should collide with each other. Defaults to true.
	 **/
	PulleyJoint *newPulleyJoint(Body *body1, Body *body2, b2Vec2 groundAnchor1, b2Vec2 groundAnchor2, b2Vec2 anchor1, b2Vec2 anchor2, float ratio, bool collideConnected);

	/**
	 * Creates a new GearJoint connecting joint1 with joint2.
	 * @param joint1 The first joint.
	 * @param joint2 The second joint.
	 * @param ratio The gear ratio.
	 * @param collideConnected Whether the connected bodies should collide with each other. Defaults to false.
	 **/
	GearJoint *newGearJoint(Joint *joint1, Joint *joint2, float ratio, bool collideConnected);

	/**
	 * Creates a new FrictionJoint connecting body1 with body2.
	 * @param xA Anchor for body 1 along the x-axis. (World coordinates)
	 * @param yA Anchor for body 1 along the y-axis. (World coordinates)
	 * @param xB Anchor for body 2 along the x-axis. (World coordinates)
	 * @param yB Anchor for body 2 along the y-axis. (World coordinates)
	 * @param collideConnected Whether the connected bodies should collide with each other. Defaults to false.
	 **/
	FrictionJoint *newFrictionJoint(Body *body1, Body *body2, float xA, float yA, float xB, float yB, bool collideConnected);

	/**
	 * Creates a new WeldJoint connecting body1 with body2.
	 * @param xA Anchor for body 1 along the x-axis. (World coordinates)
	 * @param yA Anchor for body 1 along the y-axis. (World coordinates)
	 * @param xB Anchor for body 2 along the x-axis. (World coordinates)
	 * @param yB Anchor for body 2 along the y-axis. (World coordinates)
	 * @param collideConnected Whether the connected bodies should collide with each other. Defaults to false.
	 * @param referenceAngle The reference angle.
	 **/
	WeldJoint *newWeldJoint(Body *body1, Body *body2, float xA, float yA, float xB, float yB, bool collideConnected);

	WeldJoint *newWeldJoint(Body *body1, Body *body2, float xA, float yA, float xB, float yB, bool collideConnected, float referenceAngle);

	/**
	 * Creates a new WheelJoint connecting body1 with body2.
	 * @param xA Anchor for body 1 along the x-axis. (World coordinates)
	 * @param yA Anchor for body 1 along the y-axis. (World coordinates)
	 * @param xB Anchor for body 2 along the x-axis. (World coordinates)
	 * @param yB Anchor for body 2 along the y-axis. (World coordinates)
	 * @param ax The x-component of the world-axis.
	 * @param ay The y-component of the world-axis.
	 * @param collideConnected Whether the connected bodies should collide with each other. Defaults to false.
	 **/
	WheelJoint *newWheelJoint(Body *body1, Body *body2, float xA, float yA, float xB, float yB, float ax, float ay, bool collideConnected);

	/**
	 * Creates a new RopeJoint connecting body1 with body2.
	 * @param x1 Anchor1 along the x-axis. (Local coordinates)
	 * @param y1 Anchor1 along the y-axis. (Local coordinates)
	 * @param x2 Anchor2 along the x-axis. (Local coordinates)
	 * @param y2 Anchor2 along the y-axis. (Local coordinates)
	 * @param maxLength The maximum distance for the bodies.
	 * @param collideConnected Whether the connected bodies should collide with each other. Defaults to false.
	 **/
	RopeJoint *newRopeJoint(Body *body1, Body *body2, float x1, float y1, float x2, float y2, float maxLength, bool collideConnected);

	/**
	 * Creates a new MotorJoint controlling the relative motion between body1
	 * and body2.
	 **/
	MotorJoint *newMotorJoint(Body *body1, Body *body2);
	MotorJoint *newMotorJoint(Body *body1, Body *body2, float correctionFactor, bool collideConnected);

	/**
	 * Calculates the distance between two Fixtures.
	 * @param fixtureA The first Fixture.
	 * @param fixtureB The sceond Fixture.
	 * @return The distance between them, and the two points closest
	 *         to each other.
	 **/
	int getDistance(lua_State *L);

	/**
	 * Sets the number of pixels in one meter.
	 * @param scale The number of pixels in one meter. (1m ~= 3.3ft).
	 **/
	static void setMeter(float scale);

	/**
	 * Gets the number of pixels in one meter.
	 * @return The number of pixels in one meter. (1m ~= 3.3ft).
	 **/
	static float getMeter();

	/**
	 * Scales a value down according to the current meter in pixels.
	 * @param f The unscaled input value.
	 **/
	static float scaleDown(float f);

	/**
	 * Scales a value up according to the current meter in pixels.
	 * @param f The unscaled input value.
	 **/
	static float scaleUp(float f);

	/**
	 * Scales a point down according to the current meter
	 * in pixels, for instance x = x0/meter, y = x0/meter.
	 * @param x The x-coordinate of the point to scale.
	 * @param y The y-coordinate of the point to scale.
	 **/
	static void scaleDown(float &x, float &y);

	/**
	 * Scales a point up according to the current meter
	 * in pixels, for instance x = x0/meter, y = x0/meter.
	 * @param x The x-coordinate of the point to scale.
	 * @param y The y-coordinate of the point to scale.
	 **/
	static void scaleUp(float &x, float &y);

	/**
	 * Scales a b2Vec2 down according to the current meter in pixels.
	 * @param v The unscaled input vector.
	 * @return The scaled vector.
	 **/
	static b2Vec2 scaleDown(const b2Vec2 &v);

	/**
	 * Scales a b2Vec up according to the current meter in pixels.
	 * @param v The unscaled input vector.
	 * @return The scaled vector.
	 **/
	static b2Vec2 scaleUp(const b2Vec2 &v);

	/**
	 * Scales a b2AABB down according to the current meter in pixels.
	 * @param aabb The unscaled input AABB.
	 * @return The scaled AABB.
	 **/
	static b2AABB scaleDown(const b2AABB &aabb);

	/**
	 * Scales a b2AABB up according to the current meter in pixels.
	 * @param aabb The unscaled input AABB.
	 * @return The scaled AABB.
	 **/
	static b2AABB scaleUp(const b2AABB& aabb);

	/**
	 * Calculates the stiffness and damping, given the linear frequency, and damping ratio.
	 * @param stiffness The output stiffness
	 * @param damping The output damping
	 * @param frequency The joint linear frequency
	 * @param dampingRatio The joint damping ratio
	 * @param bodyA The bodyA of the joint
	 * @param bodyB The bodyB of the joint
	 **/
	static void computeLinearStiffness(float& stiffness, float& damping, float frequency, float dampingRatio, b2Body* bodyA, b2Body* bodyB);

	/**
	 * Calculates linear frequency and damping ratio from stiffness and damping
	 * @param frequency The output frequency
	 * @param ratio The output damping ratio
	 * @param stiffness The joint stiffness
	 * @param damping The joint damping
	 * @param bodyA The bodyA of the joint
	 * @param bodyB The bodyB of the joint
	 **/
	static void computeLinearFrequency(float &frequency, float &ratio, float stiffness, float damping, b2Body *bodyA, b2Body *bodyB);

	/**
	 * Calculates the stiffness and damping, given the angular frequency, and damping ratio.
	 * @param stiffness The output stiffness
	 * @param damping The output damping
	 * @param frequency The joint angular frequency
	 * @param dampingRatio The joint damping ratio
	 * @param bodyA The bodyA of the joint
	 * @param bodyB The bodyB of the joint
	 **/
	static void computeAngularStiffness(float &stiffness, float &damping, float frequency, float dampingRatio, b2Body *bodyA, b2Body *bodyB);

	/**
	 * Calculates angular frequency and damping ratio from stiffness and damping
	 * @param frequency The output frequency
	 * @param ratio The output damping ratio
	 * @param stiffness The joint stiffness
	 * @param damping The joint damping
	 * @param bodyA The bodyA of the joint
	 * @param bodyB The bodyB of the joint
	 **/
	static void computeAngularFrequency(float &frequency, float &ratio, float stiffness, float damping, b2Body *bodyA, b2Body *bodyB);

	b2BlockAllocator *getBlockAllocator() { return &blockAllocator; }

private:

	// The length of one meter in pixels.
	static float meter;

	b2BlockAllocator blockAllocator;

}; // Physics

} // box2d
} // physics
} // love

#endif // LOVE_PHYSICS_BOX2D_PHYSICS_H
