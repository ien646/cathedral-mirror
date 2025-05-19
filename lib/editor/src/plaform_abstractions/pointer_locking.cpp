#include <cathedral/editor/platform_abstractions/pointer_locking.hpp>

#include <cathedral/editor/common/message.hpp>

namespace cathedral::editor
{
    pointer_locker::pointer_locker(QWidget* widget, const bool remap_to_native_parent)
        : QObject(widget)
        , _widget(widget)
        , _remap_to_native_parent(remap_to_native_parent)
    {
#ifdef CATHEDRAL_LINUX_PLATFORM_WAYLAND
        CRITICAL_CHECK_NOTNULL(widget);
        if (widget->windowHandle() != nullptr)
        {
            _native_widget = widget;
        }
        else if (widget->nativeParentWidget() != nullptr)
        {
            _native_widget = widget->nativeParentWidget();
        }
        else
        {
            show_error_message("For some reason, the native window handle could not be found.\nPlease report this");
            return;
        }

        _native_window = _native_widget->windowHandle();
#endif
    }

    void pointer_locker::lock_pointer()
    {
#ifdef CATHEDRAL_LINUX_PLATFORM_WAYLAND
        if (!_lock_pointer)
        {
            _lock_pointer = std::make_unique<LockPointer>(_native_window);
            connect(_lock_pointer.get(), &LockPointer::mouseMovementDelta, this, [this](const QPoint delta) {
                emit mouseMovementDelta(delta);
            });
        }

        if (_remap_to_native_parent)
        {
            const QPoint pos = _widget->mapTo(_native_widget, _widget->pos());
            _lock_pointer->lockPointer(QRect{ pos, _widget->size() });
        }
        else
        {
            _lock_pointer->lockPointer(QRect{ _widget->pos(), _widget->size() });
        }
#else
        if (!_locked)
        {
            _click_pos = QCursor::pos();
        }
        else
        {
            QCursor::setPos(_click_pos);
        }
#endif
    }

    void pointer_locker::unlock_pointer() const
    {
#ifdef CATHEDRAL_LINUX_PLATFORM_WAYLAND
        _lock_pointer->unlockPointer();
#else
        // nothing to do here...
#endif
    }

    bool pointer_locker::is_locked() const
    {
#ifdef CATHEDRAL_LINUX_PLATFORM_WAYLAND
        return _lock_pointer ? _lock_pointer->isPointerLocked() : false;
#else
        return _locked;
#endif
    }
} // namespace cathedral::editor