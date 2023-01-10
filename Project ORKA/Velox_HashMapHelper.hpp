
#pragma once

//written by Omarito
//modified by
//  Blendurian

#include <math.h>
#include "Velox_Common.hpp"
#include "Velox_Memory.hpp"
#include "Velox_Utils.hpp"
#include "Velox_UtilityConcepts.hpp"
#include "Velox_HashMapIterator.hpp"
#include "Velox_HashMapLinkedList.hpp"
#include"Velox_HashPolicies.hpp"

TRE_NS_START

template<typename K, typename V, typename H, typename HP, typename KE>
class hashMapHelper : private H, private KE
{
public:
    struct InlinePair
    {
        alignas(K) uint8 key[sizeof(K)];
        alignas(V) uint8 value[sizeof(V)];

        FORCEINLINE const K& key() const {
            return *reinterpret_cast<const K*>(key);
        }

        FORCEINLINE K& key() {
            return *reinterpret_cast<K*>(key);
        }

        FORCEINLINE const V& value() const {
            return *reinterpret_cast<const V*>(value);
        }

        FORCEINLINE V& value() {
            return *reinterpret_cast<V*>(value);
        }

        FORCEINLINE const K& first() const {
            return *reinterpret_cast<const K*>(key);
        }

        FORCEINLINE K& first() {
            return *reinterpret_cast<K*>(key);
        }

        FORCEINLINE const V& second() const {
            return *reinterpret_cast<const V*>(value);
        }

        FORCEINLINE V& second() {
            return *reinterpret_cast<V*>(value);
        }
    };

public:
    using HashPolicy = HP;
    using Hasher = H;
    using KeyEqual = KE;

    using KeyType = K;
    using ValueType = V;
    using value_type = InlinePair;
    using const_value_type = const value_type;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using hasher = H;
    using key_equal = KE;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

public:
    constexpr static usize BLOCK_SIZE = 16;
    constexpr static float MAX_LOAD_FACTOR = 0.9375f;
    constexpr static usize INITIAL_SIZE = 32;

    struct Constants
    {        
        static constexpr uint8 MAGIC_FOR_EMPTY      = uint8(0b11111111);
        static constexpr uint8 MAGIC_FOR_RESERVED   = uint8(0b11111110);
        static constexpr uint8 BITS_FOR_DIRECT_HIT  = uint8(0b10000000);
        static constexpr uint8 MAGIC_FOR_DIRECT_HIT = uint8(0b00000000);
        static constexpr uint8 MAGIC_FOR_LIST_ENTRY = uint8(0b10000000);
        static constexpr uint8 BITS_FOR_DISTANCE    = uint8(0b01111111);
        static constexpr uint32 NUM_JUMP_DISTANCES  = 126;

        FORCEINLINE static uint8 distanceFromMetadata(uint8 metadata)
        {
            return metadata & BITS_FOR_DISTANCE;
        }

        // jump distances chosen like this:
        // 1. pick the first 16 integers to promote staying in the same block
        // 2. add the next 66 triangular numbers to get even jumps when
        // the hash table is a power of two
        // 3. add 44 more triangular numbers at a much steeper growth rate
        // to get a sequence that allows large jumps so that a table
        // with 10000 sequential numbers doesn't endlessly re-allocate
        static constexpr usize JUMP_DISTANCES[NUM_JUMP_DISTANCES]
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,

            21, 28, 36, 45, 55, 66, 78, 91, 105, 120, 136, 153, 171, 190, 210, 231,
            253, 276, 300, 325, 351, 378, 406, 435, 465, 496, 528, 561, 595, 630,
            666, 703, 741, 780, 820, 861, 903, 946, 990, 1035, 1081, 1128, 1176,
            1225, 1275, 1326, 1378, 1431, 1485, 1540, 1596, 1653, 1711, 1770, 1830,
            1891, 1953, 2016, 2080, 2145, 2211, 2278, 2346, 2415, 2485, 2556,

