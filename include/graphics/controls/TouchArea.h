/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef TOUCHAREA_H
#define TOUCHAREA_H

#include "Image.h"
#include "Input.h"
#include <graphics/primitives/Rect.h>
#include <messaging/eventTypes/ClickedEvent.h>
#include <utils/Function.h>

namespace rsp::graphics
{

class Scene;

class TouchArea
{
  public:
    using TouchCallback_t = rsp::utils::Function<void(const Point&)>;

    TouchArea(const Rect &arArea);
    ~TouchArea();

    /**
     * \brief Processes input for press or click callbacks
     * \param arInput Reference to the input being processed
     */
    void ProcessInput(Input &arInput);

    /**
     * \brief Checks if a given coordinates is inside the defined Rectangle
     * \param arPoint Reference to the coordinate
     * \return True if hit
     */
    bool IsHit(const Point &arPoint) const;

    /**
     * \brief The default =operator
     * \param arTouchArea A const reference to a TouchArea
     */
    TouchArea &operator=(const TouchArea &) = default;

    /**
     * \brief Sets the area of the TouchArea
     * \param arRect The new rectangle used as the defined area
     */
    TouchArea &SetArea(const Rect &arRect)
    {
        mTouchArea = arRect;
        return *this;
    }

    /**
     * \brief Gets the area of the TouchArea
     * \return A reference to the current defined area as a Rectangle
     */
    const Rect &GetArea() const { return mTouchArea; }

    /**
     * \brief OnPress callback reference
     *
     * \return Reference to callback object
     */
    TouchCallback_t& GetOnPress() { return mOnPress; }

    /**
     * \brief OnMove callback reference
     *
     * \return Reference to callback object
     */
    TouchCallback_t& GetOnMove() { return mOnMove; }

    /**
     * \brief OnLift callback reference
     *
     * \return Reference to callback object
     */
    TouchCallback_t& GetOnLift() { return mOnLift; }

    /**
     * \brief OnClick callback reference
     *
     * \return Reference to callback object
     */
    TouchCallback_t& GetOnClick() { return mOnClick; }

  protected:
    TouchCallback_t mOnPress{};
    TouchCallback_t mOnMove{};
    TouchCallback_t mOnLift{};
    TouchCallback_t mOnClick{};

    Rect mTouchArea{};
    Point mCurrentPress{};
    Point mOriginalPress{};
};

} // namespace rsp::graphics
#endif // TOUCHAREA_H
