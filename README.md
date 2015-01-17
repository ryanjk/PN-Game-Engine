# PN Game Engine

The PN Game Engine is a game engine I originally developed as a hobby project to support a new game idea. One of the design goals was to minimize dependency on 3rd-party libraries for game engine architecture, and so only SDL and OpenGL are used as its foundation. Developement of the engine is currently on hold; however, it does contain several features fully-implemented:

- <b>Entity-Component-System</b> (ECS) based architecture. 
  - Entities (simply a game object) may contain any type and combination of defined components.
  - Components:
    - <b>Transform</b>: An entity containing this component exists in the game world. Contains data for the entity's position, scale and rotation.
    - <b>Render</b>: An entity containing this component is rendered on-screen. Contains data necessary for rendering, such as the mesh, material (i.e. the shader to use), and color properties (diffuse, specular and ambient).
    - <b>Move</b>: An entity containing this component moves around the game world. Contains data for the entity's velocity, acceleration, angular velocity and angular acceleration.
    - <b>Collide</b>: An entity containing this component may physically interact with other colliding entities. Contains data for the entity's shape (cube {AABB or OBB} or spherical) and size.
    - <b>Light</b>: An entity containing this component emits light into the game world. Contains data for the type of light (point light, spotlight or directional light), the color and intensity of the light, and other type-dependent characteristics (such as max radius and inner/outer angles for spotlights).

- <b>External scene (game world) definition</b>
  - The PN engine parses a specially-formatted JSON file (saved with a *.state file extension) to create the game world. The JSON file contains all necessary data to describe the scene.
  - Changes to the game world don't require any recompilation of the engine, so iterating and debugging the gameplay can be accomplished quickly and easily.
  - Example of a simple .state file:
  ```JSON
  	"entities": {
  		"monkey": {
  			"components": {
  				"transform": {
  					"translation": [-2.0, 3.0, 0.0],
  					"scale": [2.0, 2.0, 2.0],
  					"rotation": [0.7, 0.0, 0.0]
  				},
  				"render": {
  					"mesh":"monkey.obj",
  					"material":"dynamic_light.sp",
  					"ambient": [0.1, 0.0, 0.0, 1.0],
  					"diffuse": "monkey_diffuse.png",
  					"specular": [0.4, 0.4, 0.4, 1.0],
  					"gloss":100
  				}
  			}
  		}
  	}
  ```
  - This describes a scene that contains only a single entity named "monkey." The "monkey" entity has two components, a transform and render component. All of the component data is also defined in the .state file.

- <b>Multiple shader (material) support</b>
  - The custom OpenGL-based rendering engine supports the addition of new GLSL shader programs.
  - Currently, there are three shaders in use:
    - <b>Statically-lit materials</b> are those whose final colour is a function only of their given ambient, diffuse, and specular colours. The diffuse colour is given as a texture image.
    - <b>Dynamically-lit materials</b> are statically-lit materials with the addition of being influenced by dynamic lights in the scene. This includes point lights, spot lights and directional lights. 
    - The <b>screen-overlay shader</b> is used to produce images rendered over the world view, as you'd expect from most GUI elements or a HUD.
