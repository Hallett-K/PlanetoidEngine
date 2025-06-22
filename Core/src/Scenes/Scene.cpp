#include <Planetoid/Scenes/Scene.h>
#include <Planetoid/Scenes/Entity.h>
#include <Planetoid/Scenes/Components.h>

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

namespace PlanetoidEngine
{
    Scene::Scene()
    {
        m_paused = false;
    }

    Scene::~Scene()
    {
        m_registry.clear();
    }

    void Scene::OnLoad()
    {
        m_physicsWorld = new b2World(b2Vec2(0.0f, -15.0f));

        m_registry.view<UUID, Transform, RigidBody2D>().each([&](entt::entity entity, auto& uuid, auto& transform, auto& rb)
        {
            PlanetoidEngine::Entity e(entity, this);

            CreatePhysicsEntity(e, uuid, transform, rb);
        });
    }

    void Scene::OnUpdate(float deltaTime)
    {
        int velocityIterations = 6;
        int positionIterations = 2;
        m_physicsWorld->Step(deltaTime, velocityIterations, positionIterations);

        m_registry.view<UUID, Transform, RigidBody2D>().each([&](entt::entity entity, auto& uuid, auto& transform, auto& rb)
        {
            if (m_physicsBodyMap.find(uuid) == m_physicsBodyMap.end())
            {
                PlanetoidEngine::Entity e(entity, this);

                CreatePhysicsEntity(e, uuid, transform, rb);
            }
            b2Body* body = m_physicsBodyMap[uuid];
            if (body)
            {
                b2Vec2 position = body->GetPosition();
                transform.SetPosition(glm::vec3(position.x * m_pixelsPerMeter, position.y * m_pixelsPerMeter, transform.GetPosition().z));
                transform.SetRotation(glm::degrees(-body->GetAngle()));
            }
        });

        for (Entity entity : m_destroyList)
        {
            if (m_uuidMap.find(entity.GetComponent<UUID>()) != m_uuidMap.end())
            {
                m_uuidMap.erase(entity.GetComponent<UUID>());
            }

            if (m_physicsBodyMap.find(entity.GetComponent<UUID>()) != m_physicsBodyMap.end())
            {
                m_physicsBodyMap.erase(entity.GetComponent<UUID>());
            }

            m_registry.destroy(entity);
        }
        m_destroyList.clear();
    }

    void Scene::OnUnload()
    {
        delete m_physicsWorld;
        m_physicsWorld = nullptr;
    }

    void Scene::SetPaused(bool paused)
    {
        m_paused = paused;
    }

    bool Scene::IsPaused() const
    {
        return m_paused;
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity = Entity(m_registry.create(), this);
        entity.AddComponent<NameComponent>(name);
        PlanetoidEngine::UUID uuid = entity.AddComponent<UUID>();
        m_uuidMap[uuid] = entity;
        return entity;
    }

    Entity Scene::CreateEntity(const UUID& uuid, const std::string& name)
    {
        Entity entity = Entity(m_registry.create(), this);
        entity.AddComponent<NameComponent>(name);
        entity.AddComponent<UUID>(uuid);
        m_uuidMap[uuid] = entity;
        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        //m_registry.destroy(entity);
        m_destroyList.push_back(entity);
    }

    Entity Scene::GetEntity(const std::string& name)
    {
        auto view = m_registry.view<NameComponent>();
        for (entt::entity entity : view)
        {
            auto& nameComponent = view.get<NameComponent>(entity);
            if (nameComponent.name == name)
            {
                return Entity(entity, this);
            }
        }
        return Entity();
    }

    Entity Scene::GetEntity(const UUID& uuid)
    {
        auto it = m_uuidMap.find(uuid);
        if (it != m_uuidMap.end())
        {
            return it->second;
        }
        return Entity();
    }

    std::vector<Entity> Scene::GetAllEntities()
    {
        std::vector<Entity> entities;
        for (entt::entity entity : m_registry.view<UUID>())
        {
            entities.push_back(Entity(entity, this));
        }
        return entities;
    }

    void Scene::CreatePhysicsEntity(Entity entity, const UUID& uuid, Transform& transform, RigidBody2D& rigidBody)
    {
        b2BodyDef bodyDef;

        if (rigidBody.type == RigidBody2D::RigidBodyType::Static)
        {
            bodyDef.type = b2_staticBody;
        }
        else if (rigidBody.type == RigidBody2D::RigidBodyType::Dynamic)
        {
            bodyDef.type = b2_dynamicBody;
        }
        else if (rigidBody.type == RigidBody2D::RigidBodyType::Kinematic)
        {
            bodyDef.type = b2_kinematicBody;
        }

        glm::vec2 position = transform.GetPosition() / m_pixelsPerMeter;
        glm::vec2 scale = transform.GetScale() / m_pixelsPerMeter;

        bodyDef.position.Set(position.x, position.y);
        bodyDef.angle = glm::radians(-transform.GetRotation());

        b2Body* body = m_physicsWorld->CreateBody(&bodyDef);
        body->SetFixedRotation(!rigidBody.allowRotation);

        m_physicsBodyMap[uuid] = body;

        rigidBody._body = body;

        if (entity.HasComponent<BoxCollider2D>())
        {
            auto& boxCollider = entity.GetComponent<BoxCollider2D>();
            b2PolygonShape shape;
            b2Vec2 center = {boxCollider.offset.x / m_pixelsPerMeter, boxCollider.offset.y / m_pixelsPerMeter};
            if (boxCollider.scaleWithTransform)
            {
                shape.SetAsBox(scale.x * boxCollider.size.x, scale.y * boxCollider.size.y, center, 0.0f);
            }
            else
            {
                shape.SetAsBox(boxCollider.size.x, boxCollider.size.y, center, 0.0f);
            }

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shape;
            fixtureDef.density = boxCollider.density;
            fixtureDef.friction = boxCollider.friction;
            fixtureDef.restitution = boxCollider.restitution;
            fixtureDef.restitutionThreshold = m_restitutionThreshold;

            body->CreateFixture(&fixtureDef);
        }
        else if (entity.HasComponent<CircleCollider2D>())
        {
            auto& circleCollider = entity.GetComponent<CircleCollider2D>();
            b2CircleShape shape;
            b2Vec2 center = {circleCollider.offset.x / m_pixelsPerMeter, circleCollider.offset.y / m_pixelsPerMeter};
            if (circleCollider.scaleWithTransform)
            {
                shape.m_radius = scale.x * circleCollider.radius;
                shape.m_p = center;
            }
            else
            {
                shape.m_radius = circleCollider.radius;
                shape.m_p = center;
            }

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shape;
            fixtureDef.density = circleCollider.density;
            fixtureDef.friction = circleCollider.friction;
            fixtureDef.restitution = circleCollider.restitution;
            fixtureDef.restitutionThreshold = m_restitutionThreshold;

            body->CreateFixture(&fixtureDef);
        }
    }
}