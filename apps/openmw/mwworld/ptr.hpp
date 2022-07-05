#ifndef GAME_MWWORLD_PTR_H
#define GAME_MWWORLD_PTR_H

#include <cassert>
#include <type_traits>
#include <string>
#include <string_view>

#include "livecellref.hpp"

namespace MWWorld
{
    class ContainerStore;
    class CellStore;
    struct LiveCellRefBase;

    /// \brief Pointer to a LiveCellRef
    /// @note PtrBase is never used directly and needed only to define Ptr and ConstPtr
    template <template<class> class TypeTransform>
    class PtrBase
    {
        public:

            typedef TypeTransform<MWWorld::LiveCellRefBase> LiveCellRefBaseType;
            typedef TypeTransform<CellStore> CellStoreType;
            typedef TypeTransform<ContainerStore> ContainerStoreType;

            LiveCellRefBaseType *mRef;
            CellStoreType *mCell;
            ContainerStoreType *mContainerStore;

            bool isEmpty() const
            {
                return mRef == nullptr;
            }

            // Returns a 32-bit id of the ESM record this object is based on.
            // Specific values of ids are defined in ESM::RecNameInts.
            // Note 1: ids are not sequential. E.g. for a creature `getType` returns 0x41455243.
            // Note 2: Life is not easy and full of surprises. For example
            //         prison marker reuses ESM::Door record. Player is ESM::NPC.
            unsigned int getType() const
            {
                if(mRef != nullptr)
                    return mRef->getType();
                throw std::runtime_error("Can't get type name from an empty object.");
            }

            // `getType()` is not exactly what we usually mean by "type" because some refids have special meaning.
            // This function handles these special refids (and by this adds some performance overhead).
            // We use this "fixed" type in Lua because we don't want to expose the weirdness of Morrowind internals to our API.
            // TODO: Implement https://gitlab.com/OpenMW/openmw/-/issues/6617 and make `getType` work the same as `getLuaType`.
            unsigned int getLuaType() const
            {
                if(mRef == nullptr)
                    throw std::runtime_error("Can't get type name from an empty object.");
                std::string_view id = mRef->mRef.getRefId();
                if (id == "player")
                    return ESM::REC_INTERNAL_PLAYER;
                else if (id == "prisonmarker" || id == "divinemarker" || id == "templemarker" || id == "northmarker")
                    return ESM::REC_INTERNAL_MARKER;
                else
                    return mRef->getType();
            }

            std::string_view getTypeDescription() const
            {
                return mRef ? mRef->getTypeDescription() : "nullptr";
            }

            const Class& getClass() const
            {
                if(mRef != nullptr)
                    return *(mRef->mClass);
                throw std::runtime_error("Cannot get class of an empty object");
            }

            template <class T>
            auto* get() const { return LiveCellRefBase::dynamicCast<T>(mRef); }

            LiveCellRefBaseType *getBase() const
            {
                if (!mRef)
                    throw std::runtime_error ("Can't access cell ref pointed to by null Ptr");
                return mRef;
            }

            TypeTransform<MWWorld::CellRef>& getCellRef() const
            {
                assert(mRef);
                return mRef->mRef;
            }

            TypeTransform<RefData>& getRefData() const
            {
                assert(mRef);
                return mRef->mData;
            }

            CellStoreType *getCell() const
            {
                assert(mCell);
                return mCell;
            }

            bool isInCell() const
            {
                return (mContainerStore == nullptr) && (mCell != nullptr);
            }

            void setContainerStore (ContainerStoreType *store)
            ///< Must not be called on references that are in a cell.
            {
                assert (store);
                assert (!mCell);
                mContainerStore = store;
            }

            ContainerStoreType *getContainerStore() const
            ///< May return a 0-pointer, if reference is not in a container.
            {
                return mContainerStore;
            }

            operator const void *() const
            ///< Return a 0-pointer, if Ptr is empty; return a non-0-pointer, if Ptr is not empty
            {
                return mRef;
            }

        protected:
            PtrBase(LiveCellRefBaseType *liveCellRef, CellStoreType *cell, ContainerStoreType* containerStore) : mRef(liveCellRef), mCell(cell), mContainerStore(containerStore) {}
    };

    /// @note It is possible to get mutable values from const Ptr. So if a function accepts const Ptr&, the object is still mutable.
    /// To make it really const the argument should be const ConstPtr&.
    class Ptr : public PtrBase<std::remove_const_t>
    {
    public:
        Ptr(LiveCellRefBase *liveCellRef=nullptr, CellStoreType *cell=nullptr) : PtrBase(liveCellRef, cell, nullptr) {}
    };

    /// @note The difference between Ptr and ConstPtr is that the second one adds const to the underlying pointers.
    /// @note a Ptr can be implicitely converted to a ConstPtr, but you can not convert a ConstPtr to a Ptr.
    class ConstPtr : public PtrBase<std::add_const_t>
    {
    public:
        ConstPtr(const Ptr& ptr) : PtrBase(ptr.mRef, ptr.mCell, ptr.mContainerStore) {}
        ConstPtr(const LiveCellRefBase *liveCellRef=nullptr, const CellStoreType *cell=nullptr) : PtrBase(liveCellRef, cell, nullptr) {}
    };

}

#endif
