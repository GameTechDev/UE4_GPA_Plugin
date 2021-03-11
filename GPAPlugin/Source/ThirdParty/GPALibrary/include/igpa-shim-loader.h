/******************************************************************************
        Copyright 2020 - 2021 Intel Corporation.
 
This software and the related documents are Intel copyrighted materials,
and your use of them is governed by the express license under which they
were provided to you ("License"). Unless the License provides otherwise,
you may not use, modify, copy, publish, distribute, disclose or transmit
this software or the related documents without Intel's prior written
permission.


 This software and the related documents are provided as is, with no express
or implied warranties, other than those that are expressly stated in the
License.

******************************************************************************/

#pragma once
#include "igpa-config.h"
#include <memory>

////////////////////////////////////////////////////////////////////////////////
/**
 * @struct IGPA
 *
 * @brief IGPA implemenents an interface that allows to initialize and use the GPA 
 * capture functionality within external SW 
 *
 */
struct IGPA
{
    enum class Result {
        Ok,            //!< Operation succeeded
        NotSupported,  //!< Function is not supported for current API or platform
        Failed         //!< Operation failed
    };

    //! @brief  Initialize the GPA capture library
    //! @return possible errors:
    //!         NotSupported if the method is not supported for current API or platform
    //!         Operation failed if errors occur during initialization
    virtual Result Initialize() = 0;

    //! @brief Emit capture start/stop event that is handled by capture layer
    virtual void TriggerStreamCapture() = 0;

    //! @brief Release all created GPA resources
    virtual void Release() = 0;
};

//! @brief     Object factory method returning a created GPA instance
//! @param[in] Path to directory with installed GPA binaries
GPA_EXPORT IGPA* GetGPAInterface(const std::string& gpaBinaryPath);
