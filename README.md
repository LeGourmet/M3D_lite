# M3D_lite

## Pr�sentation

## R�sulats

## Am�liorations

- meilleur calcule de la lumi�re ambient et ajout d'ambient occlusion par effet screen space aproximant la global illumination (impl�mentation du papier : "")
- gestion de la transparence par "per pixel linked list methode" 
- calcule d'ombre projet� des objets transparent
- ajout de volumes englobant au objet et hierachie de sc�ne
- ajout de cascade shadow map et filtre pcss aux ombres
- ajout frutum culling et occlusion culling
- anti-alliasing par temporal methodes (taa - tssaa - txaa)
- am�lioration de la brdf ( ajout de clearcoat, ior, anisotropie, thin/sheen)
- ajout de screen space reflection/refraction pour les objets tr�s peut rugeux
- alpha to coverage
- alpha distribution for alpha testing
- athmospherique rendering (sebastian hillaire)
- bones and riggings