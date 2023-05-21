BUGS
	- les trois classes controller à revoir => parfois avec la perte de focus ne capte pas la fin d'un event
	- rotation de quaternion à tendance à faire tourner un objet sur lui même au recalcule de ses vecteurs
	- mauvaise gestion de la conversion d'unité de la lumière (material emissivity + light constructor)
	- target fps pas parfait => use frame count
	- auto intersection ombre (tangent - bitangent ?)
	- mauvaise calcule near/far shadow
	- emissive.glb won't load ??

TODO
	- enlever tous les news inutile
	- remplacer tinygltf par fastgltf
	- revoir la classe image
	- revoir le constructeur de la camera
	- revoir les controllers
	- ajouter la gestion du son dans window
	- coder screenshot + take video
	- ajouter les animations
	- ajouter une ui
	- revoir toutes les fonctions de scene_graph_node
	- scene_graph_node, ajouter possibilité de changer de parent (penser à remove le child chez le parent)
	- implement shadow volume : stencil + genereate geometry "shadow volulme"

WARNING
	- ne prend pas en compte l'aspect ratio des cameras perspective
	- regler pb de mipmap avec alpha des images