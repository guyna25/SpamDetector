//
// Created by guyna25 on 16/01/2020.
//
#include <vector>
#include <stdexcept>

#ifndef EX3_HASHMAP_HPP
#define EX3_HASHMAP_HPP

#define KEYS_AND_VALUES_SIZE_DIFF "Number of keys must match number of value"
#define NONEXISTANT_KEY_ERR "This key does not exist in the map"

const int START_CAPACITY = 16;
const int RESIZE_FACTOR = 2;
const int ELEMENT_NUMBER = 0;
const double DEFAULT_UPPER_LOAD_FACTOR = 0.75;
const double DEFAULT_LOWER_LOAD_FACTOR = 0.25;
const int ERROR_CODE = -1;

/**
 * @brief This class represents a generic hash map
 * @tparam KeyT the key of a pair in the map
 * @tparam ValueT the value of the said pair
 */
template <typename  KeyT, typename ValueT>
class HashMap
{
private:
    int _size = ELEMENT_NUMBER;
    int _capacity = START_CAPACITY;
    double _upperLoadFactor;
    double _lowerLoadFactor;
    std::vector<std::pair<KeyT, ValueT>> *_buckets;

    /**
     * @brief getter method for key hash code
     * @param key the key to use
     * @return hash code for the key
     */
    int _getHashCode(const KeyT &key) const
    {
        return std::hash<KeyT>{} (key) & (capacity() - 1);
    }

    /**
     * @brief this method checks if the table should be resized, and if so, resizes it
     */
    void _checkResize()
    {
        if ((double) size() / capacity() > _upperLoadFactor)
        {
            this->_resize(true);
        }
        if ((double) size() / capacity() <  _lowerLoadFactor)
        {
            this->_resize(false);
        }
    }

    /**
     * @brief this method resizes the table so that it matches the load factors range
     * @param upsize true if should be enlarged, false otherwise
     */
    void _resize(bool upsize)
    {
        int previousCapacity = capacity();
        if (upsize)
        {
            while((double) size() / capacity() > _upperLoadFactor)
            {
                this->_capacity *= RESIZE_FACTOR;
            }
        }
        else
        {
            while((double) size() / capacity() < _lowerLoadFactor)
            {
                this->_capacity /= RESIZE_FACTOR;
            }
        }
        try
        {
            this->_rehash(previousCapacity);
        }
        catch (std::bad_alloc &ex)
        {
            throw ex;
        }
    }

    /**
     * @brief rehash method that rehashes the table values
     * @param previousCapacity the previous capacity of the table
     */
    void _rehash(int previousCapacity)
    {
        try
        {
            auto *newTable = new std::vector<std::pair<KeyT, ValueT>>[_capacity];
            for (int bucketIdx = 0; bucketIdx < previousCapacity; bucketIdx++)
            {
                int currHashCode;
                for (auto pair : this->_buckets[bucketIdx])
                {
                    currHashCode = _getHashCode(pair.first);
                    newTable[currHashCode].push_back(pair);
                }
            }
            delete[] _buckets;
            this->_buckets = newTable;
        }
        catch (std::bad_alloc &ex)
        {
            throw ex;
        }
    }

    /**
     * @brief getter method for key index in a bucket
     * @param key the key to get the index for
     * @return the index of the key in the relevant bucket
     */
    int _getInnerKeyIdx(const KeyT key) const
    {
        std::vector<std::pair<KeyT, ValueT>> relevantVec = _buckets[_getHashCode(key)];
        for (size_t vecIdx = 0; vecIdx < relevantVec.size(); vecIdx++)
        {
            if (relevantVec[vecIdx].first == key)
            {
                return vecIdx;
            }
        }
        return ERROR_CODE;
    }

public:

    /**
     * @brief default constructor for this class
     */
    HashMap()
    {
        _upperLoadFactor = DEFAULT_UPPER_LOAD_FACTOR;
        _lowerLoadFactor = DEFAULT_LOWER_LOAD_FACTOR;
        try
        {
            this->_buckets = new std::vector<std::pair<KeyT, ValueT>>[_capacity];
        }
        catch (std::bad_alloc &error)
        {
            throw error; //move up call stack
        }
    }

