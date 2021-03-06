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

#include <irtkImage.h>

#include <irtkGradientImageFilter.h>

template <class VoxelType> irtkGradientImageFilter<VoxelType>::irtkGradientImageFilter(int type)
{
  _type           = type;
  _UseVoxelSize   = true;
  _UseOrientation = false;
  _Padding        = MIN_GREY;
}

template <class VoxelType> void irtkGradientImageFilter<VoxelType>::Initialize()
{
  // Print debugging information
  this->Debug("irtkImageToImage::Initialize");

  // Check inputs and outputs
  if (this->_input == NULL) {
    cerr << this->NameOfClass() << "::Run: Filter has no input" << endl;
    exit(1);
  }

  if (this->_output == NULL) {
    cerr << this->NameOfClass() << "::Run: Filter has no output" << endl;
    exit(1);
  }

  if (this->_input->IsEmpty() == true) {
    cerr << this->NameOfClass() << "::Run: Input is empty" << endl;
    exit(1);
  }

  if (this->_input->GetT() > 1) {
    cerr << this->NameOfClass() << "::Run: Only implemented for images with t = 1" << endl;
    exit(1);
  }

  // Check whether filter requires buffering
  if (this->RequiresBuffering()) {
    this->Debug("irtkGradientImageFilter::Initialize: Filter requires buffering");

    // Check whether filter has external buffer
    if (this->_input == this->_output) {
      this->Debug("irtkGradientImageFilter::Initialize: Filter has internal buffer");
      this->_tmp    = this->_output;
      this->_output = new irtkGenericImage<VoxelType>;
    } else {
      this->Debug("irtkGradientImageFilter::Initialize: Filter has external buffer");
      this->_tmp    = NULL;
    }
  } else {
    this->Debug("irtkGradientImageFilter::Initialize: Filter requires no buffering");
  }

  // Make sure that output has the correct dimensions
  if ((_type == GRADIENT_VECTOR) || (_type == NORMALISED_GRADIENT_VECTOR)) {
    this->_output->Initialize(this->_input->GetImageAttributes(), 3);
  } else {
    if (this->_input != this->_output) this->_output->Initialize(this->_input->GetImageAttributes());
  }
}

template <class VoxelType> void irtkGradientImageFilter<VoxelType>::Run()
{
  double di, dj, dk, dx, dy, dz, norm;
  int x, y, z, x1, y1, z1, x2, y2, z2;
  irtkMatrix R;

  // Do the initial set up
  this->Initialize();

  const irtkImageAttributes &attr = this->_input->GetImageAttributes();
  if (_UseOrientation) R = attr.GetWorldToImageOrientation();

  for (z = 0; z < this->_input->GetZ(); ++z) {
    z1 = z - 1;
    if (z1 < 0) z1 = 0;
    z2 = z + 1;
    if (z2 > this->_input->GetZ()-1) z2 = this->_input->GetZ()-1;

    for (y = 0; y < this->_input->GetY(); ++y) {
      y1 = y - 1;
      if (y1 < 0) y1 = 0;
      y2 = y + 1;
      if (y2 > this->_input->GetY()-1) y2 = this->_input->GetY()-1;

      for (x = 0; x < this->_input->GetX(); ++x) {
        x1 = x - 1;
        if (x1 < 0) x1 = 0;
        x2 = x + 1;
        if (x2 > this->_input->GetX()-1) x2 = this->_input->GetX()-1;

        // Compute derivatives
        if (x1 != x2 
            && this->_input->Get(x2, y, z) > _Padding
            && this->_input->Get(x1, y, z) > _Padding) {
          dx = (this->_input->Get(x2, y, z) - this->_input->Get(x1, y, z)) / (x2 - x1);
        } else {
          dx = 0;
        }
        if (y1 != y2
            && this->_input->Get(x, y2, z) > _Padding
            && this->_input->Get(x, y1, z) > _Padding) {
          dy = (this->_input->Get(x, y2, z) - this->_input->Get(x, y1, z)) / (y2 - y1);
        } else {
          dy = 0;
        }
        if (z1 != z2
            && this->_input->Get(x, y, z2) > _Padding
            && this->_input->Get(x, y, z1) > _Padding) {
          dz = (this->_input->Get(x, y, z2) - this->_input->Get(x, y, z1)) / (z2 - z1);
        } else {
          dz = 0;
        }

        if (_UseVoxelSize) {
          if (this->GetInput()->GetXSize() > .0) dx /= this->GetInput()->GetXSize();
          if (this->GetInput()->GetYSize() > .0) dy /= this->GetInput()->GetYSize();
          if (this->GetInput()->GetZSize() > .0) dz /= this->GetInput()->GetZSize();
        }
        if (_UseOrientation) {
          // Using numerator-layout for matrix calculus.
          // http://en.wikipedia.org/wiki/Matrix_calculus#Numerator-layout_notation
          di = dx, dj = dy, dk = dz;
          dx = di * R(0, 0) + dj * R(1, 0) + dk * R(2, 0);
          dy = di * R(0, 1) + dj * R(1, 1) + dk * R(2, 1);
          dz = di * R(0, 2) + dj * R(1, 2) + dk * R(2, 2);
        }

        switch (_type) {
          case GRADIENT_X:
            this->_output->PutAsDouble(x, y, z, 0, dx);
            break;
          case GRADIENT_Y:
            this->_output->PutAsDouble(x, y, z, 0, dy);
            break;
          case GRADIENT_Z:
            this->_output->PutAsDouble(x, y, z, 0, dz);
            break;
          case GRADIENT_MAGNITUDE:
            this->_output->PutAsDouble(x, y, z, 0, sqrt(dx*dx + dy*dy + dz*dz));
            break;
          case GRADIENT_VECTOR:
            this->_output->PutAsDouble(x, y, z, 0, dx);
            this->_output->PutAsDouble(x, y, z, 1, dy);
            this->_output->PutAsDouble(x, y, z, 2, dz);
            break;
          case NORMALISED_GRADIENT_VECTOR:
            norm = sqrt(dx*dx + dy*dy + dz*dz);
            dx /= (norm + 1E-10);
            dy /= (norm + 1E-10);
            dz /= (norm + 1E-10);
            this->_output->PutAsDouble(x, y, z, 0, dx);
            this->_output->PutAsDouble(x, y, z, 1, dy);
            this->_output->PutAsDouble(x, y, z, 2, dz);
            break;
          default:
            cerr << this->NameOfClass() << "::Run: Unknown gradient computation" << endl;
            exit(1);
        }
      }
    }
  }

  // Do the final cleaning up
  this->Finalize();
}

template class  irtkGradientImageFilter<unsigned char>;
template class  irtkGradientImageFilter<short>;
template class  irtkGradientImageFilter<float>;
template class  irtkGradientImageFilter<double>;
