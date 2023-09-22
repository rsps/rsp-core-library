/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef CONTROL_H
#define CONTROL_H

#include <map>
#include <functional>
#include <string_view>
#include <vector>
#include <magic_enum.hpp>
#include <exceptions/CoreException.h>
#include <graphics/GfxInputEvents.h>
#include <logging/LogChannel.h>
#include <messaging/EventBroker.h>
#include <messaging/Notifier.h>
#include <utils/ConstTypeInfo.h>
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


class Control : public rsp::utils::TypeInfo, public messaging::SubscriberInterface
{
public:
    using TouchCallback_t = rsp::messaging::Notifier<const TouchEvent&, uint32_t>;

    /**
     * \brief Enum type defining the available states
     * Not class scoped on purpose, they are used as array indexes
     */
    enum States {
        Disabled,
        Normal,
        Pressed,
        Dragged,
        CheckedDisabled,// Read only
        CheckedNormal,  // Read only
        CheckedPressed, // Read only
        CheckedDragged  // Read only
    };

    Control() : mLogger("Gfx") { initTypeInfo<Control>(); }
    virtual ~Control();

    Control(const Control &arOther) = default;
    Control &operator=(const Control &arOther) = default;

    template<class T>
    T& GetAs()
    try
    {
        return dynamic_cast<T&>(*this);
    }
    catch(const std::bad_cast &e) {
        THROW_WITH_BACKTRACE2(EControlCast, GetName(), std::string(rsp::utils::NameOf<T>()));
    }

    /**
     * \brief Gets the state of the object
     */
    Control::States GetState() const;

    /**
     * \brief Set the object as invalidated marking it to be re-rendered
     */
    void Invalidate();

    /**
     * \brief Get wether or not the object is currently marked invalid
     * \return True if the object is currently marked as invalid
     */
    bool IsInvalid() const { return mDirty; }

    virtual Control& SetDraggable(bool aValue);
    virtual bool IsDraggable() const { return mDraggable; }

    virtual Control& SetCheckable(bool aValue);
    virtual bool IsCheckable() const { return mCheckable; }
    virtual Control& SetChecked(bool aValue);
    virtual bool IsChecked() const { return mChecked; }

    virtual bool IsVisible() const {return mVisible; }
    virtual Control& Show(bool aVisible = true);
    Control& Hide() { return Show(false); }

    virtual bool IsEnabled() const { return mEnabled; }
    virtual Control& Enable(bool aEnable = true);
    Control& Disable() { return Enable(false); }

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
    bool IsTransparent() const { return mTransparent; }

    /**
     * \brief Virtual method for traversing all GUI elements before rendering.
     *        Useful if GUI elements depends on external data, that
     *        needs to be shown in sync with the GUI rendering.
     *        The method calls the refresh method on all elements.
     */
    virtual bool UpdateData();

    /**
     * \brief Virtual method for rendering the object
     * \param aCanvas The canvas the object is rendered on
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
    Rect GetArea() const;

    /**
     * \brief Expand this area to fit to the size of the parent, if it is smaller.
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
    Point GetOrigin() const;

    /**
     * \brief Gets the touch area in parent coordinates
     * \return The current defined area as a Rectangle
     */
    Rect GetTouchArea() const;

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
    bool IsHit(const Point &arPoint) const { return mTouchArea.IsHit(arPoint); }

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
    Style& GetStyle(States aState) { return mStyles[aState]; }

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
     * \return Reference to callback object
     */
    TouchCallback_t& OnPress() { return mOnPress; }

    /**
     * \brief OnMove callback reference
     *
     * \return Reference to callback object
     */
    TouchCallback_t& OnMove() { return mOnMove; }

    /**
     * \brief OnLift callback reference
     *
     * \return Reference to callback object
     */
    TouchCallback_t& OnLift() { return mOnLift; }

    /**
     * \brief OnClick callback reference
     *
     * \return Reference to callback object
     */
    TouchCallback_t& OnClick() { return mOnClick; }

    /**
     * \brief A design helper function, it will paint a rectangle indicating
     *        the touch area of Control objects in the given color.
     * \param aValue
     * \return Self
     */
    static void SetTouchAreaColor(Color aValue = Color::None) { mTouchAreaColor = aValue; }

protected:
    Rect mArea{}; // Area of Control in screen coordinates
    Rect mTouchArea{}; // Touch area of Control in screen coordinates
//    magic_enum::containers::array<States, Style> mStyles;
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
     * \brief Override this to update this objects textures after it is invalidated
     */
    virtual void update() {}

    /**
     * \brief Override this to perform custom rendering.
     *
     * \param arRenderer
     * \result Boolean true if children is already rendered
     */
    virtual bool render(Renderer &arRenderer) const { return false; }

    /**
     * \brief Override this to handle resizing events
     * \param arRect
     * \param arPoint
     */
    virtual void doSetArea(const Rect &arRect, const Point &arOriginDifference);

    virtual bool doPress(const TouchEvent &arEvent);
    virtual bool doMove(const TouchEvent &arEvent);
    virtual bool doLift(const TouchEvent &arEvent);
    virtual bool doClick(const TouchEvent &arEvent);

    bool handleTouchEvent(rsp::messaging::Event &arEvent);

private:
    static Color mTouchAreaColor;
};

std::string to_string(Control::States aState);
std::ostream& operator<<(std::ostream& os, const Control::States aState);


} // namespace rsp::graphics

#endif // CONTROL_H
