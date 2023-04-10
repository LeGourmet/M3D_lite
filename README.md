BUGS
	- les trois classes controller à revoir => parfois avec la perte de focus ne capte pas la fin d'un event
	- rotation de quaternion à tendance à faire tourner un objet sur lui même au recalcule de ses vecteurs
	- mauvaise gestion de la conversion d'unité de la lumière (material emissivity + light constructor)
	- debug plante à cause de vector (scene manager) 

TODO
	- enlever tous les news inutile
	- remplacer tinygltf par fastgltf
	- revoir la classe image
	- revoir le constructeur de la camera
	- ajouter les animations
	- ajouter une ui
	- revoir toutes les fonctions de scene_graph_node
	- scene_graph_node, ajouter possibilité de changer de parent (penser à remove le child chez le parent)

WARNING
	- ne supporte pas l'alpha mode du materiaux, si il y un canal alpha et où les valeurs != 1 => blend
	- ne prend pas en compte l'aspect ratio des cameras perspective