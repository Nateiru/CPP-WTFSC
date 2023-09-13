#include <iostream>
#include <iterator>
#include <list>
#include <unordered_map>
using namespace std;

struct Node {
  int key, val, freq;
  Node(int _key, int _val, int _freq): key(_key), val(_val), freq(_freq) {}
};

class LFUCache {
  int minfreq, capacity;
  unordered_map<int, list<Node>::iterator> key_table;
  unordered_map<int, list<list<Node>>::iterator> freq_table;
  list<list<Node>> freq_list;
public:
  LFUCache(int capacity_) {
    capacity = capacity_;
    key_table.clear();
    freq_table.clear();
    freq_list.clear();
  }

  int MINFREQ() {
    if (freq_list.empty())
      return 0;

    return freq_list.begin()->begin()->freq;
  }

  void erase(int key) {
    if (capacity == 0)
      return;

    auto it_key = key_table.find(key);

    if (it_key == key_table.end())
      return;

    auto node = it_key->second;
    int val = node->val, freq = node->freq;
    auto it_list = freq_table.find(freq);
    // 直接删除
    it_list->second->erase(node);
    if (it_list->second->empty()) {
      freq_list.erase(it_list->second);
      freq_table.erase(freq);
    }
  }

  int get(int key) {
    if (capacity == 0)
      return -1;

    auto it_key = key_table.find(key);
    if (it_key == key_table.end())
      return -1;

    auto node = it_key->second;
    int val = node->val, freq = node->freq;
    auto it_list = freq_table.find(freq);

    //  插入到 freq + 1
    if (freq_table.find(freq + 1) != freq_table.end()) {
      freq_table[freq + 1]->push_front(Node(key, val, freq + 1));
      key_table[key] = freq_table[freq + 1]->begin();
    } else {
      // 需要在 freq list 后面插入一个新的 list 表示频率是 freq + 1
      auto new_list = list<Node> {Node(key, val, freq + 1)};
      freq_table[freq + 1] = freq_list.insert(next(it_list->second), new_list);
      key_table[key] = freq_table[freq + 1]->begin();
    }

    // 删除 freq
    it_list->second->erase(node);

    if (it_list->second->empty()) {
      freq_list.erase(it_list->second);
      freq_table.erase(freq);
    }

    return val;
  }

  void put(int key, int val) {
    if (capacity == 0)
      return;

    auto it_key = key_table.find(key);

    // 不存在需要插入
    if (it_key == key_table.end()) {
      // 考虑是否以及满了
      if (key_table.size() == capacity) {
        int minfreq = MINFREQ();
        auto it_list = freq_table[minfreq];
        auto it2 = it_list->back();
        key_table.erase(it2.key);
        it_list->pop_back();

        if (it_list->empty()) {
          freq_list.erase(it_list);
          freq_table.erase(minfreq);
        }
      }

      // 现在有空位 插入
      // 插入频率是1
      auto it_list = freq_table.find(1);

      if (it_list == freq_table.end())  {
        // 需要在 freq list 后面插入一个新的 list 表示频率是 freq + 1
        auto new_list = list<Node> {Node(key, val, 1)};
        freq_list.push_front(new_list);
        freq_table[1] = freq_list.begin();
        key_table[key] = freq_table[1]->begin();
      } else {
        it_list->second->push_front(Node(key, val, 1));
        key_table[key] = it_list->second->begin();
      }
    } 
    else {
      auto node = it_key->second;
      int freq = node->freq;
      auto it_list = freq_table.find(freq);

      //  插入到 freq + 1
      if (freq_table.find(freq + 1) != freq_table.end()) {
        freq_table[freq + 1]->push_front(Node(key, val, freq + 1));
        key_table[key] = freq_table[freq + 1]->begin();
      } else {
        // 需要在 freq list 后面插入一个新的 list 表示频率是 freq + 1
        auto new_list = list<Node> {Node(key, val, freq + 1)};
        freq_table[freq + 1] = freq_list.insert(next(it_list->second), new_list);
        key_table[key] = freq_table[freq + 1]->begin();
      }
      it_list->second->erase(node);
      if (it_list->second->empty()) {
        freq_list.erase(it_list->second);
        freq_table.erase(freq);
      }
    }
  }
};

int main() {
  LFUCache cache{2};
  cache.put(2, 1);
  cache.put(2, 2);
  std::cout << cache.get(2) << std::endl;
  cache.put(1, 1);
  cache.put(4, 1);
  std::cout << cache.get(2) << std::endl;
  return 0;
}