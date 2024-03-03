# M3D_lite

## Présentation

Ceci est une version ameliorée d'un projet réalisé durant ma première année du master ISICG. Le projet consistait à créer de toute pièce un moteur de rendu temps réel basé sur opengl. Avec ce moteur, il est possible de changer dynamiquement les objets qui composent la scène en ajoutant des assets ou en chargeant des scènes complètes.

Le moteur supporte deux formats de description de scène : le gltf et glb. Ces formats de fichier permettent de décrire des scènes assez complexes et dans le moteur vous pouvez retrouver :
- Trois types de lumière : la point light, la spot light, la sun light.
- Deux types de caméra : perspective, orthographique.
- Un graphe de scène qui décrit une hiérachie de transformations qui sont appliquées aux objets de la scène. L'utilisation d'un graphe de scène permet au moteur de supporter l'instancing. 
- Des matériaux physiquement basés qui sont attachés aux objets.

En ce qui concerne le rendu de la scène, il est réalisé par deffered shading, l'ombre produite par les lumières est calculée par des shadows map et par la suite plusieurs opérations de post-processing sont réalisées pour rendre l'image finale (bloom, fxaa, tone mapping et gama correction).
Depuis peu, le moteur implémente une méthode de "order independency transparency" par le biais d'une "per pixel linked list" et permet donc l'affichage des objets transparents.

## Résulats

| Pub | Pub zoom |
| - | - |
| ![pub](screenshots/pub.png) | ![pubZoom](screenshots/pubZoom.png) |

| Sponza | Sponza zoom |
| - | - |
| ![sponza](screenshots/sponza.png) | ![sponzaZoom](screenshots/sponzaZoom.png) |

| Emissive | Musee  |
| - | - |
| ![Emissive](screenshots/emissive.png) | ![Musee](screenshots/musee.png) |

## Améliorations

Les plus urgentes :
- Implémentation du papier [Screen Space Indirect Lighting with Visibility Bitmask](https://arxiv.org/pdf/2301.11376.pdf) pour approximer l'illumination gobale de la scène.
- Changement de la méthode de calcul d'ombrage par du ray tracing. Cette méthode permettra en plus d'avoir des ombres plus propre, de prendre en compte les ombres colorées projetées par les objets transparents. En contrepartie il sera plus couteux d'omptenir des ombres douces.
- Implémentation d'une méthodes alternative d'anti-alliasing au FXAA déjà présent (SMAA ou TAA/TXAA/TSSAA).
- Changement de méthode de tone mapping (ACES => AgX).

Accélération des calculs :
- Ajout de "frustum culling" lors de la "geometrie pass" grâce à l'utilisation de boîtes englobantes et d'une hiérachie de scène. 
- Ajout d'occlusion culling lors de la "geometrie pass".
- Utilisation de micro meshes permetant une meilleur gestion de la mémoire ainsi que des LOD. 

Amélioration de la qualité visuel :
- Gestion d'une environement map (+IBL).
- Ajout d'une méthode pour amélioré la qualité visuel des materiaux les plus réfléchissant (SS réfléxion ou ray tracing).
- Ajout d'une méthode pour amélioré la qualité visuel des materiaux les plus transparent (ss réfraction ou ray tracing).
- Amélioration de la brdf utilisé pour prendre en compte plus de propriétées (sheen, coat, anisotropy, translucent/subsurface, iridecence, ...).
- Amélioration du normal mapping par des méthodes de paralax mapping (ou parallax occlusion mapping).
- Ajout d'une méthode d'alpha to coverage et d'alpha distribution for alpha testing dans le but d'améliorer le rendu des objets transparent.
 
Rendre le monde plus vivant :
- Rendu d'athmosphère grâce à la méthode de Sébastien Hillaire, [A Scalable and Production Ready Sky and Atmosphere Rendering Technique](https://sebh.github.io/publications/egsr2020.pdf).
- Gestion du skinnig des meshes.
- Ajout d'un moteur physique.
- Utilistion de boids ou de modèle de Lormtck Volterra (modèle de prédateur - proids) pour ajouter des NPC simples.

## Bugs

- les fragments transparents ont tendencent à fliker
- le bloom n'est pas stable d'une frame à l'autre
- add asset ne marche pas
- les inputs d'imgui sont flingués
- la génération des mipmap des texture d'OpenGl prend beaucoup trop de RAM 
- imgui bug end/endchild
- clear bug sometime
- si nb channel d'une image est 1 utilise le channel rouge 4 fois
- agx lut n'est pas load correctement
