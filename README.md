# M3D_lite

## Pr�sentation

Ceci est une version amelior�e d'un projet r�alis� durant ma premi�re ann�e du master ISICG. Le projet consistait � cr�er de toute pi�ce un moteur de rendu temps r�el bas� sur opengl. Avec ce moteur, il est possible de changer dynamiquement les objets qui composent la sc�ne en ajoutant des assets ou en chargeant des sc�nes compl�tes.

Le moteur supporte deux formats de description de sc�ne : le gltf et glb. Ces formats de fichier permettent de d�crire des sc�nes assez complexes et dans le moteur vous pouvez retrouver :
- Trois types de lumi�re : la point light, la spot light, la sun light.
- Deux types de cam�ra : perspective, orthographique.
- Un graphe de sc�ne qui d�crit une hi�rachie de transformations qui sont appliqu�es aux objets de la sc�ne. L'utilisation d'un graphe de sc�ne permet au moteur de supporter l'instancing. 
- Des mat�riaux physicaly based qui sont attach�s aux objets.

En ce qui concerne le rendu de la sc�ne, il est r�alis� par deffered shading, l'ombre produite par les lumi�res est calcul�e par des shadows map et par la suite plusieurs op�rations de post-processing sont r�alis�es pour rendre l'image finale (bloom, tone mapping et gama correction).

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
- Impl�mentation d'une m�thode "order independency transparency" par le biais de [per pixel linked list](https://ubm-twvideo01.s3.amazonaws.com/o1/vault/gdc10/slides/Thibieroz_Nicolas_AdvancedVisualEffectsWithDirect3D_OIT_and_Indirect_Illumination_using_DX11_Linked_Lists_part1.pdf) pour la gestion des objets transparents.
- Impl�mentation de m�thodes d'anti-alliasing (FXAA - SMAA).

Acc�l�ration des calculs :
- Ajout de "frustum culling" gr�ce � l'utilisation de bo�tes englobantes et d'une hi�rachie de sc�ne. 
- Utilisation de "frustum culling" pour la g�n�ration des cartes d'ombrage.
- Ajout d'occlusion culling.

Am�liorer les ombres :
- Calcul de l'ombre projet�e des objets transparents par l'utilisation de techniques de "weighted blending".
- Utilisation de [cascade shadow map](https://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf) pour r�duire l'effet de cr�nelage des ombres.
- Utilisation d'un biais dynamique bas� sur les m�thodes de "slope-scale depth biais" dans le but d'enti�rement supprimer le shadow acne et le Peter Panning.
- Filtrage de l'ombre par [Percentage_Closer_Soft_Shadows](https://http.download.nvidia.com/developer/presentations/2005/SIGGRAPH/Percentage_Closer_Soft_Shadows.pdf).

Rendre le monde plus vivant :
- Rendu d'athmosph�re gr�ce � la m�thode de S�bastien Hillaire, [A Scalable and Production Ready Sky and Atmosphere Rendering Technique](https://sebh.github.io/publications/egsr2020.pdf).
- Gestion du skinnig des meshes.
- Ajout d'un moteur physique.
