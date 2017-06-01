#include <iostream>
#include <stdlib.h>
 
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGDCMImageIO.h"
#include "itkScalarToRGBPixelFunctor.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkVectorCastImageFilter.h"
#include "itkVectorGradientAnisotropicDiffusionImageFilter.h"
#include "itkWatershedImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"

typedef unsigned char PixelType;
typedef float FloatPixelType;
typedef itk::RGBPixel< unsigned char > RGBPixelType;

const unsigned int Dimension = 2;

typedef itk::Image< PixelType, Dimension > ImageType;
typedef itk::Image<RGBPixelType, Dimension> RGBImageType;
typedef itk::Image<itk::IdentifierType, Dimension> LabeledImageType;
typedef itk::Image< FloatPixelType, Dimension > FloatImageType;

/*
**
** Watershed segmentation
**
** D. Mancilla, D. Pedraza
** Pontificia Universidad Javeriana, 2017
**
**/

int main( int argc, char* argv[] ) {

	if ( argc != 5 ) {
		std::cerr << "\nUsage: " << std::endl;
		std::cerr << argv[0];
		std::cerr << " input output threshold level";
		std::cerr << std::endl;
		return EXIT_FAILURE;
	}

	// Initializations
	std::cout << "\nInitializing params..." << std::endl;		
	const char * inputFileName = argv[1];
	const char * outputFileName = argv[2];	
	const float threshold = atof( argv[3] );	
	const float level = atof( argv[4] );
	
	try {

		// Reader for DICOM slice
		std::cout << "\nReading image..." << std::endl;				
		typedef itk::ImageFileReader< ImageType >  ReaderType;
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName( inputFileName );

		typedef itk::GDCMImageIO ImageIOType;
		ImageIOType::Pointer dicomIO = ImageIOType::New();
		reader->SetImageIO( dicomIO );
		reader->Update();
		ImageType::Pointer inputImage = reader->GetOutput();
		
		// Smoothing
		std::cout << "\nSmoothing image..." << std::endl;		
		typedef itk::GradientMagnitudeImageFilter< ImageType, FloatImageType > GradientMagnitudeImageFilterType;
  		GradientMagnitudeImageFilterType::Pointer gradientFilter = GradientMagnitudeImageFilterType::New(); 
		gradientFilter->SetInput( inputImage );
  		gradientFilter->Update();

		//Segmentation
		std::cout << "\nWatershed segmentation..." << std::endl;
		typedef itk::WatershedImageFilter< FloatImageType > WatershedFilterType;
  		WatershedFilterType::Pointer watershed = WatershedFilterType::New();
  		watershed->SetThreshold( threshold );
  		watershed->SetLevel( level );
 		watershed->SetInput( gradientFilter->GetOutput() );
  		watershed->Update();
		
		typedef itk::ScalarToRGBColormapImageFilter<LabeledImageType, RGBImageType> RGBFilterType;
  		RGBFilterType::Pointer colormapFilter = RGBFilterType::New();
  		colormapFilter->SetInput( watershed->GetOutput() );
  		colormapFilter->SetColormap( RGBFilterType::Jet );
  		colormapFilter->Update();

		// Writer
		typedef itk::ImageFileWriter< RGBImageType >  WriterType;
		WriterType::Pointer writer = WriterType::New();
		writer->SetFileName( outputFileName );
		writer->SetInput( colormapFilter->GetOutput() );
		writer->Update();

		std::cout << "\nFinished" << std::endl;

	}
	catch (itk::ExceptionObject & err) {
		std::cout << "\n\tCaught an exception!" << std::endl;
		std::cout << "\t" << err << std::endl;
		return EXIT_FAILURE;
	}
}