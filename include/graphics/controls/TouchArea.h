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

#include "graphics/controls/Image.h"
#include "graphics/controls/Input.h"
#include "graphics/primitives/Rect.h"
#include "messaging/eventTypes/ClickedEvent.h"

namespace rsp::graphics
{

class TouchArea
{
  public:
    TouchArea() : mTouchArea(0, 0, 0, 0), mClickedTopic(rsp::messaging::ClickTopic::NullTopic), mclickEvent(""){};
    TouchArea(Rect &aArea, rsp::messaging::ClickTopic aClickTopic = rsp::messaging::ClickTopic::NullTopic, std::string aClickInfo = "");
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
     * \brief Register a function to be called for pressed events
     * \param aFunc A function to handle pressed callbacks
     */
    void RegisterOnPressed(std::function<void(Control::States)> aFunc);

    /**
     * \brief Register a function to be called for click events
     * \param aFunc A function to handle clicked callbacks
     */
    void RegisterOnClicked(std::function<void(rsp::messaging::ClickTopic, rsp::messaging::ClickedEvent &)> aFunc);

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

  protected:
    std::function<void(Control::States)> mPressed{}; //= [](Control::States state) noexcept { (void)state; };
    std::function<void(rsp::messaging::ClickTopic, rsp::messaging::ClickedEvent &)> mClicked{};

    Rect mTouchArea{};
    Point mCurrentPress{0, 0};
    Point mOriginalPress{0, 0};
    rsp::messaging::ClickTopic mClickedTopic;
    rsp::messaging::ClickedEvent mclickEvent;
};

} // namespace rsp::graphics
#endif // TOUCHAREA_H
