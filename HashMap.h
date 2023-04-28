#pragma once

#include <vector>
#include <functional>
#include <stdexcept>

template <typename K, typename V>
class HashMap
{
public:
    struct Chain
    {
        K key;
        V value;
        Chain* next;

        Chain(K k, V val) : key(k), value(val), next(nullptr) {};
    };

private:
    size_t bucketCount;
    size_t tableSize;
    double maxLoadFactor;
    double scaleFactor;

    std::hash<K> hash;
    std::vector<Chain*> table;

    Chain* last;

public:

    HashMap();

    HashMap(int _bucketCount, double _maxLoadFactor, double _scaleFactor);

    ~HashMap();

    V search(K key);

    bool contains(K key);

    bool insert(K key, V value);

    int size();

private:

    size_t hashReduce(K key);

    void rehash();

    void deleteTable(std::vector<Chain*>& oldTable);
};


template <typename K, typename V>
HashMap<K,V>::HashMap()
{
    bucketCount = 10;
    tableSize = 0;
    maxLoadFactor = 0.75;
    scaleFactor = 2;

    table = std::vector<Chain*>(bucketCount, nullptr);

    last = nullptr;
}

template <typename K, typename V>
HashMap<K,V>::HashMap(int _bucketCount, double _maxLoadFactor, double _scaleFactor)
{
    bucketCount = _bucketCount;
    tableSize = 0;
    maxLoadFactor = _maxLoadFactor;
    scaleFactor = _scaleFactor;

    table = std::vector<Chain*>(bucketCount, nullptr);

    last = nullptr;
}

template <typename K, typename V>
HashMap<K,V>::~HashMap()
{
    deleteTable(table);
}

template <typename K, typename V>
V HashMap<K,V>::search(K key)
{
    size_t index = hashReduce(key);

    Chain* chain = table[index];

    if (last != nullptr && last->key == key)
        return last->value;

    if (chain == nullptr)
        throw std::out_of_range("Key does not exist");
    
    if (chain->key == key)
        return chain->value;

    while (chain->next != nullptr)
    {
        chain = chain->next;
        
        if (chain->key == key)
        {
            return chain->value;
        }
    }
    
    throw std::out_of_range("Key does not exist");
}

template <typename K, typename V>
bool HashMap<K,V>::contains(K key)
{
    size_t index = hashReduce(key);

    Chain* chain = table[index];

    if (chain == nullptr)
        return false;

    if (chain->key == key)
        return true;

    while (chain->next != nullptr)
    {
        chain = chain->next;
        
        if (chain->key == key)
        {
            last = chain;
            return true;
        }
    }

    return false;
}

template <typename K, typename V>
bool HashMap<K,V>::insert(K key, V value)
{
    size_t index = hashReduce(key);

    Chain* chain = table[index];

    if (chain == nullptr)
    {
        table[index] = new Chain(key, value);
        return false;
    }

    while (chain->next != nullptr)
    {
        chain = chain->next;
        
        if (chain->key == key)
        {
            chain->value = value;
            return false;
        }
    }

    chain->next = new Chain(key, value);
    tableSize++;

    if ((double)tableSize / (double)bucketCount >= maxLoadFactor)
        rehash();

    return true;
}

template <typename K, typename V>
int HashMap<K,V>::size()
{
    return tableSize;
}

template <typename K, typename V>
size_t HashMap<K,V>::hashReduce(K key)
{
    return hash(key) % bucketCount;
}

template <typename K, typename V>
void HashMap<K,V>::rehash()
{
    bucketCount = bucketCount * scaleFactor;
    tableSize = 0;

    std::vector<Chain*> oldTable = table;

    table = std::vector<Chain*>(bucketCount, nullptr);

    for (Chain* bucket : oldTable)
    {
        Chain* chain = bucket;

        if (chain == nullptr)
            continue;
        else
            insert(chain->key, chain->value);

        while (chain->next != nullptr)
        {
            chain = chain->next;
            insert(chain->key, chain->value);
        }
    }

    deleteTable(oldTable);
}

template <typename K, typename V>
void HashMap<K,V>::deleteTable(std::vector<Chain*>& oldTable)
{
    for (Chain* bucket : oldTable)
    {
        Chain* chain = bucket;

        if (chain == nullptr)
            continue;

        while (chain->next != nullptr)
        {
            Chain* prev = chain;
            chain = chain->next;
            delete prev;
        }

        delete chain;
    }
}
