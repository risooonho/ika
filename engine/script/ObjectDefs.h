#ifndef SCRIPT_OBJECTDEFS_H
#define SCRIPT_OBJECTDEFS_H

#define METHOD(x, type) PyObject* x(type* self, PyObject* args)
#define METHOD1(x, type) PyObject* x(type* self)

#include <sstream>
#include <map>
#include "Python.h"

// Rain of prototypes
namespace Ika {  // X11 fix
    struct Font;
}
struct Entity;
class Engine;
class Canvas;

class Input;
class InputDevice;
class InputControl;
class Keyboard;
class Mouse;
class Joystick;

namespace audiere   {   class OutputStream; }
namespace Video
{
    class Driver;
    class Image;
}

/// Contains implementations of Python binding things.
namespace Script
{
    // Hardware interfaces
    /// Reflects an image.
    namespace Image
    {
        // object type
        struct ImageObject
        {
            PyObject_HEAD
            ::Video::Image* img;
        };

        // Methods
        METHOD(Image_Blit, ImageObject);
        METHOD(Image_ScaleBlit, ImageObject);
        METHOD(Image_DistortBlit, ImageObject);
        METHOD(Image_Clip, ImageObject);

        void Init();
        PyObject* New(::Video::Image* image);
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(ImageObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects a sound stream.
    namespace Sound
    {
        // Object type
        struct SoundObject
        {
            PyObject_HEAD
            audiere::OutputStream* sound;
        };

        // Methods
        METHOD1(Sound_Play, SoundObject);
        METHOD1(Sound_Pause, SoundObject);

        void Init();
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(SoundObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects an input control.
    namespace Control
    {
        // Object type
        struct ControlObject
        {
            PyObject_HEAD
            InputControl* control;
        };

        // Methods
        METHOD1(Control_Pressed, ControlObject);
        METHOD1(Control_Position, ControlObject);
        METHOD1(Control_Delta, ControlObject);

        void Init();
        void Destroy(ControlObject* self);
        PyObject* New(InputControl* control);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    // Engine resources
    /// Reflects a canvas. (software image surface thingie)
    namespace Canvas
    {
        // Object type
        struct CanvasObject
        {
            PyObject_HEAD
            bool ref;
            ::Canvas* canvas;
        };

        // Methods
        METHOD(Canvas_Save, CanvasObject);
        METHOD(Canvas_Blit, CanvasObject);
        METHOD(Canvas_ScaleBlit, CanvasObject);
        METHOD(Canvas_WrapBlit, CanvasObject);
        METHOD(Canvas_GetPixel, CanvasObject);
        METHOD(Canvas_SetPixel, CanvasObject);
        METHOD(Canvas_DrawText, CanvasObject);
        METHOD(Canvas_Clear, CanvasObject);
        METHOD(Canvas_Resize, CanvasObject);
        METHOD(Canvas_Clip, CanvasObject);
        METHOD1(Canvas_Rotate, CanvasObject);
        METHOD1(Canvas_Flip, CanvasObject);
        METHOD1(Canvas_Mirror, CanvasObject);

        void Init();
        PyObject* New(::Canvas* c);
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(CanvasObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects a bitmap font. (maybe a std::vector font later on)
    namespace Font
    {
        // Object type
        struct FontObject
        {
            PyObject_HEAD
            Ika::Font* font;
        };

        // Methods
        METHOD(Font_Print, FontObject);
        METHOD(Font_CenterPrint, FontObject);
        METHOD(Font_RightPrint, FontObject);
        METHOD(Font_StringWidth, FontObject);

        void Init();
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(FontObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects a map entity.
    namespace Entity
    {
        // Object type
        struct EntityObject
        {
            PyObject_HEAD
            ::Entity* ent;
        };

        extern std::map< ::Entity*, EntityObject*> instances;

        // Methods
        METHOD(Entity_MoveTo, EntityObject);
        METHOD(Entity_Wait, EntityObject);
        METHOD(Entity_Stop, EntityObject);
        METHOD(Entity_IsMoving, EntityObject);
        METHOD(Entity_DetectCollision, EntityObject);
        METHOD(Entity_Touches, EntityObject);
        METHOD(Entity_Draw, EntityObject);

        void Init();
        PyObject* New(::Entity* ent);
        PyObject* New(PyTypeObject* type, PyObject* args, PyObject* kw);
        void Destroy(EntityObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    namespace InputDevice
    {
        struct DeviceObject
        {
            PyObject_HEAD
            ::InputDevice* device;
        };

        // Methods
        METHOD1(Device_Update, DeviceObject);
        METHOD(Device_GetControl, DeviceObject);

        void Init();
        PyObject* New(::InputDevice* device);
        void Destroy(DeviceObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    namespace Keyboard
    {
        METHOD1(Keyboard_GetKey, PyObject);
        METHOD1(Keyboard_WasKeyPressed, PyObject);
        METHOD1(Keyboard_ClearKeyQueue, PyObject);

        void Init();
        PyObject* New();
        void Destroy(PyObject* self);

        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    namespace Mouse
    {
        void Init();
        PyObject* New();
        void Destroy(PyObject* self);

        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    namespace Joystick
    {
        struct JoystickObject
        {
            PyObject_HEAD
            ::Joystick* joystick;

            // Tuples containing all the controls.
            PyObject* axes;
            PyObject* reverseAxes;
            PyObject* buttons;
        };

        void Init();
        PyObject* New(::Joystick* joystick);
        void Destroy(JoystickObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    // Singletons
    /// Reflects the input core.
    namespace Input
    {
        // Object type
        struct InputObject
        {
            PyObject_HEAD
            PyObject* keyboard;
            PyObject* mouse;
            PyObject* joysticks; // tuple containing joystick objects
        };

        // Methods
        METHOD1(Input_Update, InputObject);
        METHOD1(Input_Unpress, InputObject);

        void Init();
        PyObject* New();
        void Destroy(InputObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects the video driver.
    namespace Video
    {
        // Object type
        struct VideoObject
        {
            PyObject_HEAD
            ::Video::Driver* video;
        };

        // Methods
        METHOD(Video_Blit, VideoObject);
        METHOD(Video_ScaleBlit, VideoObject);
        METHOD(Video_DistortBlit, VideoObject);
        METHOD(Video_TileBlit, VideoObject);
        METHOD(Video_TintBlit, VideoObject);
        METHOD(Video_TintDistortBlit, VideoObject);
        METHOD(Video_DrawPixel, VideoObject);
        METHOD(Video_DrawLine, VideoObject);
        METHOD(Video_DrawRect, VideoObject);
        METHOD(Video_DrawEllipse, VideoObject);
        METHOD(Video_DrawTriangle, VideoObject);
        METHOD(Video_ClipScreen, VideoObject);
        METHOD(Video_GrabImage, VideoObject);
        METHOD(Video_GrabCanvas, VideoObject);
        METHOD1(Video_ClearScreen, VideoObject);
        METHOD1(Video_ShowPage, VideoObject);
        METHOD(Video_SetResolution, VideoObject);

        void Init();
        PyObject* New(::Video::Driver* v);
        void Destroy(VideoObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// Reflects the current map, and some aspects of the engine.
    namespace Map
    {
        // Object type

        // Methods
        METHOD(Map_Switch, PyObject);
        METHOD(Map_GetMetaData, PyObject);
        METHOD(Map_Render, PyObject);
        METHOD(Map_GetTile, PyObject);
        METHOD(Map_SetTile, PyObject);
        METHOD(Map_GetObs, PyObject);
        METHOD(Map_SetObs, PyObject);
        METHOD(Map_GetZone, PyObject);
        METHOD(Map_SetZone, PyObject);
        METHOD(Map_GetLayerName, PyObject);
        METHOD(Map_SetLayerName, PyObject);
        METHOD(Map_FindLayerByName, PyObject);
        METHOD(Map_GetParallax, PyObject);
        METHOD(Map_SetParallax, PyObject);
        METHOD(Map_GetLayerProperties, PyObject);
        METHOD(Map_GetLayerPosition, PyObject);
        METHOD(Map_SetLayerPosition, PyObject);
        METHOD(Map_GetZones, PyObject);
        METHOD(Map_GetWaypoints, PyObject);
        METHOD(Map_GetAllEntities, PyObject);

        void Init();
        PyObject* New();
        void Destroy(PyObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    /// lil' Python object used to redirect error messages to pyout.log
    namespace Error
    {
        // Object type

        // Methods
        METHOD(Error_Write, PyObject);

        void Init();
        PyObject* New();
        void Destroy(PyObject* self);

        // Method table
        extern PyMethodDef methods[];
        extern PyTypeObject type;
    }

    // "Global" things.
    // Definitely not my favourite way to go about this.
    // These are defined in ModuleFuncs.cpp, by the by.
    extern Engine*    engine;

    extern PyObject*   entityDict;
    extern PyObject*   playerEnt;
    extern PyObject*   cameraTarget;

    extern PyObject*   sysModule;                           // the system scripts (system.py)
    extern PyObject*   mapModule;                           // scripts for the currently loaded map

    extern std::stringstream  pyOutput;                     // Python's sys.stdout and sys.stderr go here (defined in ModuleFuncs.cpp)

    METHOD(std_log, PyObject);
    METHOD(std_exit, PyObject);
    METHOD1(std_getcaption, PyObject);
    METHOD(std_setcaption, PyObject);
    METHOD1(std_getframerate, PyObject);
    METHOD(std_delay, PyObject);
    METHOD(std_wait, PyObject);
    METHOD1(std_gettime, PyObject);
    METHOD(std_random, PyObject);

    METHOD(std_showpage, PyObject);
    METHOD(std_rgb, PyObject);
    METHOD(std_getrgb, PyObject);
    METHOD(std_palettemorph, PyObject);

    METHOD(std_processentities, PyObject);
    METHOD(std_setcameraTarget, PyObject);
    METHOD1(std_getcameraTarget, PyObject);
    METHOD(std_setplayer, PyObject);
    METHOD1(std_getplayer, PyObject);
    METHOD(std_entitiesat, PyObject);

    METHOD(std_hookretrace, PyObject);
    METHOD(std_unhookretrace, PyObject);
    METHOD(std_hooktimer, PyObject);
    METHOD(std_unhooktimer, PyObject);

    extern PyMethodDef standard_methods[];

#undef METHOD
#undef METHOD1
}


#endif
