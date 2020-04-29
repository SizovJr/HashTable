#include <iostream>
#include <list>
#include <stdexcept>
#include <vector>

template<typename KeyType, typename ValueType, typename Hash = std::hash<KeyType> > class HashMap {

public:
    using iterator = typename std::list<std::pair<const KeyType, ValueType>>::iterator;
    using const_iterator = typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;
    typedef std::pair<const KeyType, ValueType> cell;

private:
    Hash hasher;
    std::list<cell> data;
    std::vector<std::list<std::pair<KeyType, iterator>>> bucks;
    size_t t_size;

    void reboot() {
        if (t_size < bucks.size()) {
            return;
        }
        auto new_size = 2 * bucks.size();
        bucks.clear();
        bucks.resize(new_size);
        for (auto it = data.begin(); it != data.end(); ++it) {
            bucks[hasher(it->first) % bucks.size()].emplace_back(it->first, it);
        }
    }

public:
    size_t size() const {
        return t_size;
    }

    Hash hash_function() const {
        return hasher;
    }

    bool empty() const {
        return t_size == 0;
    }

    void clear() {
        t_size = 0;
        data.clear();
        bucks.clear();
        bucks.emplace_back();
    }

    iterator begin() {
        return data.begin();
    }

    iterator end() {
        return data.end();
    }

    const_iterator begin() const {
        return data.cbegin();
    }

    const_iterator end() const {
        return data.cend();
    }
    
    HashMap(const Hash &hasher = Hash()): hasher(hasher) {
        bucks = std::vector<std::list<std::pair<KeyType, iterator>>> (1);
        t_size = 0;
    }
    
    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> a): HashMap(a.begin(), a.end()) {} 

    template<typename Iter>
    HashMap(Iter first, Iter second, const Hash &h = Hash()): HashMap(h) {
        for (auto it = first; it != second; ++it) {
            insert(*it);
        }
    }

    HashMap& operator=(const HashMap &other) {
        if (this != &other) { 
            clear();
            for (auto it = other.begin(); it != other.end(); ++it) {
                insert(*it);
            }
        }
        return *this;
    }

    const_iterator find(const KeyType &val) const {
        auto idx = hasher(val) % bucks.size();
        for (auto it = bucks[idx].begin(); it != bucks[idx].end(); ++it) {
            if (it->first == val) {
                return it->second;
            }
        }
        return data.cend();
    }

    iterator find(const KeyType &val) {
        auto idx = hasher(val) % bucks.size();
        for (auto it = bucks[idx].begin(); it != bucks[idx].end(); ++it) {
            if (it->first == val) {
                return it->second;
            }
        }
        return data.end();
    }

    void insert(const cell &val) {
        auto idx = hasher(val.first) % bucks.size();
        for (auto cur : bucks[idx]) {
            if (val.first == cur.first) {
                return;
            }
        }
        data.push_front(val);
        bucks[idx].emplace_back(val.first, data.begin());
        ++t_size;
        reboot();
    }

    void erase(const KeyType &val) {
        auto idx = hasher(val) % bucks.size();
        for (auto it = bucks[idx].begin(); it != bucks[idx].end(); ++it) {
            if (it->first == val) {
                --t_size;
                data.erase(it->second);
                bucks[idx].erase(it);
                return;
            }
        }
    }

    ValueType& operator[](const KeyType &val) {
        auto idx = hasher(val) % bucks.size();
        for (auto it = bucks[idx].begin(); it != bucks[idx].end(); ++it) {
            if (it->first == val) {
                return (it->second)->second;
            }
        }
        ++t_size;
        data.emplace_front(val, ValueType());
        bucks[idx].emplace_back(val, data.begin());
        reboot();
        return (data.begin())->second;
    }
    
    const ValueType& at(const KeyType &val) const {
        auto idx = hasher(val) % bucks.size();
        for (auto it = bucks[idx].begin(); it != bucks[idx].end(); ++it) {
            if (it->first == val) {
                return (it->second)->second;
            }
        }
        throw std::out_of_range("Lol");
    }
};
