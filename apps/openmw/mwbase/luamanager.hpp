#ifndef GAME_MWBASE_LUAMANAGER_H
#define GAME_MWBASE_LUAMANAGER_H

#include <SDL_events.h>

namespace MWWorld
{
    class Ptr;
}

namespace Loading
{
    class Listener;
}

namespace ESM
{
    class ESMReader;
    class ESMWriter;
    class LuaScripts;
}

namespace MWBase
{

    class LuaManager
    {
    public:
        virtual ~LuaManager() = default;

        virtual void newGameStarted() = 0;
        virtual void objectAddedToScene(const MWWorld::Ptr& ptr) = 0;
        virtual void objectRemovedFromScene(const MWWorld::Ptr& ptr) = 0;
        virtual void registerObject(const MWWorld::Ptr& ptr) = 0;
        virtual void deregisterObject(const MWWorld::Ptr& ptr) = 0;
        virtual void keyPressed(const SDL_KeyboardEvent &arg) = 0;

        struct ActorControls {
            bool disableAI;
            bool controlledFromLua;

            bool jump;
            bool run;
            float movement;
            float sideMovement;
            float turn;
        };

        virtual ActorControls* getActorControls(const MWWorld::Ptr&) const = 0;

        virtual void clear() = 0;
        virtual void setupPlayer(const MWWorld::Ptr&) = 0;

        // Saving
        int countSavedGameRecords() const { return 1; };
        virtual void write(ESM::ESMWriter& writer, Loading::Listener& progress) = 0;
        virtual void saveLocalScripts(const MWWorld::Ptr& ptr, ESM::LuaScripts& data) = 0;

        // Loading from a save
        virtual void readRecord(ESM::ESMReader& reader, uint32_t type) = 0;
        virtual void loadLocalScripts(const MWWorld::Ptr& ptr, const ESM::LuaScripts& data) = 0;

        // Should be called before loading. The map is used to fix refnums if the order of content files was changed.
        virtual void setContentFileMapping(const std::map<int, int>&) = 0;

        // Drops script cache and reloads all scripts. Calls `onSave` and `onLoad` for every script.
        virtual void reloadAllScripts() = 0;
    };

}

#endif  // GAME_MWBASE_LUAMANAGER_H
