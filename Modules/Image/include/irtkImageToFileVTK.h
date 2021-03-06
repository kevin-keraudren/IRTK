/* The Image Registration Toolkit (IRTK)
 *
 * Copyright 2008-2015 Imperial College London
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#ifndef _IRTKIMAGETOFILEVTK_H

#define _IRTKIMAGETOFILEVTK_H

/**
 * Class for image to VTK file filter.
 *
 * This is a class which takes an image as input and produces an image file
 * in VTK file format.
 */

class irtkImageToFileVTK : public irtkImageToFile
{
  irtkObjectMacro(irtkImageToFileVTK);

protected:

  /// Initialize filter
  virtual void Initialize();

};

#endif
