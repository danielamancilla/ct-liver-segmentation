#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkIsolatedConnectedImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"

typedef signed short PixelType;
typedef float FloatingPixelType;
const unsigned int Dimension = 2;

typedef itk::Image< PixelType, Dimension > ImageType;
typedef itk::Image< FloatingPixelType, Dimension > InternalImageType;

/*
**
** CT Liver segmentation with multiple seeds (IsolatedConnectedImageFilter)
**
** D. Mancilla, D. Pedraza
** Pontificia Universidad Javeriana, 2017
**
** e.g. for 000016.dcm image, use:
** seed1X : 330
** seed1Y : 215
**
** seed2X : 252
** seed2Y : 250
**
** lowerThreshold : 96
**
**/

int main( int argc, char* argv[] ) {

	if ( argc != 8 ) {
		std::cerr << "\nUsage: " << std::endl;
		std::cerr << argv[0];
		std::cerr << " input output seed1X seed1Y seed2X seed2Y lowerThreshold";
		std::cerr << std::endl;
		return EXIT_FAILURE;
	}

	// Initializations
	const char * inputFileName = argv[1];
	const char * outputFileName = argv[2];
	const int seed1X = atoi( argv[3] );
	const int seed1Y = atoi( argv[4] );
    const int seed2X = atoi( argv[5] );
	const int seed2Y = atoi( argv[6] );	
	const PixelType lowerThreshold = atoi( argv[7] );

	try {

		// Reader for DICOM slice
		typedef itk::ImageFileWriter< ImageType >  WriterType;
		typedef itk::ImageFileReader< ImageType >  ReaderType;
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName( inputFileName );

		typedef itk::GDCMImageIO ImageIOType;
		ImageIOType::Pointer dicomIO = ImageIOType::New();
		reader->SetImageIO( dicomIO );
		reader->Update();
		ImageType::Pointer inputImage = reader->GetOutput();

		// Smooth image
		typedef itk::CurvatureFlowImageFilter< ImageType, ImageType > CurvatureFlowImageFilterType;
  		CurvatureFlowImageFilterType::Pointer smoother = CurvatureFlowImageFilterType::New();
		smoother->SetNumberOfIterations( 5 );
  		smoother->SetTimeStep( 0.125 );
		smoother->SetInput( inputImage );

		// Region grow
		typedef itk::IsolatedConnectedImageFilter< ImageType, ImageType > ConnectedFilterType;
		ConnectedFilterType::Pointer regionGrow = ConnectedFilterType::New();
		regionGrow->SetInput( smoother->GetOutput() );
		regionGrow->SetReplaceValue( 255 );		
		regionGrow->SetLower(  lowerThreshold  );
		
		// Seed point #1
		ImageType::IndexType seedIndex1;
		seedIndex1[0] = seed1X;
  		seedIndex1[1] = seed1Y;
		regionGrow->AddSeed1( seedIndex1 );

		// Seed point #2
		ImageType::IndexType seedIndex2;
		seedIndex2[0] = seed2X;
  		seedIndex2[1] = seed2Y;
		regionGrow->AddSeed2( seedIndex2 );

        /*for(unsigned int r = seed1X; r < seed1X+10; r++)
        {
            for(unsigned int c = seed1Y; c < seed1Y+10; c++)
            {
                ImageType::IndexType pixelIndex;
                pixelIndex[0] = r;
                pixelIndex[1] = c;
 
                inputImage->SetPixel( pixelIndex, 255.0 );
            }
        }*/

        // Casting
        /*typedef itk::CastImageFilter< InternalImageType, ImageType > CastingFilterType;        
        CastingFilterType::Pointer caster = CastingFilterType::New();
        caster->SetInput( regionGrow->GetOutput() );*/

		// Writer
		WriterType::Pointer writer = WriterType::New();
		writer->SetFileName( outputFileName );
		writer->SetImageIO( dicomIO );
		//writer->SetInput( inputImage );
		writer->SetInput( regionGrow->GetOutput() );
		writer->Update();

		std::cout << "\nFinished CT liver segmentation with multiple seeds" << std::endl;
        std::cout << "Isolated value found = ";
        std::cout << regionGrow->GetIsolatedValue()  << std::endl;
	}
	catch (itk::ExceptionObject & err) {
		std::cout << "\n\tCaught an exception!" << std::endl;
		std::cout << "\t" << err << std::endl;
		return EXIT_FAILURE;
	}
}