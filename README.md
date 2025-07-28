# Ray-Tracer

<img width="441" height="241" alt="Allobjects" src="https://github.com/user-attachments/assets/1542322f-6f61-4e83-98dd-848df6b62f17" />


I implement a ray tracer that renders 3D scenes that includes different type of geometric objects and global illumination features. The ray traced scene includes a box defined by six axis-aligned planes, each with a different colour. Several objects are present in the scene. These include:
1. Four spheres: One transparent, one reflective, one refractive, one flat sphere. 2. One single cone.
3. One textured cylinder with cap.
4. One planar object that acts as a mirror, reflecting the surrounding objects.
5. One planar surface with a chequered pattern serving as the bottom floor.
All objects are placed on top of a table and cast multiple shadows, as I have implemented several light sources along with a spotlight. Fog and anti-aliasing have also been applied to enhance the visual realism of the rendered scene.
Estimated time taken for the program to generate the output on a personal laptop: the number of cells along x and y directions:
• With NUMDIV = 1500 and anti-aliasing turned on: 5 minutes 56 seconds.
• With NUMDIV = 1500 and anti-aliasing turned off: 1 minute 18 seconds.
• With NUMDIV = 500 and anti-aliasing turned on: 41 seconds.
• With NUMDIV = 500 and anti-aliasing turned off: 12 seconds.

<img width="493" height="528" alt="WithoutFog" src="https://github.com/user-attachments/assets/d2599762-8c51-4032-be4a-2572cb7ff836" />


<img width="499" height="531" alt="WithFog" src="https://github.com/user-attachments/assets/76be022b-2f02-4dca-9afa-14bec1001098" /> />


<img width="174" height="135" alt="spotlight" src="https://github.com/user-attachments/assets/1146a911-b970-4b35-8484-5889dcb6f0da" /> />