            3741, 8385, 18915, 42486, 95703, 215496, 485605, 1091503, 2456436,
            5529475, 12437578, 27986421, 62972253, 141700195, 318819126, 717314626,
            1614000520, 3631437253, 8170829695, 18384318876, 41364501751,
            93070021080, 209407709220, 471167588430, 1060127437995, 2385287281530,
            5366895564381, 12075513791265, 27169907873235, 61132301007778,
            137547673121001, 309482258302503, 696335090510256, 1566753939653640,
            3525196427195653, 7931691866727775, 17846306747368716,
            40154190394120111, 90346928493040500, 203280588949935750,
            457381324898247375, 1029107980662394500, 2315492957028380766,
            5209859150892887590,
        };
    };

    template<usize BlockSize>
    struct Block
    {
        uint8 controlBytes[BlockSize];
        InlinePair pairs[BlockSize];
        // alignas(K) uint8 keys[sizeof(K) * BlockSize];
        // alignas(V) uint8 values[sizeof(V) * BlockSize];

        static Block* emptyBlock()
        {
            static std::array<uint8, BlockSize> emptyBytes = []
            {
                std::array<uint8, BlockSize> result;
                result.fill(Constants::MAGIC_FOR_EMPTY);
                return result;
            }();

            return reinterpret_cast<Block*>(&emptyBytes);
        }

        constexpr FORCEINLINE usize getFirstEmptyIndex() const
        {
            for (uint32 i = 0; i < BlockSize; i++) {
                if (controlBytes[i] == Constants::MAGIC_FOR_EMPTY)
                    return i;
            }
            return -1;
        }

        constexpr FORCEINLINE void fillControlBytes(uint8 value)
        {
            Utils::MemSet(controlBytes, value, BlockSize);
        }

        constexpr FORCEINLINE value_type* getPair(uint32 idx)
        {
            return reinterpret_cast<value_type*>(pairs[idx].key);
        }

        constexpr FORCEINLINE const value_type* getPair(uint32 idx) const
        {
            return reinterpret_cast<const value_type*>(pairs[idx].key);
        }

        constexpr FORCEINLINE K* getKey(uint32 idx)
        {
            return reinterpret_cast<K*>(&pairs[idx].key);
        }

        constexpr FORCEINLINE const K* getKey(uint32 idx) const
        {
            return reinterpret_cast<K*>(&pairs[idx].key);
        }

        constexpr FORCEINLINE V* getValue(uint32 idx)
        {
            return reinterpret_cast<V*>(&pairs[idx].value);
        }

        constexpr FORCEINLINE const K* getValue(uint32 idx) const
        {
            return reinterpret_cast<V*>(&pairs[idx].value);
        }

        constexpr FORCEINLINE void destroy(uint32 idx)
        {
            Utils::destroy(this->getKey(idx));
            Utils::destroy(this->getValue(idx));
        }

        constexpr static FORCEINLINE void destroy(const value_type& adr)
        {
            Utils::destroy(&adr.Key());
            Utils::destroy(&adr.value());
        }

        template<typename Key, typename... Args>
        constexpr static FORCEINLINE void construct(value_type& adr, Key&& key, Args&&... args)
        {
            new (&adr.key) K(std::forward<Key>(key));
            new (&adr.value) V(std::forward<Args>(args)...);
        }
    };

    using BlockType      = Block<BLOCK_SIZE>;
    using BlockPointer   = BlockType*;
    using iterator       = TemplatedIterator<hashMapHelper, value_type>;
    using const_iterator = TemplatedIterator<hashMapHelper, const value_type>;
    using LinkedList     = LinkedListItr<hashMapHelper>;

    friend LinkedList;

    struct IteratorProxy
    {
        BlockPointer itr;
        usize index;
        constexpr FORCEINLINE operator iterator()
        {
            if (itr->control_bytes[index % BLOCK_SIZE] == Constants::MAGIC_FOR_EMPTY)
                return ++iterator{itr, index};
            else
                return { itr, index };
        }
        constexpr FORCEINLINE operator const_iterator()
        {
            if (itr->control_bytes[index % BLOCK_SIZE] == Constants::MAGIC_FOR_EMPTY)
                return ++iterator{itr, index};
            else
                return { itr, index };
        }
    };
