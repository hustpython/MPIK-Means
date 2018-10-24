// cl/utilities/nnmap.hpp
#ifndef CLUSLIB_NNMAP_HPP
#define CLUSLIB_NNMAP_HPP

#include"types.hpp"
#include"exceptions.hpp"
#include<map>
#include<algorithm>

typedef std::pair<Size,Size> nnPair;

class compare_a {
public:
    bool operator() (const nnPair &a, const nnPair &b) const {
        Size amin = std::min(a.first, a.second);
        Size amax = std::max(a.first, a.second);
        Size bmin = std::min(b.first, b.second);
        Size bmax = std::max(b.first, b.second);

        if(amin < bmin) {
            return true;
        } else if (amin == bmin ){
            if (amax < bmax) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
};

class compare_b {
public:
    bool operator() (const nnPair &a, const nnPair &b) const { 
        if(a.first < b.first) {
            return true;
        } else if (a.first == b.first ){
            if (a.second < b.second) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
};

template<class T, class C>
class nnMap {
public:
    typedef typename std::map<nnPair, T, C>::value_type 
        value_type;
    typedef typename std::map<nnPair, T, C>::iterator 
        iterator;
    typedef typename std::map<nnPair, T, C>::const_iterator 
        const_iterator;

    std::pair<iterator, bool> 
        add_item(Size i, Size j, T item);
    bool contain_key(Size i, Size j) const;
    T& operator()(Size i, Size j);
    const T& operator()(Size i, Size j) const;
    void clear();

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const; 
private: 
    std::map<nnPair, T, C> _map;
};

typedef nnMap<Real, compare_a> iirMapA;
typedef nnMap<Size, compare_b> iiiMapB;

class compare_iir {
public:
    bool operator()(const iirMapA::value_type& a,
            const iirMapA::value_type& b) {
        if(a.second < b.second) {
            return true;
        } 
        return false;
    }
};

template<typename T, typename C>
inline std::pair<typename nnMap<T,C>::iterator, bool> 
    nnMap<T,C>::add_item(Size i, Size j, T item) {
    std::pair<iterator, bool> ret =
        _map.insert(std::pair<nnPair, T>(nnPair(i,j), item));

    return ret;
}

template<typename T, typename C>
inline bool nnMap<T,C>::contain_key(Size i, Size j) 
    const {
    const_iterator it; 
    it = _map.find(nnPair(i,j));
    if(it != _map.end() ){
        return true;
    } else {
        return false;
    }
}

template<typename T, typename C>
inline T& nnMap<T,C>::operator()(Size i, Size j) {
    iterator it; 
    it = _map.find(nnPair(i,j));
    if(it != _map.end() ){
        return it->second;
    } else {
        FAIL("Can not find key ("<<i<<","<<j<<") in nnMap");
    }
}

template<typename T, typename C>
inline const T& nnMap<T,C>::operator()(Size i, Size j) 
    const {
    const_iterator it; 
    it = _map.find(nnPair(i,j));
    if(it != _map.end() ){
        return it->second;
    } else {
        FAIL("Can not find key ("<<i<<","<<j<<") in nnMap");
    }
}

template<typename T, typename C>
inline typename nnMap<T,C>::iterator nnMap<T,C>::begin() {
    return _map.begin();
}

template<typename T, typename C>
inline typename nnMap<T,C>::iterator nnMap<T,C>::end() {
    return _map.end();
}

template<typename T, typename C>
inline typename nnMap<T,C>::const_iterator nnMap<T,C>::begin() 
    const {
    return _map.begin();
}

template<typename T, typename C>
inline typename nnMap<T,C>::const_iterator nnMap<T,C>::end() 
    const  {
    return _map.end();
}

template<typename T, typename C>
inline void nnMap<T,C>::clear() {
    _map.clear();
}

#endif
