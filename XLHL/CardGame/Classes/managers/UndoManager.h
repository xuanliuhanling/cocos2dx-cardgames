#ifndef __UNDO_MANAGER_H__
#define __UNDO_MANAGER_H__

#include <vector>
#include "models/UndoModel.h"

/**
 * @brief Manager that maintains undo stack. Must NOT be a singleton.
 * Designed to be held as a member of a controller.
 */
class UndoManager
{
public:
    void push(const UndoModel& record) { _stack.push_back(record); }

    bool canUndo() const { return !_stack.empty(); }

    bool pop(UndoModel& out)
    {
        if (_stack.empty()) return false;
        out = _stack.back();
        _stack.pop_back();
        return true;
    }

    void clear() { _stack.clear(); }

private:
    std::vector<UndoModel> _stack;
};

#endif


