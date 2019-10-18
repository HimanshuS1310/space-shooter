#ifndef GUARD_MANGO_INPUT_H
#define GUARD_MANGO_INPUT_H

#include "typedefs.h"

namespace input
{
    static const engine::key W       = 'w';
    static const engine::key A       = 'a';
    static const engine::key S       = 's';
    static const engine::key D       = 'd';

    static const engine::key SPACE   = ' ';

    static const engine::key SHIFTX  = 'X';
    static const engine::key DEFAULT = '~';

    namespace actions
    {
        enum action
        {
            up,
            down,
            left,
            right,
            shoot,
            invalid
        };
    }

    typedef typename actions::action action;

    class InputController
    {
    public: //Typedefs
        typedef input::actions::action                     player_action;
        typedef mango::semimap<engine::key, player_action> input_map;

    public: //Functions
        InputController(engine::key up, engine::key down, engine::key left, engine::key right, engine::key shoot)
        {
            m_KeyMapping.add(up,    actions::up);
            m_KeyMapping.add(down,  actions::down);
            m_KeyMapping.add(left,  actions::left);
            m_KeyMapping.add(right, actions::right);
            m_KeyMapping.add(shoot, actions::shoot);
        }

        player_action KeyPressed(engine::key in)
        {
            return (mango::npos != m_KeyMapping.find(in)) ? m_KeyMapping.at(in) : actions::invalid;
        }

    private: //Members
        input_map m_KeyMapping;
    };

    engine::key getKeyPress()
    {
        return mango::archive::mgetch();
    }
}

namespace engine
{
    typedef input::InputController                              input_device;
    typedef mango::shared_ptr<input_device>                     input_handle;
}


#endif//GUARD_MANGO_INPUT_H
