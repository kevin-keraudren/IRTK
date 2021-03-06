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

#ifndef _IRTKINVERSEAFFINETRANSFORMATION_H
#define _IRTKINVERSEAFFINETRANSFORMATION_H

#include <irtkEventDelegate.h>


/**
 * Class for inverse linear transformation.
 *
 * An instance of this class decorates either a rigid, similarity, or an
 * affine transformation and represents the inverse of this transformation,
 * i.e., T(x) = A^-1 x. The ParametricGradient function computes the
 * update of the parameters of the decorated transformation. Instances of this
 * class are used by the image registration filter for an inverse consistent
 * (possibly symmetric) affine registration.
 *
 * Note: For a symmetric inverse consistent affine registration, the use of
 *       irtkAffineTransformation and irtkInverseAffineTransformation is more
 *       efficient then two irtkPartialAffineTransformation instances.
 *       For example, use the \c ireg energy function setting
 *       "-NMI(I1 o T^-1, I2 o T)" instead of "-NMI(I1 o T^-0.5, I2 o T^0.5)".
 *       The resulting transformation has to be squared, i.e., applied twice
 *       to obtain the full transformation between I1 and I2, however.
 *
 * \sa irtkPartialAffineTransformation
 */

class irtkInverseAffineTransformation : public irtkAffineTransformation
{
  irtkTransformationMacro(irtkInverseAffineTransformation);

  // ---------------------------------------------------------------------------
  // Attributes

  /// Pointer to decorated transformation
  irtkReadOnlyAggregateMacro(irtkAffineTransformation, Transformation);

  /// Observes changes of decorated transformation
  irtkEventDelegate _TransformationObserver;

  /// Update transformation after change of decorated transformation
  void OnTransformationChanged();

public:

  /// Set decorated rigid, similarity, or affine transformation
  void Transformation(irtkAffineTransformation *);

  // ---------------------------------------------------------------------------
  // Construction/Destruction

  /// Constructor
  irtkInverseAffineTransformation(irtkAffineTransformation * = NULL);

  /// Destructor
  virtual ~irtkInverseAffineTransformation();

  // ---------------------------------------------------------------------------
  // Transformation parameters

  /// Checks whether transformation depends on the same vector of parameters
  virtual bool HasSameDOFsAs(const irtkTransformation *) const;

  // ---------------------------------------------------------------------------
  // Derivatives

  using irtkAffineTransformation::JacobianDOFs;

  /// Calculates the Jacobian of the transformation w.r.t the parameters
  virtual void JacobianDOFs(double [3], int, double, double, double, double = 0, double = 1) const;
  
};


#endif
