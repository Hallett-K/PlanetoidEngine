#include <Planetoid/Scenes/Components.h>

#include <box2d/b2_body.h>

namespace PlanetoidEngine
{
    glm::vec2 RigidBody2D::GetPosition() const
    {
        b2Body* body = (b2Body*)_body;

        return glm::vec2(body->GetPosition().x, body->GetPosition().y);
    }

    void RigidBody2D::SetPosition(const glm::vec2& position)
    {
        b2Body* body = (b2Body*)_body;

        body->SetTransform(b2Vec2(position.x, position.y), body->GetAngle());
    }

    float RigidBody2D::GetRotation() const
    {
        b2Body* body = (b2Body*)_body;

        return body->GetAngle();
    }

    void RigidBody2D::SetRotation(float rotation)
    {
        b2Body* body = (b2Body*)_body;

        body->SetTransform(body->GetPosition(), rotation);
    }

    void RigidBody2D::ApplyForce(const glm::vec2& force, const glm::vec2& point)
    {
        b2Body* body = (b2Body*)_body;

        body->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(point.x, point.y), true);
    }

    void RigidBody2D::ApplyTorque(float torque)
    {
        b2Body* body = (b2Body*)_body;

        body->ApplyTorque(torque, true);
    }

    void RigidBody2D::ApplyLinearImpulse(const glm::vec2& impulse, const glm::vec2& point)
    {
        b2Body* body = (b2Body*)_body;

        body->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(point.x, point.y), true);
    }

    void RigidBody2D::ApplyAngularImpulse(float impulse)
    {
        b2Body* body = (b2Body*)_body;

        body->ApplyAngularImpulse(impulse, true);
    }
}