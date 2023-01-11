#ifndef __UI_USER_INTERFACE__
#define __UI_USER_INTERFACE__

#include "imgui/imgui_impl_sdl.h"
#include "utils/portable_file_dialogs.h"

namespace TutoVulkan
{
namespace UI
{
    class UserInterface
    {
    public:
        // --------------------------------------------- DESTRUCTOR / CONSTRUCTOR ---------------------------------------------
        UserInterface();

        ~UserInterface();

        // ----------------------------------------------------- GETTERS -------------------------------------------------------
        SDL_Window *getWindow() const { return _window; }

        // ---------------------------------------------------- FONCTIONS ------------------------------------------------------
        void update();

    private:
        // ----------------------------------------------------- ATTRIBUTS ----------------------------------------------------
        SDL_Window *_window = nullptr;
        std::shared_ptr<pfd::open_file> _openFileDialog = nullptr;

        // ------------------------------------------------------- INIT --------------------------------------------------------
        void _initSDL2();
        static void _initIMGUI();

        // ------------------------------------------------------ DISPOSE ------------------------------------------------------
        void _disposeAll();
        void _disposeSDL2();
        static void _disposeIMGUI();
    };
} // namespace UI
} // namespace TutoVulkan

#endif
