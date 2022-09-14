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
#include <vector>
#include <string_view>
#include <graphics/primitives/Canvas.h>
#include <graphics/primitives/Color.h>
#include <graphics/primitives/Rect.h>
#include <graphics/TouchEvent.h>
#include <logging/Logger.h>
#include <utils/ConstTypeInfo.h>
#include <utils/CoreException.h>
#include <utils/Function.h>
#include "Style.h"

//#define GFXLOG(a) DLOG(a)
#define GFXLOG(a)

namespace rsp::graphics
{

class EControlCast : public rsp::utils::CoreException
{
public:
    EControlCast(const std::string &arName, const std::string &arType) : rsp::utils::CoreException(arName + " is not of type " + arType) {};
};

class TouchControl;

class Control
{
  public:
    using TouchCallback_t = rsp::utils::Function<void(const Point&, int)>;

    /**
     * \brief Enum type defining the available states
     */
    enum class States {
        disabled = 1,
        normal = 2,
        pressed = 4,
        dragged = 8,
        checked = 16,
        checkedPressed = 32
    };

    Control(const rsp::utils::TypeInfo &arTypeInfo) : mTypeInfo(arTypeInfo) {}
    virtual ~Control() {}

    Control(const Control &arOther) = default;
    Control &operator=(const Control &arOther) = default;

    /**
     * \brief Get the name of the specific scene.
     *
     * \return string with name of scene
     */
    const std::string& GetName() const
    {
        return mTypeInfo.mName;
    }
    Control& SetName(const std::string &arName)
    {
        mTypeInfo.mName = arName;
        return *this;
    }

    std::uint32_t GetId() const
    {
        return mTypeInfo.mId;
    }
    Control& SetId(uint32_t aId);
    Control& SetId(int aId) { return SetId(static_cast<uint32_t>(aId)); }
    Control& SetId(char aId) { return SetId(static_cast<uint32_t>(aId)); }

    template<class T>
    T& GetAs()
    {
        if (GetId() != T::ID) {
            THROW_WITH_BACKTRACE2(EControlCast, GetName(), std::string(T::NAME));
        }
        return *static_cast<T*>(this);
    }

    /**
     * \brief Sets the state of the object
     * \param aState State object should be set to
     */
    void SetState(States aState);

    /**
     * \brief Gets the state of the object
     */
    States GetState() const { return mState; }

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
    virtual bool IsDraggable() { return mDraggable; }

    virtual Control& SetCheckable(bool aValue);
    virtual bool IsCheckable() { return mCheckable; }
    virtual bool IsChecked() { return (mState == States::checked) || (mState == States::checkedPressed); }

    virtual bool IsVisible() {return mVisible; }
    virtual Control& Show(bool aVisible = true);
    virtual Control& Hide() { return Show(false); }

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
    virtual void UpdateData();

    /**
     * \brief Virtual method for rendering the object
     * \param aCanvas The canvas the object is rendered on
     * \return bool True if anything was rendered
     */
    virtual bool Render(Canvas &arCanvas);

    /**
     * \brief Sets the area of the object as a rectangle in parent coordinates
     * \param aRect A reference to the rectangle to define the objects area
     */
    virtual Control& SetArea(Rect aRect);

    /**
     * \brief Gets the area of the object as a rectangle in parent coordinates
     * \return A reference to the rectangle defining the objects area
     */
    Rect GetArea() const;

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
    Rect GetTouchArea();

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
     * \return A reference to this object
     */
    Control& AddChild(Control *apChild);

    /**
     * \brief Get reference to the style data associated with the given state.
     * \param aState The state for which the style is requested
     * \return A reference to a Style object
     */
    Style& GetStyle(States aState) { return mStyles[aState]; }

    Control& SetBitmapPosition(const Point &arPoint);

    rsp::utils::TypeInfo& GetInfo() { return mTypeInfo; }

    /**
     * \brief Processes input for press or click callbacks
     * \param arInput Reference to the input being processed
     * \return True if handled
     */
    bool ProcessInput(TouchEvent &arInput);

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

  protected:
    Rect mArea{}; // Area of Control in screen coordinates
    Rect mTouchArea{}; // Touch area of Control in screen coordinates
    std::map<States, Style> mStyles{};
    Control *mpParent = nullptr;
    std::vector<Control *> mChildren{};
    bool mTransparent = false;
    bool mDirty = true;
    bool mDraggable = false;
    bool mVisible = true;
    bool mCheckable = false;
    States mState = States::normal;
    rsp::utils::TypeInfo mTypeInfo;

    virtual void paint(Canvas &arCanvas, const Style &arStyle);
    virtual void refresh() {};

  private:
      TouchCallback_t mOnPress{};
      TouchCallback_t mOnMove{};
      TouchCallback_t mOnLift{};
      TouchCallback_t mOnClick{};

      virtual void doPress(const Point &arPoint);
      virtual void doMove(const Point &arPoint);
      virtual void doLift(const Point &arPoint);
      virtual void doClick(const Point &arPoint);
};

#define TYPEINFO(a) \
        static constexpr std::uint32_t ID = rsp::utils::crc32::HashOf<a>(); \
        static constexpr std::string_view NAME = rsp::utils::NameOf<a>();

std::string to_string(Control::States aState);
std::ostream& operator<<(std::ostream& os, const Control::States aState);


} // namespace rsp::graphics

#endif // CONTROL_H