public:
    constexpr hashMapHelper();

    constexpr ~hashMapHelper();

    template<typename Key, typename... Args>
    constexpr std::pair<iterator, bool> emplace(Key&& key, Args&&... args);

    template<typename Key, typename... Args>
    constexpr FORCEINLINE std::pair<iterator, bool> emplace(const Key& key, Args&&... args)
    {
        return this->emplace(key, std::forward<Args>(args)...);
    }

    template<typename Key, typename... Args>
    constexpr std::pair<iterator, bool> emplaceDirectHit(LinkedList block, Key&& key, Args&&... args);

    template<typename Key, typename... Args>
    constexpr std::pair<iterator, bool> emplaceNewKey(LinkedList parent, Key&& key, Args&&... args);

    constexpr FORCEINLINE iterator find(const K& key) const noexcept;

    constexpr FORCEINLINE IteratorProxy erase(const const_iterator& itr) noexcept;

    constexpr FORCEINLINE bool erase(const K& key) noexcept;

    constexpr FORCEINLINE void clear() noexcept;

    constexpr void reinit(usize count);

    constexpr FORCEINLINE void reserve(usize size);

    constexpr FORCEINLINE usize size() const noexcept { return m_ElementsCount; }

    constexpr FORCEINLINE usize bucketCount() const noexcept { return m_SlotsCount ? m_SlotsCount + 1 : 0 ; }

    constexpr FORCEINLINE float loadFactor() const noexcept { return static_cast<float>(m_ElementsCount) / (m_SlotsCount + 1); }

    constexpr FORCEINLINE float getMaxLoadFactor() const noexcept { return MAX_LOAD_FACTOR; }

    constexpr FORCEINLINE bool empty() const noexcept { return m_ElementsCount == 0; }

    constexpr FORCEINLINE bool isFull() const noexcept
    {
        if (!m_SlotsCount)
            return true;
        else
            return m_ElementsCount + 1 > (m_SlotsCount + 1) * static_cast<double>(MAX_LOAD_FACTOR);
    }

    constexpr FORCEINLINE iterator begin() noexcept
    {
        size_t slots = m_SlotsCount ? m_SlotsCount + 1 : 0;
        return ++iterator{ m_Entries + slots / BLOCK_SIZE, slots };
    }

    constexpr FORCEINLINE iterator begin() const noexcept
    {
        size_t slots = m_SlotsCount ? m_SlotsCount + 1 : 0;
        return ++iterator{ m_Entries + slots / BLOCK_SIZE, slots };
    }

    constexpr FORCEINLINE const_iterator cbegin() const noexcept
    {
        return this->begin();
    }

    constexpr FORCEINLINE iterator end() noexcept
    {
        return { m_Entries - 1, std::numeric_limits<usize>::max() };
    }

    constexpr FORCEINLINE iterator end() const noexcept
    {
        return { m_Entries - 1, std::numeric_limits<usize>::max() };
    }

    constexpr FORCEINLINE const_iterator cend() const noexcept
    {
        return this->end();
    }

private:
    constexpr void deallocateData(BlockPointer begin, usize slotsCount) noexcept;

    constexpr FORCEINLINE void resetToEmpty() noexcept;

    constexpr FORCEINLINE void reinitForOtherContainer(const hashMapHelper& other)
    {
        this->reinit(std::min(this->numBucketsForReserve(other.size()), other.bucketCount()));
    }

    template<typename U>
    constexpr FORCEINLINE usize hashObject(const U& key) noexcept
    {
        return static_cast<Hasher&>(*this)(key);
    }

    template<typename U>
    constexpr FORCEINLINE usize hashObject(const U& key) const noexcept
    {
        // return std::hash<U>{}(key);
        return static_cast<const Hasher&>(*this)(key);
    }

    template<typename L, typename R>
    constexpr FORCEINLINE bool comparesEqual(const L& lhs, const R& rhs) const noexcept
    {
        return lhs == rhs;
    }

    constexpr FORCEINLINE void grow()
    {
        return this->reinit(std::max(INITIAL_SIZE, 2 * this->bucketCount()));
    }

    constexpr FORCEINLINE usize numBucketsForReserve(usize size) const noexcept
    {
        return static_cast<usize>(std::ceil(size / static_cast<double>(MAX_LOAD_FACTOR)));
    }

    constexpr FORCEINLINE usize calculateMemorySize(usize blockCount) const noexcept
    {
        usize memRequired = sizeof(BlockType) * blockCount;
        memRequired += BLOCK_SIZE; // for metadata of past-the-end pointer
        return memRequired;
    }

    constexpr FORCEINLINE std::pair<uint8, LinkedList> findFreeIndex(LinkedList parent) const noexcept
    {
        for (uint8 jumpIndex = 1; jumpIndex < Constants::NUM_JUMP_DISTANCES; ++jumpIndex) {
            auto index = m_HashPolicy.keepInRange(parent.index + Constants::JUMP_DISTANCES[jumpIndex], m_SlotsCount);
            BlockPointer block = m_Entries + (index / BLOCK_SIZE);

            if (block->controlBytes[index % BLOCK_SIZE] == Constants::MAGIC_FOR_EMPTY)
                return { jumpIndex, { index, block } };
        }

        return { 0, {} };
    }

    constexpr FORCEINLINE LinkedList findDirectHit(LinkedList child) const noexcept
    {
        auto& keyValuePtr = *child;
        usize toMoveHash = hashObject(keyValuePtr.first());
        usize toMoveIndex = m_HashPolicy.indexForHash(toMoveHash, m_SlotsCount);
        return { toMoveIndex, m_Entries + toMoveIndex / BLOCK_SIZE };
    }

    constexpr FORCEINLINE LinkedList findParentBlock(LinkedList child) noexcept
    {
        LinkedList parentBlock = findDirectHit(child);
        while (1) {
            LinkedList next = parentBlock.next(*this);
            if (next == child)
                return parentBlock;
            parentBlock = next;
        }
    }
