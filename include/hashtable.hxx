#pragma once


//----------------------------------------------
//  HashTable
//
//  Maps pointer-sized integers to pointer-sized integers.
//  Uses open addressing with linear probing.
//  In the m_cells array, key = 0 is reserved to indicate an unused cell.
//  Actual value for key 0 (if any) is stored in m_zeroCell.
//  The hash table automatically doubles in size when it becomes 75% full.
//  The hash table never shrinks in size, even after Clear(), unless you explicitly call Compact().
//----------------------------------------------
class HashTable
{
public:
    struct Cell
    {
        size_t key;
        size_t value;
    };
    
private:
    Cell* m_cells;
    size_t m_arraySize;
    size_t m_population;
    bool m_zeroUsed;
    Cell m_zeroCell;
    
    void Repopulate(size_t desiredSize);

public:
    HashTable(size_t initialSize = 8);
    ~HashTable();

    // Basic operations
    Cell* Lookup(size_t key);
    Cell* Insert(size_t key);
    void Delete(Cell* cell);
    void Clear();
    void Compact();

    void Delete(size_t key)
    {
        Cell* value = Lookup(key);
        if (value)
            Delete(value);
    }

    //----------------------------------------------
    //  Iterator
    //----------------------------------------------
    friend class Iterator;
    class Iterator
    {
    private:
        HashTable& m_table;
        Cell* m_cur;

    public:
        Iterator(HashTable &table);
        Cell* Next();
        inline Cell* operator*() const { return m_cur; }
        inline Cell* operator->() const { return m_cur; }
    };
};
#pragma once


#define NULL 0

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

