#include <algorithm>
#include <print>
#include <vector>
#include <forward_list>
#include <optional>

template<typename K,typename V,typename HashFunc_t>
class HashMap{
    private:
        size_t m_num_buckets;
        HashFunc_t m_hash_func;
        std::vector<std::forward_list<std::pair<K,V>>> m_buckets_array;
    public:
        HashMap(size_t M):m_num_buckets(M),m_buckets_array(M)
        {
        };
        void insert(K const& key,V const& value){
            size_t hashed_key= m_hash_func(key)%m_num_buckets;
            auto& bucket=m_buckets_array[hashed_key];
            auto prev_it=bucket.before_begin();
            for (auto current_it=bucket.begin();current_it!=bucket.end();current_it++){
                if (current_it->first==key){
                    return;
                }
                prev_it=current_it;
            };
            bucket.insert_after(prev_it,std::pair<K,V>(key,value));
        }
        std::optional<V> get(K const& key){
            auto& bucket=m_buckets_array[m_hash_func(key)%m_num_buckets];
            auto it=std::find_if(bucket.begin(),bucket.end(),[&](auto pair){
                return pair.first==key;
            });
            if (it==bucket.end()){
                return std::nullopt;
            } else{
                return it->second;
            }

        }
};


int main(){
    auto identity=[](int x){return x;};
    HashMap<int,int,decltype(identity)> map(10);
    map.insert(1, 2);
    map.insert(2, 4);
    map.insert(3, 6);
    std::println("{}",map.get(1).value());
}
