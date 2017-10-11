/*
 * ActivationOverlay.h
 *
 * Copyright (C) 2009-17 by RStudio, Inc.
 *
 * Unless you have received this program directly from RStudio pursuant
 * to the terms of a commercial license agreement with RStudio, then
 * this program is licensed to you under the terms of version 3 of the
 * GNU Affero General Public License. This program is distributed WITHOUT
 * ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF NON-INFRINGEMENT,
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Please refer to the
 * AGPL (http://www.gnu.org/licenses/agpl-3.0.txt) for more details.
 *
 */

#ifndef DESKTOP_ACTIVATION_HPP
#define DESKTOP_ACTIVATION_HPP

#include <core/FilePath.hpp>

#import <Foundation/Foundation.h>

namespace rstudio {
namespace core {
   class Error;
}
}

@interface Activation : NSObject {
}

- (id) initWithInstallPath: (rstudio::core::FilePath&) installPath
                   devMode: (BOOL) devMode;

- (BOOL) getInitialLicense;
- (BOOL) allowProductUsage;
- (std::string) activationStateMessage;
- (BOOL) updateLicenseState;

@end

#endif // DESKTOP_ACTIVATION_HPP