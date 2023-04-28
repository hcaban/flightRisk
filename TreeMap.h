#pragma once

#include <utility>
#include <functional>
#include <stdexcept>

#define RED true
#define BLACK false

// Red-Black Tree
template <typename K, typename V>
class TreeMap
{
public:
    struct Node
    {
        Node* parent;
        Node* left;
        Node* right;

        bool color;

        K key;
        V value;

        Node(Node* _parent, std::pair<K,V> p) : key(p.first), value(p.second), color(RED), left(nullptr), right(nullptr), parent(_parent) {};
    };

private:
    int treeSize;

    Node* root;
    Node* last;

public:

    TreeMap();

    ~TreeMap();

    bool contains(K key);

    V search(K key);

    void insert(K key, V value);

    int size();

private:

    // Destructor Helper
    void destructorTraverse(Node* node);

    // Insertion helpers
    Node* insertElement(Node* node, std::pair<K,V>& pair);
    void adjustTree(Node* node);
    void colorFlip(Node* node);
    void rotate(Node* node);

    // Rotation Helpers
    void leftRotation(Node* node);
    void rightRotation(Node* node);
    void setRotateColor(Node* node);

    // Search Helpers
    Node* searchRecursive(Node* node, K key);
};


// CONSTUCTORS

template <typename K, typename V>
TreeMap<K,V>::TreeMap()
{
    treeSize = 0;

    root = nullptr;
    last = nullptr;
}

template <typename K, typename V>
TreeMap<K,V>::~TreeMap()
{
    destructorTraverse(root);
}

template <typename K, typename V>
void TreeMap<K,V>::destructorTraverse(Node* node)
{
    if (node == nullptr)
        return;

    destructorTraverse(node->left);
    destructorTraverse(node->right);
    delete node;
}

// SEARCH

template <typename K, typename V>
V TreeMap<K,V>::search(K key)
{
    if (last != nullptr && last->key == key)
        return last->value;

    Node* node = searchRecursive(root, key);

    if (node == nullptr)
        throw std::out_of_range("key does not exist");

    else return node->value;
}

template <typename K, typename V>
typename TreeMap<K,V>::Node* TreeMap<K,V>::searchRecursive(Node* node, K key)
{
    if (node == nullptr)
        return nullptr;

    if (key == node->key)
        return node;

    if (key < node->key)
        return searchRecursive(node->left, key);
    
    else
        return searchRecursive(node->right, key);
}

template <typename K, typename V>
bool TreeMap<K,V>::contains(K key)
{
    last = searchRecursive(root, key);

    if (last == nullptr)
        return false;

    else return true;
}


// INSERT

template <typename K, typename V>
void TreeMap<K,V>::insert(K key, V value)
{
    if (root == nullptr)
    {
        root = new Node(nullptr, std::make_pair(key, value));
        root->color = BLACK;
        treeSize++;
        return;
    }

    std::pair<K,V> pair(key, value);

    Node* node = insertElement(root, pair);

    if (node == nullptr)
        return;

    treeSize++;

    if (node->parent == root)
        return;

    if (node->parent->color == RED)
        adjustTree(node);
}

template <typename K, typename V>
typename TreeMap<K,V>::Node* TreeMap<K,V>::insertElement(Node* node, std::pair<K,V>& pair)
{

    if (pair.first == node->key)
    {
        node->value = pair.second;
        return nullptr;
    }

    else if (pair.first < node->key)
    {
        if (node->left == nullptr)
        {
            node->left = new Node(node, pair);
            return node->left;
        }
        
        else
            return insertElement(node->left, pair);
    }

    else
    {
        if (node->right == nullptr)
        {
            node->right = new Node(node, pair);
            return node->right;
        }
        
        else
            return insertElement(node->right, pair);
    }

}

// Assumes node has a valid grandparent
template <typename K, typename V>
void TreeMap<K,V>::adjustTree(Node* node)
{
    Node* uncle = node->parent->parent->left == node->parent ? node->parent->parent->right : node->parent->parent->left;

    if (uncle == nullptr || !uncle->color)
        rotate(node);
    else
        colorFlip(node);
}

template <typename K, typename V>
void TreeMap<K,V>::colorFlip(Node* node)
{
    Node* parent = node->parent;
    Node* grandparent = parent->parent;
    Node* uncle = grandparent->left == parent ? grandparent->right : grandparent->left;

    parent->color = BLACK;
    uncle->color = BLACK;
    grandparent->color = RED;

    if (grandparent == root)
    {
        grandparent->color = BLACK;
        return;
    }

    // No need to check for root since root will always be black thus a red node always has a parent
    if (grandparent->parent->color == RED)
    {
        adjustTree(grandparent);
        return;
    }
}

template <typename K, typename V>
void TreeMap<K,V>::rotate(Node* node)
{

    if (node->parent == node->parent->parent->right)
    {
        if (node == node->parent->left)
        {
            rightRotation(node);
            leftRotation(node);
            setRotateColor(node);
        }
        
        else
        {
            leftRotation(node->parent);
            setRotateColor(node->parent);
        }
    }

    else
    {
        if (node == node->parent->right)
        {
            leftRotation(node);
            rightRotation(node);
            setRotateColor(node);
        }

        else
        {
            rightRotation(node->parent);
            setRotateColor(node->parent);
        }
    }
}

template <typename K, typename V>
void TreeMap<K,V>::setRotateColor(Node* node)
{
    node->color = BLACK;

    if (node->left != nullptr)
        node->left->color = RED;

    if (node->right != nullptr)
        node->right->color = RED;  
}

template <typename K, typename V>
void TreeMap<K,V>::leftRotation(Node* node)
{
    Node* parent = node->parent;

    if (root == parent)
        root = node;
        
    else if (parent->parent->right == parent)
        parent->parent->right = node;
    
    else
        parent->parent->left = node;

    node->parent = parent->parent;
    parent->parent = node;
    parent->right = node->left;
    node->left = parent;
}

template <typename K, typename V>
void TreeMap<K,V>::rightRotation(Node* node)
{
    Node* parent = node->parent;

    if (root == parent)
        root = node;

    else if (parent->parent->right == parent)
        parent->parent->right = node;
    
    else
        parent->parent->left = node;

    node->parent = parent->parent;
    parent->parent = node;
    parent->left = node->right;
    node->right = parent;
}

template <typename K, typename V>
int TreeMap<K,V>::size()
{
    return treeSize;
}
