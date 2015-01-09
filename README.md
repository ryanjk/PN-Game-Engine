# PN Game Engine

The PN Game Engine is a game engine I originally developed as a hobby project to support a new game idea. One of the design goals was to minimize dependency on 3rd-party libraries for game engine architecture, and so only SDL and OpenGL are used as its foundation. Developement of the engine is currently on hold; however, it does contain several features fully-implemented:

- Entity-Component-System (ECS) based architecture. 
  - Entities (simply a game object) may contain any type and combination of defined components.
  - Components:
    - <b>Transform</b>: An entity containing this component exists in the game world. Contains data for the entity's position, scale and rotation.
    - <b>Render</b>: An entity containing this component is rendered on-screen. Contains data necessary for rendering, such as the mesh, material (i.e. the shader to use), and color properties (diffuse, specular and ambient).
    - <b>Move</b>: An entity containing this component moves around the game world. Contains data for the entity's velocity, acceleration, angular velocity and angular acceleration.
    - <b>Collide</b>: An entity containing this component may physically interact with other colliding entities. Contains data for the entity's shape (cube {AABB or OBB} or spherical) and size.
    - <b>Light</b>: An containing this component emits light into the game world. Contains data for the type of light (point light, spotlight or directional light) and the color and intensity of the light.