    /**
     * @brief Constructor for this class that accepts a group of pairs. pair's index is assumed
     * to be matching and amount of keys should be equal to values
     * @param keys the keys for the group
     * @param values the values of the pairs
     */
    HashMap(std::vector<KeyT> keys, std::vector<ValueT> values) : HashMap()
    {
        _upperLoadFactor = DEFAULT_UPPER_LOAD_FACTOR;
        _lowerLoadFactor = DEFAULT_LOWER_LOAD_FACTOR;
        try
        {
            if (keys.size() != values.size())
            {
                throw std::invalid_argument(KEYS_AND_VALUES_SIZE_DIFF);
            }
            for (size_t elementIdx = 0; elementIdx < keys.size(); elementIdx++)
            {
                if (containsKey(keys[elementIdx]))
                {
                    this->erase(keys[elementIdx]);
                }
                insert(keys[elementIdx], values[elementIdx]);
            }

        }
        catch (const std::invalid_argument &ex)
        {
            throw ex; //propagate up the call stack

        }
        catch (std::bad_alloc &error)
        {
            throw error; //move up call stack
        }
    }

    /**
     * @brief copy constructor for this class
     */
    HashMap(const HashMap &other)
    {
        _size = other._size;
        _capacity = other._capacity;
        _lowerLoadFactor = other._lowerLoadFactor;
        _upperLoadFactor = other._upperLoadFactor;
        try
        {
            this->_buckets = new std::vector<std::pair<KeyT, ValueT>>[_capacity];
            for (int bucketIdx = 0; bucketIdx < capacity(); bucketIdx++)
            {
                int currHashCode;
                for (auto pair : other._buckets[bucketIdx])
                {
                    currHashCode = _getHashCode(pair.first);
                    _buckets[currHashCode].push_back(pair);
                }
            }
        }
        catch (std::bad_alloc &error)
        {
            throw error; //propagate up call stack
        }
    };

    /**
     * @brief c'tor for this class
     */
    ~HashMap()
    {
        delete[] _buckets;
    }

    /**
     * @return number of elements in this table
     */
    int size() const
    {
        return _size;
    };

    /**
     * @brief the capacity of this table
     */
    int capacity() const
    {
        return _capacity;
    };

    /**
     * @return true if table is empty, false otherwise
     */
    bool empty() const
    {
        return size() == 0;
    };

    /**
    * @return true if table is empty, false otherwise
    */
    bool empty()
    {
        return size() == 0;
    };

    /**
     * @brief method to isnert a pair into this class
     * @param key the key of the pair
     * @param value the value of the pair
     */
    bool insert(const KeyT &key, const ValueT &value)
    {
        if (containsKey(key))
        {
            return false;
        }
        int valHashCode = this->_getHashCode(key);
        _size++;
        this->_buckets[valHashCode].push_back(std::pair<KeyT, ValueT> (key, value));
        this->_checkResize();
        return true;
    };

    /**
     * @brief method to check if a key is contained in the table
     * @param key the key to check if it's contained
     * @return true if it's contained, false otherwise
     */
    bool containsKey(const KeyT &key) const
    {
        return this->_getInnerKeyIdx(key) != ERROR_CODE;
    }

    /**
     * @brief get the matching value for a key
     * @param key the key of the value
     * @return the value matching to the key
     */
    ValueT at(KeyT key) const
    {
        if (containsKey(key))
        {
            return this->_buckets[_getHashCode(key)][_getInnerKeyIdx(key)].second;
            //we take the value of the pair in relevant key outer and inner index
        }
        else
        {
            throw std::invalid_argument(NONEXISTANT_KEY_ERR);
        }
    };

    /**
     * @brief get the matching value for a key
     * @param key the key of the value
     * @return the value matching to the key
     */
    ValueT& at(KeyT key)
    {
        if (containsKey(key))
        {
            return this->_buckets[_getHashCode(key)][_getInnerKeyIdx(key)].second;
            //we take the value of the pair in relevant key outer and inner index
        }
        else
        {
            throw std::invalid_argument(NONEXISTANT_KEY_ERR);
        }
    };