private:
    BlockPointer m_Entries;
    usize m_SlotsCount;
    usize m_ElementsCount;
    HashPolicy m_HashPolicy;
};

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr hashMapHelper<K, V, H, HP , KE>::hashMapHelper()
    : m_Entries{BlockType::emptyBlock()}, m_SlotsCount{0}, m_ElementsCount{0}, m_HashPolicy{}
{

}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr hashMapHelper<K, V, H, HP , KE>::~hashMapHelper()
{
    this->clear();
    this->deallocateData(m_Entries, m_SlotsCount);
}

template<typename K, typename V, typename H, typename HP, typename KE>
template<typename Key, typename... Args>
constexpr auto hashMapHelper<K, V, H, HP , KE>::emplace(Key&& key, Args&&... args) -> std::pair<iterator, bool>
{
    // printf("Inserting [%d] = %d\n", key, V(std::forward<Args>(args)...));
    usize index = this->hashObject(key);
    usize slots = this->m_SlotsCount;
    BlockPointer entries = this->m_Entries;
    index = m_HashPolicy.indexForHash(index, slots);
    bool first = true;

    while (1) {
        usize blockIndex = index / BLOCK_SIZE;
        uint32 indexInBlock = index % BLOCK_SIZE;
        BlockPointer block = entries + blockIndex;
        uint8 metadata = block->controlBytes[indexInBlock];

        if (first) {
            if ((metadata & Constants::BITS_FOR_DIRECT_HIT) != Constants::MAGIC_FOR_DIRECT_HIT)
                return emplaceDirectHit({ index, block }, std::forward<Key>(key), std::forward<Args>(args)...);
            first = false;
        }

        const K& inMemKey = *block->getKey(indexInBlock);
        // printf("In mem key comparing to %d\n", inMemKey);
        if (comparesEqual(key, inMemKey))
            return { { block, index }, false };

        uint8 toNextIndex = metadata & Constants::BITS_FOR_DISTANCE;
        if (toNextIndex == 0)
            return emplaceNewKey({ index, block }, std::forward<Key>(key), std::forward<Args>(args)...);

        index += Constants::JUMP_DISTANCES[toNextIndex];
        index = m_HashPolicy.keepInRange(index, slots);
    }
}

template<typename K, typename V, typename H, typename HP, typename KE>
template<typename Key, typename... Args>
constexpr auto hashMapHelper<K, V, H, HP , KE>::emplaceDirectHit(LinkedList block, Key&& key, Args&&... args) -> std::pair<iterator, bool>
{
    if (this->isFull()) {
        this->grow();
        return this->emplace(std::forward<Key>(key), std::forward<Args>(args)...);
    }

    // printf("Emplace direct hit %d\n", key);
    if (block.getMetadata() == Constants::MAGIC_FOR_EMPTY){
        // AllocatorTraits::construct(*this, std::addressof(*block), std::forward<Args>(args)...);
        BlockType::construct(*block, std::forward<Key>(key), std::forward<Args>(args)...);
        block.setMetadata(Constants::MAGIC_FOR_DIRECT_HIT);
        ++m_ElementsCount;
        return { block.iterator(), true };
    }else{
        LinkedList parentBlock = this->findParentBlock(block);
        std::pair<uint8, LinkedList> freeBlock = this->findFreeIndex(parentBlock);

        if (!freeBlock.first) {
            this->grow();
            return this->emplace(std::forward<Key>(key), std::forward<Args>(args)...);
        } // ValueType newValue(std::forward<Args>(args)...);

        for (LinkedList it = block;;) {
            auto& keyValuePairPtr = *it;
            BlockType::construct(*freeBlock.second, std::move(keyValuePairPtr.first()), std::move(keyValuePairPtr.second()));
            BlockType::destroy(keyValuePairPtr); // Should destroy the moved object
            parentBlock.setNext(freeBlock.first);
            freeBlock.second.setMetadata(Constants::MAGIC_FOR_LIST_ENTRY);

            if (!it.hasNext()) {
                it.setMetadata(Constants::MAGIC_FOR_EMPTY);
                break;
            }

            LinkedList next = it.next(*this);
            it.setMetadata(Constants::MAGIC_FOR_EMPTY);
            block.setMetadata(Constants::MAGIC_FOR_RESERVED);
            it = next;
            parentBlock = freeBlock.second;
            freeBlock = this->findFreeIndex(freeBlock.second);

            if (!freeBlock.first) {
                this->grow();
                return this->emplace(std::forward<Key>(key), std::forward<Args>(args)...);
            }
        }

        BlockType::construct(*block, std::forward<Key>(key), std::forward<Args>(args)...);
        block.setMetadata(Constants::MAGIC_FOR_DIRECT_HIT);
        ++m_ElementsCount;
        return { block.iterator(), true };
    }
}

