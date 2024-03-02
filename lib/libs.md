# Libs last update

| name | date | link |
| - | - | - |
| SDL3 | | https://github.com/libsdl-org/SDL |
| imgui | | https://github.com/ocornut/imgui |
| portable file dialogs | 11/11/2022 | https://github.com/samhocevar/portable-file-dialogs |
| OpenGl | | https://github.com/skaslev/gl3w |
| fastgltf | 29/02/2024 | https://github.com/spnda/fastgltf |
| simdjson | 27/02/2024 | https://github.com/simdjson/simdjson |
| stb | 14/12/2023 | https://github.com/nothings/stb |
| glm | 28/02/2024 | https://github.com/g-truc/glm |

Old libs

IMGUI 		    1.89.4	21/05/2023	: https://github.com/ocornut/imgui
SDL3			3.2.0	21/05/2023	: https://github.com/libsdl-org/SDL
GL (gl3w)		4.6 	19/03/2023	: https://github.com/skaslev/gl3w

### Modifications

- stb/stb_image_write.h : function stbi_write_hdr_core (line 761)
- fastgltf.cpp line 709 fs::u8path => fs::path (cpp20)
- glm 3 files #error => #pragma message(...)
(- change fastgltf #include <fastgltf\...> to #include <...>)

## Todo

- Change ImGUI for QT

- Add Vulkan : https://vulkan.lunarg.com/ 
- Add Molten VK : https://github.com/KhronosGroup/MoltenVK

- Add entt : https://github.com/skypjack/entt for entity componenent system and more