    /**
     * @brief erase a key and value from the table
     * @param key key and value to be erased
     * @return true if key was succesfully deleted, false otherwise
     */
    bool erase(KeyT key)
    {
        try
        {
            if (containsKey(key))
            {
                std::vector<std::pair<KeyT, ValueT>> &keyVec = _buckets[bucketIndex(key)];
                keyVec.erase(keyVec.begin() + this->_getInnerKeyIdx(key));
                _size--;
                _checkResize();
                return true;
            }
        }
        catch (std::invalid_argument &ex) //notice this error can only happen if the key isn't in
            // the table
        {
            return false;
        }
        return false;
    };

    /**
     * @return upper load factor of this table
     */
    double getLoadFactor() const
    {
        return (double) size() / (double) capacity();
    };

    /**
     * @param key that contains in the matching bucket
     * @return the size of the bucket
     */
    int bucketSize(KeyT key)
    {
        if (containsKey(key))
        {
            return this->_buckets[_getHashCode(key)].size();
        }
        else
        {
            throw std::invalid_argument(NONEXISTANT_KEY_ERR);
        }
    };

    /**
    * @param key that contains in the matching bucket
    * @return the size of the bucket
    */
    int bucketSize(KeyT key) const
    {
        if (containsKey(key))
        {
            return this->_buckets[_getHashCode(key)].size();
        }
        else
        {
            throw std::invalid_argument(NONEXISTANT_KEY_ERR);
        }
    };

    /**
     * @param key key to check the index for
     * @return the index of the bucket
     */
    int bucketIndex(const KeyT &key) const
    {
        if (containsKey(key))
        {
            return _getHashCode(key);
        }
        else
        {
            throw std::invalid_argument(NONEXISTANT_KEY_ERR);
        }
    };

    /**
     * @brief clears the table of all pairs
     */
    void clear()
    {
        for (int bucketIdx = 0; bucketIdx < capacity(); bucketIdx++)
        {
            _buckets[bucketIdx].clear();
        }
        _size = 0;
    };

    /**
     * @brief this class represents an iterator over this map
     */
    class hashMapIterator
    {

    public:
        typedef hashMapIterator self_type;
        typedef std::pair<KeyT, ValueT> value_type;
        typedef std::pair<KeyT, ValueT> &reference;
        typedef std::pair<KeyT, ValueT> *pointer;
        typedef int difference_type;
        typedef std::forward_iterator_tag iterator_category;

        hashMapIterator(std::vector<std::pair<KeyT, ValueT>> *bucketPtr, int mapSize, int bucketIdx
                        = 0)
        {
            _capacity = mapSize;
            _bucketIdx = bucketIdx;
            _bucketsPtr = bucketPtr;
            currBucketIt = (*_bucketsPtr).begin();
            if (bucketIdx == mapSize - 1)
            {
                currBucketIt = (bucketPtr[mapSize - 1]).end();
            }
            _updateCurrBucket();
        }

        //oprators overload
        /**
         * @brief overload for equality operator
         * @param rhs the other iterator to compare to
         * @return true if map iterators are equal, false otherwise
         */
        bool operator==(const self_type &rhs) const
        { return currBucketIt == rhs.currBucketIt; }

        /**
         * @brief overload for uneqaulity operator
         * @param other the other iterator to compare to
         * @return true if iterators are unequal, false otherwise
         */
        bool operator!=(const self_type &rhs)
        { return currBucketIt != rhs.currBucketIt; }

        /**
         * @brief dereference oparator for this class
         * @return the relevant key pair value
         */
        reference operator*()
        { return *currBucketIt; }

        /**
         * @brief increment oparator for this class for ++<index name>
         * @return iterator after increment
         */
        self_type &operator++()
        {
            if (_bucketIdx == _capacity)
            {
                currBucketIt = _bucketsPtr[_capacity - 1].end();

            }
            if (currBucketIt != _bucketsPtr[_bucketIdx].end())
            {
                ++currBucketIt;
            }
            _updateCurrBucket();
            return *this;
        }

        /**
        * @brief increment oparator for this class for <index name>++
        * @return iterator after increment
        */
        const self_type operator++(int junk)
        {
            auto tempBucketIt = *this;
            ++(*this);

            return tempBucketIt;
        }

        pointer operator->()
        {
            return &(*currBucketIt);
        }

