#include "structs.h"
#include "mymemory.h"

VectorImpl(Int, uint64_t, 4);
VectorImpl(Boolean, Boolean *, 4);
VectorImpl(Void, void *, 4);
HashTableImpl(Void, void *, void *, Ptr_hash_function, Ptr_equals);
HashSetImpl(Void, void *, Ptr_hash_function, Ptr_equals);
