/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021-2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_GRAPHICS_CONTROL_H
#define RSP_CORE_LIB_GRAPHICS_CONTROL_H

#include <string_view>
#include <vector>
#include <magic_enum/magic_enum.hpp>
#include <rsp/exceptions/CoreException.h>
#include <rsp/graphics/GfxInputEvents.h>
#include <rsp/logging/LogChannel.h>
#include <rsp/messaging/EventBroker.h>
#include <rsp/messaging/Notifier.h>
#include <rsp/utils/ConstTypeInfo.h>
#include "Canvas.h"
#include "Color.h"
#include "Rect.h"
#include "Renderer.h"
#include "Style.h"

#define GFXLOG(a) DLOG(a)
//#define GFXLOG(a)

namespace rsp::graphics {

class EControlCast : public exceptions::CoreException
{
public:
    EControlCast(const std::string &arName, const std::string &arType) : CoreException(arName + " is not of type " + arType) {};
};

class Control : public utils::TypeInfo, public messaging::SubscriberInterface
{
public:
    using TouchCallback_t = messaging::Notifier<const TouchEvent&, uint32_t>;

    /**
     * \brief Enum type defining the available states of a GUI object.
     *
     * Not enum class scoped on purpose, they are used as array indexes.
     */
    enum States { //NOSONAR
        Disabled,
        Normal,
        Pressed,
        Dragged,
        CheckedDisabled,// Read-only
        CheckedNormal,  // Read-only
        CheckedPressed, // Read-only
        CheckedDragged  // Read-only
    };

    Control() : mLogger("Gfx") { initTypeInfo<Control>(); }
    ~Control() override;

    Control(const Control &arOther) = default;
    Control &operator=(const Control &arOther) = default;

    template<class T>
    T& GetAs()
    try
    {
        return dynamic_cast<T&>(*this);
    }
    catch(const std::bad_cast&) {
        THROW_WITH_BACKTRACE2(EControlCast, GetName(), std::string(rsp::utils::NameOf<T>()));
    }

    /**
     * \brief Gets the state of the object
     */
    [[nodiscard]] States GetState() const;

    /**
     * \brief Set the object as invalidated, marking it to be re-rendered
     */
    void Invalidate();

    /**
     * \brief Get whether the object is currently marked invalid
     * \return True if the object is currently marked as invalid
     */
    [[nodiscard]] bool IsInvalid() const { return mDirty; }

    virtual Control& SetDraggable(bool aValue);
    [[nodiscard]] virtual bool IsDraggable() const { return mDraggable; }

    virtual Control& SetCheckable(bool aValue);
    [[nodiscard]] virtual bool IsCheckable() const { return mCheckable; }
    virtual Control& SetChecked(bool aValue);
    [[nodiscard]] virtual bool IsChecked() const { return mChecked; }

    virtual Control& SetVisible(bool aVisible);
    [[nodiscard]] virtual bool IsVisible() const {return mVisible; }
    Control& Show() { return SetVisible(true); };
    Control& Hide() { return SetVisible(false); }

    [[nodiscard]] virtual bool IsEnabled() const { return mEnabled; }
    virtual Control& SetEnable(bool aEnable);
    Control& Enable() { return SetEnable(true); };
    Control& Disable() { return SetEnable(false); }

    virtual Control& SetPressed(bool aValue);
    virtual Control& SetDragged(bool aValue);

    /**
     * \brief Set the object to transparent or not
     * \param aValue Boolean determining the object transparent or not
     * \return A reference to the object
     */
    Control& SetTransparent(bool aValue);

    /**
     * \brief Gets if the object is transparent
     * \return True if transparent
     */
    [[nodiscard]] bool IsTransparent() const { return mTransparent; }

    /**
     * \brief Virtual method for traversing all GUI elements before rendering.
     *        Useful if GUI elements depend on external data that
     *        needs to be shown in sync with the GUI rendering.
     *        The method calls the refresh method on all elements.
     */
    virtual bool UpdateData();

    /**
     * \brief Virtual method for rendering the object
     * \param arRenderer The Renderer interface this object is rendered to
     */
    void Render(Renderer &arRenderer) const;

    /**
     * \brief Sets the area of the object as a rectangle in parent coordinates
     * \param aRect A reference to the rectangle to define the objects area
     */
    Control& SetArea(Rect aRect);

    /**
     * \brief Gets the area of the object as a rectangle in parent coordinates
     * \return A reference to the rectangle defining the objects area
     */
    [[nodiscard]] Rect GetArea() const;

    /**
     * \brief Expand this area to fit to the size of the parent if it is smaller.
     *
     * \return self
     */
    Control& ExpandToParent();

