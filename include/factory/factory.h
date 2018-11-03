#pragma once
#include <array>
#include <memory>
#include <string_view>
#include <type_traits>
#include <unordered_map>

/// <summary>
/// Internal Class for Storing either a std::unique_ptr or the Class.
/// </summary>
template <typename Interface, typename Key>
class factory_internal {
 public:
  factory_internal() = default;
  ~factory_internal() = default;

 protected:
  using StorageType = Interface;
  std::unordered_map<Key, Interface> m_map;
};
/// <summary>
/// FWD Declaration of the Factory you should use!
/// </summary>
template <class Interface, class Key = std::string_view, typename Enable = void>
class factory;
/// <summary>
/// If the Interface is abstract, this Implementation is choosen
/// </summary>
template <class Interface, class Key>
class factory<Interface, Key, typename std::enable_if_t<std::is_abstract_v<Interface>>> : public factory_internal<std::unique_ptr<Interface>, Key> {
 public:
  /// <summary>
  /// Registering a new Type to return a Copy from at copy. Throws if the key is already used. TODO: Find a better Name!
  /// </summary>
  /// <param name="key">Key to  identify the new Type</param>
  /// <param name="arguments">Arguments to create the new Type with std::make_unique</param>
  template <class T, class... Args>
  void register_(const Key& key, Args&&... arguments) {
    // Check if Key exists
    if (this->m_map.find(key) == this->m_map.end()) {
      this->m_map[key] = std::make_unique<T>(std::forward<Args>(arguments)...);
    } else {
      throw std::runtime_error("Duplicate Key");
    }
  }
  /// <summary>
  /// Method to create a Copy of the Data Type stored associated with Key. Throws if the key isn't found!
  /// </summary>
  /// <param name="Key">Key of the Type</param>
  /// <returns>Copy of the Type stored under Key</returns>
  template <class T>
  T copy(const Key& key) const {
    if (this->m_map.find(key) == this->m_map.end()) {
      throw std::runtime_error("Key not found");
    } else {
      return *(static_cast<T*>(this->m_map.at(key).get()));
    }
  }
};
/// <summary>
/// Factory Implementation if Interface isn't abstract, be careful about Object Slicing
/// </summary>
template <class Interface, class Key>
class factory<Interface, Key, typename std::enable_if_t<!std::is_abstract_v<Interface>>> : public factory_internal<Interface, Key> {
 public:
  /// <summary>
  /// Registering a new Type with a Key. Throws if the Key already exists
  /// </summary>
  /// <param name="key">Key to use</param>
  /// <param name="Args">Arguments to use, to instaniate the new Type</param>
  template <typename T = Interface, class... Args>
  void register_(const Key & key, Args&&... arguments) {
    if (this->m_map.find(key) != this->m_map.end()) {
      throw std::runtime_error("Key already exists");
    } else {
      this->m_map[key] = T{std::forward<Args>(arguments)...};
    }
  }
  /// <summary>
  /// Method to create a Copy from a Type. Template Parameter can be used to invoke a static_cast.
  /// </summary>
  /// <param name="key">Key of the Type</param>
  template <class T = Interface>
  T copy(const Key & key) {
    return static_cast<T>(this->m_map.at(key));
  }
};
/// <summary>
/// Constexpr Factory
/// </summary>
template <class Interface, std::size_t Size, class Key = std::string_view>
class constexpr_factory {
 public:
  constexpr_factory() = delete;
  /// <summary>
  /// Constructor to create a new Factory.
  /// </summary>
  /// <param name="arguments">Template Pack, must be convertible to std::pair<Key,Interface></param>
  template <class... Args>
  constexpr constexpr_factory(Args&&... arguments) : data{std::forward<Args>(arguments)...} {
    static_assert(Size == sizeof...(arguments));
  }
  /// <summary>
  /// Function to create a Copy from a registered Type
  /// </summary>
  Interface copy(const Key& key) const {
    for (std::size_t i = 0; i < Size; i++) {
      if (key == data[i].first)
        return data[i].second;
    }
    throw std::runtime_error("Key not found");
  }

 private:
  const std::array<std::pair<Key, Interface>, Size> data;
};