template<typename K, typename V, typename H, typename HP, typename KE>
template<typename Key, typename... Args>
constexpr auto hashMapHelper<K, V, H, HP , KE>::emplaceNewKey(LinkedList parent, Key&& key, Args&&... args) -> std::pair<iterator, bool>
{
    if (this->isFull()) {
        this->grow();
        return this->emplace(std::forward<K>(key), std::forward<Args>(args)...);
    }

    std::pair<uint8, LinkedList> freeBlock = this->findFreeIndex(parent);

    if (!freeBlock.first) {
        this->grow();
        return this->emplace(std::forward<K>(key), std::forward<Args>(args)...);
    }

    // printf("Emplace new key : %d\n", key);
    BlockType::construct(*freeBlock.second, std::forward<K>(key), std::forward<Args>(args)...);
    freeBlock.second.setMetadata(Constants::MAGIC_FOR_LIST_ENTRY);
    parent.setNext(freeBlock.first);
    ++m_ElementsCount;
    return { freeBlock.second.iterator(), true };
}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr FORCEINLINE auto hashMapHelper<K, V, H, HP , KE>::find(const K& key) const noexcept -> iterator
{
    usize index = hashObject(key);
    usize slots = this->m_SlotsCount;
    BlockPointer entries = this->m_Entries;
    index = m_HashPolicy.indexForHash(index, slots);
    bool first = true;

    for (;;) {
        usize blockIndex = index / BLOCK_SIZE;
        uint32 indexInBlock = index % BLOCK_SIZE;
        BlockPointer block = entries + blockIndex;
        uint8 metadata = block->controlBytes[indexInBlock];

        if (first) {
            if ((metadata & Constants::BITS_FOR_DIRECT_HIT) != Constants::MAGIC_FOR_DIRECT_HIT)
                return this->end();
            first = false;
        }

        const K& inMemKey = *block->getKey(indexInBlock);
        // printf("[FIND] In mem key comparing to %d\n", inMemKey);
        if (this->comparesEqual(key, inMemKey))
            return { block, index };

        uint8 toNextIndex = metadata & Constants::BITS_FOR_DISTANCE;
        if (toNextIndex == 0)
            return this->end();

        index += Constants::JUMP_DISTANCES[toNextIndex];
        index = m_HashPolicy.keepInRange(index, slots);
    }

    return this->end();
}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr FORCEINLINE auto hashMapHelper<K, V, H, HP , KE>::erase(const const_iterator& toErase) noexcept -> IteratorProxy
{
    LinkedList current = { toErase.index, toErase.current };

    if (current.hasNext()) {
        LinkedList previous = current;
        LinkedList next = current.next(*this);
        while (next.hasNext()){
            previous = next;
            next = next.next(*this);
        }

        auto& keyValuePairPtr = *next;
        BlockType::destroy(*current);
        BlockType::construct(*current, std::move(keyValuePairPtr.first()), std::move(keyValuePairPtr.second()));
        BlockType::destroy(keyValuePairPtr); // Should destroy the moved object
        next.setMetadata(Constants::MAGIC_FOR_EMPTY);
        previous.clearNext();
    }else{
        if (!current.isDirectHit())
            this->findParentBlock(current).clearNext();
        BlockType::destroy(*current);
        current.setMetadata(Constants::MAGIC_FOR_EMPTY);
    }
    --m_ElementsCount;
    return { toErase.current, toErase.index };
}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr FORCEINLINE bool hashMapHelper<K, V, H, HP , KE>::erase(const K& key) noexcept
{
    auto res = this->find(key);

    if (res == this->end())
        return false;

    this->erase(res);
    return true;
}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr FORCEINLINE void hashMapHelper<K, V, H, HP , KE>::clear() noexcept
{
    if (!m_SlotsCount)
        return;

    usize slotsCount = m_SlotsCount + 1;
    usize blocksCount = slotsCount / BLOCK_SIZE;

    if (slotsCount % BLOCK_SIZE)
        ++blocksCount;

    for (BlockPointer itr = m_Entries, end = itr + blocksCount; itr != end; ++itr) {
        for (uint32 i = 0; i < BLOCK_SIZE; ++i) {
            if (itr->controlBytes[i] != Constants::MAGIC_FOR_EMPTY) {
                itr->destroy(i);
                itr->controlBytes[i] = Constants::MAGIC_FOR_EMPTY;
            }
        }
    }

    m_ElementsCount = 0;
}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr void hashMapHelper<K, V, H, HP , KE>::reinit(usize count)
{
    count = std::max(count, static_cast<usize>(std::ceil(m_ElementsCount / static_cast<double>(MAX_LOAD_FACTOR))));

    if (count == 0) {
        this->resetToEmpty();
        return;
    }

    if (count == m_SlotsCount + 1)
        return;
    auto newIndex = m_HashPolicy.nextSize(count);
    usize blocksCount = count / BLOCK_SIZE;
    if (count % BLOCK_SIZE)
        ++blocksCount;
    // printf("Growing with size : %d\n", count);
    auto memSize = this->calculateMemorySize(blocksCount);
    void* newMemory = Utils::allocateBytes(memSize);
    BlockPointer newBuckets = reinterpret_cast<BlockPointer>(newMemory);
    BlockPointer endItem = newBuckets + blocksCount;
    for (BlockPointer ptr = newBuckets; ptr <= endItem; ptr++)
        ptr->fillControlBytes(Constants::MAGIC_FOR_EMPTY);
    std::swap(m_Entries, newBuckets);
    std::swap(m_SlotsCount, count);
    --m_SlotsCount;
    m_HashPolicy.commit(newIndex);
    m_ElementsCount = 0;
    if (count)
        ++count;
    usize oldBlocksCount = count / BLOCK_SIZE;
    if (count % BLOCK_SIZE)
        ++oldBlocksCount;

    for (BlockPointer itr = newBuckets, end = newBuckets + oldBlocksCount; itr != end; ++itr) {
        for (uint32 i = 0; i < BLOCK_SIZE; ++i) {
            uint8 metadata = itr->controlBytes[i];

            if (metadata != Constants::MAGIC_FOR_EMPTY && metadata != Constants::MAGIC_FOR_RESERVED) {
                KeyType* key = itr->getKey(i);
                ValueType* value = itr->getValue(i);
                this->emplace(std::move(*key), std::move(*value));
                itr->destroy(i);
            }
        }
    }

    this->deallocateData(newBuckets, count - 1);
}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr FORCEINLINE void hashMapHelper<K, V, H, HP , KE>::reserve(usize size)
{
    usize requiredBuckets =this->numBucketsForReserve(size);
    if (requiredBuckets > this->bucketCount())
        return this->reinit(requiredBuckets);
}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr FORCEINLINE void hashMapHelper<K, V, H, HP , KE>::resetToEmpty() noexcept
{
    this->deallocateData(m_Entries, m_SlotsCount);
    m_Entries = BlockType::emptyBlock();
    m_SlotsCount = 0;
    m_HashPolicy.reset();
}

template<typename K, typename V, typename H, typename HP, typename KE>
constexpr void hashMapHelper<K, V, H, HP , KE>::deallocateData(hashMapHelper::BlockPointer begin, usize slotsCount) noexcept
{
    if (begin == BlockType::emptyBlock())
        return;
    ++slotsCount;
    usize blocksCount = slotsCount / BLOCK_SIZE;
    if (slotsCount % BLOCK_SIZE)
        ++blocksCount;
    // usize mem = this->CalculateMemorySize(blocksCount);
    Utils::freeMemory(begin);
}

TRE_NS_END
