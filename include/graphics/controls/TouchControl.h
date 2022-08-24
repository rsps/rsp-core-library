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

#include <graphics/primitives/Rect.h>
#include <graphics/TouchEvent.h>
#include <utils/Function.h>
#include "Control.h"

namespace rsp::graphics
{

class TouchControl : public Control
{
public:
    using TouchCallback_t = rsp::utils::Function<void(const Point&, int)>;

    TouchControl();
    TouchControl(const Rect &arArea);
    virtual ~TouchControl();

    TouchControl(const TouchControl&);
    TouchControl(TouchControl&&);

    TouchControl& SetId(int aId) { mId = aId; return *this; }

    /**
     * \brief Processes input for press or click callbacks
     * \param arInput Reference to the input being processed
     */
    void ProcessInput(TouchEvent &arInput);

    /**
     * \brief Checks if a given coordinates is inside the defined Rectangle
     * \param arPoint Reference to the coordinate
     * \return True if hit
     */
    bool IsHit(const Point &arPoint) const;

    /**
     * \brief Assignment operator
     * \param arTouchArea A const reference to a TouchArea
     * \return Reference to this TouchArea
     */
    TouchControl &operator=(const TouchControl &);

    /**
     * \brief Move operator
     * \param arTouchArea A const reference to a TouchArea
     * \return Reference to this TouchArea
     */
    TouchControl &operator=(TouchControl &&);

    /**
     * \brief Gets the area of the TouchArea
     * \return A reference to the current defined area as a Rectangle
     */
    Rect& GetTouchArea() { return mTouchArea; }

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
    int mId = 0;

private:
    TouchCallback_t mOnPress{};
    TouchCallback_t mOnMove{};
    TouchCallback_t mOnLift{};
    TouchCallback_t mOnClick{};

    Rect mTouchArea{};
    Point mCurrentPress{};
    Point mOriginalPress{};

    virtual void doPress(const Point &arPoint);
    virtual void doMove(const Point &arPoint);
    virtual void doLift(const Point &arPoint);
    virtual void doClick(const Point &arPoint);
};

} // namespace rsp::graphics
#endif // TOUCHAREA_H
