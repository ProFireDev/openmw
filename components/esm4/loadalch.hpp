/*
  Copyright (C) 2016, 2018, 2020 cc9cii

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  cc9cii cc9c@iinet.net.au

  Much of the information on the data structures are based on the information
  from Tes4Mod:Mod_File_Format and Tes5Mod:File_Formats but also refined by
  trial & error.  See http://en.uesp.net/wiki for details.

*/
#ifndef ESM4_ALCH_H
#define ESM4_ALCH_H

#include <cstdint>
#include <string>

#include <components/esm/defs.hpp>
#include <components/esm/formid.hpp>

#include "effect.hpp" // FormId, ScriptEffect

namespace ESM4
{
    class Reader;
    class Writer;

    struct Potion
    {
#pragma pack(push, 1)
        struct Data
        {
            float weight;
        };

        struct EnchantedItem
        {
            std::int32_t value;
            std::uint32_t flags;
            ESM::FormId32 withdrawl;
            float chanceAddition;
            ESM::FormId32 sound;
        };
#pragma pack(pop)

        ESM::FormId mId; // from the header
        std::uint32_t mFlags; // from the header, see enum type RecordFlag for details

        std::string mEditorId;
        std::string mFullName;
        std::string mModel;
        std::string mIcon; // inventory
        std::string mMiniIcon; // inventory

        ESM::FormId mPickUpSound;
        ESM::FormId mDropSound;

        ESM::FormId mScriptId;
        ScriptEffect mEffect;

        float mBoundRadius;

        Data mData;
        EnchantedItem mItem;

        void load(ESM4::Reader& reader);
        // void save(ESM4::Writer& writer) const;

        // void blank();
        static constexpr ESM::RecNameInts sRecordId = ESM::RecNameInts::REC_ALCH4;
    };
}

#endif // ESM4_ALCH_H