inline uint32_t upper_power_of_two(uint32_t v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

inline uint64_t upper_power_of_two(uint64_t v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    v++;
    return v;
}

// from code.google.com/p/smhasher/wiki/MurmurHash3
inline uint32_t integerHash(uint32_t h)
{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}

// from code.google.com/p/smhasher/wiki/MurmurHash3
inline uint64_t integerHash(uint64_t k)
{
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccd;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53;
	k ^= k >> 33;
	return k;
}
//---------------------------------------------------
// Build-time configuration
//---------------------------------------------------

#define INTEGER_MAP_CACHE_STOMPER_ENABLED 0
#define INTEGER_MAP_TWEAK_PRIORITY_AFFINITY 1
#define INTEGER_MAP_USE_DLMALLOC 1

#define INTEGER_MAP_TIMING_METHOD_QUERY_PERFORMANCE_COUNTER     0
#define INTEGER_MAP_TIMING_METHOD_RDTSC                         1
#define INTEGER_MAP_TIMING_METHOD(type) (INTEGER_MAP_TIMING_METHOD_##type == INTEGER_MAP_TIMING_METHOD_RDTSC)
#define INTEGER_MAP_TIMING_METHOD_STR "RDTSC"

#define INTEGER_MAP_EXPERIMENT_INSERT     0
#define INTEGER_MAP_EXPERIMENT_LOOKUP     1
#define INTEGER_MAP_EXPERIMENT_MEMORY     2
#define INTEGER_MAP_EXPERIMENT(type) (INTEGER_MAP_EXPERIMENT_##type == INTEGER_MAP_EXPERIMENT_INSERT)
#define INTEGER_MAP_EXPERIMENT_STR "INSERT"

#define INTEGER_MAP_CONTAINER_NONE       0
#define INTEGER_MAP_CONTAINER_JUDY       1
#define INTEGER_MAP_CONTAINER_TABLE      2
#define INTEGER_MAP_CONTAINER(type) (INTEGER_MAP_CONTAINER_##type == INTEGER_MAP_CONTAINER_TABLE)
#define INTEGER_MAP_CONTAINER_STR "TABLE"

#define INTEGER_MAP_KEY_GENERATION_LINEAR                       0
#define INTEGER_MAP_KEY_GENERATION_SORTED_ADDRESSES             1
#define INTEGER_MAP_KEY_GENERATION_SHUFFLED_ADDRESSES           2
#define INTEGER_MAP_KEY_GENERATION_RANDOM_SEQUENCE_OF_UNIQUE    3
#define INTEGER_MAP_KEY_GENERATION(type) (INTEGER_MAP_KEY_GENERATION_##type == INTEGER_MAP_KEY_GENERATION_RANDOM_SEQUENCE_OF_UNIQUE)
#define INTEGER_MAP_KEY_GENERATION_STR "RANDOM_SEQUENCE_OF_UNIQUE"

#define INTEGER_MAP_MAX_ADDRESS_BLOCK_SIZE 256



#include <assert.h>
#include <memory.h>


#define FIRST_CELL(hash) (m_cells + ((hash) & (m_arraySize - 1)))
#define CIRCULAR_NEXT(c) ((c) + 1 != m_cells + m_arraySize ? (c) + 1 : m_cells)
#define CIRCULAR_OFFSET(a, b) ((b) >= (a) ? (b) - (a) : m_arraySize + (b) - (a))


//----------------------------------------------
//  HashTable::HashTable
//----------------------------------------------
HashTable::HashTable(size_t initialSize)
{
    // Initialize regular cells
    m_arraySize = initialSize;
    assert((m_arraySize & (m_arraySize - 1)) == 0);   // Must be a power of 2
    m_cells = new Cell[m_arraySize];
    memset(m_cells, 0, sizeof(Cell) * m_arraySize);
    m_population = 0;

    // Initialize zero cell
    m_zeroUsed = 0;
    m_zeroCell.key = 0;
    m_zeroCell.value = 0;
}

//----------------------------------------------
//  HashTable::~HashTable
//----------------------------------------------
HashTable::~HashTable()
{
    // Delete regular cells
    delete[] m_cells;
}

//----------------------------------------------
//  HashTable::Lookup
//----------------------------------------------
HashTable::Cell* HashTable::Lookup(size_t key)
{
    if (key)
    {
        // Check regular cells
        for (Cell* cell = FIRST_CELL(integerHash(key));; cell = CIRCULAR_NEXT(cell))
        {
            if (cell->key == key)
                return cell;
            if (!cell->key)
                return NULL;
        }
    }
    else
    {
        // Check zero cell
        if (m_zeroUsed)
            return &m_zeroCell;
        return NULL;
    }
};

//----------------------------------------------
//  HashTable::Insert
//----------------------------------------------
HashTable::Cell* HashTable::Insert(size_t key)
{
    if (key)
    {
        // Check regular cells
        for (;;)
        {
            for (Cell* cell = FIRST_CELL(integerHash(key));; cell = CIRCULAR_NEXT(cell))
            {
                if (cell->key == key)
                    return cell;        // Found
                if (cell->key == 0)
                {
                    // Insert here
                    if ((m_population + 1) * 4 >= m_arraySize * 3)
                    {
                        // Time to resize
                        Repopulate(m_arraySize * 2);
                        break;
                    }
                    ++m_population;
                    cell->key = key;
                    return cell;
                }
            }
        }
    }
    else
    {
        // Check zero cell
        if (!m_zeroUsed)
        {
            // Insert here
            m_zeroUsed = true;
            if (++m_population * 4 >= m_arraySize * 3)
			{
				// Even though we didn't use a regular slot, let's keep the sizing rules consistent
                Repopulate(m_arraySize * 2);
			}
        }
        return &m_zeroCell;
    }
}

//----------------------------------------------
//  HashTable::Delete
//----------------------------------------------
void HashTable::Delete(Cell* cell)
{
    if (cell != &m_zeroCell)
    {
        // Delete from regular cells
        assert(cell >= m_cells && cell - m_cells < m_arraySize);
        assert(cell->key);

        // Remove this cell by shuffling neighboring cells so there are no gaps in anyone's probe chain
        for (Cell* neighbor = CIRCULAR_NEXT(cell);; neighbor = CIRCULAR_NEXT(neighbor))
        {
            if (!neighbor->key)
            {
                // There's nobody to swap with. Go ahead and clear this cell, then return
                cell->key = 0;
                cell->value = 0;
                m_population--;
                return;
            }
            Cell* ideal = FIRST_CELL(integerHash(neighbor->key));
            if (CIRCULAR_OFFSET(ideal, cell) < CIRCULAR_OFFSET(ideal, neighbor))
            {
                // Swap with neighbor, then make neighbor the new cell to remove.
                *cell = *neighbor;
                cell = neighbor;
            }
        }
    }
    else
    {
        // Delete zero cell
        assert(m_zeroUsed);
        m_zeroUsed = false;
        cell->value = 0;
        m_population--;
        return;
    }
}

//----------------------------------------------
//  HashTable::Clear
//----------------------------------------------
void HashTable::Clear()
{
    // (Does not resize the array)
    // Clear regular cells
    memset(m_cells, 0, sizeof(Cell) * m_arraySize);
    m_population = 0;
    // Clear zero cell
    m_zeroUsed = false;
    m_zeroCell.value = 0;
}

//----------------------------------------------
//  HashTable::Compact
//----------------------------------------------
void HashTable::Compact()
{
    Repopulate(upper_power_of_two((m_population * 4 + 3) / 3));
}

//----------------------------------------------
//  HashTable::Repopulate
//----------------------------------------------
void HashTable::Repopulate(size_t desiredSize)
{
    assert((desiredSize & (desiredSize - 1)) == 0);   // Must be a power of 2
    assert(m_population * 4  <= desiredSize * 3);

    // Get start/end pointers of old array
    Cell* oldCells = m_cells;
    Cell* end = m_cells + m_arraySize;

    // Allocate new array
    m_arraySize = desiredSize;
    m_cells = new Cell[m_arraySize];
    memset(m_cells, 0, sizeof(Cell) * m_arraySize);

    // Iterate through old array
    for (Cell* c = oldCells; c != end; c++)
    {
        if (c->key)
        {
            // Insert this element into new array
            for (Cell* cell = FIRST_CELL(integerHash(c->key));; cell = CIRCULAR_NEXT(cell))
            {
                if (!cell->key)
                {
                    // Insert here
                    *cell = *c;
                    break;
                }
            }
        }
    }

    // Delete old array
    delete[] oldCells;
}

//----------------------------------------------
//  Iterator::Iterator
//----------------------------------------------
HashTable::Iterator::Iterator(HashTable &table) : m_table(table)
{
    m_cur = &m_table.m_zeroCell;
    if (!m_table.m_zeroUsed)
        Next();
}

//----------------------------------------------
//  Iterator::Next
//----------------------------------------------
HashTable::Cell* HashTable::Iterator::Next()
{
    // Already finished?
    if (!m_cur)
        return m_cur;

    // Iterate past zero cell
    if (m_cur == &m_table.m_zeroCell)
        m_cur = &m_table.m_cells[-1];

    // Iterate through the regular cells
    Cell* end = m_table.m_cells + m_table.m_arraySize;
    while (++m_cur != end)
    {
        if (m_cur->key)
            return m_cur;
    }

    // Finished
    return m_cur = NULL;
}
