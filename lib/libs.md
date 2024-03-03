# Libs last update

| name | date | link |
| - | - | - |
| SDL3 | 02/03/2024 | https://github.com/libsdl-org/SDL |
| imgui | 01/03/2024 | https://github.com/ocornut/imgui |
| portable file dialogs | 11/11/2022 | https://github.com/samhocevar/portable-file-dialogs |
| OpenGl | --- | https://github.com/skaslev/gl3w |
| fastgltf | 02/03/2024 | https://github.com/spnda/fastgltf |
| simdjson | 27/02/2024 | https://github.com/simdjson/simdjson |
| stb | 14/12/2023 | https://github.com/nothings/stb |
| glm | 28/02/2024 | https://github.com/g-truc/glm |

### Modifications

- stb/stb_image_write.h : function stbi_write_hdr_core (line 761)
- fastgltf.cpp line 709 fs::u8path => fs::path (cpp20)
- glm 3 files #error => #pragma message(...)

## Todo

- Change ImGUI for QT

- Add Vulkan : https://vulkan.lunarg.com/ 
- Add Molten VK : https://github.com/KhronosGroup/MoltenVK

- Add entt : https://github.com/skypjack/entt for entity componenent system and more
