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

#include "graphics/primitives/Color.h"
#include "graphics/primitives/Rect.h"
#include <vector>

namespace rsp::graphics
{

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
    Control(const Rect &arRect);
    virtual ~Control() {}

    Control(const Control &arOther) = default;
    Control &operator=(const Control &arOther) = default;

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
    Control &SetTransparent(bool aValue)
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
     */
    virtual void Render(Canvas &aCanvas) = 0;

    /**
     * \brief Sets the area of the object as a rectangle
     * \param aRect A reference to the rectangle to define the objects area
     */
    void SetArea(const Rect &aRect);

    /**
     * \brief Gets the area of the object as a rectangle
     * \return A reference to the rectangle defining the objects area
     */
    const Rect &GetArea() const { return mArea; }

    /**
     * \brief Adds a child to the vector of child control pointer objects
     * \param apChild A pointer to the child
     * \return A reference to this object
     */
    Control &AddChild(Control *apChild)
    {
        mChildren.push_back(apChild);
        apChild->mpParent = this;
        return *this;
    }

  protected:
    Rect mArea{};
    Color mBackground{0};
    bool mTransparent = false;
    Control *mpParent = nullptr;
    std::vector<Control *> mChildren{};
    bool mIsInvalid = true;
    States mState = States::normal;
};

} // namespace rsp::graphics

#endif // CONTROL_H
