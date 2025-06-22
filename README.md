
# Planetoid Engine

Planetoid Engine is a 2D toy game engine that I've been working on in my free time. There's no real purpose to it beyond as a learning project and I wouldn't recommend use of it for serious projects - although there's absolutely nothing stopping you.


## Changelog

2025-06-22
-
Starship:

- Corrected Viewport Orientation - no longer rendering upside down
- Renamed main.cpp to Starship.cpp 
- Camera Component Clear Colour widget in inspector, serializes and deserializes
- Performance Metric Report window that displays frametime metrics opens at the end of PIE
- Changed Editor versioning format to follow commit dates
- Temporarily removed versioning check when loading project

Engine:

- Enabled Alpha Blending for transparent PNG support
- Added Clear Colour property to Camera Component 
- Application::Exit() is now virtual
- Changed Engine versioning format to follow commit dates
- Entity destruction now safer

General:

- README.md added to repo

2025-06-11
-

General:

- Initial Commit

## Planned Features

Starship:

- Sample Projects
- Improved Performance metrics - Per-Scene, draw calls
- Render modes - Wireframe, Collision
- Drag/Drop Entity List
- Parent/Child Hierarchy
- Better Editor Camera Controls
- Customisable Build Pipelines
- Project Settings
- Editor Preferences
- Sprite Atlasing and Animation Tooling

Engine:

- Better Box2D Integration
- Improved Script API
- Collision Detection
- SDF Text Rendering
- Audio
- Particle System
- Materials
- Improved Input System
- Threaded Game Loop
- Sprite Atlasing
- Linux Support

General:

- Planetoid Engine Runtime