/*================================================================
Filename: LinkedHashMap.h
Date: 2018.1.29
Created by AirGuanZ
================================================================*/
#ifndef VW_LINKED_HASH_MAP_H
#define VW_LINKED_HASH_MAP_H

#include <list>
#include <unordered_map>

template<typename KeyType, typename ValueType, typename KeyHasherType = std::hash<KeyType>>
class LinkedHashMap
{
public:
    using Key = KeyType;
    using Value = ValueType;
    using Hasher = KeyHasherType;

    bool Empty(void) const noexcept
    {
        return list_.empty();
    }

    size_t Size(void) const noexcept
    {
        return list_.size();
    }

    bool FindAndErase(const Key &key, ValueType &output)
    {
        auto it = map_.find(key);
        if(it == map_.end())
            return false;

        output = it->second.listItor->value;
        list_.erase(it->second.listItor);
        map_.erase(it);
        return true;
    }

    ValueType &Back(void)
    {
        return list_.back().value;
    }

    void PopBack(void)
    {
        auto it = list_.back().hashItor;
        list_.erase(it->second.listItor);
        map_.erase(it);
    }

    ValueType &Front(void)
    {
        return list_.front().value;
    }

    void PushFront(const Key &key, Value &value)
    {
        ListNode lstNode;
        lstNode.value = value;
        list_.push_front(lstNode);
        auto itRt = map_.insert(std::make_pair(key, HashNode{ list_.begin() }));
        list_.front().hashItor = itRt.first;
    }

    template<typename FuncType>
    void ForEach(FuncType &&func)
    {
        for(ListNode &node : list_)
            func(node.value);
    }

    void Clear(void)
    {
        list_.clear();
        map_.clear();
    }

    bool Exists(const Key &key)
    {
        return map_.find(key) != map_.end();
    }

private:
    struct HashNode;

    struct ListNode
    {
        typename std::unordered_map<Key, HashNode, Hasher>::iterator hashItor;
        Value value;
    };

    struct HashNode
    {
        typename std::list<ListNode>::iterator listItor;
    };

    std::list<ListNode> list_;
    std::unordered_map<Key, HashNode, Hasher> map_;
};

#endif //VW_LINKED_HASH_MAP_H
