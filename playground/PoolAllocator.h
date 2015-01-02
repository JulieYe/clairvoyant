#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <malloc.h>

namespace simulator
{

    template<typename T, int AllocationBoundary = 64>
    class PoolAllocator {
    public:
    PoolAllocator(int preallocate = 10) {
	_firstFree = NULL;
	for (int i = 0; i < preallocate; ++i) {
	    free(newT());
	}
    }
    
    ~PoolAllocator() {
	while (_firstFree) {
	    FreeItem* toFree = _firstFree;
	    _firstFree = _firstFree->next;
	    ::free(toFree);
	}
    }
    
    T* allocate() {
	if (_firstFree) {
	    void* result = reinterpret_cast<void*>(_firstFree);
	    _firstFree = _firstFree->next;
	    return new (result) T();
	}
	return newT();
    }
    
    void free(T* object) {
	if (!object) return;
	object->~T();
	FreeItem* item = reinterpret_cast<FreeItem*>(object);
	item->next = _firstFree;
	_firstFree = item;
    }
    
    void trimFreeList(int maxEntries) {
	FreeItem** freePtrPtr = &_firstFree;
	for (int i = 0; i < maxEntries; ++i) {
	    if (*freePtrPtr == NULL) {
		return;
	    }
	    freePtrPtr = &(*freePtrPtr)->next;
	}
	FreeItem* iter = *freePtrPtr;
	*freePtrPtr = NULL;
	while (iter) {
	    FreeItem* toFree = iter;
	    iter = iter->next;
	    ::free(toFree);
	}
    }
    
    private:
    T* newT() const {
	void* memory = ::memalign(AllocationBoundary, sizeof(T));
	return new (memory) T();
    }

    struct FreeItem {
	FreeItem* next;
    };

    FreeItem* _firstFree;
    };
    
}

#endif    // POOL_ALLOCATOR_H
