#include "Engine.h"

bool Engine::Initialise(HINSTANCE hInstance, std::string window_title, std::string window_class, float width, float height)
{
    if(!this->render_window.Initialise(this, hInstance, window_title, window_class, width, height)) return false;
    if (!gfx.Initialise(this->render_window.GetHWND(), width, height))return false;
    
    return true;
   
}

bool Engine::ProcessMessages()
{
    return this->render_window.ProcessMessages();
}

void Engine::Update()
{
    while (!keyboard.CharBufferIsEmpty())
    {
        unsigned char ch = keyboard.ReadChar();
    }
    while (!keyboard.KeyBufferIsEmpty())
    {
        KeyboardEvent kbe = keyboard.ReadKey();
        unsigned char keycode = kbe.GetKeyCode();
    }
    while (!mouse.EventBufferIsEmoty())
    {
        MouseEvent me = mouse.ReadEvent();
    }


}

void Engine::RenderFrame()
{
    gfx.RenderFrame();
}
