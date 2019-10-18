#ifndef GUARD_MANGO_TYPEDEFS_H
#define GUARD_MANGO_TYPEDEFS_H

#define NO_THREAD_ALLOC //disables thread_safety in my customallocator for better performance since this project is single threaded and locks are not needed (will be removed for multithreaded implementation)
#include <Gaming/pixelutility.h>

namespace engine
{
    typedef const mango::int32                      const_size;
    typedef mango::archive::thread_pool             thread_pool;
    typedef mango::shared_ptr<thread_pool>          pool_ptr;
    typedef mango::int32                            colour;
    typedef mango::int32                            counter;
    typedef mango::int64                            large_counter;
    typedef mango::delta_timer                      timer;
    typedef double                                  seconds;
    typedef graphic::KatCoord                       point;

    typedef graphic::Character                      player;
    typedef graphic::Character                      actor;
    typedef graphic::Character                      enemy;
    typedef graphic::Character                      effect;
    typedef graphic::Character                      background;

    typedef mango::shared_ptr<player>               player_handle;
    typedef mango::shared_ptr<actor>                actor_handle;
    typedef mango::shared_ptr<enemy>                enemy_handle;
    typedef mango::shared_ptr<effect>               effect_handle;
    typedef mango::shared_ptr<background>           background_handle;

    typedef std::string                             mesh_name;

    typedef mango::vector<actor_handle>             actor_pipeline;
    typedef mango::semimap<mesh_name, actor_handle> sprite_pipeline;
    typedef mango::semimap<effect_handle, counter>  effect_pipeline;

    typedef graphic::GraphicController              graphic_device;
    typedef mango::shared_ptr<graphic_device>       graphic_handle;

    typedef bool                                    flag;

    typedef mango::Byte                             key;
    typedef mango::queued_mutex                     lock;
    typedef mango::shared_ptr<lock>                 lock_handle;
    typedef mango::lock_guard<lock>                 lock_guard;
} 

#endif//GUARD_MANGO_TYPEDEFS_H
