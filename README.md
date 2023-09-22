# M3D_lite

## Présentation

## Résulats

## Améliorations

- meilleur calcule de la lumière ambient et ajout d'ambient occlusion par effet screen space aproximant la global illumination (implémentation du papier : "")
- gestion de la transparence par "per pixel linked list methode" 
- calcule d'ombre projeté des objets transparent
- ajout de volumes englobant au objet et hierachie de scène
- ajout de cascade shadow map et filtre pcss aux ombres
- ajout frutum culling et occlusion culling
- anti-alliasing par temporal methodes (taa - tssaa - txaa)
- amélioration de la brdf ( ajout de clearcoat, ior, anisotropie, thin/sheen)
- ajout de screen space reflection/refraction pour les objets très peut rugeux
- alpha to coverage
- alpha distribution for alpha testing
- athmospherique rendering (sebastian hillaire)
- bones and riggings