#pragma once

#include <cathedral/core.hpp>

FORWARD_CLASS_INLINE(QWidget);

#ifdef CATHEDRAL_LINUX_PLATFORM_WAYLAND
#include <WaylandQtPointerConstraints/LockPointer.h>
#endif

#include <memory>

namespace cathedral::editor
{
    class pointer_locker final : public QObject
    {
        Q_OBJECT
    public:
        explicit pointer_locker(QWidget* widget, bool remap_to_native_parent = true);

        void lock_pointer();
        void unlock_pointer() const;

        bool is_locked() const;

    signals:
        void mouseMovementDelta(QPoint delta);

    private:
        QWidget* _widget;
        bool _remap_to_native_parent;

#ifdef CATHEDRAL_LINUX_PLATFORM_WAYLAND
        std::unique_ptr<LockPointer> _lock_pointer;
        QWidget* _native_widget = nullptr;
        QWindow* _native_window = nullptr;
#else
        QPoint _click_pos = {};
        bool _locked = false;
#endif
    };
} // namespace cathedral::editor