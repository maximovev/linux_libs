#ifndef __raw__
#define __raw__

#include "enums.h"
#include "cubic_interpolate.h"
#include "matrix.h"

namespace maxssau
{
	enum DemosaicMode
	{
		FullColor=0
	}


	template <typename Type>struct ColorMatrix
	{
		Type Values[3][3];
	}

    template <typename Type>struct MinMaxValues
    {
        Type Min;
        Type Max;
    };

    template <typename Type>struct RGBMinMaxValues
    {
        MinMaxValues<Type> R;
        MinMaxValues<Type> G;
        MinMaxValues<Type> B;
    };

	typedef struct raw_converter_settings
	{
		bool use_colormatrix;
		bool use_white_balance;
		bool use_white_balance_user;
		bool use_gammacurve;
		bool use_gammacurve_user;
		bool normalize_input_data;
		unsigned int demosaic_mode;
	};
    

    template <typename TypeInputData, typename TypeOutputData> class raw_converter
    {
        public:
			raw_converter_settings 				settings;
			Matrix<TypeOutputData>				color_matrix;
			CubicInterpolator<TypeOutputData>	gamma_curve_user;

			unsigned int 						white_balance_target;
			Matrix<TypeOutputData>				white_balance_coeff;

            raw_converter()
            {
				settings.use_colormatrix=true;
				settings.use_gammacurve=true;
				settings.use_white_balance=true;

				constexpr ClassType max = std::numeric_limits<ClassType>::max();
				InputDataMaximum=max;
            };

            ~raw_converter()
            {
                
            }

			unsigned int GetIndexInputData(unsigned int x, unsigned int y, unsigned int width)
			{
				return y*width+x;
			}

			Matrix<TypeOutputData> Get_RGB_RAW_Data(unsigned int x, unsigned int y)
			{

			}

            int Process(TypeInputData *input, TypeOutputData *output,MinMaxValues<TypeInputData> MinMax, unsigned int height, unsigned int width)
            {


                return STATUS_OK;
            }

        private:

		TypeInputData		InputDataMaximum;
    };


}

#endif