        /**
         * @brief overload for assigment operator
         * @return the new iterator
         */

        hashMapIterator &operator = (hashMapIterator const &other)
        {
            _bucketsPtr = other._bucketsPtr;
            _capacity = other._capacity;
            _bucketIdx = other._bucketIdx;
            currBucketIt(other.currBucketIt);
        }

    private:
        std::vector<std::pair<KeyT, ValueT>> *_bucketsPtr;
        int _capacity;
        int _bucketIdx;
        typename std::vector<std::pair<KeyT, ValueT>>::iterator currBucketIt;
        void _updateCurrBucket()
        {
            while (_bucketIdx < _capacity - 1 && currBucketIt == _bucketsPtr[_bucketIdx].end())
            {
                _bucketIdx++;
                currBucketIt = (_bucketsPtr)[_bucketIdx].begin();
            }
        }
    };

    /**
     * @brief creates a new iterator for this class
     * @return an iterator ove this map
     */
    hashMapIterator begin() const
    {
        return hashMapIterator(this->_buckets, capacity());
    };

    /**
     * @return the end of the iterator
     */
    hashMapIterator end() const
    {
        return hashMapIterator(this->_buckets, capacity(), capacity() - 1);
    };

    /**
    * @brief creates a new constant iterator for this class
    * @return an iterator ove this map
    */
    hashMapIterator cbegin() const
    {
        return begin();
    };

    /**
    * @return the end of the const iterator
    */
    hashMapIterator cend() const
    {
        return end();
    };

    //operators overload

    /**
    * @brief overload for equality operator
    * @param other the other map to compare to
    * @return true if maps are equal, false otherwise
    */
    bool operator == (const HashMap &other) const
    {
        if(other.size() == _size)
        {
            for (int tempBucketIdx = 0; tempBucketIdx < _capacity; tempBucketIdx++)
            {
                for (auto element:_buckets[tempBucketIdx])
                {
                    if (!other.containsKey(element.first))
                    {
                        return false;
                    }
                }
            }
            return true;
        }
        return false;
    };

    /**
    * @brief overload for equality operator
    * @param other the other map to compare to
    * @return true if maps are equal, false otherwise
    */
    bool operator == (const HashMap &other)
    {
        if (this->size() == other.size())
        {
            for (auto &element: (*this))
            {
                if (!other.containsKey(element.first))
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    };

    /**
    * @brief overload for unequality operator
    * @param other the other map to compare to
    * @return true if maps are unequal, false otherwise
    */
    bool operator != (const HashMap &other) const
    {
        return !(*this == other);
    };

    /**
    * @brief overload for unequality operator
    * @param other the other map to compare to
    * @return true if maps are unequal, false otherwise
    */
    bool operator != (const HashMap &other)
    {
        return !(*this == other);
    };

    /**
     * @brief overload for subscript operator
     * @param key the key to get the value for
     * @return the value that matches the key
     */
    ValueT &operator [] (const KeyT key)
    {
        if (!this->containsKey(key))
        {
            this->insert(key, ValueT());
        }
        return _buckets[_getHashCode(key)][_getInnerKeyIdx(key)].second;
    };

    /**
 * @brief overload for subscript operator
 * @param key the key to get the value for
 * @return the value that matches the key
 */
    ValueT operator [] (const KeyT key) const
    {
        if (this->containsKey(key))
        {
            return _buckets[_getHashCode(key)][_getInnerKeyIdx(key)].second;
        }
        else
        {
            throw std::invalid_argument(NONEXISTANT_KEY_ERR);
        }
    };

    /**
     * @brief assigment operator =
     * @param other the map to assign to this map
     * @return the new map
     */
    HashMap &operator = (const HashMap &other)
    {
        _capacity = other._capacity;
        delete[] _buckets;
        _buckets = new std::vector<std::pair<KeyT, ValueT>>[_capacity];
        for (int bucketIdx = 0; bucketIdx < _capacity; bucketIdx++)
        {
            for (auto &element: other._buckets[bucketIdx])
            {
                _buckets[bucketIdx] = other._buckets[bucketIdx];
                this->_size++;
            }
        }
        return *this;
    }
};

#endif //EX3_HASHMAP_HPP
