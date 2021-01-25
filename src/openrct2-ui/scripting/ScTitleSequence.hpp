/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include <memory>
#    include <openrct2/scripting/ScriptEngine.h>
#    include <openrct2/title/TitleSequence.h>
#    include <openrct2/title/TitleSequenceManager.h>

namespace OpenRCT2::Scripting
{
    class ScTitleSequencePark
    {
    private:
        std::string _titleSequencePath;
        std::string _fileName;

    public:
        ScTitleSequencePark(std::string_view path, std::string_view fileName)
            : _titleSequencePath(path)
            , _fileName(fileName)
        {
        }

    private:
        std::string fileName_get() const
        {
            return _fileName;
        }

        void fileName_set(const std::string& value)
        {
            if (value == _fileName)
                return;

            auto seq = LoadTitleSequence(_titleSequencePath);
            if (seq != nullptr)
            {
                // Check if name already in use
                auto index = GetIndex(*seq, value);
                if (!index)
                {
                    index = GetIndex(*seq, _fileName);
                    if (index)
                    {
                        TitleSequenceRenamePark(*seq, *index, value.c_str());
                        TitleSequenceSave(*seq);
                    }
                }
            }
        }

        void delete_()
        {
            auto seq = LoadTitleSequence(_titleSequencePath);
            if (seq != nullptr)
            {
                auto index = GetIndex(*seq, _fileName);
                if (index)
                {
                    TitleSequenceRemovePark(*seq, *index);
                    TitleSequenceSave(*seq);
                }
            }
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScTitleSequencePark::fileName_get, &ScTitleSequencePark::fileName_set, "fileName");
            dukglue_register_method(ctx, &ScTitleSequencePark::delete_, "delete");
        }

    private:
        static std::optional<size_t> GetIndex(const TitleSequence& seq, const std::string_view needle)
        {
            for (size_t i = 0; i < seq.Saves.size(); i++)
            {
                if (seq.Saves[i] == needle)
                {
                    return i;
                }
            }
            return {};
        }
    };

    class ScTitleSequence
    {
    private:
        std::string _path;

    public:
        ScTitleSequence(const std::string& path)
        {
            _path = path;
        }

    private:
        std::string name_get() const
        {
            const auto* item = GetItem();
            if (item != nullptr)
            {
                return item->Name;
            }
            return {};
        }

        void name_set(const std::string& value)
        {
            auto index = GetManagerIndex();
            if (index)
            {
                auto newIndex = TitleSequenceManager::RenameItem(*index, value.c_str());

                // Update path to new value
                auto newItem = TitleSequenceManager::GetItem(newIndex);
                _path = newItem != nullptr ? newItem->Path : std::string();
            }
        }

        std::string path_get() const
        {
            const auto* item = GetItem();
            if (item != nullptr)
            {
                return item->Path;
            }
            return {};
        }

        bool isDirectory_get() const
        {
            const auto* item = GetItem();
            if (item != nullptr)
            {
                return !item->IsZip;
            }
            return {};
        }

        bool isReadOnly_get() const
        {
            const auto* item = GetItem();
            if (item != nullptr)
            {
                return item->PredefinedIndex != std::numeric_limits<size_t>::max();
            }
            return {};
        }

        std::vector<std::shared_ptr<ScTitleSequencePark>> parks_get() const
        {
            std::vector<std::shared_ptr<ScTitleSequencePark>> result;
            auto titleSeq = LoadTitleSequence(_path);
            if (titleSeq != nullptr)
            {
                for (size_t i = 0; i < titleSeq->Saves.size(); i++)
                {
                    result.push_back(std::make_shared<ScTitleSequencePark>(_path, titleSeq->Saves[i]));
                }
            }
            return result;
        }

        std::shared_ptr<ScTitleSequence> clone(const std::string& name) const
        {
            auto copyIndex = GetManagerIndex();
            if (copyIndex)
            {
                auto index = TitleSequenceManager::DuplicateItem(*copyIndex, name.c_str());
                auto* item = TitleSequenceManager::GetItem(index);
                if (item != nullptr)
                {
                    return std::make_shared<ScTitleSequence>(item->Path);
                }
            }
            return nullptr;
        }

        void delete_()
        {
            auto index = GetManagerIndex();
            if (index)
            {
                TitleSequenceManager::DeleteItem(*index);
            }
            _path = {};
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScTitleSequence::name_get, &ScTitleSequence::name_set, "name");
            dukglue_register_property(ctx, &ScTitleSequence::path_get, nullptr, "path");
            dukglue_register_property(ctx, &ScTitleSequence::isDirectory_get, nullptr, "isDirectory");
            dukglue_register_property(ctx, &ScTitleSequence::isReadOnly_get, nullptr, "isReadOnly");
            dukglue_register_property(ctx, &ScTitleSequence::parks_get, nullptr, "parks");
            dukglue_register_method(ctx, &ScTitleSequence::clone, "clone");
            dukglue_register_method(ctx, &ScTitleSequence::delete_, "delete");
        }

    private:
        std::optional<size_t> GetManagerIndex() const
        {
            auto count = TitleSequenceManager::GetCount();
            for (size_t i = 0; i < count; i++)
            {
                auto item = TitleSequenceManager::GetItem(i);
                if (item != nullptr && item->Path == _path)
                {
                    return i;
                }
            }
            return {};
        }

        const TitleSequenceManagerItem* GetItem() const
        {
            auto index = GetManagerIndex();
            if (index)
            {
                return TitleSequenceManager::GetItem(*index);
            }
            return nullptr;
        }
    };

    class ScTitleSequenceManager
    {
    private:
        std::vector<std::shared_ptr<ScTitleSequence>> titleSequences_get() const
        {
            std::vector<std::shared_ptr<ScTitleSequence>> result;
            auto count = TitleSequenceManager::GetCount();
            for (size_t i = 0; i < count; i++)
            {
                const auto& path = TitleSequenceManager::GetItem(i)->Path;
                result.push_back(std::make_shared<ScTitleSequence>(path));
            }
            return result;
        }

        std::shared_ptr<ScTitleSequence> create(const std::string& name)
        {
            auto index = TitleSequenceManager::CreateItem(name.c_str());
            auto* item = TitleSequenceManager::GetItem(index);
            if (item != nullptr)
            {
                return std::make_shared<ScTitleSequence>(item->Path);
            }
            return nullptr;
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScTitleSequenceManager::titleSequences_get, nullptr, "titleSequences");
            dukglue_register_method(ctx, &ScTitleSequenceManager::create, "create");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
