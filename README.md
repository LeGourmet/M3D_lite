# M3D_lite

## Pr�sentation

Ceci est une version amelior�e d'un projet r�alis� durant ma premi�re ann�e du master ISICG. Le projet consistait � cr�er de toute pi�ce un moteur de rendu temps r�el bas� sur opengl. Avec ce moteur, il est possible de changer dynamiquement les objets qui composent la sc�ne en ajoutant des assets ou en chargeant des sc�nes compl�tes.

Le moteur supporte deux formats de description de sc�ne : le gltf et glb. Ces formats de fichier permettent de d�crire des sc�nes assez complexes et dans le moteur vous pouvez retrouver :
- Trois types de lumi�re : la point light, la spot light, la sun light.
- Deux types de cam�ra : perspective, orthographique.
- Un graphe de sc�ne qui d�crit une hi�rachie de transformations qui sont appliqu�es aux objets de la sc�ne. L'utilisation d'un graphe de sc�ne permet au moteur de supporter l'instancing. 
- Des mat�riaux physiquement bas�s qui sont attach�s aux objets.

En ce qui concerne le rendu de la sc�ne, il est r�alis� par deffered shading, l'ombre produite par les lumi�res est calcul�e par des shadows map et par la suite plusieurs op�rations de post-processing sont r�alis�es pour rendre l'image finale (bloom, FXAA, tone mapping et gama correction (AgX)).
Depuis peu, le moteur impl�mente une m�thode de "order independency transparency" par le biais d'une "per pixel linked list" et permet donc l'affichage des objets transparents.

## R�sulats

| Pub | Pub zoom |
| - | - |
| ![pub](screenshots/pub.png) | ![pubZoom](screenshots/pubZoom.png) |

| Sponza | Sponza zoom |
| - | - |
| ![sponza](screenshots/sponza.png) | ![sponzaZoom](screenshots/sponzaZoom.png) |

| Emissive | Musee  |
| - | - |
| ![Emissive](screenshots/emissive.png) | ![Musee](screenshots/musee.png) |

## Am�liorations

Les plus urgentes :
- Impl�mentation du papier [Screen Space Indirect Lighting with Visibility Bitmask](https://arxiv.org/pdf/2301.11376.pdf) pour approximer l'illumination gobale de la sc�ne.
- Changement de la m�thode de calcul d'ombrage par du ray tracing. Cette m�thode permettra en plus d'avoir des ombres plus propre, de prendre en compte les ombres color�es projet�es par les objets transparents. En contrepartie il sera plus couteux d'omptenir des ombres douces.
- Impl�mentation d'une m�thodes alternative d'anti-alliasing au FXAA d�j� pr�sent (SMAA ou TAA/TXAA/TSSAA).

Acc�l�ration des calculs :
- Ajout de "frustum culling" lors de la "geometrie pass" gr�ce � l'utilisation de bo�tes englobantes et d'une hi�rachie de sc�ne. 
- Ajout d'occlusion culling lors de la "geometrie pass".
- Utilisation de micro meshes permetant une meilleur gestion de la m�moire ainsi que des LOD.
 
Am�lioration de la qualit� visuel :
- Gestion d'une environement map (+IBL).
- Ajout d'une m�thode pour am�lior� la qualit� visuel des materiaux les plus r�fl�chissant (SS r�fl�xion ou ray tracing).
- Ajout d'une m�thode pour am�lior� la qualit� visuel des materiaux les plus transparent (ss r�fraction ou ray tracing).
- Am�lioration de la brdf utilis� pour prendre en compte plus de propri�t�es (sheen, coat, anisotropy, translucent/subsurface, iridecence, ...).
- Am�lioration du normal mapping par des m�thodes de paralax mapping (ou parallax occlusion mapping).
- Ajout d'une m�thode d'alpha to coverage et d'alpha distribution for alpha testing dans le but d'am�liorer le rendu des objets transparent.
- Ajout d'effet de post processing comme lens ghosting, vignetting ou autre filtres.
- Revoir la m�thode de g�n�ration du bloom effect et ajouter un treshold
 
Rendre le monde plus vivant :
- Rendu d'athmosph�re gr�ce � la m�thode de S�bastien Hillaire, [A Scalable and Production Ready Sky and Atmosphere Rendering Technique](https://sebh.github.io/publications/egsr2020.pdf).
- Gestion du skinnig des meshes.
- Ajout d'un moteur physique.
- Utilistion de boids ou de mod�le de Lormtck Volterra (mod�le de pr�dateur - proids) pour ajouter des NPC simples.

## Bugs

- le bloom n'est pas stable d'une frame � l'autre
- la g�n�ration des mipmap des texture d'OpenGl prend beaucoup trop de RAM
- le normal induit des zones compl�tement noir aux angles razant quelquesoit la sc�ne (compute lighting) (usual suspect cosNL)
- brdf ne semble pas tout a fait donner la bonne illumination (chess)

- imgui debug mode : cannot close settings : end/endchild

- certaines scene ne sont pas correctement load � cause de fastgltf (toycar crash, pub (texture 1 cannal to repeat))
- clear scene ne marchent pas => semblent provenir d'un bug sur les maps utilis� par le gpu 

- la compilation ne se fait pas au bon endroit de l'arborecense

- presences de plusieurs botleneck qui bride grandement le GPU (pass filterAndSortFrament + autre)
- check si compute shader ne sont pas plus rentable pour le calcule de l'illumination des fragments