    /**
     * \brief Set the origin for this Control object in screen coordinates
     *
     * \param arPoint
     * \return Reference to this
     */
    virtual Control& SetOrigin(const Point &arPoint);
    /**
     * \brief Get the origin for this Control object in screen coordinates
     * \return Point
     */
    [[nodiscard]] Point GetOrigin() const;

    /**
     * \brief Gets the touch area in parent coordinates
     * \return The current defined area as a Rectangle
     */
    [[nodiscard]] Rect GetTouchArea() const;

    /**
     * \brief Set the touch area in parent coordinates
     * \param aRect
     * \return Reference to this
     */
    Control& SetTouchArea(Rect aRect);

    /**
     * \brief Check if a touch coordinate is inside this objects touch area
     *
     * \param arPoint
     * \return bool
     */
    [[nodiscard]] bool IsHit(const Point &arPoint) const { return mTouchArea.IsHit(arPoint); }

    /**
     * \brief Adds a child to the vector of child control pointer objects
     * \param apChild A pointer to the child
     * \return self
     */
    Control& AddChild(Control *apChild);

    /**
     * \brief Removes a child from this object
     * \param apChild
     * \return self
     */
    Control& RemoveChild(Control *apChild);

    /**
     * \brief Get reference to the style data associated with the given state.
     * \param aState The state for which the style is requested
     * \return A reference to a Style object
     */
    Style& GetStyle(const States aState) { return mStyles[aState]; }

    Control& SetTexturePosition(const Point &arPoint);

    /**
     * \brief Processes events for this object
     * \param arEvent Reference to the event being processed
     * \return True if handled
     */
    bool ProcessEvent(rsp::messaging::Event &arEvent) override;

    /**
     * \brief OnPress callback reference
     *
     * \return Reference to the callback object
     */
    TouchCallback_t& OnPress() { return mOnPress; }

    /**
     * \brief OnMove callback reference
     *
     * \return Reference to the callback object
     */
    TouchCallback_t& OnMove() { return mOnMove; }

    /**
     * \brief OnLift callback reference
     *
     * \return Reference to the callback object
     */
    TouchCallback_t& OnLift() { return mOnLift; }

    /**
     * \brief OnClick callback reference
     *
     * \return Reference to the callback object
     */
    TouchCallback_t& OnClick() { return mOnClick; }

    /**
     * \brief A design helper function, it will paint a rectangle indicating
     *        the touch area of Control objects in the given color.
     * \param aValue The color to set for the touch area
     */
    static void SetTouchAreaColor(Color aValue = Color::None) { mTouchAreaColor = std::move(aValue); }

protected:
    Rect mArea{}; // Area of Control in screen coordinates
    Rect mTouchArea{}; // Touch area of Control in screen coordinates
    // magic_enum::containers::array<States, Style> mStyles{};
    Style mStyles[magic_enum::enum_count<States>()]{};
    Control *mpParent = nullptr;
    std::vector<Control *> mChildren{};
    bool mTransparent = false;
    bool mDirty = true;
    bool mDraggable = false;
    bool mVisible = true;
    bool mCheckable = false;
    bool mChecked = false;
    bool mEnabled = true;
    bool mPressed = false;
    bool mDragged = false;
    rsp::logging::LogChannel mLogger;

    TouchCallback_t mOnPress{};
    TouchCallback_t mOnMove{};
    TouchCallback_t mOnLift{};
    TouchCallback_t mOnClick{};

    void setName(const std::string &arName) override;
    void setId(uint32_t aId) override;

    /**
     * \brief Override this to refresh this objects data during UpdateData
     */
    virtual void refresh() {}

    /**
     * \brief Override this to update this object textures after it is invalidated
     */
    virtual void update() {}

    /**
     * \brief Override this to perform custom rendering.
     *
     * \param arRenderer
     * \return Boolean true if children are already rendered
     */
    virtual bool render([[maybe_unused]] Renderer &arRenderer) const { return false; }

    /**
     * \brief Override this to add functionality control is resized
     *
     * \param arRect
     * \param arOriginDifference
     */
    virtual void doSetArea([[maybe_unused]] const Rect &arRect, [[maybe_unused]]const Point &arOriginDifference) {}

    virtual bool doPress(const TouchEvent &arEvent);
    virtual bool doMove(const TouchEvent &arEvent);
    virtual bool doLift(const TouchEvent &arEvent);
    virtual bool doClick(const TouchEvent &arEvent);

    bool processChildren(messaging::Event &arEvent) const;
    bool handleTouchEvent(messaging::Event &arEvent);

private:
    static Color mTouchAreaColor;
};

std::string to_string(Control::States aState);
std::ostream& operator<<(std::ostream& os, Control::States aState);


} // namespace rsp::graphics

#endif // RSP_CORE_LIB_GRAPHICS_CONTROL_H
