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
#include <utils/ConstTypeInfo.h>
#include "Style.h"

namespace rsp::graphics
{

class TouchControl;

class Control
{
  public:
    /**
     * \brief Enum type defining the available states
     */
    enum class States : int {
        disabled,
        normal,
        pressed,
        dragged,
        checked
    };

    Control() {}
    Control(const rsp::utils::TypeInfo &arInfo) : mTypeInfo(arInfo) {}
    virtual ~Control() {}

    Control(const Control &arOther) = default;
    Control &operator=(const Control &arOther) = default;

    template<class T>
    T& GetAs()
    {
        return *static_cast<T*>(this);
    }

    /**
     * \brief Get the name of the specific scene.
     *
     * \return string with name of scene
     */
    std::string GetName()
    {
        return std::string(mTypeInfo.mName);
    }

    std::uint32_t GetId()
    {
        return mTypeInfo.mId;
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
    bool IsInvalid() const { return mIsInvalid; }

    /**
     * \brief Set the object to transparent or not
     * \param aValue Boolean determining the object transparent or not
     * \return A reference to the object
     */
    Control& SetTransparent(bool aValue)
    {
        mTransparent = aValue;
        return *this;
    }

    /**
     * \brief Gets if the object is transparent
     * \return True if transparent
     */
    bool IsTransparent() const { return mTransparent; }

    /**
     * \brief Abstract method for rendering the object
     * \param aCanvas The canvas the object is rendered on
     * \return bool True if anything was rendered
     */
    virtual bool Render(Canvas &arCanvas);

    /**
     * \brief Sets the area of the object as a rectangle
     * \param aRect A reference to the rectangle to define the objects area
     */
    virtual Control& SetArea(const Rect &arRect);

    /**
     * \brief Gets the area of the object as a rectangle
     * \return A reference to the rectangle defining the objects area
     */
    const Rect& GetArea() const { return mArea; }

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

    rsp::utils::TypeInfo& GetInfo() { return mTypeInfo; }

  protected:
    Rect mArea{};
    std::map<States, Style> mStyles{};
    bool mTransparent = false;
    Control *mpParent = nullptr;
    std::vector<Control *> mChildren{};
    bool mIsInvalid = true;
    States mState = States::normal;
    rsp::utils::TypeInfo mTypeInfo{rsp::utils::MakeTypeInfo<Control>()};

    virtual void addTouchable(TouchControl *apTouchControl);
    virtual void removeTouchable(TouchControl *apTouchControl);

    virtual void paint(Canvas &arCanvas, const Style &arStyle);
};

std::string to_string(Control::States aState);
std::ostream& operator<<(std::ostream& os, const Control::States aState);


} // namespace rsp::graphics

#endif // CONTROL_H
